// Qt NX CB version 2026.11.11
#ifndef QT_NAMESPACE_NX_CB_H
#define QT_NAMESPACE_NX_CB_H

#include <type_traits>
#include <initializer_list>

// Enum and flag declarations are independent of QString.

namespace Qt 
{

// =============================================
// 1. Флаги и перечисления для Case Sensitivity
// =============================================

enum CaseSensitivity
{
   CaseInsensitive = 0,
   CaseSensitive = 1
};

// =============================================
// 2. Флаги для Split/Join операций
// =============================================

enum SplitBehavior
{
   KeepEmptyParts = 0,
   SkipEmptyParts = 1
};

enum SectionFlag
{
   SectionDefault = 0x00,
   SectionSkipEmpty = 0x01,
   SectionIncludeLeadingSep = 0x02,
   SectionIncludeTrailingSep = 0x04,
   SectionCaseInsensitiveSeps = 0x08
};

// =============================================
// 3. Флаги выравнивания (для текста, GUI)
// =============================================

enum AlignmentFlag
{
   AlignLeft = 0x0001,
   AlignRight = 0x0002,
   AlignHCenter = 0x0004,
   AlignJustify = 0x0008,
   AlignTop = 0x0020,
   AlignBottom = 0x0040,
   AlignVCenter = 0x0080,
   AlignBaseline = 0x0100,
   
   AlignCenter = AlignVCenter | AlignHCenter,
   AlignAbsolute = 0x0010
};

// =============================================
// 4. Модификаторы клавиш (для GUI обработки)
// =============================================

enum KeyboardModifier
{
   NoModifier = 0x00000000,
   ShiftModifier = 0x02000000,
   ControlModifier = 0x04000000,
   AltModifier = 0x08000000,
   MetaModifier = 0x10000000,
   KeypadModifier = 0x20000000,
   GroupSwitchModifier = 0x40000000
};

// =============================================
// 5. Флаги текстового взаимодействия
// =============================================

enum TextInteractionFlag
{
   NoTextInteraction = 0,
   TextSelectableByMouse = 1,
   TextSelectableByKeyboard = 2,
   LinksAccessibleByMouse = 4,
   LinksAccessibleByKeyboard = 8,
   TextEditable = 16,
   TextEditorInteraction = TextSelectableByMouse | TextSelectableByKeyboard | TextEditable,
   TextBrowserInteraction = TextSelectableByMouse | LinksAccessibleByMouse | LinksAccessibleByKeyboard
};

// =============================================
// 6. Ориентация (для GUI элементов)
// =============================================

enum Orientation
{
   Horizontal = 0x1,
   Vertical = 0x2
};

// =============================================
// 7. Перечисления для кодов клавиш
// =============================================

enum Key
{
   Key_0 = 0x30,
   Key_1 = 0x31,
   Key_2 = 0x32,
   Key_3 = 0x33,
   Key_4 = 0x34,
   Key_5 = 0x35,
   Key_6 = 0x36,
   Key_7 = 0x37,
   Key_8 = 0x38,
   Key_9 = 0x39,
   
   Key_A = 0x41,
   Key_B = 0x42,
   Key_C = 0x43,
   Key_D = 0x44,
   Key_E = 0x45,
   Key_F = 0x46,
   Key_G = 0x47,
   Key_H = 0x48,
   Key_I = 0x49,
   Key_J = 0x4a,
   Key_K = 0x4b,
   Key_L = 0x4c,
   Key_M = 0x4d,
   Key_N = 0x4e,
   Key_O = 0x4f,
   Key_P = 0x50,
   Key_Q = 0x51,
   Key_R = 0x52,
   Key_S = 0x53,
   Key_T = 0x54,
   Key_U = 0x55,
   Key_V = 0x56,
   Key_W = 0x57,
   Key_X = 0x58,
   Key_Y = 0x59,
   Key_Z = 0x5a,
   
