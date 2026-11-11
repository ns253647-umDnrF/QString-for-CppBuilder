#include "qstring_nx_cb.h"
#include <System.Math.hpp>
#include <fstream>
#include <cstdint>
#include <cstring>
struct Case { unsigned char op;std::uint64_t bits;int precision;std::string text; };
static void emit(std::ofstream& out,const std::string& text) {
    std::uint32_t n=text.size();out.write(reinterpret_cast<const char*>(&n),4);out.write(text.data(),n);
}
int main(int argc,char**argv) {
    if(argc!=3)return 2;
    std::ifstream in(argv[1],std::ios::binary);std::ofstream out(argv[2],std::ios::binary);
    std::uint32_t count;in.read(reinterpret_cast<char*>(&count),4);std::vector<Case> cases(count);
    for(auto& c:cases) {
        in.read(reinterpret_cast<char*>(&c.op),1);in.read(reinterpret_cast<char*>(&c.bits),8);
        in.read(reinterpret_cast<char*>(&c.precision),4);std::uint32_t n;in.read(reinterpret_cast<char*>(&n),4);
        c.text.resize(n);if(n)in.read(&c.text[0],n);
    }
    const System::Math::TRoundingMode modes[]={System::Math::rmNearest,System::Math::rmDown,System::Math::rmUp,System::Math::rmTruncate};
    const System::Math::TRoundingMode original=System::Math::GetRoundMode();
    for(auto mode:modes) {
        System::Math::SetRoundMode(mode);
        if(System::Math::GetRoundMode()!=mode)return 3;
        for(const auto& c:cases) {
            double value;std::memcpy(&value,&c.bits,8);
            if(c.op==0)emit(out,QString::number(value,c.text[0],c.precision).toStdString());
            else if(c.op==1)emit(out,QString::asprintf(c.text.c_str(),value).toStdString());
            else {
                bool ok=false;std::uint64_t bits=0;
                if(c.op==2) {double v=QString(c.text).toDouble(&ok);std::memcpy(&bits,&v,8);}
                else {float v=QString(c.text).toFloat(&ok);std::uint32_t b;std::memcpy(&b,&v,4);bits=b;}
                emit(out,std::string(reinterpret_cast<const char*>(&bits),8)+std::string(1,char(ok)));
            }
            if(System::Math::GetRoundMode()!=mode)return 4;
        }
    }
    System::Math::SetRoundMode(original);
    return in && out?0:5;
}
