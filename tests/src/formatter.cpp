// Additional coverage for the replacement printf parser, based on Qt 5.15.2
// qstring.cpp and qlocale.cpp (including intentional differences from libc).
#include "qstring_nx_cb.h"
#include <cstdio>
#include <limits>

static int checks=0, failures=0;
static void equal(const QString& actual,const QString& expected,const char* label) {
    ++checks;
    if(actual!=expected) {
        ++failures;
        std::printf("FAIL %s: [%s] != [%s]\n",label,actual.toUtf8().c_str(),expected.toUtf8().c_str());
    }
}
static void check(bool value,const char* label) {
    ++checks;
    if(!value) { ++failures; std::printf("FAIL %s\n",label); }
}
int main() {
    equal(QString::asprintf("%+06d",42),"+00042","signed zero padding");
    equal(QString::asprintf("%+06d",-42),"-00042","negative zero padding");
    equal(QString::asprintf("%-6d",42),"42    ","left padding");
    equal(QString::asprintf("%06.4d",42),"  0042","precision suppresses zero flag");
    equal(QString::asprintf("%#08X",42u),"0x00002A","Qt uppercase hex retains lowercase prefix");
    equal(QString::asprintf("%#.4o",9u),"0011","octal precision and prefix");
    equal(QString::asprintf("%#o",0u),"0","octal zero");
    equal(QString::asprintf("%+06u",42u),"+000042","Qt unsigned sign follows zero padding");
    equal(QString::asprintf("%.0d",0),"","Qt signed zero with zero precision");
    equal(QString::asprintf("%hhd",256),"256","Qt hh reads promoted int without narrowing");
    equal(QString::asprintf("%lld",(std::numeric_limits<long long>::min)()),"-9223372036854775808","signed 64-bit minimum");
    equal(QString::asprintf("%llu",(std::numeric_limits<unsigned long long>::max)()),"18446744073709551615","unsigned 64-bit maximum");
    equal(QString::asprintf("%zu",std::size_t(123)),"123","size_t vararg");
    equal(QString::asprintf("%'d",1234567),"1,234,567","integer grouping");
    equal(QString::asprintf("%#.0f",2.5),"3.","Qt halfway rounding and decimal point");
    equal(QString::asprintf("%#.6g",0.0012),"0.001200","Qt counts leading fractional zeros for alternate g");
    equal(QString::asprintf("%#.6g",12345.0),"12345.0","integer significant trailing zeros");
    equal(QString::asprintf("%#.6g",1e6),"1.00000e+06","scientific significant trailing zeros");
    equal(QString::asprintf("%#.3g",0.0),"0.00","zero significant trailing zeros");
    equal(QString::asprintf("%'.2f",1234.5),"1,234.50","fixed grouping");
    equal(QString::asprintf("%'.6g",12345.5),"12,345.5","general grouping");
    equal(QString::asprintf("%010f",std::numeric_limits<double>::infinity()),"       inf","nonfinite width uses spaces");
    equal(QString::asprintf("%+G",std::numeric_limits<double>::quiet_NaN()),"+NAN","NaN sign and uppercase");
    equal(QString::asprintf("%a",1.5),"1.500000","Qt a uses decimal format");
    equal(QString::asprintf("%Lf",static_cast<long double>(1.25)),"1.250000","long double vararg");
    equal(QString::asprintf("%*d",-5,3),"3","Qt negative star width is unspecified");
    equal(QString::asprintf("%.*f",-2,1.25),"1.250000","negative star precision is unspecified");
    equal(QString::asprintf("%.s","abc"),"abc","Qt bare dot leaves precision unspecified");
    equal(QString::asprintf("a%q%s","v"),"a%qv","unknown escape preserves text and next argument");
    equal(QString::asprintf("a%5l"),"a%5l","unfinished escape");
    equal(QString::asprintf("100%%"),"100%","escaped percent");
    equal(QString::asprintf("%*.*s",4,1,"\xC3\xA9"),QString(L"   \x00E9"),"UTF8 star width and precision");
    const unsigned short wide[]={0x0416,0x20AC,0};
    equal(QString::asprintf("%3.1ls",wide),QString(L"  \x0416"),"UTF16 pointer format");
    equal(QString::asprintf("%s",static_cast<const char*>(nullptr)),"","null UTF8 argument");
    const QString nul=QString::asprintf("x%cy",0);
    check(nul.length()==3 && nul.at(1).isNull(),"embedded NUL character");
    equal(QString::asprintf("%d\xEF\xBB\xBF" "z",1),"1z","Qt decodes each literal chunk independently");
    signed char a=-1; short b=-1; long c=-1; long long d=-1;
    equal(QString::asprintf("%ls%hhn%hn%ln%lln!",wide,&a,&b,&c,&d),QString(L"\x0416\x20AC!"),"n modifier argument alignment");
    check(a==2 && b==2 && c==2 && d==2,"n modifiers count UTF16");
    std::printf("%d printf regression checks, %d failures\n",checks,failures);
    return failures ? 1 : 0;
}
