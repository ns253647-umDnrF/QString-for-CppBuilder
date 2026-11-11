#include "qstring_nx_cb.h"
#include <chrono>
#include <cstdio>
#include <functional>
static bool measure(const char* name,const std::function<int()>& operation,int expected,int runs) {
    volatile long long sum=0;bool valid=true;auto start=std::chrono::steady_clock::now();
    for(int i=0;i<runs;++i){const int result=operation();valid=valid && result==expected;sum+=result;}
    const double elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/runs;
    std::printf("%s ms=%.9f result=%d valid=%d\n",name,elapsed,int(sum/runs),int(valid));return valid;
}
int main() {
    const QString haystack(std::wstring(500000,L'a'));
    const QString reverseNeedle(std::wstring(8191,L'a')+L'b');
    const QString dense(std::wstring(100000,L'a')),needle(std::wstring(2048,L'a'));
    std::wstring longText(2000000,L'a');longText[0]=L'A';longText[1]=L'b';longText.back()=L'B';
    const QString early(longText),shortNeedle("ab");
    const int normalRuns=10,earlyRuns=1000;
    bool ok=true;
    ok&=measure("reverse_missing",[&](){return haystack.lastIndexOf(reverseNeedle);},-1,normalRuns);
    ok&=measure("dense_count",[&](){return dense.count(needle);},100000-2048+1,normalRuns);
    ok&=measure("insensitive_early",[&](){return early.indexOf(shortNeedle,0,Qt::CaseInsensitive);},0,earlyRuns);
    ok&=measure("insensitive_late_start",[&](){return early.indexOf(shortNeedle,1999998,Qt::CaseInsensitive);},1999998,earlyRuns);
    const QString denseUpper(std::wstring(2048,L'A'));
    ok&=measure("dense_count_insensitive",[&](){return dense.count(denseUpper,Qt::CaseInsensitive);},100000-2048+1,normalRuns);
    const QString reverseMatch(std::wstring(500000,L'a')+L"AB");
    ok&=measure("reverse_near_end",[&](){return reverseMatch.lastIndexOf(shortNeedle,-1,Qt::CaseInsensitive);},500000,earlyRuns);
    const auto parts=dense.split(needle,Qt::KeepEmptyParts,Qt::CaseSensitive);
    if(parts.size()!=49 || parts.back().length()!=1696)ok=false;
    return ok?0:1;
}