   Key_Escape = 0x01000000,
   Key_Tab = 0x01000001,
   Key_Backtab = 0x01000002,
   Key_Backspace = 0x01000003,
   Key_Return = 0x01000004,
   Key_Enter = 0x01000005,
   Key_Insert = 0x01000006,
   Key_Delete = 0x01000007,
   Key_Pause = 0x01000008,
   Key_Print = 0x01000009,
   Key_SysReq = 0x0100000a,
   Key_Clear = 0x0100000b,
   Key_Home = 0x01000010,
   Key_End = 0x01000011,
   Key_Left = 0x01000012,
   Key_Up = 0x01000013,
   Key_Right = 0x01000014,
   Key_Down = 0x01000015,
   Key_PageUp = 0x01000016,
   Key_PageDown = 0x01000017,
   Key_Shift = 0x01000020,
   Key_Control = 0x01000021,
   Key_Meta = 0x01000022,
   Key_Alt = 0x01000023,
   Key_CapsLock = 0x01000024,
   Key_NumLock = 0x01000025,
   Key_ScrollLock = 0x01000026,
   Key_F1 = 0x01000030,
   Key_F2 = 0x01000031,
   Key_F3 = 0x01000032,
   Key_F4 = 0x01000033,
   Key_F5 = 0x01000034,
   Key_F6 = 0x01000035,
   Key_F7 = 0x01000036,
   Key_F8 = 0x01000037,
   Key_F9 = 0x01000038,
   Key_F10 = 0x01000039,
   Key_F11 = 0x0100003a,
   Key_F12 = 0x0100003b,
   
