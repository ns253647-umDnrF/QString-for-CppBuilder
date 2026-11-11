#include "qchar_nx_cb.h"
#include "qstring_nx_cb.h"
#include <type_traits>
#ifdef CHECK_CHARREF
bool charRefAccess() { QString s("ab"); s[0]='x'; return s[0]==QChar('x'); }
#endif
#ifdef CHECK_NUM64
QString integer64() { return QString::number(1234567890123456789LL); }
#endif
#ifdef CHECK_MAIN
static_assert(std::is_same<decltype(Qt::AlignLeft | Qt::AlignTop), Qt::Alignment>::value,"typed enum OR");
static_assert(std::is_same<decltype(Qt::AlignLeft | Qt::Alignment(Qt::AlignTop)), Qt::Alignment>::value,"typed enum/flags OR");
int main() {
   const bool passed=charRefAccess() && integer64()==QString("1234567890123456789");
   std::printf("Compile/link smoke: %s\n",passed ? "PASS" : "FAIL");
   return passed ? 0 : 1;
}
#endif
