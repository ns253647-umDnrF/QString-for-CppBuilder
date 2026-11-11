# QString for Embarcadero C++Builder 10.4. Ver. 2026.11.11

An adapter implementing a subset of `QString`, `QStringList`, `QChar`, `QCharRef`,
and Qt enumerations/flags for Windows and C++Builder 10.4.
Qt 5.15.2 is the behavioral reference for the implemented operations.

## Contents

- `qstring_nx_cb.h` - UTF-16 strings, conversions, searching, formatting, and string lists.
- `qchar_nx_cb.h` - UTF-16 characters, character properties, and proxies for modifying characters in strings.
- `qtnamespace_nx_cb.h` - enumerations and strongly typed flags.
- `examples/basic.cpp` - minimal example.
- `tests/` - standalone regression test suite and reference data.
- `docs/` - changes, testing scope, and verification results.
- `MANIFEST.json`, `SHA256SUMS` â€” version, package contents, and file checksums.

## Dependencies and integration

Windows and an installed C++Builder 10.4 are required, including its headers, libraries,
and Delphi RTL. The Clang-based compilers `bcc32c` (Win32) and `bcc64` (Win64),
version 7.50, have been tested. Use C++14 for the string and character headers.
The standalone `qtnamespace_nx_cb.h` has also been tested with C++11.

Place **all three headers in the same folder**, add that folder to the project's
include path, and include `qstring_nx_cb.h`. The adapter does not depend on Qt DLLs.
It uses `System.hpp`, `System.SysUtils.hpp`, `System.Math.hpp`,
`System.Character.hpp`, the Windows API, and the C++Builder standard library.
The compiler, its standard library, and the RTL are not included in the package.

Example using the RAD Studio command prompt, from the root of the extracted package:

```bat
mkdir build
bcc32c -std=c++14 -I. -tC -o build\basic32.exe examples\basic.cpp rtl.bpi
bcc64  -std=c++14 -I. -tC -o build\basic64.exe examples\basic.cpp rtl.bpi
build\basic32.exe
build\basic64.exe
```

The test builds link against `rtl.bpi`; the corresponding RTL environment must be
available at runtime. When distributing your own application, use the normal
C++Builder runtime dependencies appropriate for your build settings.

```cpp
#include "qstring_nx_cb.h"

QString text = QString::fromUtf8("Hello");
QString result = QString("%1: %2").arg(text).arg(42);
bool found = result.contains(QString("HELLO"), Qt::CaseInsensitive);
```

## Compatibility limitations

- Only a subset of the Qt API is implemented. Binary compatibility with Qt is not
  provided; adapter objects cannot be passed to Qt DLLs. The class names are in
  the global namespace; the adapter headers and the original Qt classes with
  the same names should be used in separate translation units/projects.
- Conversions to byte strings return `std::string`; string `split()` returns
  `std::vector<QString>`. The `QByteArray`, `QTextCodec`, and `QLocale` APIs
  are not included. Qt's custom codec for the local encoding is not supported.
- `length()` and indices are measured in 16-bit UTF-16 code units. Character
  property and case-conversion tables are based on Unicode 13.0.
- `fromLocal8Bit()`/`toLocal8Bit()` use the system Windows ACP and are unaffected
  by `System::SetMultiByteConversionCodePage()`. Full local-encoding comparison
  has been performed with ACP 1251. Other system ACPs, including 65001 and DBCS,
  are not yet part of the verified test matrix. Changing the Delphi RTL code
  page does not replace testing with a different system ACP.
- Win32/Win64 have been tested with normal optimization and `-O2`. The classic
  `bcc32`, other C++Builder versions, other compilers, and other operating
  systems are outside the test matrix.
- For deliberately invalid indices and overflows, the adapter sometimes
  returns a safe result or throws a standard exception; behavior of operations
  that are undefined in Qt is not reproduced.
- Independent string copies have been tested in threads created with
  `_beginthreadex` when `System::IsMultiThread = true`. Concurrent modification
  of the same object requires external synchronization.

## Package verification

Python 3.8+ from the standard distribution is required; no additional Python
packages are needed. Open a RAD Studio command prompt where `bcc32c` and `bcc64`
are available:

```bat
python tests\run_tests.py --verify-only
python tests\run_tests.py
```

The second command builds and runs tests for both architectures, with normal
optimization and with `-O2`. Neither Qt nor network access is required.
Test results and temporary build artifacts are created only in the new
`tests/build/` folder.
The full test inventory, skips for other ACPs, and additional options are
described in [docs/TESTING.md](docs/TESTING.md).

## Version and third-party data

Changes are listed in [docs/CHANGELOG.md](docs/CHANGELOG.md).
The release packaging did not change the logic of the three headers: only
version comments were added or updated. The relationship between the new
SHA-256 checksums and the previously verified build is recorded in
`docs/release_provenance.json`.

The Unicode data is accompanied by a notice and terms in
[licenses/UNICODE-LICENSE.txt](licenses/UNICODE-LICENSE.txt), obtained from
the [official Unicode source](https://www.unicode.org/license.txt).
This license applies to the Unicode data. The package does not assign a new
license to the remaining original code; its terms of use are determined by
the copyright holder.
