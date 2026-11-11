#include "qstring_nx_cb.h"
#include <cstdio>
#include <type_traits>
#include <climits>
static QString& callVsprintf(QString& s,const char* format,...) {
   va_list args; va_start(args,format);
   try { QString& result=s.vsprintf(format,args); va_end(args); return result; }
   catch(...) { va_end(args); throw; }
}
static QString callVasprintf(const char* format,...) {
   va_list args; va_start(args,format);
   try { QString result=QString::vasprintf(format,args); va_end(args); return result; }
   catch(...) { va_end(args); throw; }
}
static int tests=0, failures=0;
static void check(bool value,const char* label) {
   ++tests;
   if(!value) { ++failures; std::printf("FAIL: %s\n",label); }
}
static void equal(const QString& actual,const QString& expected,const char* label) {
   ++tests;
   if(actual!=expected) {
      ++failures;
      std::printf("FAIL: %s (actual UTF8='%s', expected='%s')\n",label,actual.toUtf8().c_str(),expected.toUtf8().c_str());
   }
}
int main() {
   const QString sharp(L"\x00DF"), dotted(L"\x0130");
   equal(sharp.toCaseFolded(),sharp,"Qt 5.15 toCaseFolded uses simple folding");
   const QString raw=QString::fromStdU16String(std::u16string(1,static_cast<char16_t>(0xD800)));
   check(raw.toCaseFolded().toStdU16String()==raw.toStdU16String(),"toCaseFolded preserves unpaired surrogate");
   equal(QString(L"\x1C89").toCaseFolded(),QString(L"\x1C89"),"folding uses Qt 5.15 Unicode version");
   equal(sharp.toUpper(),"SS","uppercase expansion");
   equal(dotted.toLower(),QString(L"i\x0307"),"lowercase expansion");
   equal(QString(L"\xFB03").toUpper(),"FFI","uppercase ligature expansion");
   equal(QString::fromStdU32String(U"\U00010400").toLower(),QString::fromStdU32String(U"\U00010428"),"supplementary lowercase");
   equal(QString::number(1.25),"1.25","default number format g");
   equal(QString::number(255,16),"ff","integer base 16 overload");
   equal(QString::number(2.5,'f',0),"3","fixed halfway rounding");
   equal(QString::number(-2.5,'f',0),"-3","negative halfway rounding");
   equal(QString::number(2.5,'e',0),"3e+00","scientific halfway rounding");
   equal(QString::number(2.5,'g',1),"3","significant halfway rounding");
   QString s="old"; s.sprintf("%d",42);
   equal(s,"42","sprintf mutates receiver");
   s=QString(); s.replace("",""); check(s.isNull(),"no-op replace preserves null");
   const auto parts=QString("a").split("");
   check(parts.size()==3 && !parts[0].isNull() && !parts[2].isNull(),"empty separator split parts are non-null");
   check(QString("abc").mid(-1,1).isNull(),"mid negative empty intersection is null in Qt 5.15");
   check(QString("abc").mid(-2,1).isNull(),"mid disjoint negative range is null");
   check(QString("abc").mid(4).isNull(),"mid beyond end is null");
   check(std::is_same<decltype(Qt::AlignLeft | Qt::AlignTop),Qt::Alignment>::value,"enum OR preserves QFlags type");
   check(std::is_same<decltype(Qt::AlignLeft | Qt::Alignment(Qt::AlignTop)),Qt::Alignment>::value,"enum OR flags preserves QFlags type");
   s="abc";
   bool resized=false;
   try { s.resize(-1); resized=s.isEmpty(); } catch(...) {}
   check(resized,"negative resize means zero");
   s="a"; s.append('x'); equal(s,"ax","append char is not decimal formatting");
   s="a"; s+='x'; equal(s,"ax","operator+= char is not decimal formatting");
   s='x'; equal(s,"x","assignment char is not decimal formatting");
   s="x"; s.prepend('a'); equal(s,"ax","prepend char");
   equal(QString("a")+'x',"ax","concatenation char");
   s=static_cast<char>(0xFF); check(s.length()==1 && s.at(0).unicode()==0xFF,"signed char is Latin1");
   s='\0'; check(s.length()==1 && s.at(0).isNull(),"assignment NUL char");
   s=L'\x0416'; check(s.at(0).unicode()==0x0016,"wide char assignment selects Qt ASCII overload");
   s=u'\x0416'; check(s.at(0).unicode()==0x0016,"UTF16 char assignment selects Qt ASCII overload");
   s=123; equal(s,"{","numeric assignment selects Qt ASCII overload");
   s="ab"; QString copy=s;
   s[0]='x'; check(s[0]==QChar('x') && QChar('x')==s[0],"QCharRef equality both directions");
   check(s[0]=='x' && 'x'==s[0],"QCharRef char equality");
   check(s[1] < s[0] && s[0]>s[1] && s[0]!=s[1],"QCharRef ordering");
   check(s[0]>=QChar('x') && QChar('x')<=s[0],"QCharRef inclusive ordering");
   s[1]=s[0]; equal(s,"xx","QCharRef assignment from proxy");
   equal(copy,"ab","QCharRef still detaches shared buffer");
   s[0]=L'\x0416'; check(s[0]==L'\x0416' && L'\x0416'==s[0],"QCharRef wide equality no narrowing");
   s[0]=u'\x0416'; check(s[0]==u'\x0416',"QCharRef UTF16 assignment");
   equal(QString::number(LLONG_MIN),"-9223372036854775808","minimum signed 64-bit");
   equal(QString::number(ULLONG_MAX),"18446744073709551615","maximum unsigned 64-bit");
   equal(QString::number(-1,16),"ffffffffffffffff","nondecimal negative uses unsigned 64-bit");
   equal(QString::number(35u,36),"z","base36");
   equal(QString::number(8L,2),"1000","long base2");
   equal(QString::number(255UL,16),"ff","unsigned long base16");
   equal(QString::number(0,8),"0","zero octal");
   equal(QString::number(1.25,'f',1),"1.3","exact fractional halfway");
   equal(QString::number(9.5,'g',1),"1e+01","rounding carries into exponent");
   equal(QString::number(2.49,'f',0),"2","value below halfway");
   s="old"; check(&s.sprintf("%d",42)==&s,"sprintf returns receiver reference");
   check(&callVsprintf(s,"%s-%d","x",7)==&s && s==QString("x-7"),"vsprintf mutates receiver");
   equal(QString::asprintf("%s-%d","x",7),"x-7","static asprintf");
   equal(callVasprintf("%d",42),"42","static vasprintf");
   const std::string longText(700,'x');
   equal(QString::asprintf("%s",longText.c_str()),QString(longText),"printf buffer growth");
   check(raw.toUpper().toStdU16String()==raw.toStdU16String(),"uppercase preserves surrogate");
   check(raw.toLower().toStdU16String()==raw.toStdU16String(),"lowercase preserves surrogate");
   equal(QString(L"\x1E9E").toCaseFolded(),sharp,"simple folding capital sharp s");
   equal(dotted.toCaseFolded(),dotted,"simple folding dotted I");
   check(QString().toUpper().isNull() && QString().toLower().isNull(),"case conversion preserves null");
   check(!QString("").toUpper().isNull() && !QString("").toLower().isNull(),"case conversion preserves empty");
   auto flags=Qt::AlignLeft | Qt::AlignTop; check(flags.testFlag(Qt::AlignLeft) && flags.testFlag(Qt::AlignTop),"typed alignment flags");
   check((Qt::LeftButton | Qt::RightButton).testFlag(Qt::RightButton),"typed mouse flags");
   check((Qt::ShiftModifier | Qt::ControlModifier).testFlag(Qt::ControlModifier),"typed keyboard flags");
   check((Qt::WindowMinimized | Qt::WindowActive).testFlag(Qt::WindowActive),"typed window flags");
   std::printf("%d tests, %d failures\n",tests,failures);
   return failures ? 1 : 0;
}