   Key_Space = 0x20,
   Key_Exclam = 0x21,      // !
   Key_QuoteDbl = 0x22,     // "
   Key_NumberSign = 0x23,   // #
   Key_Dollar = 0x24,      // $
   Key_Percent = 0x25,      // %
   Key_Ampersand = 0x26,    // &
   Key_Apostrophe = 0x27,   // '
   Key_ParenLeft = 0x28,    // (
   Key_ParenRight = 0x29,   // )
   Key_Asterisk = 0x2a,     // *
   Key_Plus = 0x2b,        // +
   Key_Comma = 0x2c,       // ,
   Key_Minus = 0x2d,       // -
   Key_Period = 0x2e,      // .
   Key_Slash = 0x2f,       // /
   Key_Colon = 0x3a,       // :
   Key_Semicolon = 0x3b,    // ;
   Key_Less = 0x3c,        // <
   Key_Equal = 0x3d,       // =
   Key_Greater = 0x3e,      // >
   Key_Question = 0x3f,     // ?
   Key_At = 0x40,         // @
   Key_BracketLeft = 0x5b,   // [
   Key_Backslash = 0x5c,    // backslash
   Key_BracketRight = 0x5d,  // ]
   Key_AsciiCircum = 0x5e,   // ^
   Key_Underscore = 0x5f,   // _
   Key_QuoteLeft = 0x60,    // `
   Key_BraceLeft = 0x7b,    // {
   Key_Bar = 0x7c,         // |
   Key_BraceRight = 0x7d,   // }
   Key_AsciiTilde = 0x7e    // ~
};

// =============================================
// 8. Флаги для MouseEvent
// =============================================

enum MouseButton
{
   NoButton = 0x00000000,
   LeftButton = 0x00000001,
   RightButton = 0x00000002,
   MiddleButton = 0x00000004,
   BackButton = 0x00000008,
   ForwardButton = 0x00000010,
   TaskButton = 0x00000020,
   ExtraButton4 = 0x00000040,
   ExtraButton5 = 0x00000080,
   ExtraButton6 = 0x00000100,
   ExtraButton7 = 0x00000200,
   ExtraButton8 = 0x00000400,
   ExtraButton9 = 0x00000800,
   ExtraButton10 = 0x00001000,
   ExtraButton11 = 0x00002000,
   ExtraButton12 = 0x00004000,
   ExtraButton13 = 0x00008000,
   ExtraButton14 = 0x00010000,
   ExtraButton15 = 0x00020000,
   ExtraButton16 = 0x00040000,
   ExtraButton17 = 0x00080000,
   ExtraButton18 = 0x00100000,
   ExtraButton19 = 0x00200000,
   ExtraButton20 = 0x00400000,
   ExtraButton21 = 0x00800000,
   ExtraButton22 = 0x01000000,
   ExtraButton23 = 0x02000000,
   ExtraButton24 = 0x04000000,
   AllButtons = 0x07ffffff
};

// =============================================
// 9. Флаги для состояния элемента
// =============================================

enum ItemFlag
{
   NoItemFlags = 0,
   ItemIsSelectable = 1,
   ItemIsEditable = 2,
   ItemIsDragEnabled = 4,
   ItemIsDropEnabled = 8,
   ItemIsUserCheckable = 16,
   ItemIsEnabled = 32,
   ItemIsAutoTristate = 64,
   ItemNeverHasChildren = 128,
   ItemIsUserTristate = 256
};

// =============================================
// 10. Флаги для работы с окнами
// =============================================

enum WindowType
{
   Widget = 0x00000000,
   Window = 0x00000001,
   Dialog = 0x00000002 | Window,
   Sheet = 0x00000004 | Window,
   Drawer = 0x00000006 | Window,
   Popup = 0x00000008 | Window,
   Tool = 0x0000000a | Window,
   ToolTip = 0x0000000c | Window,
   SplashScreen = 0x0000000e | Window,
   Desktop = 0x00000010 | Window,
   SubWindow = 0x00000012,
   ForeignWindow = 0x00000020 | Window,
   CoverWindow = 0x00000040 | Window
};

enum WindowState
{
   WindowNoState = 0x00000000,
   WindowMinimized = 0x00000001,
   WindowMaximized = 0x00000002,
   WindowFullScreen = 0x00000004,
   WindowActive = 0x00000008
};

// =============================================
// 11. Вспомогательные типы
// =============================================

enum TimerType
{
   PreciseTimer,
   CoarseTimer,
   VeryCoarseTimer
};

enum FocusReason
{
   MouseFocusReason,
   TabFocusReason,
   BacktabFocusReason,
   ActiveWindowFocusReason,
   PopupFocusReason,
   ShortcutFocusReason,
   MenuBarFocusReason,
   OtherFocusReason,
   NoFocusReason
};

enum CheckState
{
   Unchecked,
   PartiallyChecked,
   Checked
};

// =============================================
// 12. Операторы для работы с флагами (Qt-стиль)
// =============================================

// Keep integer masks one conversion away from QFlag and two from QFlags.
// This permits direct construction without making mixed bitwise calls ambiguous.
class QFlag
{
   int i;
public:
   constexpr QFlag(int value) noexcept : i(value) {}
   constexpr QFlag(unsigned int value) noexcept : i(static_cast<int>(value)) {}
   constexpr QFlag(long value) noexcept : i(static_cast<int>(value)) {}
   constexpr QFlag(unsigned long value) noexcept : i(static_cast<int>(value)) {}
   constexpr QFlag(short value) noexcept : i(value) {}
   constexpr QFlag(unsigned short value) noexcept : i(value) {}
   constexpr operator int() const noexcept { return i; }
   constexpr operator unsigned int() const noexcept { return static_cast<unsigned int>(i); }
};

// Mixed enum/integer expressions must not implicitly become a QFlags mask.
class QIncompatibleFlag
{
   int i;
public:
   constexpr explicit QIncompatibleFlag(int value) noexcept : i(value) {}
   constexpr operator int() const noexcept { return i; }
};

template<typename Enum>
class QFlags
{
   static_assert(sizeof(Enum) <= sizeof(int),
      "QFlags uses an int as storage, so an enum with underlying long long will overflow.");
public:
   using Int = typename std::conditional<
      std::is_unsigned<typename std::underlying_type<Enum>::type>::value,
      unsigned int, int>::type;
private:
   struct Private;
   using Zero = int Private::*;
   using ListIterator = typename std::initializer_list<Enum>::const_iterator;
   static constexpr Int combineFlags(ListIterator first, ListIterator last) noexcept {
      return first==last ? Int(0) : (static_cast<Int>(*first) | combineFlags(first+1,last));
   }
   Int i;
public:
   using enum_type = Enum;
   constexpr QFlags() noexcept : i(0) {}
   constexpr QFlags(Enum e) noexcept : i(static_cast<Int>(e)) {}
   constexpr QFlags(QFlag value) noexcept : i(static_cast<Int>(value)) {}
   // Preserve Qt 5.15 legacy initialization and assignment from literal zero.
   constexpr QFlags(Zero) noexcept : i(0) {}
   constexpr QFlags(std::initializer_list<Enum> flags) noexcept : i(combineFlags(flags.begin(),flags.end())) {}
   
