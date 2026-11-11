#include "qstring_nx_cb.h"
#include <fstream>
#include <cstdint>
#include <cstdio>
template<class T> T read(std::ifstream& in) {T v;in.read(reinterpret_cast<char*>(&v),sizeof(v));return v;}
static void emit(std::ofstream& out,const QString& s) {
    const auto w=s.toStdWString();std::uint32_t n=w.size();out.write(reinterpret_cast<const char*>(&n),4);out.put(s.isNull());out.write(reinterpret_cast<const char*>(w.data()),2*n);
}
int main(int argc,char**argv) {
    if(argc!=3)return 2;
    std::ifstream in(argv[1],std::ios::binary);std::ofstream out(argv[2],std::ios::binary);
    const auto count=read<std::uint32_t>(in);
    for(std::uint32_t i=0;i<count;++i) {
        int n=read<int>(in);std::wstring w(n<0?0:n,L'\0');if(n>0)in.read(reinterpret_cast<char*>(&w[0]),n*2);
        QString s=n<0?QString():QString(w);const auto index=read<int>(in);const auto code=read<unsigned short>(in);const auto mode=read<unsigned char>(in);
        auto ref=s[index];const QString shared=s;
        const unsigned short before=QChar(ref).unicode();out.write(reinterpret_cast<const char*>(&before),2);emit(out,s);
        if(mode==1)s.clear();
        else if(mode==2)s.truncate(1);
        if(mode==3)ref=s[index+10];else ref=QChar(code);
        emit(out,s);emit(out,shared);
    }
    QString s("ab");s[3]='x';if(s!="ab x")return 3;
    QString empty;auto beyond=empty[5];if(!empty.isNull() || !QChar(beyond).isNull())return 4;
    beyond=0;if(empty.isNull() || empty.length()!=6)return 5;
    bool threw=false;try{empty[-1]='x';}catch(const std::out_of_range&){threw=true;}if(!threw)return 6;
    threw=false;try{empty[(std::numeric_limits<int>::max)()]='x';}catch(const std::length_error&){threw=true;}if(!threw)return 7;
    return in && out?0:8;
}
