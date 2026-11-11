#include "qstring_nx_cb.h"
#include <fstream>
#include <cstdint>
#include <cstdio>
#include <chrono>
template<class T> T read(std::ifstream& in) {T v;in.read(reinterpret_cast<char*>(&v),sizeof(v));return v;}
QString string(std::ifstream& in) {
    const int n=read<int>(in);if(n<0)return QString();
    std::wstring s(n,L'\0');if(n)in.read(reinterpret_cast<char*>(&s[0]),n*2);return QString(s);
}
int main(int argc,char**argv) {
    if(argc!=3)return 2;
    std::ifstream in(argv[1],std::ios::binary);std::ofstream out(argv[2],std::ios::binary);
    const auto count=read<std::uint32_t>(in);
    for(std::uint32_t i=0;i<count;++i) {
        const auto op=read<unsigned char>(in);QString a=string(in);
        if(op==0) {
            QString b=string(in);const int from=read<int>(in);const auto cs=static_cast<Qt::CaseSensitivity>(read<int>(in));
            const int index=a.indexOf(b,from,cs);out.write(reinterpret_cast<const char*>(&index),4);out.put(a.contains(b,cs));
        } else {
            const int base=read<int>(in);bool ok;const int value=a.toInt(&ok,base);out.write(reinterpret_cast<const char*>(&value),4);out.put(ok);
        }
    }
    // Explicitly compile both reported Qt call forms.
    bool ok=false;if(!QString("AB").contains(QString("ab"),Qt::CaseInsensitive) || QString("7f").toInt(&ok,16)!=127 || !ok)return 3;
    // The adversarial pattern used by the review; consume results on every run.
    const QString haystack(std::wstring(500000,L'a'));
    const QString needle(std::wstring(8191,L'a')+L'b');volatile int sink=0;
    auto start=std::chrono::steady_clock::now();
    for(int i=0;i<20;++i)sink+=haystack.indexOf(needle);
    const double elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/20;
    std::printf("search_ms=%.6f sink=%d\n",elapsed,int(sink));
    // A repeated matching prefix must also find a later match.
    if((haystack+needle).indexOf(needle)!=500000 || haystack.contains(needle))return 4;
    return in && out && sink==-20?0:5;
}