   constexpr operator Int() const noexcept { return i; }
   
   constexpr bool testFlag(Enum flag) const noexcept {
      return (i & static_cast<Int>(flag)) == static_cast<Int>(flag) && (static_cast<Int>(flag) != 0 || i == 0);
   }
   
#if __cplusplus >= 201402L
   constexpr
#endif
   QFlags& setFlag(Enum flag, bool on=true) noexcept {
      return on ? (*this |= flag) : (*this &= ~static_cast<Int>(flag));
   }

   constexpr QFlags operator|(Enum flag) const noexcept
   {
      return QFlags(i | static_cast<Int>(flag));
   }
   
   constexpr QFlags operator|(QFlags other) const noexcept
   {
      return QFlags(i | other.i);
   }
   
#if __cplusplus >= 201402L
   constexpr
#endif
   QFlags& operator|=(Enum flag) noexcept
   {
      i |= static_cast<Int>(flag);
      return *this;
   }
   
#if __cplusplus >= 201402L
   constexpr
#endif
   QFlags& operator|=(QFlags other) noexcept
   {
      i |= other.i;
      return *this;
   }
   
   constexpr QFlags operator&(int mask) const noexcept
   {
      return QFlags(i & mask);
   }
   
   constexpr QFlags operator&(unsigned int mask) const noexcept
   {
      return QFlags(i & static_cast<Int>(mask));
   }
   
   constexpr QFlags operator&(Enum flag) const noexcept
   {
      return QFlags(i & static_cast<Int>(flag));
   }
   
   constexpr QFlags operator&(QFlags other) const noexcept
   {
      return QFlags(i & other.i);
   }
   
#if __cplusplus >= 201402L
   constexpr
#endif
   QFlags& operator&=(int mask) noexcept
   {
      i &= mask;
      return *this;
   }
   
#if __cplusplus >= 201402L
   constexpr
#endif
   QFlags& operator&=(unsigned int mask) noexcept
   {
      i &= static_cast<Int>(mask);
      return *this;
   }
   
#if __cplusplus >= 201402L
   constexpr
#endif
   QFlags& operator&=(Enum flag) noexcept
   {
      i &= static_cast<Int>(flag);
      return *this;
   }
   
#if __cplusplus >= 201402L
   constexpr
#endif
   QFlags& operator&=(QFlags other) noexcept
   {
      i &= other.i;
      return *this;
   }
   
   constexpr QFlags operator^(QFlags other) const noexcept
   {
      return QFlags(i ^ other.i);
   }
   
   constexpr QFlags operator^(Enum flag) const noexcept
   {
      return QFlags(i ^ static_cast<Int>(flag));
   }
   
#if __cplusplus >= 201402L
   constexpr
#endif
   QFlags& operator^=(QFlags other) noexcept
   {
      i ^= other.i;
      return *this;
   }
   
#if __cplusplus >= 201402L
   constexpr
#endif
   QFlags& operator^=(Enum flag) noexcept
   {
      i ^= static_cast<Int>(flag);
      return *this;
   }
   
