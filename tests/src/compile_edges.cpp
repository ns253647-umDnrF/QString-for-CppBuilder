// These valid Qt 5.15 construction forms must compile on both Builder targets.
#include "qchar_nx_cb.h"

#if defined(CHECK_QCHAR_INT)
QChar fromInt(0x0416);
#elif defined(CHECK_QCHAR_USHORT)
unsigned short codeUnit = 0x0416;
QChar fromUShort(codeUnit);
#else
#error Define CHECK_QCHAR_INT or CHECK_QCHAR_USHORT
#endif
