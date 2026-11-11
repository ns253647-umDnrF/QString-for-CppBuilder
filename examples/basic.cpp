#include "qstring_nx_cb.h"
#include <cstdio>
int main() {
    const QString text=QString::fromUtf8("Hello, C++Builder");
    const QString value=QString("%1: %2").arg(text).arg(42);
    std::puts(value.toUtf8().c_str());
    return value=="Hello, C++Builder: 42" ? 0 : 1;
}
