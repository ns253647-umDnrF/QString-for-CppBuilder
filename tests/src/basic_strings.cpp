#include "qstring_nx_cb.h"
#include <cstdio>
#include <climits>
static int checks=0, failures=0;
static void check(bool value, const char* name) {
   ++checks;
   if(!value) { ++failures; std::printf("FAIL: %s\n",name); }
}
static void equal(const QString& value, const QString& expected, const char* name) { check(value==expected,name); }
int main() {
   const Qt::CaseSensitivity ci=Qt::CaseInsensitive;
   const QString sigma(L"\x03C2"), normalSigma(L"\x03C3");
   QStringList list{sigma,QString("ABC"),QString(L"Stra\x00DF" L"e")};
   check(list.contains(normalSigma,ci),"sigma contains");
   check(!list.contains(normalSigma),"case-sensitive contains preserved");
   check(!list.contains("STRASSE",ci),"Qt comparison does not expand sharp s");
   check(list.filter(normalSigma,ci).size()==1,"sigma filter");
   check(list.filter("ASSE",ci).empty(),"Qt filter does not expand sharp s");
   QStringList ordered{QString(L"\x03C2" L"z"),QString(L"\x03C3" L"a")};
   ordered.sort(ci); equal(ordered[0],QString(L"\x03C3" L"a"),"folded sort");
   QString s=sigma; equal(s.replace(normalSigma,"X",ci),"X","sigma replace");
   s=QString(L"a\x00DF" L"b\x00DF" L"c");
   equal(s.replace("SS","!",ci),QString(L"a\x00DF" L"b\x00DF" L"c"),"Qt replacement does not expand haystack");
   s="aSSbss"; equal(s.replace(QString(L"\x00DF"),"!",ci),"aSSbss","Qt replacement does not expand needle");
   s=QString(L"\x00DFs"); equal(s.replace("s","!",ci),QString(L"\x00DF!"),"no partial expansion replacement");
   s=QString::fromStdU32String(U"\U00010400x\U00010428");
   equal(s.replace(QString::fromStdU32String(U"\U00010428"),"!",ci),"!x!","supplementary folding positions");
   s="ab"; equal(s.replace("","-",ci),"-a-b-","empty needle");
   s="aaa"; equal(s.replace("aa","x"),"xa","non-overlapping replacement");
   s="a"; equal(s.replace("a",s),"a","self replacement");
   s="a"; equal(s.replace(s,"x"),"x","self needle");
   s=QString::fromLatin1("a\0B",3);
   equal(s.replace("b","x",ci),QString::fromLatin1("a\0x",3),"embedded NUL replacement");
   QStringList alias{"a","a"}; alias.replaceInStrings(alias[0],"b");
   equal(alias.join(","),"b,b","aliased before");
   alias={"ab","a"}; alias.replaceInStrings("a",alias[0]);
   equal(alias.join(","),"abb,ab","after snapshot survives mutation");
   alias={"a","b","a"}; alias.replaceInStrings(alias[0],alias[1]);
   equal(alias.join(","),"b,b,b","both arguments aliased");
   s="a"; equal(s.insert(3,"b"),"a  b","insert padding");
   s="ab"; equal(s.insert(4,s),"ab  ab","insert self after end");
   s="ab"; equal(s.insert(1,s),"aabb","insert self inside");
   s="a"; equal(s.insert(-1,"b"),"a","negative insertion ignored");
   s="a"; equal(s.insert(3,""),"a","empty insertion ignored");
   s="a"; bool overflow=false;
   try { s.insert(INT_MAX,"b"); } catch(const std::length_error&) { overflow=true; }
   check(overflow && s==QString("a"),"insert overflow checked before mutation");
   check(QString().isNull(),"default null");
   check(QString(static_cast<const char*>(nullptr)).isNull(),"null UTF8 pointer");
   check(QString(static_cast<const wchar_t*>(nullptr)).isNull(),"null UTF16 pointer");
   check(!QString("").isNull() && QString("").isEmpty(),"empty UTF8 non-null");
   check(!QString(L"").isNull(),"empty UTF16 non-null");
   check(!QString(std::string()).isNull(),"empty std string non-null");
   check(!QString::fromUtf8("",0).isNull(),"empty fromUtf8");
   check(QString::fromUtf8(nullptr).isNull(),"null fromUtf8");
   check(!QString::fromLatin1("",0).isNull(),"empty Latin1");
   s=""; QString copy=s; check(!copy.isNull(),"copy empty state");
   QString moved=std::move(copy); check(!moved.isNull(),"move empty state");
   s=QString(); check(s.isNull(),"move assign null");
   s=moved; check(!s.isNull(),"copy assign empty");
   s.clear(); check(s.isNull(),"clear restores null");
   s.append(""); check(!s.isNull(),"append empty makes non-null");
   s="a"; s.remove(0,1); check(!s.isNull() && s.isEmpty(),"remove all leaves empty");
   s="a"; s.replace("a",""); check(!s.isNull() && s.isEmpty(),"replace all leaves empty");
   s="a"; s.chop(1); check(!s.isNull() && s.isEmpty(),"chop all leaves empty");
   s="a"; s.truncate(0); check(!s.isNull() && s.isEmpty(),"truncate leaves empty");
   s=QString(); s.resize(0); check(!s.isNull(),"resize materializes empty");
   check(QString().toCaseFolded().isNull(),"fold preserves null");
   check(!QString("").toCaseFolded().isNull(),"fold preserves empty");
   check(QString().trimmed().isNull(),"trim preserves null");
   check(!QString(" ").trimmed().isNull(),"trim to empty non-null");
   check(!QString("").mid(0).isNull(),"mid empty non-null");
   check(!QString("").repeated(2).isNull(),"repeat empty non-null");
   check(QString()==QString(""),"null and empty compare equal");
   check(std::hash<QString>()(QString())==std::hash<QString>()(QString("")),"null and empty same hash");
   s="abc"; copy=s; s[0]=QChar('X'); equal(copy,"abc","QCharRef COW regression");
   equal(s,"Xbc","QCharRef assignment");
   equal(QString::number(1.25,'f',2),"1.25","number regression");
   check(Qt::Alignment(Qt::AlignLeft).testFlag(Qt::AlignLeft),"QFlags regression");
   std::printf("%d checks, %d failures\n",checks,failures);
   return failures ? 1 : 0;
}
