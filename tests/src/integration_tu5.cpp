#include <Windows.h>
#include "qtnamespace_nx_cb.h"
#include "qstring_nx_cb.h"
#include "qchar_nx_cb.h"
typedef bool (*NullCompare)(QChar,std::nullptr_t);
static const QString initial=QString::asprintf("%.2f",1.25);
NullCompare release_address5() {return static_cast<NullCompare>(&operator==);}
QString release_value5() {return QString(QChar('A').toLower())+":"+initial+":"+QString::number(0.1,'g',-128);}
