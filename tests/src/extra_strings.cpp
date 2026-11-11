#include "qstring_nx_cb.h"
#include <cstdio>
#include <cmath>
static int checks=0, failures=0;
static void check(bool value,const char* label) {
    ++checks; if(!value) { ++failures; std::printf("FAIL: %s\n",label); }
}
int main() {
    const Qt::CaseSensitivity ci=Qt::CaseInsensitive;
    QString sharp(L"\x00DF");
    QStringList list{sharp};
    QString replacement=sharp; replacement.replace("s","X",ci);
    check(list.filter("s",ci).empty(),"filter rejects partial expansion just as replace does");
    check(replacement==sharp,"replace preserves partial expansion");
    const std::u16string raw(1,static_cast<char16_t>(0xD800));
    QString bad=QString::fromStdU16String(raw);
    QString original=bad;
    bad.replace(QString(L"\xFFFD"),"X",ci);
    check(bad==original,"case-insensitive replace does not confuse surrogate with U+FFFD");
    QString other=QString::fromStdU16String(std::u16string(1,static_cast<char16_t>(0xD801)));
    check(!QStringList{original}.contains(other,ci),"different unpaired surrogates remain distinct");
    bool ok=true;
    double d=QString("1e999").toDouble(&ok);
    std::printf("double overflow: value=%g ok=%d\n",d,ok);
    check(std::isinf(d) && d>0 && !ok,"double overflow returns positive infinity and failure");
    float f=QString("-1e99").toFloat(&ok);
    std::printf("float overflow: value=%g ok=%d\n",static_cast<double>(f),ok);
    check(std::isinf(f) && f<0 && !ok,"float overflow returns negative infinity and failure");
    QString nul(QChar::Null);
    check(nul.length()==1 && nul.at(0).isNull(),"QChar NUL preserved");
    QString x="abc"; QString y=x; QCharRef ref=x[1]; x.append("d"); ref=QChar('X');
    check(x==QString("aXcd") && y==QString("abc"),"saved reference and shared buffer");
    x="abc"; y=x; wchar_t* p=x.data(); p[0]=L'X';
    check(x==QString("Xbc") && y==QString("abc"),"mutable data COW");
    check(QString("a\0b").length()==1,"NUL-terminated constructor");
    check(QString::fromUtf8("a\0b",3).length()==3,"explicit UTF8 NUL");
    check(QString("abc").lastIndexOf("a",999)==-1,"lastIndexOf out-of-range from");
    check(QString("abc").indexOf("a",-999)==-1,"QString needle negative from matches Qt 5.15");
    check(!QStringList{QString(L"Stra\x00DF" L"e")}.contains("STRASSE",ci),"Qt 5.15 comparison does not expand sharp s");
    replacement=sharp; replacement.replace("ss","X",ci);
    check(replacement==sharp,"Qt 5.15 replacement does not expand sharp s");
    const char* overflowValues[]={"1e309","-1e309","1.7976931348623159e308","9e999999999999999999999"};
    for(const char* token:overflowValues) {
        d=QString(token).toDouble(&ok);
        check(!ok && std::isinf(d) && (std::signbit(d)==(token[0]=='-')),"signed double overflow");
    }
    const char* invalidValues[]={"1e999junk","1e+","+",".","1,2","nanx","1e999.0"};
    for(const char* token:invalidValues) {
        d=QString(token).toDouble(&ok);
        check(!ok && d==0,"invalid numeric token stays zero");
    }
    d=QString("1e-999").toDouble(&ok); check(!ok && d==0,"double underflow");
    d=QString("-0e999999999999999999999").toDouble(&ok);
    check(ok && d==0 && std::signbit(d),"signed zero with huge exponent");
    d=QString("1.7976931348623157e308").toDouble(&ok);
    check(ok && std::isfinite(d) && d>1e308,"maximum double remains finite");
    d=QString("4.9406564584124654e-324").toDouble(&ok);
    check(ok && d>0,"subnormal double remains valid");
    f=QString("1e999").toFloat(&ok); check(!ok && std::isinf(f) && f>0,"float overflow beyond double");
    f=QString("1e-99").toFloat(&ok); check(!ok && f==0,"float underflow remains zero");
    f=QString("inf").toFloat(&ok); check(ok && std::isinf(f),"explicit infinity succeeds");
    check(QString("abc").lastIndexOf("a",3)==-1,"lastIndexOf at end nonempty needle");
    check(QString("abc").lastIndexOf("",3)==3,"lastIndexOf at end empty needle");
    check(QString("abc").lastIndexOf("",4)==-1,"lastIndexOf beyond end empty needle");
    check(QString("abc").lastIndexOf("bc",2)==1,"lastIndexOf normal reverse search");
    check(QString("").lastIndexOf("",0)==0,"lastIndexOf empty string at zero");
    check(QString("abc").lastIndexOf("a",-4)==-1,"lastIndexOf negative out-of-range");
    bad=original; bad.replace(other,"X",ci); check(bad==original,"different surrogate needle is not replaced");
    bad.replace(original,"X",ci); check(bad==QString("X"),"identical surrogate needle is replaced");
    check(QStringList{original}.filter(other,ci).empty(),"filter distinguishes surrogates");
    QString dotted(L"\x0130");
    check(!QStringList{dotted}.contains(QString(L"i\x0307"),ci),"multi-character dotted I fold excluded");
    check(QStringList{QString(L"\x212A")}.contains("k",ci),"Kelvin sign common fold");
    check(QStringList{QString(L"\x1E9E")}.contains(sharp,ci),"capital sharp s simple fold");
    check(QStringList{QString(L"\x1F88")}.contains(QString(L"\x1F80"),ci),"Greek simple fold");
    replacement=QString(L"\x1E9E"); replacement.replace(sharp,"X",ci);
    check(replacement==QString("X"),"replacement uses simple fold");
    // Qt 5.15 uses Unicode 13 simple folding in this public API too.
    check(sharp.toCaseFolded()==sharp,"Qt 5.15 simple case folding API");
    std::printf("%d checks, %d failures\n",checks,failures);
    return failures ? 1 : 0;
}
