"""Offline release tests. Run with Python 3.8+ in a C++Builder command prompt."""
import argparse
import ctypes
from datetime import datetime, timezone
import gzip
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
HEADERS = ('qchar_nx_cb.h', 'qstring_nx_cb.h', 'qtnamespace_nx_cb.h')


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def verify_package():
    manifest = json.loads((ROOT / 'MANIFEST.json').read_text(encoding='utf-8'))
    for name, wanted in manifest['files'].items():
        path = ROOT / name
        if not path.is_file() or digest(path) != wanted:
            raise RuntimeError('Package checksum mismatch: ' + name)
    return manifest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--arch', choices=('both', 'win32', 'win64'), default='both')
    parser.add_argument('--opt', choices=('both', 'default', 'O2'), default='both')
    parser.add_argument('--compiler32', default='bcc32c')
    parser.add_argument('--compiler64', default='bcc64')
    parser.add_argument('--build-dir', type=Path)
    parser.add_argument('--verify-only', action='store_true')
    parser.add_argument('--strict-performance', action='store_true',
                        help='Enforce machine-dependent timing limits in O2 builds')
    parser.add_argument('--require-acp1251', action='store_true',
                        help='Fail instead of skipping the frozen ACP1251 corpus on other ACPs')
    args = parser.parse_args()
    manifest = verify_package()
    print('Package checksums: PASS (' + manifest['version'] + ')', flush=True)
    if args.verify_only:
        return 0
    if os.name != 'nt':
        raise RuntimeError('Compilation and runtime tests require Windows and C++Builder 10.4')
    compilers = {}
    for arch, value in (('win32', args.compiler32), ('win64', args.compiler64)):
        if args.arch not in ('both', arch):
            continue
        executable = shutil.which(value)
        if not executable and Path(value).is_file():
            executable = str(Path(value).resolve())
        if not executable:
            raise RuntimeError('Compiler not found: ' + value + '; initialize the RAD Studio command prompt')
        compilers[arch] = executable
    build = (args.build_dir or ROOT / 'tests' / 'build' /
             datetime.now().strftime('%Y%m%d_%H%M%S_%f')).resolve()
    build.mkdir(parents=True, exist_ok=False)
    acp = ctypes.windll.kernel32.GetACP()
    if args.require_acp1251 and acp != 1251:
        raise RuntimeError('The frozen local8Bit reference requires Windows ACP1251, got ' + str(acp))
    corpus = json.loads((ROOT / 'tests' / 'fixtures' / 'corpus.json').read_text(encoding='utf-8'))
    inputs = {}
    for name, row in corpus['suites'].items():
        source = build / (name + '.input.bin')
        source.write_bytes(gzip.decompress((ROOT / 'tests' / 'fixtures' / row['input']).read_bytes()))
        inputs[name] = source
    report = {'version': manifest['version'], 'started_utc': datetime.now(timezone.utc).isoformat(),
              'header_sha256': {n: digest(ROOT / n) for n in HEADERS},
              'windows_acp': acp, 'compiler_paths': compilers, 'profiles': {}, 'passed': False}
    log = []

    def command(argv, cwd, timeout=300):
        result = subprocess.run([str(a) for a in argv], cwd=str(cwd), stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, timeout=timeout)
        output = result.stdout.decode('utf-8', errors='replace')
        log.append('> ' + subprocess.list2cmdline([str(a) for a in argv]) + '\n' + output)
        if result.returncode:
            raise RuntimeError('Command failed (exit %d):\n%s\n%s' %
                               (result.returncode, subprocess.list2cmdline([str(a) for a in argv]), output))
        return output

    try:
        for arch, compiler in compilers.items():
            for opt in (('default', 'O2') if args.opt == 'both' else (args.opt,)):
                profile = arch + '_' + opt
                folder = build / profile
                folder.mkdir()
                rows = []
                report['profiles'][profile] = rows
                flags = ['-std=c++14', '-I' + str(ROOT)] + (['-O2'] if opt == 'O2' else [])

                def compile_program(name, defines=(), source=None):
                    source = source or ROOT / 'tests' / 'src' / (name + '.cpp')
                    exe = folder / (name + '.exe')
                    command([compiler] + flags + ['-tC'] + ['-D' + d for d in defines] +
                            ['-o', exe, source, 'rtl.bpi'], folder)
                    return exe

                def finish(name, **values):
                    rows.append(dict(name=name, passed=True, **values))
                    print(profile + ' ' + name + ': PASS', flush=True)

                for name in ('basic_strings', 'extra_strings', 'compatibility', 'edges', 'formatter'):
                    output = command([compile_program(name)], folder)
                    finish(name, output=output)
                output = command([compile_program('example', source=ROOT / 'examples' / 'basic.cpp')], folder)
                finish('example', output=output)

                for source, define in (('compile_baseline', 'CHECK_CHARREF'),
                                       ('compile_baseline', 'CHECK_NUM64'),
                                       ('compile_edges', 'CHECK_QCHAR_INT'),
                                       ('compile_edges', 'CHECK_QCHAR_USHORT')):
                    obj = folder / (define + '.o')
                    command([compiler] + flags + ['-c', '-D' + define, '-o', obj,
                            ROOT / 'tests' / 'src' / (source + '.cpp')], folder)
                for standard in ('c++11', 'c++14'):
                    command([compiler, '-std=' + standard, '-I' + str(ROOT), '-c', '-o',
                            folder / ('namespace_' + standard.replace('+', 'p') + '.o'),
                            ROOT / 'tests' / 'src' / 'namespace_values.cpp'], folder)
                command([compile_program('compile_baseline', ('CHECK_CHARREF', 'CHECK_NUM64', 'CHECK_MAIN'))], folder)
                command([compile_program('api_smoke')], folder)
                finish('compilation', compile_only_checks=6, linked_checks=2)

                for name, row in corpus['suites'].items():
                    if row.get('windows_acp') and row['windows_acp'] != acp:
                        rows.append({'name': name, 'skipped': True, 'reason': 'requires Windows ACP1251'})
                        print(profile + ' ' + name + ': SKIP (Windows ACP is ' + str(acp) + ')', flush=True)
                        continue
                    output_file = folder / (name + '.actual.bin')
                    output = command([compile_program(name), inputs[name], output_file], folder)
                    expected = gzip.decompress((ROOT / 'tests' / 'fixtures' / row['expected']).read_bytes())
                    actual = output_file.read_bytes()
                    if actual != expected:
                        offset = next((i for i, (a, b) in enumerate(zip(actual, expected)) if a != b),
                                      min(len(actual), len(expected)))
                        raise RuntimeError('%s %s: byte mismatch at %d (actual %d, expected %d bytes)' %
                                           (profile, name, offset, len(actual), len(expected)))
                    finish(name, checks=row['checks'], expected_sha256=hashlib.sha256(expected).hexdigest(), output=output)

                objects = []
                for name in ['integration_tu' + str(i) for i in range(6)] + ['integration']:
                    obj = folder / (name + '.o')
                    command([compiler] + flags + ['-tM', '-c', '-o', obj,
                            ROOT / 'tests' / 'src' / (name + '.cpp')], folder)
                    objects.append(obj)
                exe = folder / 'integration.exe'
                command([compiler, '-tM', '-tC', '-o', exe] + objects + ['rtl.bpi'], folder)
                output = command([exe], folder)
                match = re.search(r'(\d+) checks, (\d+) failures', output)
                if not match or int(match.group(2)):
                    raise RuntimeError('Integration test did not report success: ' + output)
                finish('integration', checks=int(match.group(1)), output=output)

                output = command([compile_program('search_performance')], folder)
                timings = {name: float(ms) for name, ms in re.findall(r'(\w+) ms=([\d.]+)', output)}
                limits = {'reverse_missing': 100.0, 'dense_count': 100.0,
                          'dense_count_insensitive': 100.0, 'insensitive_early': 5.0,
                          'insensitive_late_start': 5.0, 'reverse_near_end': 5.0}
                if set(timings) != set(limits):
                    raise RuntimeError('Incomplete performance output: ' + output)
                if args.strict_performance and opt == 'O2':
                    for name, limit in limits.items():
                        if timings[name] >= limit:
                            raise RuntimeError('Timing limit exceeded: ' + name)
                finish('search_performance', milliseconds=timings,
                       timing_limits_enforced=args.strict_performance and opt == 'O2')
        report['passed'] = True
    except Exception as error:
        report['error'] = str(error)
        print('FAIL: ' + str(error), file=sys.stderr, flush=True)
    finally:
        report['finished_utc'] = datetime.now(timezone.utc).isoformat()
        report['skipped'] = sum(bool(r.get('skipped')) for rows in report['profiles'].values() for r in rows)
        report['corpus_and_integration_checks'] = sum(r.get('checks', 0) for rows in report['profiles'].values() for r in rows)
        report['headers_unchanged'] = report['header_sha256'] == {n: digest(ROOT / n) for n in HEADERS}
        report['passed'] = report['passed'] and report['headers_unchanged']
        (build / 'results.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
        (build / 'build.log').write_text('\n'.join(log), encoding='utf-8')
        print('Report: ' + str(build / 'results.json'), flush=True)
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    try:
        sys.exit(main())
    except Exception as error:
        print(str(error), file=sys.stderr)
        sys.exit(1)
