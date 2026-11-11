#include "qstring_nx_cb.h"
#include <fstream>
#include <cstdint>
template<class T> T read(std::ifstream& in) {T v;in.read(reinterpret_cast<char*>(&v),sizeof(v));return v;}
QString string(std::ifstream& in) {
    const int n=read<int>(in);if(n<0)return QString();
    std::wstring s(n,L'\0');if(n)in.read(reinterpret_cast<char*>(&s[0]),n*2);return QString(s);
}
static void emit(std::ofstream& out,const QString& s) {
    auto w=s.toStdWString();std::uint32_t n=w.size();out.write(reinterpret_cast<const char*>(&n),4);out.put(s.isNull());out.write(reinterpret_cast<const char*>(w.data()),n*2);
}
int main(int argc,char**argv) {
    if(argc!=3)return 2;
    std::ifstream in(argv[1],std::ios::binary);std::ofstream out(argv[2],std::ios::binary);
    const auto count=read<std::uint32_t>(in);
    for(std::uint32_t i=0;i<count;++i) {
        const auto op=read<unsigned char>(in);const QString a=string(in),b=string(in);
        const int from=read<int>(in);const auto cs=static_cast<Qt::CaseSensitivity>(read<int>(in));
        if(op==0 || op==1) {
            const int v=op==0?a.indexOf(b,from,cs):a.lastIndexOf(b,from,cs);out.write(reinterpret_cast<const char*>(&v),4);
        } else if(op==2) {
            const int n=a.count(b,cs);out.write(reinterpret_cast<const char*>(&n),4);out.put(a.startsWith(b,cs));out.put(a.endsWith(b,cs));
        } else {
            const auto parts=a.split(b,static_cast<Qt::SplitBehavior>(from),cs);std::uint32_t n=parts.size();out.write(reinterpret_cast<const char*>(&n),4);
            for(const auto& s:parts)emit(out,s);
        }
    }
    if(QString("aBab").lastIndexOf(QString("AB"),-1,Qt::CaseInsensitive)!=2)return 3;
    if(!QString("Ab").startsWith(QString("a"),Qt::CaseInsensitive))return 4;
    if(QString("aBaB").split(QString("AB"),Qt::SkipEmptyParts,Qt::CaseInsensitive).size()!=0)return 5;
    if(QString("aaaaa").count(QString("aaa"))!=3)return 6;
    return in && out?0:7;
}
