#include "qstring_nx_cb.h"
#include <fstream>
#include <cstdint>
#include <cstdio>
template<class T> T read(std::ifstream& in) {T v;in.read(reinterpret_cast<char*>(&v),sizeof(v));return v;}
int main(int argc,char**argv) {
    if(argc!=3)return 2;
    const int original=System::DefaultSystemCodePage;const UINT acp=GetACP();
    std::ofstream out(argv[2],std::ios::binary);
    for(int cp:{65001,1252,932,static_cast<int>(acp)}) {
        System::SetMultiByteConversionCodePage(cp);
        if(GetACP()!=acp || System::DefaultSystemCodePage!=cp)return 3;
        std::ifstream in(argv[1],std::ios::binary);const auto count=read<std::uint32_t>(in);
        for(std::uint32_t i=0;i<count;++i) {
            const auto op=read<unsigned char>(in);int n=read<int>(in);
            if(op==0) {
                std::string data(n<0?0:n,'\0');if(n>0)in.read(&data[0],n);
                const auto s=QString::fromLocal8Bit(n<0?nullptr:data.data(),n);
                const auto units=s.toStdWString();std::uint32_t bytes=units.size()*2;
                out.write(reinterpret_cast<const char*>(&bytes),4);out.write(reinterpret_cast<const char*>(units.data()),bytes);out.put(s.isNull());
                const auto zeroTerminated=QString::fromLocal8Bit(n<0?nullptr:data.c_str());
                const auto z=zeroTerminated.toStdWString();bytes=z.size()*2;
                out.write(reinterpret_cast<const char*>(&bytes),4);out.write(reinterpret_cast<const char*>(z.data()),bytes);out.put(zeroTerminated.isNull());
            } else {
                std::wstring text(n<0?0:n,L'\0');if(n>0)in.read(reinterpret_cast<char*>(&text[0]),n*2);
                const auto s=n<0?QString():QString(text);const auto bytes=s.toLocal8Bit();std::uint32_t size=bytes.size();
                out.write(reinterpret_cast<const char*>(&size),4);out.write(bytes.data(),size);
            }
            if(System::DefaultSystemCodePage!=cp || GetACP()!=acp)return 4;
        }
        if(!in)return 5;
        std::printf("RTL=%d ACP=%u preserved\n",cp,acp);
    }
    System::SetMultiByteConversionCodePage(original);return out?0:6;
}
