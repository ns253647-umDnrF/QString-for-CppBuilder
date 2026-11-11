#include "qstring_nx_cb.h"
#include <type_traits>
int main() {
    QString s("aBab");
    bool ok=false;
    if(s.indexOf(QString("AB"),0,Qt::CaseInsensitive)!=0) return 1;
    if(s.lastIndexOf(QString("AB"),-1,Qt::CaseInsensitive)!=2) return 2;
    if(!s.contains(QString("AB"),Qt::CaseInsensitive)) return 3;
    if(!s.startsWith(QString("AB"),Qt::CaseInsensitive)) return 4;
    if(!s.endsWith(QString("AB"),Qt::CaseInsensitive)) return 5;
    if(s.count(QString("AB"),Qt::CaseInsensitive)!=2) return 6;
    if(!s.split(QString("AB"),Qt::SkipEmptyParts,Qt::CaseInsensitive).empty()) return 7;
    if(QString("7f").toInt(&ok,16)!=127 || !ok) return 8;
    QString grown("ab"); grown[3]='x'; if(grown!="ab x") return 9;
    static_assert(std::is_same<decltype(s.split(QString("x"))),std::vector<QString>>::value,
                  "Adapter split returns std::vector<QString>");
    return 0;
}