   constexpr QFlags operator~() const noexcept
   {
      return QFlags(~i);
   }
   
   constexpr bool operator!() const noexcept
   {
      return !i;
   }
};

// =============================================
// 13. Типизированные флаги (для удобства)
// =============================================

using Alignment = QFlags<AlignmentFlag>;
using MouseButtons = QFlags<MouseButton>;
using KeyboardModifiers = QFlags<KeyboardModifier>;
using WindowStates = QFlags<WindowState>;

// =============================================
// 14. Константы для работы с текстом/строками
// =============================================

//......

// =============================================
// 15. Глобальные функции (Qt-like)
// =============================================

inline unsigned int qHash(const char* key, unsigned int seed = 0) noexcept
{
   unsigned int hash = seed;
   while (key && *key) {
      hash = (hash << 5) + hash + static_cast<unsigned char>(*key++);
   }
   return hash;
}

template<typename T>
inline constexpr T qAbs(const T& value)
{
   return value >= 0 ? value : -value;
}

template<typename T>
inline constexpr const T& qMin(const T& a, const T& b)
{
   return (a < b) ? a : b;
}

template<typename T>
inline constexpr const T& qMax(const T& a, const T& b)
{
   return (a < b) ? b : a;
}

// =============================================
// 16. Глобальные макросы (опционально)
// =============================================

#define Q_DECLARE_FLAGS(Flags, Enum) \
   using Flags = ::Qt::QFlags<Enum>;

#define Q_DECLARE_INCOMPATIBLE_FLAGS(Flags) \
   inline constexpr ::Qt::QIncompatibleFlag operator|(Flags::enum_type f1, int f2) noexcept \
   { return ::Qt::QIncompatibleFlag(static_cast<int>(f1) | f2); }

#define Q_DECLARE_OPERATORS_FOR_FLAGS(Flags) \
   inline constexpr ::Qt::QFlags<Flags::enum_type> operator|(Flags::enum_type f1, Flags::enum_type f2) noexcept \
   { return ::Qt::QFlags<Flags::enum_type>(f1) | f2; } \
   inline constexpr ::Qt::QFlags<Flags::enum_type> operator|(Flags::enum_type f1, ::Qt::QFlags<Flags::enum_type> f2) noexcept \
   { return f2 | f1; } \
   Q_DECLARE_INCOMPATIBLE_FLAGS(Flags)

Q_DECLARE_OPERATORS_FOR_FLAGS(Alignment)
Q_DECLARE_OPERATORS_FOR_FLAGS(MouseButtons)
Q_DECLARE_OPERATORS_FOR_FLAGS(KeyboardModifiers)
Q_DECLARE_OPERATORS_FOR_FLAGS(WindowStates)

} // namespace Qt

// =============================================
// 17. Глобальные using-декларации для удобства
// =============================================

using Qt::CaseSensitivity;
using Qt::SplitBehavior;
using Qt::Alignment;
using Qt::AlignmentFlag;
using Qt::Key;
using Qt::KeyboardModifier;
using Qt::KeyboardModifiers;
using Qt::MouseButton;
using Qt::MouseButtons;
using Qt::Orientation;
using Qt::CheckState;
using Qt::WindowType;
using Qt::WindowState;
using Qt::WindowStates;
using Qt::ItemFlag;

// =============================================
// 18. Макрос для версии (Qt-стиль)
// =============================================

#define QT_VERSION_MAJOR 5
#define QT_VERSION_MINOR 15
#define QT_VERSION_PATCH 0
#define QT_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))
#define QT_VERSION QT_VERSION_CHECK(QT_VERSION_MAJOR, QT_VERSION_MINOR, QT_VERSION_PATCH)


// =============================================
// =============================================

namespace Qt
{
    namespace Literals
    {
        // Объявляем, что пространство существует
		// Реализация в отдельном файле
        // qtliterals_nx_cb.h
    }
}




#endif // QT_NAMESPACE_NX_CB_H
