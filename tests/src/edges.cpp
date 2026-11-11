// Additional audit after the 170-check suite. Expectations: Qt 5.15.2.
// Regression coverage for the incompatibilities found after the first 170 checks.
#include "qstring_nx_cb.h"
#include <cstdio>
#include <clocale>
#include <limits>

static int checks = 0, failures = 0;
static void check(bool value, const char* name) {
    ++checks;
    if (!value) { ++failures; std::printf("FAIL: %s\n", name); }
}
static void dump(const QString& s) {
    std::printf("null=%d size=%d [", s.isNull(), s.length());
    for (int i=0; i<s.length(); ++i) std::printf("%04X ", unsigned(s.at(i).unicode()));
    std::printf("]\n");
}
static void equal(const QString& actual, const QString& expected, const char* name) {
    const bool passed = actual == expected;
    check(passed, name);
    if (!passed) { std::printf(" actual "); dump(actual); std::printf(" expect "); dump(expected); }
}
template<class Action> static void formatCheck(Action action, const QString& expected, const char* name) {
    try { equal(action(), expected, name); }
    catch (...) { check(false, name); std::printf(" threw an exception\n"); }
}

int main() {
    std::setlocale(LC_ALL, "C");
    // Qt formats Unicode directly; widths, precisions and %n count UTF-16 units.
    formatCheck([] { return QString::asprintf("%ls", L"\x0416\x20AC\x4E2D"); },
                QString(L"\x0416\x20AC\x4E2D"), "printf wide string");
    formatCheck([] { return QString::asprintf("%lc", 0x0416); },
                QString(L"\x0416"), "printf wide character");
    formatCheck([] { return QString::asprintf("%3s", "\xD0\x96"); },
                QString(L"  \x0416"), "printf UTF8 width counts UTF16");
    formatCheck([] { return QString::asprintf("%.1s", "\xD0\x96"); },
                QString(L"\x0416"), "printf UTF8 precision counts UTF16");
    int written=-1;
    formatCheck([&] { return QString::asprintf("%s%n", "\xD0\x96", &written); },
                QString(L"\x0416"), "printf UTF8 with percent n text");
    check(written==1, "printf percent n counts UTF16");
    check(!QString::asprintf(nullptr).isNull(), "printf null format returns nonnull empty");
    formatCheck([] { return QString::asprintf("%c", 0xFF); }, QString(QChar(wchar_t(0xFF))), "printf c is Latin1");

    // Qt suppresses the sign of exact negative zero and honours E/G for inf/nan.
    equal(QString::number(-0.0), "0", "number negative zero g");
    equal(QString::number(-0.0,'f',2), "0.00", "number negative zero f");
    equal(QString::number(-0.0,'e',2), "0.00e+00", "number negative zero e");
    equal(QString::number(-0.01,'f',1), "-0.0", "negative nonzero rounded to zero keeps sign");
    equal(QString::number(std::numeric_limits<double>::infinity(),'G'), "INF", "uppercase infinity");
    equal(QString::number(-std::numeric_limits<double>::infinity(),'E'), "-INF", "uppercase negative infinity");
    equal(QString::number(std::numeric_limits<double>::quiet_NaN(),'G'), "NAN", "uppercase NaN");

    // Unicode 13 properties and simple one-code-unit case mappings in QChar.
    check(QChar(wchar_t(0x1C90)).isLetter(), "QChar Unicode11 Georgian letter");
    check(QChar(wchar_t(0x1C90)).isUpper(), "QChar Unicode11 Georgian upper");
    equal(QString(QChar(wchar_t(0x1C90)).toLower()), QString(L"\x10D0"), "QChar Georgian lowercase");
    equal(QString(QChar(wchar_t(0x10D0)).toUpper()), QString(L"\x1C90"), "QChar Georgian uppercase");
    equal(QString(QChar(wchar_t(0xA7C4)).toLower()), QString(L"\xA794"), "QChar Unicode12 lowercase");
    check(QChar(wchar_t(0xA7C8)).isLetter(), "QChar Unicode13 letter");
    check(!QChar(wchar_t(0x180E)).isSpace(), "QChar U180E is not whitespace in Unicode13");
    equal(QString(L"\x180E").trimmed(), QString(L"\x180E"), "trimmed preserves U180E");
    equal(QString(L"a\x180E\x0062").simplified(), QString(L"a\x180E\x0062"), "simplified preserves U180E");
    check(QChar(wchar_t(0x00DF)).toUpper().unicode()==0x00DF, "QChar sharp s does not expand");
    // Qt's QChar cannot return the two-unit full lowercase mapping (i + dot).
    check(QChar(wchar_t(0x0130)).toLower().unicode()==0x0130, "QChar dotted I does not expand");
    equal(QString(L"\x1C90").toLower(), QString(L"\x10D0"), "QString Georgian lowercase control");

    QString s("abc");
    s.insert(-1,QChar('X')); equal(s,"abXc","insert QChar relative negative position");
    s="abc"; s.insert(-1,QString("X")); equal(s,"abc","insert QString ignores negative position");
    check(QString("abc").indexOf(QChar('a'),-99)==0, "indexOf QChar clamps negative start");
    check(QString("abc").indexOf(QString("a"),-99)==-1, "indexOf QString negative start control");
    check(QString("abc").indexOf(QString("c"),-4)==2, "indexOf one-unit QString Qt515 edge");
    s="abc"; s.remove(-1,1); equal(s,"ab","remove relative negative position");
    s="abc"; s.remove(-2,99); equal(s,"a","remove negative position with length clamp");
    s="abc"; s.remove(-4,1); equal(s,"abc","remove too-negative position control");
    s="abc"; s.truncate(-1); equal(s,"","truncate negative position clears");
    s=QString(); s.truncate(-1); check(!s.isNull(), "truncate negative on null makes empty");
    s=QString(); s.chop(1); check(!s.isNull(), "chop positive on null makes empty");
    s=QString(); s.prepend(QString("")); check(s.isNull(), "prepend empty preserves null");
    s=QString(); s.append(QString("")); check(!s.isNull(), "append empty makes nonnull control");
    s=QString(); s.prepend(QString()); check(s.isNull(), "prepend null preserves null control");

    // Qt's char constructor is explicit: comparisons promote char to int,
    // then use the implicit QChar(int), which keeps the low 16 bits.
    const char latin1 = static_cast<char>(0xFF);
    check(!(QChar(latin1)==latin1), "QChar U00FF differs from signed char minus one");
    check(!(latin1==QChar(latin1)), "signed char minus one differs from QChar U00FF");
    s=latin1; check(!(s[0]==latin1), "QCharRef U00FF differs from signed char minus one");
    check(!(s.at(0)==latin1), "const U00FF comparison control");
    check(QChar(wchar_t(0xFFFF))==-1, "QChar compares int through low 16 bits");
    check(-1==QChar(wchar_t(0xFFFF)), "int compares QChar through low 16 bits");
    s=QString(QChar(wchar_t(0xFFFF)));
    check(s[0]==latin1, "QCharRef UFFFF equals signed char minus one");
    check(latin1==s[0], "signed char minus one equals QCharRef UFFFF");

    equal(QString::fromUtf8("\xEF\xBB\xBF" "a"), "a", "fromUtf8 strips leading BOM");
    equal(QString::fromUtf8("a\xEF\xBB\xBF"), QString(L"a\xFEFF"), "fromUtf8 preserves inner BOM control");
    equal(QString::fromUtf8("\xE2\x82"), QString(L"\xFFFD\xFFFD"), "fromUtf8 truncated three-byte sequence");
    equal(QString::fromUtf8("\xED\xA0\x80"), QString(L"\xFFFD\xFFFD\xFFFD"), "fromUtf8 encoded surrogate");
    equal(QString("\xEF\xBB\xBF" "a"), "a", "UTF8 constructor strips leading BOM");
    equal(QString::fromStdString("\xEF\xBB\xBF" "a"), "a", "fromStdString strips leading BOM");
    check(QString(L"\xD800").toUtf8()=="?", "toUtf8 replaces unpaired high surrogate with question mark");
    check(QString(L"\xDC00").toUtf8()=="?", "toUtf8 replaces unpaired low surrogate with question mark");
    check(QString(L"\xD83D\xDE00").toUtf8()=="\xF0\x9F\x98\x80", "toUtf8 valid surrogate pair control");

    std::printf("%d additional checks, %d failures\n",checks,failures);
    return failures ? 1 : 0;
}
