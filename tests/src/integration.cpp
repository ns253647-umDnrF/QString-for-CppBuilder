#include <Windows.h>
#include "qstring_nx_cb.h"
#include <cstdio>
#include <cstdint>
#include <iterator>
#include <process.h>
typedef bool (*NullCompare)(QChar,std::nullptr_t);
extern NullCompare release_address0();extern NullCompare release_address1();
extern NullCompare release_address2();extern NullCompare release_address3();
extern NullCompare release_address4();extern NullCompare release_address5();
extern QString release_value0();extern QString release_value1();extern QString release_value2();
extern QString release_value3();extern QString release_value4();extern QString release_value5();
static unsigned checks=0,failures=0;
static void check(bool value) { ++checks;if(!value)++failures; }
static unsigned rng=51521113;
static unsigned next() { rng=rng*1664525u+1013904223u;return rng; }
static std::wstring token() {
    const wchar_t units[]={0,L'A',L'b',L' ',wchar_t(0x00df),wchar_t(0xd801),wchar_t(0xdc00),wchar_t(0xffff)};
    std::wstring s;const unsigned n=next()%12;
    for(unsigned i=0;i<n;++i)s.push_back(units[(next()>>16)%8]);return s;
}
static bool same(const QStringList& values,const std::vector<std::wstring>& expected) {
    if(values.size()!=expected.size())return false;
    for(std::size_t i=0;i<expected.size();++i)if(values.at(i).toStdWString()!=expected[i])return false;
    return true;
}
static void containers() {
    QStringList a,b;std::vector<std::wstring> x,y;
    for(unsigned step=0;step<10000;++step) {
        const unsigned op=(next()>>16)%16;std::wstring text=token();
        const std::size_t at=x.empty()?0:next()%(x.size()+1);
        switch(op) {
        case 0:a.push_back(QString(text));x.push_back(text);break;
        case 1:if(!x.empty()){a.pop_back();x.pop_back();}break;
        case 2:{auto& ref=a.emplace_back(text);x.emplace_back(text);check(&ref==&a.back());break;}
        case 3:{auto it=a.insert(a.cbegin()+at,QString(text));x.insert(x.begin()+at,text);check(it==a.begin()+at);break;}
        case 4:{unsigned n=next()%4;a.insert(a.cbegin()+at,n,QString(text));x.insert(x.begin()+at,n,text);break;}
        case 5:if(at<x.size()){auto it=a.erase(a.cbegin()+at);x.erase(x.begin()+at);check(it==a.begin()+at);}break;
        case 6:if(at<x.size()){a.erase(a.cbegin()+at,a.cend());x.erase(x.begin()+at,x.end());}break;
        case 7:{const unsigned n=next()%24;const auto old=x.size();a.resize(n);x.resize(n);if(n>old)check(a.back().isNull());break;}
        case 8:{const unsigned n=next()%24;a.resize(n,QString(text));x.resize(n,text);break;}
        case 9:a.swap(b);x.swap(y);break;
        case 10:{QStringList c(a.begin(),a.end());check(same(c,x));b=c;y=x;break;}
        case 11:{a.reserve(64);check(a.capacity()>=64);a.shrink_to_fit();break;}
        case 12:{a.insert(a.cbegin()+at,{QString(text),QString("tail")});x.insert(x.begin()+at,{text,L"tail"});break;}
        case 13:{QStringList c=a;auto copy=c.toStdVector();check(copy.size()==x.size());
            auto moved=std::move(c).toStdVector();check(moved.size()==x.size());
            for(std::size_t i=0;i<x.size();++i)check(moved[i].toStdWString()==x[i]);c.clear();break;}
        case 14:{auto bytes=a.toStdStringVector();check(bytes.size()==x.size());
            for(std::size_t i=0;i<x.size();++i)check(bytes[i]==QString(x[i]).toUtf8());break;}
        case 15:{QStringList c=a;if(!c.empty() && !c.front().isEmpty())c.front()[0]=QChar('!');check(same(a,x));break;}
        }
        check(same(a,x) && same(b,y));
        const QStringList& view=a;
        check(std::distance(view.cbegin(),view.cend())==static_cast<std::ptrdiff_t>(x.size()));
        check(std::distance(view.crbegin(),view.crend())==static_cast<std::ptrdiff_t>(x.size()));
        if(!x.empty())check(view.data()[0].toStdWString()==x.front() && view.rbegin()->toStdWString()==x.back());
        if(x.size()>100){a.clear();x.clear();}
    }
}
struct Worker { const QString* source;volatile LONG failures;unsigned checks; };
static unsigned __stdcall worker(void* param) {
    Worker& w=*static_cast<Worker*>(param);
    try {
        for(unsigned i=0;i<1000;++i) {
            QString copy=*w.source;copy[0]=QChar('x');copy.append(copy);
            const std::wstring expected={L'x',L'b',L'c',0,L't',L'a',L'i',L'l'};
            const QString value(expected);
            if(copy!=value+value || w.source->at(0)!=QChar('a'))InterlockedIncrement(&w.failures);
            if(QString::number(0.1,'g',-128)!="0.1" || QString::asprintf("%.2f",1.25)!="1.25")InterlockedIncrement(&w.failures);
            if(QChar(0x0391).toLower()!=QChar(0x03b1))InterlockedIncrement(&w.failures);
            w.checks+=3;
        }
    } catch(...) {InterlockedIncrement(&w.failures);}
    return 0;
}
int main() {
    std::puts("entered main");std::fflush(stdout);
    try {
    NullCompare funcs[]={release_address0(),release_address1(),release_address2(),release_address3(),release_address4(),release_address5()};
    for(auto fn:funcs)check(fn==funcs[0] && fn(QChar(),nullptr) && !fn(QChar('x'),nullptr));
    const QString values[]={release_value0(),release_value1(),release_value2(),release_value3(),release_value4(),release_value5()};
    for(const auto& s:values)check(s=="a:1.25:0.1");
    std::puts("linked values checked");std::fflush(stdout);
    containers();
    std::puts("containers checked");std::fflush(stdout);
    const QString shared(std::wstring({L'a',L'b',L'c',0,L't',L'a',L'i',L'l'}));
    Worker workers[8];HANDLE threads[8];
    System::IsMultiThread=true;
    for(unsigned i=0;i<8;++i) {
        workers[i]={&shared,0,0};unsigned threadId=0;
        threads[i]=reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(_beginthreadex(nullptr,0,worker,&workers[i],0,&threadId)));
        if(!threads[i])return 3;
    }
    const DWORD waited=WaitForMultipleObjects(8,threads,TRUE,60000);if(waited!=WAIT_OBJECT_0)return 4;
    std::puts("threads finished");std::fflush(stdout);
    for(unsigned i=0;i<8;++i){checks+=workers[i].checks;failures+=workers[i].failures;CloseHandle(threads[i]);}
    check(shared.toStdWString()==std::wstring({L'a',L'b',L'c',0,L't',L'a',L'i',L'l'}));
    std::printf("%u checks, %u failures\n",checks,failures);
    return failures?1:0;
    } catch(const std::exception& e) {std::printf("std exception: %s\n",e.what());return 90;}
      catch(...) {std::puts("unknown exception");return 91;}
}
