// Реализация Qt-подобных классов QString и QStringList для C++Builder 10.4
// Версия модуля: 2026.11.11 release
#ifndef QSTRING_NX_H
#define QSTRING_NX_H

#include <System.hpp>
#include <System.SysUtils.hpp>
#include <System.Math.hpp>
#include <Windows.h>
#include <locale>
#include <cstring>
#include <functional>
#include <memory>
#include <unordered_set>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <limits>
#include <stdio.h>
#include <cstdarg>

#include "qtnamespace_nx_cb.h"
#include "qchar_nx_cb.h"




class QString
{
private:
   static int checked_size(std::size_t size) {
      if(size > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
         throw std::length_error("QString exceeds INT_MAX");
      return static_cast<int>(size);
   }

   friend class QStringList;
   // Integer codes need exact overloads because this adapter also has QString(int).
   template<class T> struct IsCharacterCode : std::integral_constant<bool,
      std::is_integral<T>::value || (std::is_enum<T>::value && std::is_convertible<T,int>::value)> {};
   template<class T> struct IsCharacterArgument : std::integral_constant<bool,
      IsCharacterCode<T>::value || std::is_same<T,QChar>::value || std::is_same<T,QCharRef>::value> {};
   template<class T> static typename std::enable_if<IsCharacterCode<T>::value,QChar>::type
   characterArgument(T ch) { return QChar(static_cast<wchar_t>(ch)); }
   static QChar characterArgument(QChar ch) { return ch; }
   static bool equalCharacters(QChar a, QChar b, Qt::CaseSensitivity cs) {
      return cs==Qt::CaseSensitive ? a==b : comparisonFold(a.unicode())==comparisonFold(b.unicode());
   }
   int compareCharacter(QChar ch) const {
      if(isEmpty()) return -1;
      const unsigned first=static_cast<unsigned>(m_str.c_str()[0]);
      const unsigned other=static_cast<unsigned>(ch.unicode());
      if(first!=other) return first<other ? -1 : 1;
      return length()==1 ? 0 : 1;
   }
   // Unicode 13.0 common/simple folds, matching Qt 5.15's comparison.
   // Source: https://www.unicode.org/Public/13.0.0/ucd/CaseFolding.txt
   // Unicode data license: https://www.unicode.org/license.txt
   static char32_t comparisonFold(char32_t code) {
      struct Pair { char32_t from, to; };
      static const Pair pairs[] = {
         {0x41,0x61},
         {0x42,0x62},
         {0x43,0x63},
         {0x44,0x64},
         {0x45,0x65},
         {0x46,0x66},
         {0x47,0x67},
         {0x48,0x68},
         {0x49,0x69},
         {0x4A,0x6A},
         {0x4B,0x6B},
         {0x4C,0x6C},
         {0x4D,0x6D},
         {0x4E,0x6E},
         {0x4F,0x6F},
         {0x50,0x70},
         {0x51,0x71},
         {0x52,0x72},
         {0x53,0x73},
         {0x54,0x74},
         {0x55,0x75},
         {0x56,0x76},
         {0x57,0x77},
         {0x58,0x78},
         {0x59,0x79},
         {0x5A,0x7A},
         {0xB5,0x3BC},
         {0xC0,0xE0},
         {0xC1,0xE1},
         {0xC2,0xE2},
         {0xC3,0xE3},
         {0xC4,0xE4},
         {0xC5,0xE5},
         {0xC6,0xE6},
         {0xC7,0xE7},
         {0xC8,0xE8},
         {0xC9,0xE9},
         {0xCA,0xEA},
         {0xCB,0xEB},
         {0xCC,0xEC},
         {0xCD,0xED},
         {0xCE,0xEE},
         {0xCF,0xEF},
         {0xD0,0xF0},
         {0xD1,0xF1},
         {0xD2,0xF2},
         {0xD3,0xF3},
         {0xD4,0xF4},
         {0xD5,0xF5},
         {0xD6,0xF6},
         {0xD8,0xF8},
         {0xD9,0xF9},
         {0xDA,0xFA},
         {0xDB,0xFB},
         {0xDC,0xFC},
         {0xDD,0xFD},
         {0xDE,0xFE},
         {0x100,0x101},
         {0x102,0x103},
         {0x104,0x105},
         {0x106,0x107},
         {0x108,0x109},
         {0x10A,0x10B},
         {0x10C,0x10D},
         {0x10E,0x10F},
         {0x110,0x111},
         {0x112,0x113},
         {0x114,0x115},
         {0x116,0x117},
         {0x118,0x119},
         {0x11A,0x11B},
         {0x11C,0x11D},
         {0x11E,0x11F},
         {0x120,0x121},
         {0x122,0x123},
         {0x124,0x125},
         {0x126,0x127},
         {0x128,0x129},
         {0x12A,0x12B},
         {0x12C,0x12D},
         {0x12E,0x12F},
         {0x132,0x133},
         {0x134,0x135},
         {0x136,0x137},
         {0x139,0x13A},
         {0x13B,0x13C},
         {0x13D,0x13E},
         {0x13F,0x140},
         {0x141,0x142},
         {0x143,0x144},
         {0x145,0x146},
         {0x147,0x148},
         {0x14A,0x14B},
         {0x14C,0x14D},
         {0x14E,0x14F},
         {0x150,0x151},
         {0x152,0x153},
         {0x154,0x155},
         {0x156,0x157},
         {0x158,0x159},
         {0x15A,0x15B},
         {0x15C,0x15D},
         {0x15E,0x15F},
         {0x160,0x161},
         {0x162,0x163},
         {0x164,0x165},
         {0x166,0x167},
         {0x168,0x169},
         {0x16A,0x16B},
         {0x16C,0x16D},
         {0x16E,0x16F},
         {0x170,0x171},
         {0x172,0x173},
         {0x174,0x175},
         {0x176,0x177},
         {0x178,0xFF},
         {0x179,0x17A},
         {0x17B,0x17C},
         {0x17D,0x17E},
         {0x17F,0x73},
         {0x181,0x253},
         {0x182,0x183},
         {0x184,0x185},
         {0x186,0x254},
         {0x187,0x188},
         {0x189,0x256},
         {0x18A,0x257},
         {0x18B,0x18C},
         {0x18E,0x1DD},
         {0x18F,0x259},
         {0x190,0x25B},
         {0x191,0x192},
         {0x193,0x260},
         {0x194,0x263},
         {0x196,0x269},
         {0x197,0x268},
         {0x198,0x199},
         {0x19C,0x26F},
         {0x19D,0x272},
         {0x19F,0x275},
         {0x1A0,0x1A1},
         {0x1A2,0x1A3},
         {0x1A4,0x1A5},
         {0x1A6,0x280},
         {0x1A7,0x1A8},
         {0x1A9,0x283},
         {0x1AC,0x1AD},
         {0x1AE,0x288},
         {0x1AF,0x1B0},
         {0x1B1,0x28A},
         {0x1B2,0x28B},
         {0x1B3,0x1B4},
         {0x1B5,0x1B6},
         {0x1B7,0x292},
         {0x1B8,0x1B9},
         {0x1BC,0x1BD},
         {0x1C4,0x1C6},
         {0x1C5,0x1C6},
         {0x1C7,0x1C9},
         {0x1C8,0x1C9},
         {0x1CA,0x1CC},
         {0x1CB,0x1CC},
         {0x1CD,0x1CE},
         {0x1CF,0x1D0},
         {0x1D1,0x1D2},
         {0x1D3,0x1D4},
         {0x1D5,0x1D6},
         {0x1D7,0x1D8},
         {0x1D9,0x1DA},
         {0x1DB,0x1DC},
         {0x1DE,0x1DF},
         {0x1E0,0x1E1},
         {0x1E2,0x1E3},
         {0x1E4,0x1E5},
         {0x1E6,0x1E7},
         {0x1E8,0x1E9},
         {0x1EA,0x1EB},
         {0x1EC,0x1ED},
         {0x1EE,0x1EF},
         {0x1F1,0x1F3},
         {0x1F2,0x1F3},
         {0x1F4,0x1F5},
         {0x1F6,0x195},
         {0x1F7,0x1BF},
         {0x1F8,0x1F9},
         {0x1FA,0x1FB},
         {0x1FC,0x1FD},
         {0x1FE,0x1FF},
         {0x200,0x201},
         {0x202,0x203},
         {0x204,0x205},
         {0x206,0x207},
         {0x208,0x209},
         {0x20A,0x20B},
         {0x20C,0x20D},
         {0x20E,0x20F},
         {0x210,0x211},
         {0x212,0x213},
         {0x214,0x215},
         {0x216,0x217},
         {0x218,0x219},
         {0x21A,0x21B},
         {0x21C,0x21D},
         {0x21E,0x21F},
         {0x220,0x19E},
         {0x222,0x223},
         {0x224,0x225},
         {0x226,0x227},
         {0x228,0x229},
         {0x22A,0x22B},
         {0x22C,0x22D},
         {0x22E,0x22F},
         {0x230,0x231},
         {0x232,0x233},
         {0x23A,0x2C65},
         {0x23B,0x23C},
         {0x23D,0x19A},
         {0x23E,0x2C66},
         {0x241,0x242},
         {0x243,0x180},
         {0x244,0x289},
         {0x245,0x28C},
         {0x246,0x247},
         {0x248,0x249},
         {0x24A,0x24B},
         {0x24C,0x24D},
         {0x24E,0x24F},
         {0x345,0x3B9},
         {0x370,0x371},
         {0x372,0x373},
         {0x376,0x377},
         {0x37F,0x3F3},
         {0x386,0x3AC},
         {0x388,0x3AD},
         {0x389,0x3AE},
         {0x38A,0x3AF},
         {0x38C,0x3CC},
         {0x38E,0x3CD},
         {0x38F,0x3CE},
         {0x391,0x3B1},
         {0x392,0x3B2},
         {0x393,0x3B3},
         {0x394,0x3B4},
         {0x395,0x3B5},
         {0x396,0x3B6},
         {0x397,0x3B7},
         {0x398,0x3B8},
         {0x399,0x3B9},
         {0x39A,0x3BA},
         {0x39B,0x3BB},
         {0x39C,0x3BC},
         {0x39D,0x3BD},
         {0x39E,0x3BE},
         {0x39F,0x3BF},
         {0x3A0,0x3C0},
         {0x3A1,0x3C1},
         {0x3A3,0x3C3},
         {0x3A4,0x3C4},
         {0x3A5,0x3C5},
         {0x3A6,0x3C6},
         {0x3A7,0x3C7},
         {0x3A8,0x3C8},
         {0x3A9,0x3C9},
         {0x3AA,0x3CA},
         {0x3AB,0x3CB},
         {0x3C2,0x3C3},
         {0x3CF,0x3D7},
         {0x3D0,0x3B2},
         {0x3D1,0x3B8},
         {0x3D5,0x3C6},
         {0x3D6,0x3C0},
         {0x3D8,0x3D9},
         {0x3DA,0x3DB},
         {0x3DC,0x3DD},
         {0x3DE,0x3DF},
         {0x3E0,0x3E1},
         {0x3E2,0x3E3},
         {0x3E4,0x3E5},
         {0x3E6,0x3E7},
         {0x3E8,0x3E9},
         {0x3EA,0x3EB},
         {0x3EC,0x3ED},
         {0x3EE,0x3EF},
         {0x3F0,0x3BA},
         {0x3F1,0x3C1},
         {0x3F4,0x3B8},
         {0x3F5,0x3B5},
         {0x3F7,0x3F8},
         {0x3F9,0x3F2},
         {0x3FA,0x3FB},
         {0x3FD,0x37B},
         {0x3FE,0x37C},
         {0x3FF,0x37D},
         {0x400,0x450},
         {0x401,0x451},
         {0x402,0x452},
         {0x403,0x453},
         {0x404,0x454},
         {0x405,0x455},
         {0x406,0x456},
         {0x407,0x457},
         {0x408,0x458},
         {0x409,0x459},
         {0x40A,0x45A},
         {0x40B,0x45B},
         {0x40C,0x45C},
         {0x40D,0x45D},
         {0x40E,0x45E},
         {0x40F,0x45F},
         {0x410,0x430},
         {0x411,0x431},
         {0x412,0x432},
         {0x413,0x433},
         {0x414,0x434},
         {0x415,0x435},
         {0x416,0x436},
         {0x417,0x437},
         {0x418,0x438},
         {0x419,0x439},
         {0x41A,0x43A},
         {0x41B,0x43B},
         {0x41C,0x43C},
         {0x41D,0x43D},
         {0x41E,0x43E},
         {0x41F,0x43F},
         {0x420,0x440},
         {0x421,0x441},
         {0x422,0x442},
         {0x423,0x443},
         {0x424,0x444},
         {0x425,0x445},
         {0x426,0x446},
         {0x427,0x447},
         {0x428,0x448},
         {0x429,0x449},
         {0x42A,0x44A},
         {0x42B,0x44B},
         {0x42C,0x44C},
         {0x42D,0x44D},
         {0x42E,0x44E},
         {0x42F,0x44F},
         {0x460,0x461},
         {0x462,0x463},
         {0x464,0x465},
         {0x466,0x467},
         {0x468,0x469},
         {0x46A,0x46B},
         {0x46C,0x46D},
         {0x46E,0x46F},
         {0x470,0x471},
         {0x472,0x473},
         {0x474,0x475},
         {0x476,0x477},
         {0x478,0x479},
         {0x47A,0x47B},
         {0x47C,0x47D},
         {0x47E,0x47F},
         {0x480,0x481},
         {0x48A,0x48B},
         {0x48C,0x48D},
         {0x48E,0x48F},
         {0x490,0x491},
         {0x492,0x493},
         {0x494,0x495},
         {0x496,0x497},
         {0x498,0x499},
         {0x49A,0x49B},
         {0x49C,0x49D},
         {0x49E,0x49F},
         {0x4A0,0x4A1},
         {0x4A2,0x4A3},
         {0x4A4,0x4A5},
         {0x4A6,0x4A7},
         {0x4A8,0x4A9},
         {0x4AA,0x4AB},
         {0x4AC,0x4AD},
         {0x4AE,0x4AF},
         {0x4B0,0x4B1},
         {0x4B2,0x4B3},
         {0x4B4,0x4B5},
         {0x4B6,0x4B7},
         {0x4B8,0x4B9},
         {0x4BA,0x4BB},
         {0x4BC,0x4BD},
         {0x4BE,0x4BF},
         {0x4C0,0x4CF},
         {0x4C1,0x4C2},
         {0x4C3,0x4C4},
         {0x4C5,0x4C6},
         {0x4C7,0x4C8},
         {0x4C9,0x4CA},
         {0x4CB,0x4CC},
         {0x4CD,0x4CE},
         {0x4D0,0x4D1},
         {0x4D2,0x4D3},
         {0x4D4,0x4D5},
         {0x4D6,0x4D7},
         {0x4D8,0x4D9},
         {0x4DA,0x4DB},
         {0x4DC,0x4DD},
         {0x4DE,0x4DF},
         {0x4E0,0x4E1},
         {0x4E2,0x4E3},
         {0x4E4,0x4E5},
         {0x4E6,0x4E7},
         {0x4E8,0x4E9},
         {0x4EA,0x4EB},
         {0x4EC,0x4ED},
         {0x4EE,0x4EF},
         {0x4F0,0x4F1},
         {0x4F2,0x4F3},
         {0x4F4,0x4F5},
         {0x4F6,0x4F7},
         {0x4F8,0x4F9},
         {0x4FA,0x4FB},
         {0x4FC,0x4FD},
         {0x4FE,0x4FF},
         {0x500,0x501},
         {0x502,0x503},
         {0x504,0x505},
         {0x506,0x507},
         {0x508,0x509},
         {0x50A,0x50B},
         {0x50C,0x50D},
         {0x50E,0x50F},
         {0x510,0x511},
         {0x512,0x513},
         {0x514,0x515},
         {0x516,0x517},
         {0x518,0x519},
         {0x51A,0x51B},
         {0x51C,0x51D},
         {0x51E,0x51F},
         {0x520,0x521},
         {0x522,0x523},
         {0x524,0x525},
         {0x526,0x527},
         {0x528,0x529},
         {0x52A,0x52B},
         {0x52C,0x52D},
         {0x52E,0x52F},
         {0x531,0x561},
         {0x532,0x562},
         {0x533,0x563},
         {0x534,0x564},
         {0x535,0x565},
         {0x536,0x566},
         {0x537,0x567},
         {0x538,0x568},
         {0x539,0x569},
         {0x53A,0x56A},
         {0x53B,0x56B},
         {0x53C,0x56C},
         {0x53D,0x56D},
         {0x53E,0x56E},
         {0x53F,0x56F},
         {0x540,0x570},
         {0x541,0x571},
         {0x542,0x572},
         {0x543,0x573},
         {0x544,0x574},
         {0x545,0x575},
         {0x546,0x576},
         {0x547,0x577},
         {0x548,0x578},
         {0x549,0x579},
         {0x54A,0x57A},
         {0x54B,0x57B},
         {0x54C,0x57C},
         {0x54D,0x57D},
         {0x54E,0x57E},
         {0x54F,0x57F},
         {0x550,0x580},
         {0x551,0x581},
         {0x552,0x582},
         {0x553,0x583},
         {0x554,0x584},
         {0x555,0x585},
         {0x556,0x586},
         {0x10A0,0x2D00},
         {0x10A1,0x2D01},
         {0x10A2,0x2D02},
         {0x10A3,0x2D03},
         {0x10A4,0x2D04},
         {0x10A5,0x2D05},
         {0x10A6,0x2D06},
         {0x10A7,0x2D07},
         {0x10A8,0x2D08},
         {0x10A9,0x2D09},
         {0x10AA,0x2D0A},
         {0x10AB,0x2D0B},
         {0x10AC,0x2D0C},
         {0x10AD,0x2D0D},
         {0x10AE,0x2D0E},
         {0x10AF,0x2D0F},
         {0x10B0,0x2D10},
         {0x10B1,0x2D11},
         {0x10B2,0x2D12},
         {0x10B3,0x2D13},
         {0x10B4,0x2D14},
         {0x10B5,0x2D15},
         {0x10B6,0x2D16},
         {0x10B7,0x2D17},
         {0x10B8,0x2D18},
         {0x10B9,0x2D19},
         {0x10BA,0x2D1A},
         {0x10BB,0x2D1B},
         {0x10BC,0x2D1C},
         {0x10BD,0x2D1D},
         {0x10BE,0x2D1E},
         {0x10BF,0x2D1F},
         {0x10C0,0x2D20},
         {0x10C1,0x2D21},
         {0x10C2,0x2D22},
         {0x10C3,0x2D23},
         {0x10C4,0x2D24},
         {0x10C5,0x2D25},
         {0x10C7,0x2D27},
         {0x10CD,0x2D2D},
         {0x13F8,0x13F0},
         {0x13F9,0x13F1},
         {0x13FA,0x13F2},
         {0x13FB,0x13F3},
         {0x13FC,0x13F4},
         {0x13FD,0x13F5},
         {0x1C80,0x432},
         {0x1C81,0x434},
         {0x1C82,0x43E},
         {0x1C83,0x441},
         {0x1C84,0x442},
         {0x1C85,0x442},
         {0x1C86,0x44A},
         {0x1C87,0x463},
         {0x1C88,0xA64B},
         {0x1C90,0x10D0},
         {0x1C91,0x10D1},
         {0x1C92,0x10D2},
         {0x1C93,0x10D3},
         {0x1C94,0x10D4},
         {0x1C95,0x10D5},
         {0x1C96,0x10D6},
         {0x1C97,0x10D7},
         {0x1C98,0x10D8},
         {0x1C99,0x10D9},
         {0x1C9A,0x10DA},
         {0x1C9B,0x10DB},
         {0x1C9C,0x10DC},
         {0x1C9D,0x10DD},
         {0x1C9E,0x10DE},
         {0x1C9F,0x10DF},
         {0x1CA0,0x10E0},
         {0x1CA1,0x10E1},
         {0x1CA2,0x10E2},
         {0x1CA3,0x10E3},
         {0x1CA4,0x10E4},
         {0x1CA5,0x10E5},
         {0x1CA6,0x10E6},
         {0x1CA7,0x10E7},
         {0x1CA8,0x10E8},
         {0x1CA9,0x10E9},
         {0x1CAA,0x10EA},
         {0x1CAB,0x10EB},
         {0x1CAC,0x10EC},
         {0x1CAD,0x10ED},
         {0x1CAE,0x10EE},
         {0x1CAF,0x10EF},
         {0x1CB0,0x10F0},
         {0x1CB1,0x10F1},
         {0x1CB2,0x10F2},
         {0x1CB3,0x10F3},
         {0x1CB4,0x10F4},
         {0x1CB5,0x10F5},
         {0x1CB6,0x10F6},
         {0x1CB7,0x10F7},
         {0x1CB8,0x10F8},
         {0x1CB9,0x10F9},
         {0x1CBA,0x10FA},
         {0x1CBD,0x10FD},
         {0x1CBE,0x10FE},
         {0x1CBF,0x10FF},
         {0x1E00,0x1E01},
         {0x1E02,0x1E03},
         {0x1E04,0x1E05},
         {0x1E06,0x1E07},
         {0x1E08,0x1E09},
         {0x1E0A,0x1E0B},
         {0x1E0C,0x1E0D},
         {0x1E0E,0x1E0F},
         {0x1E10,0x1E11},
         {0x1E12,0x1E13},
         {0x1E14,0x1E15},
         {0x1E16,0x1E17},
         {0x1E18,0x1E19},
         {0x1E1A,0x1E1B},
         {0x1E1C,0x1E1D},
         {0x1E1E,0x1E1F},
         {0x1E20,0x1E21},
         {0x1E22,0x1E23},
         {0x1E24,0x1E25},
         {0x1E26,0x1E27},
         {0x1E28,0x1E29},
         {0x1E2A,0x1E2B},
         {0x1E2C,0x1E2D},
         {0x1E2E,0x1E2F},
         {0x1E30,0x1E31},
         {0x1E32,0x1E33},
         {0x1E34,0x1E35},
         {0x1E36,0x1E37},
         {0x1E38,0x1E39},
         {0x1E3A,0x1E3B},
         {0x1E3C,0x1E3D},
         {0x1E3E,0x1E3F},
         {0x1E40,0x1E41},
         {0x1E42,0x1E43},
         {0x1E44,0x1E45},
         {0x1E46,0x1E47},
         {0x1E48,0x1E49},
         {0x1E4A,0x1E4B},
         {0x1E4C,0x1E4D},
         {0x1E4E,0x1E4F},
         {0x1E50,0x1E51},
         {0x1E52,0x1E53},
         {0x1E54,0x1E55},
         {0x1E56,0x1E57},
         {0x1E58,0x1E59},
         {0x1E5A,0x1E5B},
         {0x1E5C,0x1E5D},
         {0x1E5E,0x1E5F},
         {0x1E60,0x1E61},
         {0x1E62,0x1E63},
         {0x1E64,0x1E65},
         {0x1E66,0x1E67},
         {0x1E68,0x1E69},
         {0x1E6A,0x1E6B},
         {0x1E6C,0x1E6D},
         {0x1E6E,0x1E6F},
         {0x1E70,0x1E71},
         {0x1E72,0x1E73},
         {0x1E74,0x1E75},
         {0x1E76,0x1E77},
         {0x1E78,0x1E79},
         {0x1E7A,0x1E7B},
         {0x1E7C,0x1E7D},
         {0x1E7E,0x1E7F},
         {0x1E80,0x1E81},
         {0x1E82,0x1E83},
         {0x1E84,0x1E85},
         {0x1E86,0x1E87},
         {0x1E88,0x1E89},
         {0x1E8A,0x1E8B},
         {0x1E8C,0x1E8D},
         {0x1E8E,0x1E8F},
         {0x1E90,0x1E91},
         {0x1E92,0x1E93},
         {0x1E94,0x1E95},
         {0x1E9B,0x1E61},
         {0x1E9E,0xDF},
         {0x1EA0,0x1EA1},
         {0x1EA2,0x1EA3},
         {0x1EA4,0x1EA5},
         {0x1EA6,0x1EA7},
         {0x1EA8,0x1EA9},
         {0x1EAA,0x1EAB},
         {0x1EAC,0x1EAD},
         {0x1EAE,0x1EAF},
         {0x1EB0,0x1EB1},
         {0x1EB2,0x1EB3},
         {0x1EB4,0x1EB5},
         {0x1EB6,0x1EB7},
         {0x1EB8,0x1EB9},
         {0x1EBA,0x1EBB},
         {0x1EBC,0x1EBD},
         {0x1EBE,0x1EBF},
         {0x1EC0,0x1EC1},
         {0x1EC2,0x1EC3},
         {0x1EC4,0x1EC5},
         {0x1EC6,0x1EC7},
         {0x1EC8,0x1EC9},
         {0x1ECA,0x1ECB},
         {0x1ECC,0x1ECD},
         {0x1ECE,0x1ECF},
         {0x1ED0,0x1ED1},
         {0x1ED2,0x1ED3},
         {0x1ED4,0x1ED5},
         {0x1ED6,0x1ED7},
         {0x1ED8,0x1ED9},
         {0x1EDA,0x1EDB},
         {0x1EDC,0x1EDD},
         {0x1EDE,0x1EDF},
         {0x1EE0,0x1EE1},
         {0x1EE2,0x1EE3},
         {0x1EE4,0x1EE5},
         {0x1EE6,0x1EE7},
         {0x1EE8,0x1EE9},
         {0x1EEA,0x1EEB},
         {0x1EEC,0x1EED},
         {0x1EEE,0x1EEF},
         {0x1EF0,0x1EF1},
         {0x1EF2,0x1EF3},
         {0x1EF4,0x1EF5},
         {0x1EF6,0x1EF7},
         {0x1EF8,0x1EF9},
         {0x1EFA,0x1EFB},
         {0x1EFC,0x1EFD},
         {0x1EFE,0x1EFF},
         {0x1F08,0x1F00},
         {0x1F09,0x1F01},
         {0x1F0A,0x1F02},
         {0x1F0B,0x1F03},
         {0x1F0C,0x1F04},
         {0x1F0D,0x1F05},
         {0x1F0E,0x1F06},
         {0x1F0F,0x1F07},
         {0x1F18,0x1F10},
         {0x1F19,0x1F11},
         {0x1F1A,0x1F12},
         {0x1F1B,0x1F13},
         {0x1F1C,0x1F14},
         {0x1F1D,0x1F15},
         {0x1F28,0x1F20},
         {0x1F29,0x1F21},
         {0x1F2A,0x1F22},
         {0x1F2B,0x1F23},
         {0x1F2C,0x1F24},
         {0x1F2D,0x1F25},
         {0x1F2E,0x1F26},
         {0x1F2F,0x1F27},
         {0x1F38,0x1F30},
         {0x1F39,0x1F31},
         {0x1F3A,0x1F32},
         {0x1F3B,0x1F33},
         {0x1F3C,0x1F34},
         {0x1F3D,0x1F35},
         {0x1F3E,0x1F36},
         {0x1F3F,0x1F37},
         {0x1F48,0x1F40},
         {0x1F49,0x1F41},
         {0x1F4A,0x1F42},
         {0x1F4B,0x1F43},
         {0x1F4C,0x1F44},
         {0x1F4D,0x1F45},
         {0x1F59,0x1F51},
         {0x1F5B,0x1F53},
         {0x1F5D,0x1F55},
         {0x1F5F,0x1F57},
         {0x1F68,0x1F60},
         {0x1F69,0x1F61},
         {0x1F6A,0x1F62},
         {0x1F6B,0x1F63},
         {0x1F6C,0x1F64},
         {0x1F6D,0x1F65},
         {0x1F6E,0x1F66},
         {0x1F6F,0x1F67},
         {0x1F88,0x1F80},
         {0x1F89,0x1F81},
         {0x1F8A,0x1F82},
         {0x1F8B,0x1F83},
         {0x1F8C,0x1F84},
         {0x1F8D,0x1F85},
         {0x1F8E,0x1F86},
         {0x1F8F,0x1F87},
         {0x1F98,0x1F90},
         {0x1F99,0x1F91},
         {0x1F9A,0x1F92},
         {0x1F9B,0x1F93},
         {0x1F9C,0x1F94},
         {0x1F9D,0x1F95},
         {0x1F9E,0x1F96},
         {0x1F9F,0x1F97},
         {0x1FA8,0x1FA0},
         {0x1FA9,0x1FA1},
         {0x1FAA,0x1FA2},
         {0x1FAB,0x1FA3},
         {0x1FAC,0x1FA4},
         {0x1FAD,0x1FA5},
         {0x1FAE,0x1FA6},
         {0x1FAF,0x1FA7},
         {0x1FB8,0x1FB0},
         {0x1FB9,0x1FB1},
         {0x1FBA,0x1F70},
         {0x1FBB,0x1F71},
         {0x1FBC,0x1FB3},
         {0x1FBE,0x3B9},
         {0x1FC8,0x1F72},
         {0x1FC9,0x1F73},
         {0x1FCA,0x1F74},
         {0x1FCB,0x1F75},
         {0x1FCC,0x1FC3},
         {0x1FD8,0x1FD0},
         {0x1FD9,0x1FD1},
         {0x1FDA,0x1F76},
         {0x1FDB,0x1F77},
         {0x1FE8,0x1FE0},
         {0x1FE9,0x1FE1},
         {0x1FEA,0x1F7A},
         {0x1FEB,0x1F7B},
         {0x1FEC,0x1FE5},
         {0x1FF8,0x1F78},
         {0x1FF9,0x1F79},
         {0x1FFA,0x1F7C},
         {0x1FFB,0x1F7D},
         {0x1FFC,0x1FF3},
         {0x2126,0x3C9},
         {0x212A,0x6B},
         {0x212B,0xE5},
         {0x2132,0x214E},
         {0x2160,0x2170},
         {0x2161,0x2171},
         {0x2162,0x2172},
         {0x2163,0x2173},
         {0x2164,0x2174},
         {0x2165,0x2175},
         {0x2166,0x2176},
         {0x2167,0x2177},
         {0x2168,0x2178},
         {0x2169,0x2179},
         {0x216A,0x217A},
         {0x216B,0x217B},
         {0x216C,0x217C},
         {0x216D,0x217D},
         {0x216E,0x217E},
         {0x216F,0x217F},
         {0x2183,0x2184},
         {0x24B6,0x24D0},
         {0x24B7,0x24D1},
         {0x24B8,0x24D2},
         {0x24B9,0x24D3},
         {0x24BA,0x24D4},
         {0x24BB,0x24D5},
         {0x24BC,0x24D6},
         {0x24BD,0x24D7},
         {0x24BE,0x24D8},
         {0x24BF,0x24D9},
         {0x24C0,0x24DA},
         {0x24C1,0x24DB},
         {0x24C2,0x24DC},
         {0x24C3,0x24DD},
         {0x24C4,0x24DE},
         {0x24C5,0x24DF},
         {0x24C6,0x24E0},
         {0x24C7,0x24E1},
         {0x24C8,0x24E2},
         {0x24C9,0x24E3},
         {0x24CA,0x24E4},
         {0x24CB,0x24E5},
         {0x24CC,0x24E6},
         {0x24CD,0x24E7},
         {0x24CE,0x24E8},
         {0x24CF,0x24E9},
         {0x2C00,0x2C30},
         {0x2C01,0x2C31},
         {0x2C02,0x2C32},
         {0x2C03,0x2C33},
         {0x2C04,0x2C34},
         {0x2C05,0x2C35},
         {0x2C06,0x2C36},
         {0x2C07,0x2C37},
         {0x2C08,0x2C38},
         {0x2C09,0x2C39},
         {0x2C0A,0x2C3A},
         {0x2C0B,0x2C3B},
         {0x2C0C,0x2C3C},
         {0x2C0D,0x2C3D},
         {0x2C0E,0x2C3E},
         {0x2C0F,0x2C3F},
         {0x2C10,0x2C40},
         {0x2C11,0x2C41},
         {0x2C12,0x2C42},
         {0x2C13,0x2C43},
         {0x2C14,0x2C44},
         {0x2C15,0x2C45},
         {0x2C16,0x2C46},
         {0x2C17,0x2C47},
         {0x2C18,0x2C48},
         {0x2C19,0x2C49},
         {0x2C1A,0x2C4A},
         {0x2C1B,0x2C4B},
         {0x2C1C,0x2C4C},
         {0x2C1D,0x2C4D},
         {0x2C1E,0x2C4E},
         {0x2C1F,0x2C4F},
         {0x2C20,0x2C50},
         {0x2C21,0x2C51},
         {0x2C22,0x2C52},
         {0x2C23,0x2C53},
         {0x2C24,0x2C54},
         {0x2C25,0x2C55},
         {0x2C26,0x2C56},
         {0x2C27,0x2C57},
         {0x2C28,0x2C58},
         {0x2C29,0x2C59},
         {0x2C2A,0x2C5A},
         {0x2C2B,0x2C5B},
         {0x2C2C,0x2C5C},
         {0x2C2D,0x2C5D},
         {0x2C2E,0x2C5E},
         {0x2C60,0x2C61},
         {0x2C62,0x26B},
         {0x2C63,0x1D7D},
         {0x2C64,0x27D},
         {0x2C67,0x2C68},
         {0x2C69,0x2C6A},
         {0x2C6B,0x2C6C},
         {0x2C6D,0x251},
         {0x2C6E,0x271},
         {0x2C6F,0x250},
         {0x2C70,0x252},
         {0x2C72,0x2C73},
         {0x2C75,0x2C76},
         {0x2C7E,0x23F},
         {0x2C7F,0x240},
         {0x2C80,0x2C81},
         {0x2C82,0x2C83},
         {0x2C84,0x2C85},
         {0x2C86,0x2C87},
         {0x2C88,0x2C89},
         {0x2C8A,0x2C8B},
         {0x2C8C,0x2C8D},
         {0x2C8E,0x2C8F},
         {0x2C90,0x2C91},
         {0x2C92,0x2C93},
         {0x2C94,0x2C95},
         {0x2C96,0x2C97},
         {0x2C98,0x2C99},
         {0x2C9A,0x2C9B},
         {0x2C9C,0x2C9D},
         {0x2C9E,0x2C9F},
         {0x2CA0,0x2CA1},
         {0x2CA2,0x2CA3},
         {0x2CA4,0x2CA5},
         {0x2CA6,0x2CA7},
         {0x2CA8,0x2CA9},
         {0x2CAA,0x2CAB},
         {0x2CAC,0x2CAD},
         {0x2CAE,0x2CAF},
         {0x2CB0,0x2CB1},
         {0x2CB2,0x2CB3},
         {0x2CB4,0x2CB5},
         {0x2CB6,0x2CB7},
         {0x2CB8,0x2CB9},
         {0x2CBA,0x2CBB},
         {0x2CBC,0x2CBD},
         {0x2CBE,0x2CBF},
         {0x2CC0,0x2CC1},
         {0x2CC2,0x2CC3},
         {0x2CC4,0x2CC5},
         {0x2CC6,0x2CC7},
         {0x2CC8,0x2CC9},
         {0x2CCA,0x2CCB},
         {0x2CCC,0x2CCD},
         {0x2CCE,0x2CCF},
         {0x2CD0,0x2CD1},
         {0x2CD2,0x2CD3},
         {0x2CD4,0x2CD5},
         {0x2CD6,0x2CD7},
         {0x2CD8,0x2CD9},
         {0x2CDA,0x2CDB},
         {0x2CDC,0x2CDD},
         {0x2CDE,0x2CDF},
         {0x2CE0,0x2CE1},
         {0x2CE2,0x2CE3},
         {0x2CEB,0x2CEC},
         {0x2CED,0x2CEE},
         {0x2CF2,0x2CF3},
         {0xA640,0xA641},
         {0xA642,0xA643},
         {0xA644,0xA645},
         {0xA646,0xA647},
         {0xA648,0xA649},
         {0xA64A,0xA64B},
         {0xA64C,0xA64D},
         {0xA64E,0xA64F},
         {0xA650,0xA651},
         {0xA652,0xA653},
         {0xA654,0xA655},
         {0xA656,0xA657},
         {0xA658,0xA659},
         {0xA65A,0xA65B},
         {0xA65C,0xA65D},
         {0xA65E,0xA65F},
         {0xA660,0xA661},
         {0xA662,0xA663},
         {0xA664,0xA665},
         {0xA666,0xA667},
         {0xA668,0xA669},
         {0xA66A,0xA66B},
         {0xA66C,0xA66D},
         {0xA680,0xA681},
         {0xA682,0xA683},
         {0xA684,0xA685},
         {0xA686,0xA687},
         {0xA688,0xA689},
         {0xA68A,0xA68B},
         {0xA68C,0xA68D},
         {0xA68E,0xA68F},
         {0xA690,0xA691},
         {0xA692,0xA693},
         {0xA694,0xA695},
         {0xA696,0xA697},
         {0xA698,0xA699},
         {0xA69A,0xA69B},
         {0xA722,0xA723},
         {0xA724,0xA725},
         {0xA726,0xA727},
         {0xA728,0xA729},
         {0xA72A,0xA72B},
         {0xA72C,0xA72D},
         {0xA72E,0xA72F},
         {0xA732,0xA733},
         {0xA734,0xA735},
         {0xA736,0xA737},
         {0xA738,0xA739},
         {0xA73A,0xA73B},
         {0xA73C,0xA73D},
         {0xA73E,0xA73F},
         {0xA740,0xA741},
         {0xA742,0xA743},
         {0xA744,0xA745},
         {0xA746,0xA747},
         {0xA748,0xA749},
         {0xA74A,0xA74B},
         {0xA74C,0xA74D},
         {0xA74E,0xA74F},
         {0xA750,0xA751},
         {0xA752,0xA753},
         {0xA754,0xA755},
         {0xA756,0xA757},
         {0xA758,0xA759},
         {0xA75A,0xA75B},
         {0xA75C,0xA75D},
         {0xA75E,0xA75F},
         {0xA760,0xA761},
         {0xA762,0xA763},
         {0xA764,0xA765},
         {0xA766,0xA767},
         {0xA768,0xA769},
         {0xA76A,0xA76B},
         {0xA76C,0xA76D},
         {0xA76E,0xA76F},
         {0xA779,0xA77A},
         {0xA77B,0xA77C},
         {0xA77D,0x1D79},
         {0xA77E,0xA77F},
         {0xA780,0xA781},
         {0xA782,0xA783},
         {0xA784,0xA785},
         {0xA786,0xA787},
         {0xA78B,0xA78C},
         {0xA78D,0x265},
         {0xA790,0xA791},
         {0xA792,0xA793},
         {0xA796,0xA797},
         {0xA798,0xA799},
         {0xA79A,0xA79B},
         {0xA79C,0xA79D},
         {0xA79E,0xA79F},
         {0xA7A0,0xA7A1},
         {0xA7A2,0xA7A3},
         {0xA7A4,0xA7A5},
         {0xA7A6,0xA7A7},
         {0xA7A8,0xA7A9},
         {0xA7AA,0x266},
         {0xA7AB,0x25C},
         {0xA7AC,0x261},
         {0xA7AD,0x26C},
         {0xA7AE,0x26A},
         {0xA7B0,0x29E},
         {0xA7B1,0x287},
         {0xA7B2,0x29D},
         {0xA7B3,0xAB53},
         {0xA7B4,0xA7B5},
         {0xA7B6,0xA7B7},
         {0xA7B8,0xA7B9},
         {0xA7BA,0xA7BB},
         {0xA7BC,0xA7BD},
         {0xA7BE,0xA7BF},
         {0xA7C2,0xA7C3},
         {0xA7C4,0xA794},
         {0xA7C5,0x282},
         {0xA7C6,0x1D8E},
         {0xA7C7,0xA7C8},
         {0xA7C9,0xA7CA},
         {0xA7F5,0xA7F6},
         {0xAB70,0x13A0},
         {0xAB71,0x13A1},
         {0xAB72,0x13A2},
         {0xAB73,0x13A3},
         {0xAB74,0x13A4},
         {0xAB75,0x13A5},
         {0xAB76,0x13A6},
         {0xAB77,0x13A7},
         {0xAB78,0x13A8},
         {0xAB79,0x13A9},
         {0xAB7A,0x13AA},
         {0xAB7B,0x13AB},
         {0xAB7C,0x13AC},
         {0xAB7D,0x13AD},
         {0xAB7E,0x13AE},
         {0xAB7F,0x13AF},
         {0xAB80,0x13B0},
         {0xAB81,0x13B1},
         {0xAB82,0x13B2},
         {0xAB83,0x13B3},
         {0xAB84,0x13B4},
         {0xAB85,0x13B5},
         {0xAB86,0x13B6},
         {0xAB87,0x13B7},
         {0xAB88,0x13B8},
         {0xAB89,0x13B9},
         {0xAB8A,0x13BA},
         {0xAB8B,0x13BB},
         {0xAB8C,0x13BC},
         {0xAB8D,0x13BD},
         {0xAB8E,0x13BE},
         {0xAB8F,0x13BF},
         {0xAB90,0x13C0},
         {0xAB91,0x13C1},
         {0xAB92,0x13C2},
         {0xAB93,0x13C3},
         {0xAB94,0x13C4},
         {0xAB95,0x13C5},
         {0xAB96,0x13C6},
         {0xAB97,0x13C7},
         {0xAB98,0x13C8},
         {0xAB99,0x13C9},
         {0xAB9A,0x13CA},
         {0xAB9B,0x13CB},
         {0xAB9C,0x13CC},
         {0xAB9D,0x13CD},
         {0xAB9E,0x13CE},
         {0xAB9F,0x13CF},
         {0xABA0,0x13D0},
         {0xABA1,0x13D1},
         {0xABA2,0x13D2},
         {0xABA3,0x13D3},
         {0xABA4,0x13D4},
         {0xABA5,0x13D5},
         {0xABA6,0x13D6},
         {0xABA7,0x13D7},
         {0xABA8,0x13D8},
         {0xABA9,0x13D9},
         {0xABAA,0x13DA},
         {0xABAB,0x13DB},
         {0xABAC,0x13DC},
         {0xABAD,0x13DD},
         {0xABAE,0x13DE},
         {0xABAF,0x13DF},
         {0xABB0,0x13E0},
         {0xABB1,0x13E1},
         {0xABB2,0x13E2},
         {0xABB3,0x13E3},
         {0xABB4,0x13E4},
         {0xABB5,0x13E5},
         {0xABB6,0x13E6},
         {0xABB7,0x13E7},
         {0xABB8,0x13E8},
         {0xABB9,0x13E9},
         {0xABBA,0x13EA},
         {0xABBB,0x13EB},
         {0xABBC,0x13EC},
         {0xABBD,0x13ED},
         {0xABBE,0x13EE},
         {0xABBF,0x13EF},
         {0xFF21,0xFF41},
         {0xFF22,0xFF42},
         {0xFF23,0xFF43},
         {0xFF24,0xFF44},
         {0xFF25,0xFF45},
         {0xFF26,0xFF46},
         {0xFF27,0xFF47},
         {0xFF28,0xFF48},
         {0xFF29,0xFF49},
         {0xFF2A,0xFF4A},
         {0xFF2B,0xFF4B},
         {0xFF2C,0xFF4C},
         {0xFF2D,0xFF4D},
         {0xFF2E,0xFF4E},
         {0xFF2F,0xFF4F},
         {0xFF30,0xFF50},
         {0xFF31,0xFF51},
         {0xFF32,0xFF52},
         {0xFF33,0xFF53},
         {0xFF34,0xFF54},
         {0xFF35,0xFF55},
         {0xFF36,0xFF56},
         {0xFF37,0xFF57},
         {0xFF38,0xFF58},
         {0xFF39,0xFF59},
         {0xFF3A,0xFF5A},
         {0x10400,0x10428},
         {0x10401,0x10429},
         {0x10402,0x1042A},
         {0x10403,0x1042B},
         {0x10404,0x1042C},
         {0x10405,0x1042D},
         {0x10406,0x1042E},
         {0x10407,0x1042F},
         {0x10408,0x10430},
         {0x10409,0x10431},
         {0x1040A,0x10432},
         {0x1040B,0x10433},
         {0x1040C,0x10434},
         {0x1040D,0x10435},
         {0x1040E,0x10436},
         {0x1040F,0x10437},
         {0x10410,0x10438},
         {0x10411,0x10439},
         {0x10412,0x1043A},
         {0x10413,0x1043B},
         {0x10414,0x1043C},
         {0x10415,0x1043D},
         {0x10416,0x1043E},
         {0x10417,0x1043F},
         {0x10418,0x10440},
         {0x10419,0x10441},
         {0x1041A,0x10442},
         {0x1041B,0x10443},
         {0x1041C,0x10444},
         {0x1041D,0x10445},
         {0x1041E,0x10446},
         {0x1041F,0x10447},
         {0x10420,0x10448},
         {0x10421,0x10449},
         {0x10422,0x1044A},
         {0x10423,0x1044B},
         {0x10424,0x1044C},
         {0x10425,0x1044D},
         {0x10426,0x1044E},
         {0x10427,0x1044F},
         {0x104B0,0x104D8},
         {0x104B1,0x104D9},
         {0x104B2,0x104DA},
         {0x104B3,0x104DB},
         {0x104B4,0x104DC},
         {0x104B5,0x104DD},
         {0x104B6,0x104DE},
         {0x104B7,0x104DF},
         {0x104B8,0x104E0},
         {0x104B9,0x104E1},
         {0x104BA,0x104E2},
         {0x104BB,0x104E3},
         {0x104BC,0x104E4},
         {0x104BD,0x104E5},
         {0x104BE,0x104E6},
         {0x104BF,0x104E7},
         {0x104C0,0x104E8},
         {0x104C1,0x104E9},
         {0x104C2,0x104EA},
         {0x104C3,0x104EB},
         {0x104C4,0x104EC},
         {0x104C5,0x104ED},
         {0x104C6,0x104EE},
         {0x104C7,0x104EF},
         {0x104C8,0x104F0},
         {0x104C9,0x104F1},
         {0x104CA,0x104F2},
         {0x104CB,0x104F3},
         {0x104CC,0x104F4},
         {0x104CD,0x104F5},
         {0x104CE,0x104F6},
         {0x104CF,0x104F7},
         {0x104D0,0x104F8},
         {0x104D1,0x104F9},
         {0x104D2,0x104FA},
         {0x104D3,0x104FB},
         {0x10C80,0x10CC0},
         {0x10C81,0x10CC1},
         {0x10C82,0x10CC2},
         {0x10C83,0x10CC3},
         {0x10C84,0x10CC4},
         {0x10C85,0x10CC5},
         {0x10C86,0x10CC6},
         {0x10C87,0x10CC7},
         {0x10C88,0x10CC8},
         {0x10C89,0x10CC9},
         {0x10C8A,0x10CCA},
         {0x10C8B,0x10CCB},
         {0x10C8C,0x10CCC},
         {0x10C8D,0x10CCD},
         {0x10C8E,0x10CCE},
         {0x10C8F,0x10CCF},
         {0x10C90,0x10CD0},
         {0x10C91,0x10CD1},
         {0x10C92,0x10CD2},
         {0x10C93,0x10CD3},
         {0x10C94,0x10CD4},
         {0x10C95,0x10CD5},
         {0x10C96,0x10CD6},
         {0x10C97,0x10CD7},
         {0x10C98,0x10CD8},
         {0x10C99,0x10CD9},
         {0x10C9A,0x10CDA},
         {0x10C9B,0x10CDB},
         {0x10C9C,0x10CDC},
         {0x10C9D,0x10CDD},
         {0x10C9E,0x10CDE},
         {0x10C9F,0x10CDF},
         {0x10CA0,0x10CE0},
         {0x10CA1,0x10CE1},
         {0x10CA2,0x10CE2},
         {0x10CA3,0x10CE3},
         {0x10CA4,0x10CE4},
         {0x10CA5,0x10CE5},
         {0x10CA6,0x10CE6},
         {0x10CA7,0x10CE7},
         {0x10CA8,0x10CE8},
         {0x10CA9,0x10CE9},
         {0x10CAA,0x10CEA},
         {0x10CAB,0x10CEB},
         {0x10CAC,0x10CEC},
         {0x10CAD,0x10CED},
         {0x10CAE,0x10CEE},
         {0x10CAF,0x10CEF},
         {0x10CB0,0x10CF0},
         {0x10CB1,0x10CF1},
         {0x10CB2,0x10CF2},
         {0x118A0,0x118C0},
         {0x118A1,0x118C1},
         {0x118A2,0x118C2},
         {0x118A3,0x118C3},
         {0x118A4,0x118C4},
         {0x118A5,0x118C5},
         {0x118A6,0x118C6},
         {0x118A7,0x118C7},
         {0x118A8,0x118C8},
         {0x118A9,0x118C9},
         {0x118AA,0x118CA},
         {0x118AB,0x118CB},
         {0x118AC,0x118CC},
         {0x118AD,0x118CD},
         {0x118AE,0x118CE},
         {0x118AF,0x118CF},
         {0x118B0,0x118D0},
         {0x118B1,0x118D1},
         {0x118B2,0x118D2},
         {0x118B3,0x118D3},
         {0x118B4,0x118D4},
         {0x118B5,0x118D5},
         {0x118B6,0x118D6},
         {0x118B7,0x118D7},
         {0x118B8,0x118D8},
         {0x118B9,0x118D9},
         {0x118BA,0x118DA},
         {0x118BB,0x118DB},
         {0x118BC,0x118DC},
         {0x118BD,0x118DD},
         {0x118BE,0x118DE},
         {0x118BF,0x118DF},
         {0x16E40,0x16E60},
         {0x16E41,0x16E61},
         {0x16E42,0x16E62},
         {0x16E43,0x16E63},
         {0x16E44,0x16E64},
         {0x16E45,0x16E65},
         {0x16E46,0x16E66},
         {0x16E47,0x16E67},
         {0x16E48,0x16E68},
         {0x16E49,0x16E69},
         {0x16E4A,0x16E6A},
         {0x16E4B,0x16E6B},
         {0x16E4C,0x16E6C},
         {0x16E4D,0x16E6D},
         {0x16E4E,0x16E6E},
         {0x16E4F,0x16E6F},
         {0x16E50,0x16E70},
         {0x16E51,0x16E71},
         {0x16E52,0x16E72},
         {0x16E53,0x16E73},
         {0x16E54,0x16E74},
         {0x16E55,0x16E75},
         {0x16E56,0x16E76},
         {0x16E57,0x16E77},
         {0x16E58,0x16E78},
         {0x16E59,0x16E79},
         {0x16E5A,0x16E7A},
         {0x16E5B,0x16E7B},
         {0x16E5C,0x16E7C},
         {0x16E5D,0x16E7D},
         {0x16E5E,0x16E7E},
         {0x16E5F,0x16E7F},
         {0x1E900,0x1E922},
         {0x1E901,0x1E923},
         {0x1E902,0x1E924},
         {0x1E903,0x1E925},
         {0x1E904,0x1E926},
         {0x1E905,0x1E927},
         {0x1E906,0x1E928},
         {0x1E907,0x1E929},
         {0x1E908,0x1E92A},
         {0x1E909,0x1E92B},
         {0x1E90A,0x1E92C},
         {0x1E90B,0x1E92D},
         {0x1E90C,0x1E92E},
         {0x1E90D,0x1E92F},
         {0x1E90E,0x1E930},
         {0x1E90F,0x1E931},
         {0x1E910,0x1E932},
         {0x1E911,0x1E933},
         {0x1E912,0x1E934},
         {0x1E913,0x1E935},
         {0x1E914,0x1E936},
         {0x1E915,0x1E937},
         {0x1E916,0x1E938},
         {0x1E917,0x1E939},
         {0x1E918,0x1E93A},
         {0x1E919,0x1E93B},
         {0x1E91A,0x1E93C},
         {0x1E91B,0x1E93D},
         {0x1E91C,0x1E93E},
         {0x1E91D,0x1E93F},
         {0x1E91E,0x1E940},
         {0x1E91F,0x1E941},
         {0x1E920,0x1E942},
         {0x1E921,0x1E943},
      };
      std::size_t first=0, last=sizeof(pairs)/sizeof(pairs[0]);
      while(first<last) {
         const std::size_t mid=first+(last-first)/2;
         if(pairs[mid].from<code) first=mid+1; else last=mid;
      }
      return first<sizeof(pairs)/sizeof(pairs[0]) && pairs[first].from==code ? pairs[first].to : code;
   }
   QString comparisonFolded() const {
      if(isEmpty()) return *this;
      std::wstring result=toStdWString();
      for(std::size_t i=0;i<result.size();++i) {
         char32_t code=static_cast<char32_t>(result[i]);
         if(code>=0xD800 && code<=0xDBFF && i+1<result.size() &&
            result[i+1]>=0xDC00 && result[i+1]<=0xDFFF) {
            code=0x10000+((code-0xD800)<<10)+(result[i+1]-0xDC00);
            code=comparisonFold(code)-0x10000;
            result[i]=static_cast<wchar_t>(0xD800+(code>>10));
            result[++i]=static_cast<wchar_t>(0xDC00+(code&0x3FF));
         } else result[i]=static_cast<wchar_t>(comparisonFold(code));
      }
      return QString(result);
   }

   std::u32string comparisonSearchKeys() const {
      // QStringMatcher folds each UTF-16 unit in its original context. A low
      // surrogate after a high surrogate has a full UCS-4 key, not a low-half
      // key that could also match an isolated surrogate in the pattern.
      std::u32string result;
      result.reserve(static_cast<std::size_t>(length()));
      const wchar_t* data=m_str.c_str();
      for(int i=0;i<length();++i) {
         char32_t code=static_cast<char32_t>(data[i]);
         if(code>=0xDC00 && code<=0xDFFF && i>0 && data[i-1]>=0xD800 && data[i-1]<=0xDBFF)
            code=0x10000+((static_cast<char32_t>(data[i-1])-0xD800)<<10)+(code-0xDC00);
         result.push_back(comparisonFold(code));
      }
      return result;
   }

   static char32_t searchKey(const wchar_t* units, int index) {
      char32_t code=static_cast<char32_t>(units[index]);
      if(code<0x80) return code>='A' && code<='Z' ? code+('a'-'A') : code;
      if(code>=0xDC00 && code<=0xDFFF && index>0 && units[index-1]>=0xD800 && units[index-1]<=0xDBFF)
         code=0x10000+((static_cast<char32_t>(units[index-1])-0xD800)<<10)+(code-0xDC00);
      return comparisonFold(code);
   }

   // Prepare only the pattern. Source keys are folded lazily, so an early
   // match or a search near the end never processes the unused source text.
   class PreparedSearch {
      std::vector<char32_t> pattern;
      std::vector<int> prefix;
      bool insensitive;
   public:
      PreparedSearch(const wchar_t* units, int length, Qt::CaseSensitivity cs, bool reverse=false)
         : pattern(static_cast<std::size_t>(length)),prefix(static_cast<std::size_t>(length),0),
           insensitive(cs!=Qt::CaseSensitive) {
         for(int i=0;i<length;++i) {
            const int source=reverse ? length-1-i : i;
            pattern[static_cast<std::size_t>(i)]=insensitive ? searchKey(units,source) : static_cast<char32_t>(units[source]);
         }
         for(int i=1,j=0;i<length;++i) {
            while(j && pattern[i]!=pattern[j]) j=prefix[j-1];
            if(pattern[i]==pattern[j]) ++j;
            prefix[i]=j;
         }
      }
      bool consume(const wchar_t* units, int index, int& matched) const {
         const char32_t key=insensitive ? searchKey(units,index) : static_cast<char32_t>(units[index]);
         while(matched && key!=pattern[matched]) matched=prefix[matched-1];
         if(key==pattern[matched]) ++matched;
         if(static_cast<std::size_t>(matched)!=pattern.size()) return false;
         matched=prefix.back();
         return true;
      }
      int forward(const wchar_t* units, int from, int end) const {
         for(int i=from,matched=0;i<end;++i)
            if(consume(units,i,matched)) return i-static_cast<int>(pattern.size())+1;
         return -1;
      }
      int backward(const wchar_t* units, int last) const {
         for(int i=last,matched=0;i>=0;--i)
            if(consume(units,i,matched)) return i;
         return -1;
      }
      int occurrences(const wchar_t* units, int length) const {
         int result=0;
         for(int i=0,matched=0;i<length;++i) if(consume(units,i,matched)) ++result;
         return result;
      }
   };

   static bool equalStringUnits(const wchar_t* left, const wchar_t* right, int count, Qt::CaseSensitivity cs) {
      if(!count) return true;
      if(cs==Qt::CaseSensitive) return std::equal(left,left+count,right);
      for(int i=0;i<count;++i) {
         char32_t a=left[i], b=right[i];
         const bool pairA=a>=0xD800 && a<=0xDBFF && i+1<count && left[i+1]>=0xDC00 && left[i+1]<=0xDFFF;
         const bool pairB=b>=0xD800 && b<=0xDBFF && i+1<count && right[i+1]>=0xDC00 && right[i+1]<=0xDFFF;
         if(pairA!=pairB) return false;
         if(pairA) { a=0x10000+((a-0xD800)<<10)+(left[i+1]-0xDC00); b=0x10000+((b-0xD800)<<10)+(right[i+1]-0xDC00); ++i; }
         if(comparisonFold(a)!=comparisonFold(b)) return false;
      }
      return true;
   }

   std::string numericToken() const {
      // Qt's C-locale parser accepts U+2212 for both the value and exponent sign.
      QString text=trimmed();
      for(int i=0;i<text.length();++i)
         if(text.at(i).unicode()==0x2212) text.m_str[i+1]=L'-';
      return text.toUtf8();
   }

   // Unicode 13.0 UnicodeData.txt + unconditional SpecialCasing.txt mappings.
   QString convertedCase(bool upper) const {
      struct Mapping { char32_t code; unsigned short offset; unsigned char size; };
      static const Mapping upperMap[] = {
         {0x61,0,1},
         {0x62,1,1},
         {0x63,2,1},
         {0x64,3,1},
         {0x65,4,1},
         {0x66,5,1},
         {0x67,6,1},
         {0x68,7,1},
         {0x69,8,1},
         {0x6A,9,1},
         {0x6B,10,1},
         {0x6C,11,1},
         {0x6D,12,1},
         {0x6E,13,1},
         {0x6F,14,1},
         {0x70,15,1},
         {0x71,16,1},
         {0x72,17,1},
         {0x73,18,1},
         {0x74,19,1},
         {0x75,20,1},
         {0x76,21,1},
         {0x77,22,1},
         {0x78,23,1},
         {0x79,24,1},
         {0x7A,25,1},
         {0xB5,26,1},
         {0xDF,27,2},
         {0xE0,29,1},
         {0xE1,30,1},
         {0xE2,31,1},
         {0xE3,32,1},
         {0xE4,33,1},
         {0xE5,34,1},
         {0xE6,35,1},
         {0xE7,36,1},
         {0xE8,37,1},
         {0xE9,38,1},
         {0xEA,39,1},
         {0xEB,40,1},
         {0xEC,41,1},
         {0xED,42,1},
         {0xEE,43,1},
         {0xEF,44,1},
         {0xF0,45,1},
         {0xF1,46,1},
         {0xF2,47,1},
         {0xF3,48,1},
         {0xF4,49,1},
         {0xF5,50,1},
         {0xF6,51,1},
         {0xF8,52,1},
         {0xF9,53,1},
         {0xFA,54,1},
         {0xFB,55,1},
         {0xFC,56,1},
         {0xFD,57,1},
         {0xFE,58,1},
         {0xFF,59,1},
         {0x101,60,1},
         {0x103,61,1},
         {0x105,62,1},
         {0x107,63,1},
         {0x109,64,1},
         {0x10B,65,1},
         {0x10D,66,1},
         {0x10F,67,1},
         {0x111,68,1},
         {0x113,69,1},
         {0x115,70,1},
         {0x117,71,1},
         {0x119,72,1},
         {0x11B,73,1},
         {0x11D,74,1},
         {0x11F,75,1},
         {0x121,76,1},
         {0x123,77,1},
         {0x125,78,1},
         {0x127,79,1},
         {0x129,80,1},
         {0x12B,81,1},
         {0x12D,82,1},
         {0x12F,83,1},
         {0x131,84,1},
         {0x133,85,1},
         {0x135,86,1},
         {0x137,87,1},
         {0x13A,88,1},
         {0x13C,89,1},
         {0x13E,90,1},
         {0x140,91,1},
         {0x142,92,1},
         {0x144,93,1},
         {0x146,94,1},
         {0x148,95,1},
         {0x149,96,2},
         {0x14B,98,1},
         {0x14D,99,1},
         {0x14F,100,1},
         {0x151,101,1},
         {0x153,102,1},
         {0x155,103,1},
         {0x157,104,1},
         {0x159,105,1},
         {0x15B,106,1},
         {0x15D,107,1},
         {0x15F,108,1},
         {0x161,109,1},
         {0x163,110,1},
         {0x165,111,1},
         {0x167,112,1},
         {0x169,113,1},
         {0x16B,114,1},
         {0x16D,115,1},
         {0x16F,116,1},
         {0x171,117,1},
         {0x173,118,1},
         {0x175,119,1},
         {0x177,120,1},
         {0x17A,121,1},
         {0x17C,122,1},
         {0x17E,123,1},
         {0x17F,124,1},
         {0x180,125,1},
         {0x183,126,1},
         {0x185,127,1},
         {0x188,128,1},
         {0x18C,129,1},
         {0x192,130,1},
         {0x195,131,1},
         {0x199,132,1},
         {0x19A,133,1},
         {0x19E,134,1},
         {0x1A1,135,1},
         {0x1A3,136,1},
         {0x1A5,137,1},
         {0x1A8,138,1},
         {0x1AD,139,1},
         {0x1B0,140,1},
         {0x1B4,141,1},
         {0x1B6,142,1},
         {0x1B9,143,1},
         {0x1BD,144,1},
         {0x1BF,145,1},
         {0x1C5,146,1},
         {0x1C6,147,1},
         {0x1C8,148,1},
         {0x1C9,149,1},
         {0x1CB,150,1},
         {0x1CC,151,1},
         {0x1CE,152,1},
         {0x1D0,153,1},
         {0x1D2,154,1},
         {0x1D4,155,1},
         {0x1D6,156,1},
         {0x1D8,157,1},
         {0x1DA,158,1},
         {0x1DC,159,1},
         {0x1DD,160,1},
         {0x1DF,161,1},
         {0x1E1,162,1},
         {0x1E3,163,1},
         {0x1E5,164,1},
         {0x1E7,165,1},
         {0x1E9,166,1},
         {0x1EB,167,1},
         {0x1ED,168,1},
         {0x1EF,169,1},
         {0x1F0,170,2},
         {0x1F2,172,1},
         {0x1F3,173,1},
         {0x1F5,174,1},
         {0x1F9,175,1},
         {0x1FB,176,1},
         {0x1FD,177,1},
         {0x1FF,178,1},
         {0x201,179,1},
         {0x203,180,1},
         {0x205,181,1},
         {0x207,182,1},
         {0x209,183,1},
         {0x20B,184,1},
         {0x20D,185,1},
         {0x20F,186,1},
         {0x211,187,1},
         {0x213,188,1},
         {0x215,189,1},
         {0x217,190,1},
         {0x219,191,1},
         {0x21B,192,1},
         {0x21D,193,1},
         {0x21F,194,1},
         {0x223,195,1},
         {0x225,196,1},
         {0x227,197,1},
         {0x229,198,1},
         {0x22B,199,1},
         {0x22D,200,1},
         {0x22F,201,1},
         {0x231,202,1},
         {0x233,203,1},
         {0x23C,204,1},
         {0x23F,205,1},
         {0x240,206,1},
         {0x242,207,1},
         {0x247,208,1},
         {0x249,209,1},
         {0x24B,210,1},
         {0x24D,211,1},
         {0x24F,212,1},
         {0x250,213,1},
         {0x251,214,1},
         {0x252,215,1},
         {0x253,216,1},
         {0x254,217,1},
         {0x256,218,1},
         {0x257,219,1},
         {0x259,220,1},
         {0x25B,221,1},
         {0x25C,222,1},
         {0x260,223,1},
         {0x261,224,1},
         {0x263,225,1},
         {0x265,226,1},
         {0x266,227,1},
         {0x268,228,1},
         {0x269,229,1},
         {0x26A,230,1},
         {0x26B,231,1},
         {0x26C,232,1},
         {0x26F,233,1},
         {0x271,234,1},
         {0x272,235,1},
         {0x275,236,1},
         {0x27D,237,1},
         {0x280,238,1},
         {0x282,239,1},
         {0x283,240,1},
         {0x287,241,1},
         {0x288,242,1},
         {0x289,243,1},
         {0x28A,244,1},
         {0x28B,245,1},
         {0x28C,246,1},
         {0x292,247,1},
         {0x29D,248,1},
         {0x29E,249,1},
         {0x345,250,1},
         {0x371,251,1},
         {0x373,252,1},
         {0x377,253,1},
         {0x37B,254,1},
         {0x37C,255,1},
         {0x37D,256,1},
         {0x390,257,3},
         {0x3AC,260,1},
         {0x3AD,261,1},
         {0x3AE,262,1},
         {0x3AF,263,1},
         {0x3B0,264,3},
         {0x3B1,267,1},
         {0x3B2,268,1},
         {0x3B3,269,1},
         {0x3B4,270,1},
         {0x3B5,271,1},
         {0x3B6,272,1},
         {0x3B7,273,1},
         {0x3B8,274,1},
         {0x3B9,275,1},
         {0x3BA,276,1},
         {0x3BB,277,1},
         {0x3BC,278,1},
         {0x3BD,279,1},
         {0x3BE,280,1},
         {0x3BF,281,1},
         {0x3C0,282,1},
         {0x3C1,283,1},
         {0x3C2,284,1},
         {0x3C3,285,1},
         {0x3C4,286,1},
         {0x3C5,287,1},
         {0x3C6,288,1},
         {0x3C7,289,1},
         {0x3C8,290,1},
         {0x3C9,291,1},
         {0x3CA,292,1},
         {0x3CB,293,1},
         {0x3CC,294,1},
         {0x3CD,295,1},
         {0x3CE,296,1},
         {0x3D0,297,1},
         {0x3D1,298,1},
         {0x3D5,299,1},
         {0x3D6,300,1},
         {0x3D7,301,1},
         {0x3D9,302,1},
         {0x3DB,303,1},
         {0x3DD,304,1},
         {0x3DF,305,1},
         {0x3E1,306,1},
         {0x3E3,307,1},
         {0x3E5,308,1},
         {0x3E7,309,1},
         {0x3E9,310,1},
         {0x3EB,311,1},
         {0x3ED,312,1},
         {0x3EF,313,1},
         {0x3F0,314,1},
         {0x3F1,315,1},
         {0x3F2,316,1},
         {0x3F3,317,1},
         {0x3F5,318,1},
         {0x3F8,319,1},
         {0x3FB,320,1},
         {0x430,321,1},
         {0x431,322,1},
         {0x432,323,1},
         {0x433,324,1},
         {0x434,325,1},
         {0x435,326,1},
         {0x436,327,1},
         {0x437,328,1},
         {0x438,329,1},
         {0x439,330,1},
         {0x43A,331,1},
         {0x43B,332,1},
         {0x43C,333,1},
         {0x43D,334,1},
         {0x43E,335,1},
         {0x43F,336,1},
         {0x440,337,1},
         {0x441,338,1},
         {0x442,339,1},
         {0x443,340,1},
         {0x444,341,1},
         {0x445,342,1},
         {0x446,343,1},
         {0x447,344,1},
         {0x448,345,1},
         {0x449,346,1},
         {0x44A,347,1},
         {0x44B,348,1},
         {0x44C,349,1},
         {0x44D,350,1},
         {0x44E,351,1},
         {0x44F,352,1},
         {0x450,353,1},
         {0x451,354,1},
         {0x452,355,1},
         {0x453,356,1},
         {0x454,357,1},
         {0x455,358,1},
         {0x456,359,1},
         {0x457,360,1},
         {0x458,361,1},
         {0x459,362,1},
         {0x45A,363,1},
         {0x45B,364,1},
         {0x45C,365,1},
         {0x45D,366,1},
         {0x45E,367,1},
         {0x45F,368,1},
         {0x461,369,1},
         {0x463,370,1},
         {0x465,371,1},
         {0x467,372,1},
         {0x469,373,1},
         {0x46B,374,1},
         {0x46D,375,1},
         {0x46F,376,1},
         {0x471,377,1},
         {0x473,378,1},
         {0x475,379,1},
         {0x477,380,1},
         {0x479,381,1},
         {0x47B,382,1},
         {0x47D,383,1},
         {0x47F,384,1},
         {0x481,385,1},
         {0x48B,386,1},
         {0x48D,387,1},
         {0x48F,388,1},
         {0x491,389,1},
         {0x493,390,1},
         {0x495,391,1},
         {0x497,392,1},
         {0x499,393,1},
         {0x49B,394,1},
         {0x49D,395,1},
         {0x49F,396,1},
         {0x4A1,397,1},
         {0x4A3,398,1},
         {0x4A5,399,1},
         {0x4A7,400,1},
         {0x4A9,401,1},
         {0x4AB,402,1},
         {0x4AD,403,1},
         {0x4AF,404,1},
         {0x4B1,405,1},
         {0x4B3,406,1},
         {0x4B5,407,1},
         {0x4B7,408,1},
         {0x4B9,409,1},
         {0x4BB,410,1},
         {0x4BD,411,1},
         {0x4BF,412,1},
         {0x4C2,413,1},
         {0x4C4,414,1},
         {0x4C6,415,1},
         {0x4C8,416,1},
         {0x4CA,417,1},
         {0x4CC,418,1},
         {0x4CE,419,1},
         {0x4CF,420,1},
         {0x4D1,421,1},
         {0x4D3,422,1},
         {0x4D5,423,1},
         {0x4D7,424,1},
         {0x4D9,425,1},
         {0x4DB,426,1},
         {0x4DD,427,1},
         {0x4DF,428,1},
         {0x4E1,429,1},
         {0x4E3,430,1},
         {0x4E5,431,1},
         {0x4E7,432,1},
         {0x4E9,433,1},
         {0x4EB,434,1},
         {0x4ED,435,1},
         {0x4EF,436,1},
         {0x4F1,437,1},
         {0x4F3,438,1},
         {0x4F5,439,1},
         {0x4F7,440,1},
         {0x4F9,441,1},
         {0x4FB,442,1},
         {0x4FD,443,1},
         {0x4FF,444,1},
         {0x501,445,1},
         {0x503,446,1},
         {0x505,447,1},
         {0x507,448,1},
         {0x509,449,1},
         {0x50B,450,1},
         {0x50D,451,1},
         {0x50F,452,1},
         {0x511,453,1},
         {0x513,454,1},
         {0x515,455,1},
         {0x517,456,1},
         {0x519,457,1},
         {0x51B,458,1},
         {0x51D,459,1},
         {0x51F,460,1},
         {0x521,461,1},
         {0x523,462,1},
         {0x525,463,1},
         {0x527,464,1},
         {0x529,465,1},
         {0x52B,466,1},
         {0x52D,467,1},
         {0x52F,468,1},
         {0x561,469,1},
         {0x562,470,1},
         {0x563,471,1},
         {0x564,472,1},
         {0x565,473,1},
         {0x566,474,1},
         {0x567,475,1},
         {0x568,476,1},
         {0x569,477,1},
         {0x56A,478,1},
         {0x56B,479,1},
         {0x56C,480,1},
         {0x56D,481,1},
         {0x56E,482,1},
         {0x56F,483,1},
         {0x570,484,1},
         {0x571,485,1},
         {0x572,486,1},
         {0x573,487,1},
         {0x574,488,1},
         {0x575,489,1},
         {0x576,490,1},
         {0x577,491,1},
         {0x578,492,1},
         {0x579,493,1},
         {0x57A,494,1},
         {0x57B,495,1},
         {0x57C,496,1},
         {0x57D,497,1},
         {0x57E,498,1},
         {0x57F,499,1},
         {0x580,500,1},
         {0x581,501,1},
         {0x582,502,1},
         {0x583,503,1},
         {0x584,504,1},
         {0x585,505,1},
         {0x586,506,1},
         {0x587,507,2},
         {0x10D0,509,1},
         {0x10D1,510,1},
         {0x10D2,511,1},
         {0x10D3,512,1},
         {0x10D4,513,1},
         {0x10D5,514,1},
         {0x10D6,515,1},
         {0x10D7,516,1},
         {0x10D8,517,1},
         {0x10D9,518,1},
         {0x10DA,519,1},
         {0x10DB,520,1},
         {0x10DC,521,1},
         {0x10DD,522,1},
         {0x10DE,523,1},
         {0x10DF,524,1},
         {0x10E0,525,1},
         {0x10E1,526,1},
         {0x10E2,527,1},
         {0x10E3,528,1},
         {0x10E4,529,1},
         {0x10E5,530,1},
         {0x10E6,531,1},
         {0x10E7,532,1},
         {0x10E8,533,1},
         {0x10E9,534,1},
         {0x10EA,535,1},
         {0x10EB,536,1},
         {0x10EC,537,1},
         {0x10ED,538,1},
         {0x10EE,539,1},
         {0x10EF,540,1},
         {0x10F0,541,1},
         {0x10F1,542,1},
         {0x10F2,543,1},
         {0x10F3,544,1},
         {0x10F4,545,1},
         {0x10F5,546,1},
         {0x10F6,547,1},
         {0x10F7,548,1},
         {0x10F8,549,1},
         {0x10F9,550,1},
         {0x10FA,551,1},
         {0x10FD,552,1},
         {0x10FE,553,1},
         {0x10FF,554,1},
         {0x13F8,555,1},
         {0x13F9,556,1},
         {0x13FA,557,1},
         {0x13FB,558,1},
         {0x13FC,559,1},
         {0x13FD,560,1},
         {0x1C80,561,1},
         {0x1C81,562,1},
         {0x1C82,563,1},
         {0x1C83,564,1},
         {0x1C84,565,1},
         {0x1C85,566,1},
         {0x1C86,567,1},
         {0x1C87,568,1},
         {0x1C88,569,1},
         {0x1D79,570,1},
         {0x1D7D,571,1},
         {0x1D8E,572,1},
         {0x1E01,573,1},
         {0x1E03,574,1},
         {0x1E05,575,1},
         {0x1E07,576,1},
         {0x1E09,577,1},
         {0x1E0B,578,1},
         {0x1E0D,579,1},
         {0x1E0F,580,1},
         {0x1E11,581,1},
         {0x1E13,582,1},
         {0x1E15,583,1},
         {0x1E17,584,1},
         {0x1E19,585,1},
         {0x1E1B,586,1},
         {0x1E1D,587,1},
         {0x1E1F,588,1},
         {0x1E21,589,1},
         {0x1E23,590,1},
         {0x1E25,591,1},
         {0x1E27,592,1},
         {0x1E29,593,1},
         {0x1E2B,594,1},
         {0x1E2D,595,1},
         {0x1E2F,596,1},
         {0x1E31,597,1},
         {0x1E33,598,1},
         {0x1E35,599,1},
         {0x1E37,600,1},
         {0x1E39,601,1},
         {0x1E3B,602,1},
         {0x1E3D,603,1},
         {0x1E3F,604,1},
         {0x1E41,605,1},
         {0x1E43,606,1},
         {0x1E45,607,1},
         {0x1E47,608,1},
         {0x1E49,609,1},
         {0x1E4B,610,1},
         {0x1E4D,611,1},
         {0x1E4F,612,1},
         {0x1E51,613,1},
         {0x1E53,614,1},
         {0x1E55,615,1},
         {0x1E57,616,1},
         {0x1E59,617,1},
         {0x1E5B,618,1},
         {0x1E5D,619,1},
         {0x1E5F,620,1},
         {0x1E61,621,1},
         {0x1E63,622,1},
         {0x1E65,623,1},
         {0x1E67,624,1},
         {0x1E69,625,1},
         {0x1E6B,626,1},
         {0x1E6D,627,1},
         {0x1E6F,628,1},
         {0x1E71,629,1},
         {0x1E73,630,1},
         {0x1E75,631,1},
         {0x1E77,632,1},
         {0x1E79,633,1},
         {0x1E7B,634,1},
         {0x1E7D,635,1},
         {0x1E7F,636,1},
         {0x1E81,637,1},
         {0x1E83,638,1},
         {0x1E85,639,1},
         {0x1E87,640,1},
         {0x1E89,641,1},
         {0x1E8B,642,1},
         {0x1E8D,643,1},
         {0x1E8F,644,1},
         {0x1E91,645,1},
         {0x1E93,646,1},
         {0x1E95,647,1},
         {0x1E96,648,2},
         {0x1E97,650,2},
         {0x1E98,652,2},
         {0x1E99,654,2},
         {0x1E9A,656,2},
         {0x1E9B,658,1},
         {0x1EA1,659,1},
         {0x1EA3,660,1},
         {0x1EA5,661,1},
         {0x1EA7,662,1},
         {0x1EA9,663,1},
         {0x1EAB,664,1},
         {0x1EAD,665,1},
         {0x1EAF,666,1},
         {0x1EB1,667,1},
         {0x1EB3,668,1},
         {0x1EB5,669,1},
         {0x1EB7,670,1},
         {0x1EB9,671,1},
         {0x1EBB,672,1},
         {0x1EBD,673,1},
         {0x1EBF,674,1},
         {0x1EC1,675,1},
         {0x1EC3,676,1},
         {0x1EC5,677,1},
         {0x1EC7,678,1},
         {0x1EC9,679,1},
         {0x1ECB,680,1},
         {0x1ECD,681,1},
         {0x1ECF,682,1},
         {0x1ED1,683,1},
         {0x1ED3,684,1},
         {0x1ED5,685,1},
         {0x1ED7,686,1},
         {0x1ED9,687,1},
         {0x1EDB,688,1},
         {0x1EDD,689,1},
         {0x1EDF,690,1},
         {0x1EE1,691,1},
         {0x1EE3,692,1},
         {0x1EE5,693,1},
         {0x1EE7,694,1},
         {0x1EE9,695,1},
         {0x1EEB,696,1},
         {0x1EED,697,1},
         {0x1EEF,698,1},
         {0x1EF1,699,1},
         {0x1EF3,700,1},
         {0x1EF5,701,1},
         {0x1EF7,702,1},
         {0x1EF9,703,1},
         {0x1EFB,704,1},
         {0x1EFD,705,1},
         {0x1EFF,706,1},
         {0x1F00,707,1},
         {0x1F01,708,1},
         {0x1F02,709,1},
         {0x1F03,710,1},
         {0x1F04,711,1},
         {0x1F05,712,1},
         {0x1F06,713,1},
         {0x1F07,714,1},
         {0x1F10,715,1},
         {0x1F11,716,1},
         {0x1F12,717,1},
         {0x1F13,718,1},
         {0x1F14,719,1},
         {0x1F15,720,1},
         {0x1F20,721,1},
         {0x1F21,722,1},
         {0x1F22,723,1},
         {0x1F23,724,1},
         {0x1F24,725,1},
         {0x1F25,726,1},
         {0x1F26,727,1},
         {0x1F27,728,1},
         {0x1F30,729,1},
         {0x1F31,730,1},
         {0x1F32,731,1},
         {0x1F33,732,1},
         {0x1F34,733,1},
         {0x1F35,734,1},
         {0x1F36,735,1},
         {0x1F37,736,1},
         {0x1F40,737,1},
         {0x1F41,738,1},
         {0x1F42,739,1},
         {0x1F43,740,1},
         {0x1F44,741,1},
         {0x1F45,742,1},
         {0x1F50,743,2},
         {0x1F51,745,1},
         {0x1F52,746,3},
         {0x1F53,749,1},
         {0x1F54,750,3},
         {0x1F55,753,1},
         {0x1F56,754,3},
         {0x1F57,757,1},
         {0x1F60,758,1},
         {0x1F61,759,1},
         {0x1F62,760,1},
         {0x1F63,761,1},
         {0x1F64,762,1},
         {0x1F65,763,1},
         {0x1F66,764,1},
         {0x1F67,765,1},
         {0x1F70,766,1},
         {0x1F71,767,1},
         {0x1F72,768,1},
         {0x1F73,769,1},
         {0x1F74,770,1},
         {0x1F75,771,1},
         {0x1F76,772,1},
         {0x1F77,773,1},
         {0x1F78,774,1},
         {0x1F79,775,1},
         {0x1F7A,776,1},
         {0x1F7B,777,1},
         {0x1F7C,778,1},
         {0x1F7D,779,1},
         {0x1F80,780,2},
         {0x1F81,782,2},
         {0x1F82,784,2},
         {0x1F83,786,2},
         {0x1F84,788,2},
         {0x1F85,790,2},
         {0x1F86,792,2},
         {0x1F87,794,2},
         {0x1F88,796,2},
         {0x1F89,798,2},
         {0x1F8A,800,2},
         {0x1F8B,802,2},
         {0x1F8C,804,2},
         {0x1F8D,806,2},
         {0x1F8E,808,2},
         {0x1F8F,810,2},
         {0x1F90,812,2},
         {0x1F91,814,2},
         {0x1F92,816,2},
         {0x1F93,818,2},
         {0x1F94,820,2},
         {0x1F95,822,2},
         {0x1F96,824,2},
         {0x1F97,826,2},
         {0x1F98,828,2},
         {0x1F99,830,2},
         {0x1F9A,832,2},
         {0x1F9B,834,2},
         {0x1F9C,836,2},
         {0x1F9D,838,2},
         {0x1F9E,840,2},
         {0x1F9F,842,2},
         {0x1FA0,844,2},
         {0x1FA1,846,2},
         {0x1FA2,848,2},
         {0x1FA3,850,2},
         {0x1FA4,852,2},
         {0x1FA5,854,2},
         {0x1FA6,856,2},
         {0x1FA7,858,2},
         {0x1FA8,860,2},
         {0x1FA9,862,2},
         {0x1FAA,864,2},
         {0x1FAB,866,2},
         {0x1FAC,868,2},
         {0x1FAD,870,2},
         {0x1FAE,872,2},
         {0x1FAF,874,2},
         {0x1FB0,876,1},
         {0x1FB1,877,1},
         {0x1FB2,878,2},
         {0x1FB3,880,2},
         {0x1FB4,882,2},
         {0x1FB6,884,2},
         {0x1FB7,886,3},
         {0x1FBC,889,2},
         {0x1FBE,891,1},
         {0x1FC2,892,2},
         {0x1FC3,894,2},
         {0x1FC4,896,2},
         {0x1FC6,898,2},
         {0x1FC7,900,3},
         {0x1FCC,903,2},
         {0x1FD0,905,1},
         {0x1FD1,906,1},
         {0x1FD2,907,3},
         {0x1FD3,910,3},
         {0x1FD6,913,2},
         {0x1FD7,915,3},
         {0x1FE0,918,1},
         {0x1FE1,919,1},
         {0x1FE2,920,3},
         {0x1FE3,923,3},
         {0x1FE4,926,2},
         {0x1FE5,928,1},
         {0x1FE6,929,2},
         {0x1FE7,931,3},
         {0x1FF2,934,2},
         {0x1FF3,936,2},
         {0x1FF4,938,2},
         {0x1FF6,940,2},
         {0x1FF7,942,3},
         {0x1FFC,945,2},
         {0x214E,947,1},
         {0x2170,948,1},
         {0x2171,949,1},
         {0x2172,950,1},
         {0x2173,951,1},
         {0x2174,952,1},
         {0x2175,953,1},
         {0x2176,954,1},
         {0x2177,955,1},
         {0x2178,956,1},
         {0x2179,957,1},
         {0x217A,958,1},
         {0x217B,959,1},
         {0x217C,960,1},
         {0x217D,961,1},
         {0x217E,962,1},
         {0x217F,963,1},
         {0x2184,964,1},
         {0x24D0,965,1},
         {0x24D1,966,1},
         {0x24D2,967,1},
         {0x24D3,968,1},
         {0x24D4,969,1},
         {0x24D5,970,1},
         {0x24D6,971,1},
         {0x24D7,972,1},
         {0x24D8,973,1},
         {0x24D9,974,1},
         {0x24DA,975,1},
         {0x24DB,976,1},
         {0x24DC,977,1},
         {0x24DD,978,1},
         {0x24DE,979,1},
         {0x24DF,980,1},
         {0x24E0,981,1},
         {0x24E1,982,1},
         {0x24E2,983,1},
         {0x24E3,984,1},
         {0x24E4,985,1},
         {0x24E5,986,1},
         {0x24E6,987,1},
         {0x24E7,988,1},
         {0x24E8,989,1},
         {0x24E9,990,1},
         {0x2C30,991,1},
         {0x2C31,992,1},
         {0x2C32,993,1},
         {0x2C33,994,1},
         {0x2C34,995,1},
         {0x2C35,996,1},
         {0x2C36,997,1},
         {0x2C37,998,1},
         {0x2C38,999,1},
         {0x2C39,1000,1},
         {0x2C3A,1001,1},
         {0x2C3B,1002,1},
         {0x2C3C,1003,1},
         {0x2C3D,1004,1},
         {0x2C3E,1005,1},
         {0x2C3F,1006,1},
         {0x2C40,1007,1},
         {0x2C41,1008,1},
         {0x2C42,1009,1},
         {0x2C43,1010,1},
         {0x2C44,1011,1},
         {0x2C45,1012,1},
         {0x2C46,1013,1},
         {0x2C47,1014,1},
         {0x2C48,1015,1},
         {0x2C49,1016,1},
         {0x2C4A,1017,1},
         {0x2C4B,1018,1},
         {0x2C4C,1019,1},
         {0x2C4D,1020,1},
         {0x2C4E,1021,1},
         {0x2C4F,1022,1},
         {0x2C50,1023,1},
         {0x2C51,1024,1},
         {0x2C52,1025,1},
         {0x2C53,1026,1},
         {0x2C54,1027,1},
         {0x2C55,1028,1},
         {0x2C56,1029,1},
         {0x2C57,1030,1},
         {0x2C58,1031,1},
         {0x2C59,1032,1},
         {0x2C5A,1033,1},
         {0x2C5B,1034,1},
         {0x2C5C,1035,1},
         {0x2C5D,1036,1},
         {0x2C5E,1037,1},
         {0x2C61,1038,1},
         {0x2C65,1039,1},
         {0x2C66,1040,1},
         {0x2C68,1041,1},
         {0x2C6A,1042,1},
         {0x2C6C,1043,1},
         {0x2C73,1044,1},
         {0x2C76,1045,1},
         {0x2C81,1046,1},
         {0x2C83,1047,1},
         {0x2C85,1048,1},
         {0x2C87,1049,1},
         {0x2C89,1050,1},
         {0x2C8B,1051,1},
         {0x2C8D,1052,1},
         {0x2C8F,1053,1},
         {0x2C91,1054,1},
         {0x2C93,1055,1},
         {0x2C95,1056,1},
         {0x2C97,1057,1},
         {0x2C99,1058,1},
         {0x2C9B,1059,1},
         {0x2C9D,1060,1},
         {0x2C9F,1061,1},
         {0x2CA1,1062,1},
         {0x2CA3,1063,1},
         {0x2CA5,1064,1},
         {0x2CA7,1065,1},
         {0x2CA9,1066,1},
         {0x2CAB,1067,1},
         {0x2CAD,1068,1},
         {0x2CAF,1069,1},
         {0x2CB1,1070,1},
         {0x2CB3,1071,1},
         {0x2CB5,1072,1},
         {0x2CB7,1073,1},
         {0x2CB9,1074,1},
         {0x2CBB,1075,1},
         {0x2CBD,1076,1},
         {0x2CBF,1077,1},
         {0x2CC1,1078,1},
         {0x2CC3,1079,1},
         {0x2CC5,1080,1},
         {0x2CC7,1081,1},
         {0x2CC9,1082,1},
         {0x2CCB,1083,1},
         {0x2CCD,1084,1},
         {0x2CCF,1085,1},
         {0x2CD1,1086,1},
         {0x2CD3,1087,1},
         {0x2CD5,1088,1},
         {0x2CD7,1089,1},
         {0x2CD9,1090,1},
         {0x2CDB,1091,1},
         {0x2CDD,1092,1},
         {0x2CDF,1093,1},
         {0x2CE1,1094,1},
         {0x2CE3,1095,1},
         {0x2CEC,1096,1},
         {0x2CEE,1097,1},
         {0x2CF3,1098,1},
         {0x2D00,1099,1},
         {0x2D01,1100,1},
         {0x2D02,1101,1},
         {0x2D03,1102,1},
         {0x2D04,1103,1},
         {0x2D05,1104,1},
         {0x2D06,1105,1},
         {0x2D07,1106,1},
         {0x2D08,1107,1},
         {0x2D09,1108,1},
         {0x2D0A,1109,1},
         {0x2D0B,1110,1},
         {0x2D0C,1111,1},
         {0x2D0D,1112,1},
         {0x2D0E,1113,1},
         {0x2D0F,1114,1},
         {0x2D10,1115,1},
         {0x2D11,1116,1},
         {0x2D12,1117,1},
         {0x2D13,1118,1},
         {0x2D14,1119,1},
         {0x2D15,1120,1},
         {0x2D16,1121,1},
         {0x2D17,1122,1},
         {0x2D18,1123,1},
         {0x2D19,1124,1},
         {0x2D1A,1125,1},
         {0x2D1B,1126,1},
         {0x2D1C,1127,1},
         {0x2D1D,1128,1},
         {0x2D1E,1129,1},
         {0x2D1F,1130,1},
         {0x2D20,1131,1},
         {0x2D21,1132,1},
         {0x2D22,1133,1},
         {0x2D23,1134,1},
         {0x2D24,1135,1},
         {0x2D25,1136,1},
         {0x2D27,1137,1},
         {0x2D2D,1138,1},
         {0xA641,1139,1},
         {0xA643,1140,1},
         {0xA645,1141,1},
         {0xA647,1142,1},
         {0xA649,1143,1},
         {0xA64B,1144,1},
         {0xA64D,1145,1},
         {0xA64F,1146,1},
         {0xA651,1147,1},
         {0xA653,1148,1},
         {0xA655,1149,1},
         {0xA657,1150,1},
         {0xA659,1151,1},
         {0xA65B,1152,1},
         {0xA65D,1153,1},
         {0xA65F,1154,1},
         {0xA661,1155,1},
         {0xA663,1156,1},
         {0xA665,1157,1},
         {0xA667,1158,1},
         {0xA669,1159,1},
         {0xA66B,1160,1},
         {0xA66D,1161,1},
         {0xA681,1162,1},
         {0xA683,1163,1},
         {0xA685,1164,1},
         {0xA687,1165,1},
         {0xA689,1166,1},
         {0xA68B,1167,1},
         {0xA68D,1168,1},
         {0xA68F,1169,1},
         {0xA691,1170,1},
         {0xA693,1171,1},
         {0xA695,1172,1},
         {0xA697,1173,1},
         {0xA699,1174,1},
         {0xA69B,1175,1},
         {0xA723,1176,1},
         {0xA725,1177,1},
         {0xA727,1178,1},
         {0xA729,1179,1},
         {0xA72B,1180,1},
         {0xA72D,1181,1},
         {0xA72F,1182,1},
         {0xA733,1183,1},
         {0xA735,1184,1},
         {0xA737,1185,1},
         {0xA739,1186,1},
         {0xA73B,1187,1},
         {0xA73D,1188,1},
         {0xA73F,1189,1},
         {0xA741,1190,1},
         {0xA743,1191,1},
         {0xA745,1192,1},
         {0xA747,1193,1},
         {0xA749,1194,1},
         {0xA74B,1195,1},
         {0xA74D,1196,1},
         {0xA74F,1197,1},
         {0xA751,1198,1},
         {0xA753,1199,1},
         {0xA755,1200,1},
         {0xA757,1201,1},
         {0xA759,1202,1},
         {0xA75B,1203,1},
         {0xA75D,1204,1},
         {0xA75F,1205,1},
         {0xA761,1206,1},
         {0xA763,1207,1},
         {0xA765,1208,1},
         {0xA767,1209,1},
         {0xA769,1210,1},
         {0xA76B,1211,1},
         {0xA76D,1212,1},
         {0xA76F,1213,1},
         {0xA77A,1214,1},
         {0xA77C,1215,1},
         {0xA77F,1216,1},
         {0xA781,1217,1},
         {0xA783,1218,1},
         {0xA785,1219,1},
         {0xA787,1220,1},
         {0xA78C,1221,1},
         {0xA791,1222,1},
         {0xA793,1223,1},
         {0xA794,1224,1},
         {0xA797,1225,1},
         {0xA799,1226,1},
         {0xA79B,1227,1},
         {0xA79D,1228,1},
         {0xA79F,1229,1},
         {0xA7A1,1230,1},
         {0xA7A3,1231,1},
         {0xA7A5,1232,1},
         {0xA7A7,1233,1},
         {0xA7A9,1234,1},
         {0xA7B5,1235,1},
         {0xA7B7,1236,1},
         {0xA7B9,1237,1},
         {0xA7BB,1238,1},
         {0xA7BD,1239,1},
         {0xA7BF,1240,1},
         {0xA7C3,1241,1},
         {0xA7C8,1242,1},
         {0xA7CA,1243,1},
         {0xA7F6,1244,1},
         {0xAB53,1245,1},
         {0xAB70,1246,1},
         {0xAB71,1247,1},
         {0xAB72,1248,1},
         {0xAB73,1249,1},
         {0xAB74,1250,1},
         {0xAB75,1251,1},
         {0xAB76,1252,1},
         {0xAB77,1253,1},
         {0xAB78,1254,1},
         {0xAB79,1255,1},
         {0xAB7A,1256,1},
         {0xAB7B,1257,1},
         {0xAB7C,1258,1},
         {0xAB7D,1259,1},
         {0xAB7E,1260,1},
         {0xAB7F,1261,1},
         {0xAB80,1262,1},
         {0xAB81,1263,1},
         {0xAB82,1264,1},
         {0xAB83,1265,1},
         {0xAB84,1266,1},
         {0xAB85,1267,1},
         {0xAB86,1268,1},
         {0xAB87,1269,1},
         {0xAB88,1270,1},
         {0xAB89,1271,1},
         {0xAB8A,1272,1},
         {0xAB8B,1273,1},
         {0xAB8C,1274,1},
         {0xAB8D,1275,1},
         {0xAB8E,1276,1},
         {0xAB8F,1277,1},
         {0xAB90,1278,1},
         {0xAB91,1279,1},
         {0xAB92,1280,1},
         {0xAB93,1281,1},
         {0xAB94,1282,1},
         {0xAB95,1283,1},
         {0xAB96,1284,1},
         {0xAB97,1285,1},
         {0xAB98,1286,1},
         {0xAB99,1287,1},
         {0xAB9A,1288,1},
         {0xAB9B,1289,1},
         {0xAB9C,1290,1},
         {0xAB9D,1291,1},
         {0xAB9E,1292,1},
         {0xAB9F,1293,1},
         {0xABA0,1294,1},
         {0xABA1,1295,1},
         {0xABA2,1296,1},
         {0xABA3,1297,1},
         {0xABA4,1298,1},
         {0xABA5,1299,1},
         {0xABA6,1300,1},
         {0xABA7,1301,1},
         {0xABA8,1302,1},
         {0xABA9,1303,1},
         {0xABAA,1304,1},
         {0xABAB,1305,1},
         {0xABAC,1306,1},
         {0xABAD,1307,1},
         {0xABAE,1308,1},
         {0xABAF,1309,1},
         {0xABB0,1310,1},
         {0xABB1,1311,1},
         {0xABB2,1312,1},
         {0xABB3,1313,1},
         {0xABB4,1314,1},
         {0xABB5,1315,1},
         {0xABB6,1316,1},
         {0xABB7,1317,1},
         {0xABB8,1318,1},
         {0xABB9,1319,1},
         {0xABBA,1320,1},
         {0xABBB,1321,1},
         {0xABBC,1322,1},
         {0xABBD,1323,1},
         {0xABBE,1324,1},
         {0xABBF,1325,1},
         {0xFB00,1326,2},
         {0xFB01,1328,2},
         {0xFB02,1330,2},
         {0xFB03,1332,3},
         {0xFB04,1335,3},
         {0xFB05,1338,2},
         {0xFB06,1340,2},
         {0xFB13,1342,2},
         {0xFB14,1344,2},
         {0xFB15,1346,2},
         {0xFB16,1348,2},
         {0xFB17,1350,2},
         {0xFF41,1352,1},
         {0xFF42,1353,1},
         {0xFF43,1354,1},
         {0xFF44,1355,1},
         {0xFF45,1356,1},
         {0xFF46,1357,1},
         {0xFF47,1358,1},
         {0xFF48,1359,1},
         {0xFF49,1360,1},
         {0xFF4A,1361,1},
         {0xFF4B,1362,1},
         {0xFF4C,1363,1},
         {0xFF4D,1364,1},
         {0xFF4E,1365,1},
         {0xFF4F,1366,1},
         {0xFF50,1367,1},
         {0xFF51,1368,1},
         {0xFF52,1369,1},
         {0xFF53,1370,1},
         {0xFF54,1371,1},
         {0xFF55,1372,1},
         {0xFF56,1373,1},
         {0xFF57,1374,1},
         {0xFF58,1375,1},
         {0xFF59,1376,1},
         {0xFF5A,1377,1},
         {0x10428,1378,1},
         {0x10429,1379,1},
         {0x1042A,1380,1},
         {0x1042B,1381,1},
         {0x1042C,1382,1},
         {0x1042D,1383,1},
         {0x1042E,1384,1},
         {0x1042F,1385,1},
         {0x10430,1386,1},
         {0x10431,1387,1},
         {0x10432,1388,1},
         {0x10433,1389,1},
         {0x10434,1390,1},
         {0x10435,1391,1},
         {0x10436,1392,1},
         {0x10437,1393,1},
         {0x10438,1394,1},
         {0x10439,1395,1},
         {0x1043A,1396,1},
         {0x1043B,1397,1},
         {0x1043C,1398,1},
         {0x1043D,1399,1},
         {0x1043E,1400,1},
         {0x1043F,1401,1},
         {0x10440,1402,1},
         {0x10441,1403,1},
         {0x10442,1404,1},
         {0x10443,1405,1},
         {0x10444,1406,1},
         {0x10445,1407,1},
         {0x10446,1408,1},
         {0x10447,1409,1},
         {0x10448,1410,1},
         {0x10449,1411,1},
         {0x1044A,1412,1},
         {0x1044B,1413,1},
         {0x1044C,1414,1},
         {0x1044D,1415,1},
         {0x1044E,1416,1},
         {0x1044F,1417,1},
         {0x104D8,1418,1},
         {0x104D9,1419,1},
         {0x104DA,1420,1},
         {0x104DB,1421,1},
         {0x104DC,1422,1},
         {0x104DD,1423,1},
         {0x104DE,1424,1},
         {0x104DF,1425,1},
         {0x104E0,1426,1},
         {0x104E1,1427,1},
         {0x104E2,1428,1},
         {0x104E3,1429,1},
         {0x104E4,1430,1},
         {0x104E5,1431,1},
         {0x104E6,1432,1},
         {0x104E7,1433,1},
         {0x104E8,1434,1},
         {0x104E9,1435,1},
         {0x104EA,1436,1},
         {0x104EB,1437,1},
         {0x104EC,1438,1},
         {0x104ED,1439,1},
         {0x104EE,1440,1},
         {0x104EF,1441,1},
         {0x104F0,1442,1},
         {0x104F1,1443,1},
         {0x104F2,1444,1},
         {0x104F3,1445,1},
         {0x104F4,1446,1},
         {0x104F5,1447,1},
         {0x104F6,1448,1},
         {0x104F7,1449,1},
         {0x104F8,1450,1},
         {0x104F9,1451,1},
         {0x104FA,1452,1},
         {0x104FB,1453,1},
         {0x10CC0,1454,1},
         {0x10CC1,1455,1},
         {0x10CC2,1456,1},
         {0x10CC3,1457,1},
         {0x10CC4,1458,1},
         {0x10CC5,1459,1},
         {0x10CC6,1460,1},
         {0x10CC7,1461,1},
         {0x10CC8,1462,1},
         {0x10CC9,1463,1},
         {0x10CCA,1464,1},
         {0x10CCB,1465,1},
         {0x10CCC,1466,1},
         {0x10CCD,1467,1},
         {0x10CCE,1468,1},
         {0x10CCF,1469,1},
         {0x10CD0,1470,1},
         {0x10CD1,1471,1},
         {0x10CD2,1472,1},
         {0x10CD3,1473,1},
         {0x10CD4,1474,1},
         {0x10CD5,1475,1},
         {0x10CD6,1476,1},
         {0x10CD7,1477,1},
         {0x10CD8,1478,1},
         {0x10CD9,1479,1},
         {0x10CDA,1480,1},
         {0x10CDB,1481,1},
         {0x10CDC,1482,1},
         {0x10CDD,1483,1},
         {0x10CDE,1484,1},
         {0x10CDF,1485,1},
         {0x10CE0,1486,1},
         {0x10CE1,1487,1},
         {0x10CE2,1488,1},
         {0x10CE3,1489,1},
         {0x10CE4,1490,1},
         {0x10CE5,1491,1},
         {0x10CE6,1492,1},
         {0x10CE7,1493,1},
         {0x10CE8,1494,1},
         {0x10CE9,1495,1},
         {0x10CEA,1496,1},
         {0x10CEB,1497,1},
         {0x10CEC,1498,1},
         {0x10CED,1499,1},
         {0x10CEE,1500,1},
         {0x10CEF,1501,1},
         {0x10CF0,1502,1},
         {0x10CF1,1503,1},
         {0x10CF2,1504,1},
         {0x118C0,1505,1},
         {0x118C1,1506,1},
         {0x118C2,1507,1},
         {0x118C3,1508,1},
         {0x118C4,1509,1},
         {0x118C5,1510,1},
         {0x118C6,1511,1},
         {0x118C7,1512,1},
         {0x118C8,1513,1},
         {0x118C9,1514,1},
         {0x118CA,1515,1},
         {0x118CB,1516,1},
         {0x118CC,1517,1},
         {0x118CD,1518,1},
         {0x118CE,1519,1},
         {0x118CF,1520,1},
         {0x118D0,1521,1},
         {0x118D1,1522,1},
         {0x118D2,1523,1},
         {0x118D3,1524,1},
         {0x118D4,1525,1},
         {0x118D5,1526,1},
         {0x118D6,1527,1},
         {0x118D7,1528,1},
         {0x118D8,1529,1},
         {0x118D9,1530,1},
         {0x118DA,1531,1},
         {0x118DB,1532,1},
         {0x118DC,1533,1},
         {0x118DD,1534,1},
         {0x118DE,1535,1},
         {0x118DF,1536,1},
         {0x16E60,1537,1},
         {0x16E61,1538,1},
         {0x16E62,1539,1},
         {0x16E63,1540,1},
         {0x16E64,1541,1},
         {0x16E65,1542,1},
         {0x16E66,1543,1},
         {0x16E67,1544,1},
         {0x16E68,1545,1},
         {0x16E69,1546,1},
         {0x16E6A,1547,1},
         {0x16E6B,1548,1},
         {0x16E6C,1549,1},
         {0x16E6D,1550,1},
         {0x16E6E,1551,1},
         {0x16E6F,1552,1},
         {0x16E70,1553,1},
         {0x16E71,1554,1},
         {0x16E72,1555,1},
         {0x16E73,1556,1},
         {0x16E74,1557,1},
         {0x16E75,1558,1},
         {0x16E76,1559,1},
         {0x16E77,1560,1},
         {0x16E78,1561,1},
         {0x16E79,1562,1},
         {0x16E7A,1563,1},
         {0x16E7B,1564,1},
         {0x16E7C,1565,1},
         {0x16E7D,1566,1},
         {0x16E7E,1567,1},
         {0x16E7F,1568,1},
         {0x1E922,1569,1},
         {0x1E923,1570,1},
         {0x1E924,1571,1},
         {0x1E925,1572,1},
         {0x1E926,1573,1},
         {0x1E927,1574,1},
         {0x1E928,1575,1},
         {0x1E929,1576,1},
         {0x1E92A,1577,1},
         {0x1E92B,1578,1},
         {0x1E92C,1579,1},
         {0x1E92D,1580,1},
         {0x1E92E,1581,1},
         {0x1E92F,1582,1},
         {0x1E930,1583,1},
         {0x1E931,1584,1},
         {0x1E932,1585,1},
         {0x1E933,1586,1},
         {0x1E934,1587,1},
         {0x1E935,1588,1},
         {0x1E936,1589,1},
         {0x1E937,1590,1},
         {0x1E938,1591,1},
         {0x1E939,1592,1},
         {0x1E93A,1593,1},
         {0x1E93B,1594,1},
         {0x1E93C,1595,1},
         {0x1E93D,1596,1},
         {0x1E93E,1597,1},
         {0x1E93F,1598,1},
         {0x1E940,1599,1},
         {0x1E941,1600,1},
         {0x1E942,1601,1},
         {0x1E943,1602,1},
      };
      static const char32_t upperValues[] = {
         0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,
         0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
         0x59,0x5A,0x39C,0x53,0x53,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,
         0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,
         0xD3,0xD4,0xD5,0xD6,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x178,
         0x100,0x102,0x104,0x106,0x108,0x10A,0x10C,0x10E,0x110,0x112,0x114,0x116,
         0x118,0x11A,0x11C,0x11E,0x120,0x122,0x124,0x126,0x128,0x12A,0x12C,0x12E,
         0x49,0x132,0x134,0x136,0x139,0x13B,0x13D,0x13F,0x141,0x143,0x145,0x147,
         0x2BC,0x4E,0x14A,0x14C,0x14E,0x150,0x152,0x154,0x156,0x158,0x15A,0x15C,
         0x15E,0x160,0x162,0x164,0x166,0x168,0x16A,0x16C,0x16E,0x170,0x172,0x174,
         0x176,0x179,0x17B,0x17D,0x53,0x243,0x182,0x184,0x187,0x18B,0x191,0x1F6,
         0x198,0x23D,0x220,0x1A0,0x1A2,0x1A4,0x1A7,0x1AC,0x1AF,0x1B3,0x1B5,0x1B8,
         0x1BC,0x1F7,0x1C4,0x1C4,0x1C7,0x1C7,0x1CA,0x1CA,0x1CD,0x1CF,0x1D1,0x1D3,
         0x1D5,0x1D7,0x1D9,0x1DB,0x18E,0x1DE,0x1E0,0x1E2,0x1E4,0x1E6,0x1E8,0x1EA,
         0x1EC,0x1EE,0x4A,0x30C,0x1F1,0x1F1,0x1F4,0x1F8,0x1FA,0x1FC,0x1FE,0x200,
         0x202,0x204,0x206,0x208,0x20A,0x20C,0x20E,0x210,0x212,0x214,0x216,0x218,
         0x21A,0x21C,0x21E,0x222,0x224,0x226,0x228,0x22A,0x22C,0x22E,0x230,0x232,
         0x23B,0x2C7E,0x2C7F,0x241,0x246,0x248,0x24A,0x24C,0x24E,0x2C6F,0x2C6D,0x2C70,
         0x181,0x186,0x189,0x18A,0x18F,0x190,0xA7AB,0x193,0xA7AC,0x194,0xA78D,0xA7AA,
         0x197,0x196,0xA7AE,0x2C62,0xA7AD,0x19C,0x2C6E,0x19D,0x19F,0x2C64,0x1A6,0xA7C5,
         0x1A9,0xA7B1,0x1AE,0x244,0x1B1,0x1B2,0x245,0x1B7,0xA7B2,0xA7B0,0x399,0x370,
         0x372,0x376,0x3FD,0x3FE,0x3FF,0x399,0x308,0x301,0x386,0x388,0x389,0x38A,
         0x3A5,0x308,0x301,0x391,0x392,0x393,0x394,0x395,0x396,0x397,0x398,0x399,
         0x39A,0x39B,0x39C,0x39D,0x39E,0x39F,0x3A0,0x3A1,0x3A3,0x3A3,0x3A4,0x3A5,
         0x3A6,0x3A7,0x3A8,0x3A9,0x3AA,0x3AB,0x38C,0x38E,0x38F,0x392,0x398,0x3A6,
         0x3A0,0x3CF,0x3D8,0x3DA,0x3DC,0x3DE,0x3E0,0x3E2,0x3E4,0x3E6,0x3E8,0x3EA,
         0x3EC,0x3EE,0x39A,0x3A1,0x3F9,0x37F,0x395,0x3F7,0x3FA,0x410,0x411,0x412,
         0x413,0x414,0x415,0x416,0x417,0x418,0x419,0x41A,0x41B,0x41C,0x41D,0x41E,
         0x41F,0x420,0x421,0x422,0x423,0x424,0x425,0x426,0x427,0x428,0x429,0x42A,
         0x42B,0x42C,0x42D,0x42E,0x42F,0x400,0x401,0x402,0x403,0x404,0x405,0x406,
         0x407,0x408,0x409,0x40A,0x40B,0x40C,0x40D,0x40E,0x40F,0x460,0x462,0x464,
         0x466,0x468,0x46A,0x46C,0x46E,0x470,0x472,0x474,0x476,0x478,0x47A,0x47C,
         0x47E,0x480,0x48A,0x48C,0x48E,0x490,0x492,0x494,0x496,0x498,0x49A,0x49C,
         0x49E,0x4A0,0x4A2,0x4A4,0x4A6,0x4A8,0x4AA,0x4AC,0x4AE,0x4B0,0x4B2,0x4B4,
         0x4B6,0x4B8,0x4BA,0x4BC,0x4BE,0x4C1,0x4C3,0x4C5,0x4C7,0x4C9,0x4CB,0x4CD,
         0x4C0,0x4D0,0x4D2,0x4D4,0x4D6,0x4D8,0x4DA,0x4DC,0x4DE,0x4E0,0x4E2,0x4E4,
         0x4E6,0x4E8,0x4EA,0x4EC,0x4EE,0x4F0,0x4F2,0x4F4,0x4F6,0x4F8,0x4FA,0x4FC,
         0x4FE,0x500,0x502,0x504,0x506,0x508,0x50A,0x50C,0x50E,0x510,0x512,0x514,
         0x516,0x518,0x51A,0x51C,0x51E,0x520,0x522,0x524,0x526,0x528,0x52A,0x52C,
         0x52E,0x531,0x532,0x533,0x534,0x535,0x536,0x537,0x538,0x539,0x53A,0x53B,
         0x53C,0x53D,0x53E,0x53F,0x540,0x541,0x542,0x543,0x544,0x545,0x546,0x547,
         0x548,0x549,0x54A,0x54B,0x54C,0x54D,0x54E,0x54F,0x550,0x551,0x552,0x553,
         0x554,0x555,0x556,0x535,0x552,0x1C90,0x1C91,0x1C92,0x1C93,0x1C94,0x1C95,0x1C96,
         0x1C97,0x1C98,0x1C99,0x1C9A,0x1C9B,0x1C9C,0x1C9D,0x1C9E,0x1C9F,0x1CA0,0x1CA1,0x1CA2,
         0x1CA3,0x1CA4,0x1CA5,0x1CA6,0x1CA7,0x1CA8,0x1CA9,0x1CAA,0x1CAB,0x1CAC,0x1CAD,0x1CAE,
         0x1CAF,0x1CB0,0x1CB1,0x1CB2,0x1CB3,0x1CB4,0x1CB5,0x1CB6,0x1CB7,0x1CB8,0x1CB9,0x1CBA,
         0x1CBD,0x1CBE,0x1CBF,0x13F0,0x13F1,0x13F2,0x13F3,0x13F4,0x13F5,0x412,0x414,0x41E,
         0x421,0x422,0x422,0x42A,0x462,0xA64A,0xA77D,0x2C63,0xA7C6,0x1E00,0x1E02,0x1E04,
         0x1E06,0x1E08,0x1E0A,0x1E0C,0x1E0E,0x1E10,0x1E12,0x1E14,0x1E16,0x1E18,0x1E1A,0x1E1C,
         0x1E1E,0x1E20,0x1E22,0x1E24,0x1E26,0x1E28,0x1E2A,0x1E2C,0x1E2E,0x1E30,0x1E32,0x1E34,
         0x1E36,0x1E38,0x1E3A,0x1E3C,0x1E3E,0x1E40,0x1E42,0x1E44,0x1E46,0x1E48,0x1E4A,0x1E4C,
         0x1E4E,0x1E50,0x1E52,0x1E54,0x1E56,0x1E58,0x1E5A,0x1E5C,0x1E5E,0x1E60,0x1E62,0x1E64,
         0x1E66,0x1E68,0x1E6A,0x1E6C,0x1E6E,0x1E70,0x1E72,0x1E74,0x1E76,0x1E78,0x1E7A,0x1E7C,
         0x1E7E,0x1E80,0x1E82,0x1E84,0x1E86,0x1E88,0x1E8A,0x1E8C,0x1E8E,0x1E90,0x1E92,0x1E94,
         0x48,0x331,0x54,0x308,0x57,0x30A,0x59,0x30A,0x41,0x2BE,0x1E60,0x1EA0,
         0x1EA2,0x1EA4,0x1EA6,0x1EA8,0x1EAA,0x1EAC,0x1EAE,0x1EB0,0x1EB2,0x1EB4,0x1EB6,0x1EB8,
         0x1EBA,0x1EBC,0x1EBE,0x1EC0,0x1EC2,0x1EC4,0x1EC6,0x1EC8,0x1ECA,0x1ECC,0x1ECE,0x1ED0,
         0x1ED2,0x1ED4,0x1ED6,0x1ED8,0x1EDA,0x1EDC,0x1EDE,0x1EE0,0x1EE2,0x1EE4,0x1EE6,0x1EE8,
         0x1EEA,0x1EEC,0x1EEE,0x1EF0,0x1EF2,0x1EF4,0x1EF6,0x1EF8,0x1EFA,0x1EFC,0x1EFE,0x1F08,
         0x1F09,0x1F0A,0x1F0B,0x1F0C,0x1F0D,0x1F0E,0x1F0F,0x1F18,0x1F19,0x1F1A,0x1F1B,0x1F1C,
         0x1F1D,0x1F28,0x1F29,0x1F2A,0x1F2B,0x1F2C,0x1F2D,0x1F2E,0x1F2F,0x1F38,0x1F39,0x1F3A,
         0x1F3B,0x1F3C,0x1F3D,0x1F3E,0x1F3F,0x1F48,0x1F49,0x1F4A,0x1F4B,0x1F4C,0x1F4D,0x3A5,
         0x313,0x1F59,0x3A5,0x313,0x300,0x1F5B,0x3A5,0x313,0x301,0x1F5D,0x3A5,0x313,
         0x342,0x1F5F,0x1F68,0x1F69,0x1F6A,0x1F6B,0x1F6C,0x1F6D,0x1F6E,0x1F6F,0x1FBA,0x1FBB,
         0x1FC8,0x1FC9,0x1FCA,0x1FCB,0x1FDA,0x1FDB,0x1FF8,0x1FF9,0x1FEA,0x1FEB,0x1FFA,0x1FFB,
         0x1F08,0x399,0x1F09,0x399,0x1F0A,0x399,0x1F0B,0x399,0x1F0C,0x399,0x1F0D,0x399,
         0x1F0E,0x399,0x1F0F,0x399,0x1F08,0x399,0x1F09,0x399,0x1F0A,0x399,0x1F0B,0x399,
         0x1F0C,0x399,0x1F0D,0x399,0x1F0E,0x399,0x1F0F,0x399,0x1F28,0x399,0x1F29,0x399,
         0x1F2A,0x399,0x1F2B,0x399,0x1F2C,0x399,0x1F2D,0x399,0x1F2E,0x399,0x1F2F,0x399,
         0x1F28,0x399,0x1F29,0x399,0x1F2A,0x399,0x1F2B,0x399,0x1F2C,0x399,0x1F2D,0x399,
         0x1F2E,0x399,0x1F2F,0x399,0x1F68,0x399,0x1F69,0x399,0x1F6A,0x399,0x1F6B,0x399,
         0x1F6C,0x399,0x1F6D,0x399,0x1F6E,0x399,0x1F6F,0x399,0x1F68,0x399,0x1F69,0x399,
         0x1F6A,0x399,0x1F6B,0x399,0x1F6C,0x399,0x1F6D,0x399,0x1F6E,0x399,0x1F6F,0x399,
         0x1FB8,0x1FB9,0x1FBA,0x399,0x391,0x399,0x386,0x399,0x391,0x342,0x391,0x342,
         0x399,0x391,0x399,0x399,0x1FCA,0x399,0x397,0x399,0x389,0x399,0x397,0x342,
         0x397,0x342,0x399,0x397,0x399,0x1FD8,0x1FD9,0x399,0x308,0x300,0x399,0x308,
         0x301,0x399,0x342,0x399,0x308,0x342,0x1FE8,0x1FE9,0x3A5,0x308,0x300,0x3A5,
         0x308,0x301,0x3A1,0x313,0x1FEC,0x3A5,0x342,0x3A5,0x308,0x342,0x1FFA,0x399,
         0x3A9,0x399,0x38F,0x399,0x3A9,0x342,0x3A9,0x342,0x399,0x3A9,0x399,0x2132,
         0x2160,0x2161,0x2162,0x2163,0x2164,0x2165,0x2166,0x2167,0x2168,0x2169,0x216A,0x216B,
         0x216C,0x216D,0x216E,0x216F,0x2183,0x24B6,0x24B7,0x24B8,0x24B9,0x24BA,0x24BB,0x24BC,
         0x24BD,0x24BE,0x24BF,0x24C0,0x24C1,0x24C2,0x24C3,0x24C4,0x24C5,0x24C6,0x24C7,0x24C8,
         0x24C9,0x24CA,0x24CB,0x24CC,0x24CD,0x24CE,0x24CF,0x2C00,0x2C01,0x2C02,0x2C03,0x2C04,
         0x2C05,0x2C06,0x2C07,0x2C08,0x2C09,0x2C0A,0x2C0B,0x2C0C,0x2C0D,0x2C0E,0x2C0F,0x2C10,
         0x2C11,0x2C12,0x2C13,0x2C14,0x2C15,0x2C16,0x2C17,0x2C18,0x2C19,0x2C1A,0x2C1B,0x2C1C,
         0x2C1D,0x2C1E,0x2C1F,0x2C20,0x2C21,0x2C22,0x2C23,0x2C24,0x2C25,0x2C26,0x2C27,0x2C28,
         0x2C29,0x2C2A,0x2C2B,0x2C2C,0x2C2D,0x2C2E,0x2C60,0x23A,0x23E,0x2C67,0x2C69,0x2C6B,
         0x2C72,0x2C75,0x2C80,0x2C82,0x2C84,0x2C86,0x2C88,0x2C8A,0x2C8C,0x2C8E,0x2C90,0x2C92,
         0x2C94,0x2C96,0x2C98,0x2C9A,0x2C9C,0x2C9E,0x2CA0,0x2CA2,0x2CA4,0x2CA6,0x2CA8,0x2CAA,
         0x2CAC,0x2CAE,0x2CB0,0x2CB2,0x2CB4,0x2CB6,0x2CB8,0x2CBA,0x2CBC,0x2CBE,0x2CC0,0x2CC2,
         0x2CC4,0x2CC6,0x2CC8,0x2CCA,0x2CCC,0x2CCE,0x2CD0,0x2CD2,0x2CD4,0x2CD6,0x2CD8,0x2CDA,
         0x2CDC,0x2CDE,0x2CE0,0x2CE2,0x2CEB,0x2CED,0x2CF2,0x10A0,0x10A1,0x10A2,0x10A3,0x10A4,
         0x10A5,0x10A6,0x10A7,0x10A8,0x10A9,0x10AA,0x10AB,0x10AC,0x10AD,0x10AE,0x10AF,0x10B0,
         0x10B1,0x10B2,0x10B3,0x10B4,0x10B5,0x10B6,0x10B7,0x10B8,0x10B9,0x10BA,0x10BB,0x10BC,
         0x10BD,0x10BE,0x10BF,0x10C0,0x10C1,0x10C2,0x10C3,0x10C4,0x10C5,0x10C7,0x10CD,0xA640,
         0xA642,0xA644,0xA646,0xA648,0xA64A,0xA64C,0xA64E,0xA650,0xA652,0xA654,0xA656,0xA658,
         0xA65A,0xA65C,0xA65E,0xA660,0xA662,0xA664,0xA666,0xA668,0xA66A,0xA66C,0xA680,0xA682,
         0xA684,0xA686,0xA688,0xA68A,0xA68C,0xA68E,0xA690,0xA692,0xA694,0xA696,0xA698,0xA69A,
         0xA722,0xA724,0xA726,0xA728,0xA72A,0xA72C,0xA72E,0xA732,0xA734,0xA736,0xA738,0xA73A,
         0xA73C,0xA73E,0xA740,0xA742,0xA744,0xA746,0xA748,0xA74A,0xA74C,0xA74E,0xA750,0xA752,
         0xA754,0xA756,0xA758,0xA75A,0xA75C,0xA75E,0xA760,0xA762,0xA764,0xA766,0xA768,0xA76A,
         0xA76C,0xA76E,0xA779,0xA77B,0xA77E,0xA780,0xA782,0xA784,0xA786,0xA78B,0xA790,0xA792,
         0xA7C4,0xA796,0xA798,0xA79A,0xA79C,0xA79E,0xA7A0,0xA7A2,0xA7A4,0xA7A6,0xA7A8,0xA7B4,
         0xA7B6,0xA7B8,0xA7BA,0xA7BC,0xA7BE,0xA7C2,0xA7C7,0xA7C9,0xA7F5,0xA7B3,0x13A0,0x13A1,
         0x13A2,0x13A3,0x13A4,0x13A5,0x13A6,0x13A7,0x13A8,0x13A9,0x13AA,0x13AB,0x13AC,0x13AD,
         0x13AE,0x13AF,0x13B0,0x13B1,0x13B2,0x13B3,0x13B4,0x13B5,0x13B6,0x13B7,0x13B8,0x13B9,
         0x13BA,0x13BB,0x13BC,0x13BD,0x13BE,0x13BF,0x13C0,0x13C1,0x13C2,0x13C3,0x13C4,0x13C5,
         0x13C6,0x13C7,0x13C8,0x13C9,0x13CA,0x13CB,0x13CC,0x13CD,0x13CE,0x13CF,0x13D0,0x13D1,
         0x13D2,0x13D3,0x13D4,0x13D5,0x13D6,0x13D7,0x13D8,0x13D9,0x13DA,0x13DB,0x13DC,0x13DD,
         0x13DE,0x13DF,0x13E0,0x13E1,0x13E2,0x13E3,0x13E4,0x13E5,0x13E6,0x13E7,0x13E8,0x13E9,
         0x13EA,0x13EB,0x13EC,0x13ED,0x13EE,0x13EF,0x46,0x46,0x46,0x49,0x46,0x4C,
         0x46,0x46,0x49,0x46,0x46,0x4C,0x53,0x54,0x53,0x54,0x544,0x546,
         0x544,0x535,0x544,0x53B,0x54E,0x546,0x544,0x53D,0xFF21,0xFF22,0xFF23,0xFF24,
         0xFF25,0xFF26,0xFF27,0xFF28,0xFF29,0xFF2A,0xFF2B,0xFF2C,0xFF2D,0xFF2E,0xFF2F,0xFF30,
         0xFF31,0xFF32,0xFF33,0xFF34,0xFF35,0xFF36,0xFF37,0xFF38,0xFF39,0xFF3A,0x10400,0x10401,
         0x10402,0x10403,0x10404,0x10405,0x10406,0x10407,0x10408,0x10409,0x1040A,0x1040B,0x1040C,0x1040D,
         0x1040E,0x1040F,0x10410,0x10411,0x10412,0x10413,0x10414,0x10415,0x10416,0x10417,0x10418,0x10419,
         0x1041A,0x1041B,0x1041C,0x1041D,0x1041E,0x1041F,0x10420,0x10421,0x10422,0x10423,0x10424,0x10425,
         0x10426,0x10427,0x104B0,0x104B1,0x104B2,0x104B3,0x104B4,0x104B5,0x104B6,0x104B7,0x104B8,0x104B9,
         0x104BA,0x104BB,0x104BC,0x104BD,0x104BE,0x104BF,0x104C0,0x104C1,0x104C2,0x104C3,0x104C4,0x104C5,
         0x104C6,0x104C7,0x104C8,0x104C9,0x104CA,0x104CB,0x104CC,0x104CD,0x104CE,0x104CF,0x104D0,0x104D1,
         0x104D2,0x104D3,0x10C80,0x10C81,0x10C82,0x10C83,0x10C84,0x10C85,0x10C86,0x10C87,0x10C88,0x10C89,
         0x10C8A,0x10C8B,0x10C8C,0x10C8D,0x10C8E,0x10C8F,0x10C90,0x10C91,0x10C92,0x10C93,0x10C94,0x10C95,
         0x10C96,0x10C97,0x10C98,0x10C99,0x10C9A,0x10C9B,0x10C9C,0x10C9D,0x10C9E,0x10C9F,0x10CA0,0x10CA1,
         0x10CA2,0x10CA3,0x10CA4,0x10CA5,0x10CA6,0x10CA7,0x10CA8,0x10CA9,0x10CAA,0x10CAB,0x10CAC,0x10CAD,
         0x10CAE,0x10CAF,0x10CB0,0x10CB1,0x10CB2,0x118A0,0x118A1,0x118A2,0x118A3,0x118A4,0x118A5,0x118A6,
         0x118A7,0x118A8,0x118A9,0x118AA,0x118AB,0x118AC,0x118AD,0x118AE,0x118AF,0x118B0,0x118B1,0x118B2,
         0x118B3,0x118B4,0x118B5,0x118B6,0x118B7,0x118B8,0x118B9,0x118BA,0x118BB,0x118BC,0x118BD,0x118BE,
         0x118BF,0x16E40,0x16E41,0x16E42,0x16E43,0x16E44,0x16E45,0x16E46,0x16E47,0x16E48,0x16E49,0x16E4A,
         0x16E4B,0x16E4C,0x16E4D,0x16E4E,0x16E4F,0x16E50,0x16E51,0x16E52,0x16E53,0x16E54,0x16E55,0x16E56,
         0x16E57,0x16E58,0x16E59,0x16E5A,0x16E5B,0x16E5C,0x16E5D,0x16E5E,0x16E5F,0x1E900,0x1E901,0x1E902,
         0x1E903,0x1E904,0x1E905,0x1E906,0x1E907,0x1E908,0x1E909,0x1E90A,0x1E90B,0x1E90C,0x1E90D,0x1E90E,
         0x1E90F,0x1E910,0x1E911,0x1E912,0x1E913,0x1E914,0x1E915,0x1E916,0x1E917,0x1E918,0x1E919,0x1E91A,
         0x1E91B,0x1E91C,0x1E91D,0x1E91E,0x1E91F,0x1E920,0x1E921,
      };
      static const Mapping lowerMap[] = {
         {0x41,0,1},
         {0x42,1,1},
         {0x43,2,1},
         {0x44,3,1},
         {0x45,4,1},
         {0x46,5,1},
         {0x47,6,1},
         {0x48,7,1},
         {0x49,8,1},
         {0x4A,9,1},
         {0x4B,10,1},
         {0x4C,11,1},
         {0x4D,12,1},
         {0x4E,13,1},
         {0x4F,14,1},
         {0x50,15,1},
         {0x51,16,1},
         {0x52,17,1},
         {0x53,18,1},
         {0x54,19,1},
         {0x55,20,1},
         {0x56,21,1},
         {0x57,22,1},
         {0x58,23,1},
         {0x59,24,1},
         {0x5A,25,1},
         {0xC0,26,1},
         {0xC1,27,1},
         {0xC2,28,1},
         {0xC3,29,1},
         {0xC4,30,1},
         {0xC5,31,1},
         {0xC6,32,1},
         {0xC7,33,1},
         {0xC8,34,1},
         {0xC9,35,1},
         {0xCA,36,1},
         {0xCB,37,1},
         {0xCC,38,1},
         {0xCD,39,1},
         {0xCE,40,1},
         {0xCF,41,1},
         {0xD0,42,1},
         {0xD1,43,1},
         {0xD2,44,1},
         {0xD3,45,1},
         {0xD4,46,1},
         {0xD5,47,1},
         {0xD6,48,1},
         {0xD8,49,1},
         {0xD9,50,1},
         {0xDA,51,1},
         {0xDB,52,1},
         {0xDC,53,1},
         {0xDD,54,1},
         {0xDE,55,1},
         {0x100,56,1},
         {0x102,57,1},
         {0x104,58,1},
         {0x106,59,1},
         {0x108,60,1},
         {0x10A,61,1},
         {0x10C,62,1},
         {0x10E,63,1},
         {0x110,64,1},
         {0x112,65,1},
         {0x114,66,1},
         {0x116,67,1},
         {0x118,68,1},
         {0x11A,69,1},
         {0x11C,70,1},
         {0x11E,71,1},
         {0x120,72,1},
         {0x122,73,1},
         {0x124,74,1},
         {0x126,75,1},
         {0x128,76,1},
         {0x12A,77,1},
         {0x12C,78,1},
         {0x12E,79,1},
         {0x130,80,2},
         {0x132,82,1},
         {0x134,83,1},
         {0x136,84,1},
         {0x139,85,1},
         {0x13B,86,1},
         {0x13D,87,1},
         {0x13F,88,1},
         {0x141,89,1},
         {0x143,90,1},
         {0x145,91,1},
         {0x147,92,1},
         {0x14A,93,1},
         {0x14C,94,1},
         {0x14E,95,1},
         {0x150,96,1},
         {0x152,97,1},
         {0x154,98,1},
         {0x156,99,1},
         {0x158,100,1},
         {0x15A,101,1},
         {0x15C,102,1},
         {0x15E,103,1},
         {0x160,104,1},
         {0x162,105,1},
         {0x164,106,1},
         {0x166,107,1},
         {0x168,108,1},
         {0x16A,109,1},
         {0x16C,110,1},
         {0x16E,111,1},
         {0x170,112,1},
         {0x172,113,1},
         {0x174,114,1},
         {0x176,115,1},
         {0x178,116,1},
         {0x179,117,1},
         {0x17B,118,1},
         {0x17D,119,1},
         {0x181,120,1},
         {0x182,121,1},
         {0x184,122,1},
         {0x186,123,1},
         {0x187,124,1},
         {0x189,125,1},
         {0x18A,126,1},
         {0x18B,127,1},
         {0x18E,128,1},
         {0x18F,129,1},
         {0x190,130,1},
         {0x191,131,1},
         {0x193,132,1},
         {0x194,133,1},
         {0x196,134,1},
         {0x197,135,1},
         {0x198,136,1},
         {0x19C,137,1},
         {0x19D,138,1},
         {0x19F,139,1},
         {0x1A0,140,1},
         {0x1A2,141,1},
         {0x1A4,142,1},
         {0x1A6,143,1},
         {0x1A7,144,1},
         {0x1A9,145,1},
         {0x1AC,146,1},
         {0x1AE,147,1},
         {0x1AF,148,1},
         {0x1B1,149,1},
         {0x1B2,150,1},
         {0x1B3,151,1},
         {0x1B5,152,1},
         {0x1B7,153,1},
         {0x1B8,154,1},
         {0x1BC,155,1},
         {0x1C4,156,1},
         {0x1C5,157,1},
         {0x1C7,158,1},
         {0x1C8,159,1},
         {0x1CA,160,1},
         {0x1CB,161,1},
         {0x1CD,162,1},
         {0x1CF,163,1},
         {0x1D1,164,1},
         {0x1D3,165,1},
         {0x1D5,166,1},
         {0x1D7,167,1},
         {0x1D9,168,1},
         {0x1DB,169,1},
         {0x1DE,170,1},
         {0x1E0,171,1},
         {0x1E2,172,1},
         {0x1E4,173,1},
         {0x1E6,174,1},
         {0x1E8,175,1},
         {0x1EA,176,1},
         {0x1EC,177,1},
         {0x1EE,178,1},
         {0x1F1,179,1},
         {0x1F2,180,1},
         {0x1F4,181,1},
         {0x1F6,182,1},
         {0x1F7,183,1},
         {0x1F8,184,1},
         {0x1FA,185,1},
         {0x1FC,186,1},
         {0x1FE,187,1},
         {0x200,188,1},
         {0x202,189,1},
         {0x204,190,1},
         {0x206,191,1},
         {0x208,192,1},
         {0x20A,193,1},
         {0x20C,194,1},
         {0x20E,195,1},
         {0x210,196,1},
         {0x212,197,1},
         {0x214,198,1},
         {0x216,199,1},
         {0x218,200,1},
         {0x21A,201,1},
         {0x21C,202,1},
         {0x21E,203,1},
         {0x220,204,1},
         {0x222,205,1},
         {0x224,206,1},
         {0x226,207,1},
         {0x228,208,1},
         {0x22A,209,1},
         {0x22C,210,1},
         {0x22E,211,1},
         {0x230,212,1},
         {0x232,213,1},
         {0x23A,214,1},
         {0x23B,215,1},
         {0x23D,216,1},
         {0x23E,217,1},
         {0x241,218,1},
         {0x243,219,1},
         {0x244,220,1},
         {0x245,221,1},
         {0x246,222,1},
         {0x248,223,1},
         {0x24A,224,1},
         {0x24C,225,1},
         {0x24E,226,1},
         {0x370,227,1},
         {0x372,228,1},
         {0x376,229,1},
         {0x37F,230,1},
         {0x386,231,1},
         {0x388,232,1},
         {0x389,233,1},
         {0x38A,234,1},
         {0x38C,235,1},
         {0x38E,236,1},
         {0x38F,237,1},
         {0x391,238,1},
         {0x392,239,1},
         {0x393,240,1},
         {0x394,241,1},
         {0x395,242,1},
         {0x396,243,1},
         {0x397,244,1},
         {0x398,245,1},
         {0x399,246,1},
         {0x39A,247,1},
         {0x39B,248,1},
         {0x39C,249,1},
         {0x39D,250,1},
         {0x39E,251,1},
         {0x39F,252,1},
         {0x3A0,253,1},
         {0x3A1,254,1},
         {0x3A3,255,1},
         {0x3A4,256,1},
         {0x3A5,257,1},
         {0x3A6,258,1},
         {0x3A7,259,1},
         {0x3A8,260,1},
         {0x3A9,261,1},
         {0x3AA,262,1},
         {0x3AB,263,1},
         {0x3CF,264,1},
         {0x3D8,265,1},
         {0x3DA,266,1},
         {0x3DC,267,1},
         {0x3DE,268,1},
         {0x3E0,269,1},
         {0x3E2,270,1},
         {0x3E4,271,1},
         {0x3E6,272,1},
         {0x3E8,273,1},
         {0x3EA,274,1},
         {0x3EC,275,1},
         {0x3EE,276,1},
         {0x3F4,277,1},
         {0x3F7,278,1},
         {0x3F9,279,1},
         {0x3FA,280,1},
         {0x3FD,281,1},
         {0x3FE,282,1},
         {0x3FF,283,1},
         {0x400,284,1},
         {0x401,285,1},
         {0x402,286,1},
         {0x403,287,1},
         {0x404,288,1},
         {0x405,289,1},
         {0x406,290,1},
         {0x407,291,1},
         {0x408,292,1},
         {0x409,293,1},
         {0x40A,294,1},
         {0x40B,295,1},
         {0x40C,296,1},
         {0x40D,297,1},
         {0x40E,298,1},
         {0x40F,299,1},
         {0x410,300,1},
         {0x411,301,1},
         {0x412,302,1},
         {0x413,303,1},
         {0x414,304,1},
         {0x415,305,1},
         {0x416,306,1},
         {0x417,307,1},
         {0x418,308,1},
         {0x419,309,1},
         {0x41A,310,1},
         {0x41B,311,1},
         {0x41C,312,1},
         {0x41D,313,1},
         {0x41E,314,1},
         {0x41F,315,1},
         {0x420,316,1},
         {0x421,317,1},
         {0x422,318,1},
         {0x423,319,1},
         {0x424,320,1},
         {0x425,321,1},
         {0x426,322,1},
         {0x427,323,1},
         {0x428,324,1},
         {0x429,325,1},
         {0x42A,326,1},
         {0x42B,327,1},
         {0x42C,328,1},
         {0x42D,329,1},
         {0x42E,330,1},
         {0x42F,331,1},
         {0x460,332,1},
         {0x462,333,1},
         {0x464,334,1},
         {0x466,335,1},
         {0x468,336,1},
         {0x46A,337,1},
         {0x46C,338,1},
         {0x46E,339,1},
         {0x470,340,1},
         {0x472,341,1},
         {0x474,342,1},
         {0x476,343,1},
         {0x478,344,1},
         {0x47A,345,1},
         {0x47C,346,1},
         {0x47E,347,1},
         {0x480,348,1},
         {0x48A,349,1},
         {0x48C,350,1},
         {0x48E,351,1},
         {0x490,352,1},
         {0x492,353,1},
         {0x494,354,1},
         {0x496,355,1},
         {0x498,356,1},
         {0x49A,357,1},
         {0x49C,358,1},
         {0x49E,359,1},
         {0x4A0,360,1},
         {0x4A2,361,1},
         {0x4A4,362,1},
         {0x4A6,363,1},
         {0x4A8,364,1},
         {0x4AA,365,1},
         {0x4AC,366,1},
         {0x4AE,367,1},
         {0x4B0,368,1},
         {0x4B2,369,1},
         {0x4B4,370,1},
         {0x4B6,371,1},
         {0x4B8,372,1},
         {0x4BA,373,1},
         {0x4BC,374,1},
         {0x4BE,375,1},
         {0x4C0,376,1},
         {0x4C1,377,1},
         {0x4C3,378,1},
         {0x4C5,379,1},
         {0x4C7,380,1},
         {0x4C9,381,1},
         {0x4CB,382,1},
         {0x4CD,383,1},
         {0x4D0,384,1},
         {0x4D2,385,1},
         {0x4D4,386,1},
         {0x4D6,387,1},
         {0x4D8,388,1},
         {0x4DA,389,1},
         {0x4DC,390,1},
         {0x4DE,391,1},
         {0x4E0,392,1},
         {0x4E2,393,1},
         {0x4E4,394,1},
         {0x4E6,395,1},
         {0x4E8,396,1},
         {0x4EA,397,1},
         {0x4EC,398,1},
         {0x4EE,399,1},
         {0x4F0,400,1},
         {0x4F2,401,1},
         {0x4F4,402,1},
         {0x4F6,403,1},
         {0x4F8,404,1},
         {0x4FA,405,1},
         {0x4FC,406,1},
         {0x4FE,407,1},
         {0x500,408,1},
         {0x502,409,1},
         {0x504,410,1},
         {0x506,411,1},
         {0x508,412,1},
         {0x50A,413,1},
         {0x50C,414,1},
         {0x50E,415,1},
         {0x510,416,1},
         {0x512,417,1},
         {0x514,418,1},
         {0x516,419,1},
         {0x518,420,1},
         {0x51A,421,1},
         {0x51C,422,1},
         {0x51E,423,1},
         {0x520,424,1},
         {0x522,425,1},
         {0x524,426,1},
         {0x526,427,1},
         {0x528,428,1},
         {0x52A,429,1},
         {0x52C,430,1},
         {0x52E,431,1},
         {0x531,432,1},
         {0x532,433,1},
         {0x533,434,1},
         {0x534,435,1},
         {0x535,436,1},
         {0x536,437,1},
         {0x537,438,1},
         {0x538,439,1},
         {0x539,440,1},
         {0x53A,441,1},
         {0x53B,442,1},
         {0x53C,443,1},
         {0x53D,444,1},
         {0x53E,445,1},
         {0x53F,446,1},
         {0x540,447,1},
         {0x541,448,1},
         {0x542,449,1},
         {0x543,450,1},
         {0x544,451,1},
         {0x545,452,1},
         {0x546,453,1},
         {0x547,454,1},
         {0x548,455,1},
         {0x549,456,1},
         {0x54A,457,1},
         {0x54B,458,1},
         {0x54C,459,1},
         {0x54D,460,1},
         {0x54E,461,1},
         {0x54F,462,1},
         {0x550,463,1},
         {0x551,464,1},
         {0x552,465,1},
         {0x553,466,1},
         {0x554,467,1},
         {0x555,468,1},
         {0x556,469,1},
         {0x10A0,470,1},
         {0x10A1,471,1},
         {0x10A2,472,1},
         {0x10A3,473,1},
         {0x10A4,474,1},
         {0x10A5,475,1},
         {0x10A6,476,1},
         {0x10A7,477,1},
         {0x10A8,478,1},
         {0x10A9,479,1},
         {0x10AA,480,1},
         {0x10AB,481,1},
         {0x10AC,482,1},
         {0x10AD,483,1},
         {0x10AE,484,1},
         {0x10AF,485,1},
         {0x10B0,486,1},
         {0x10B1,487,1},
         {0x10B2,488,1},
         {0x10B3,489,1},
         {0x10B4,490,1},
         {0x10B5,491,1},
         {0x10B6,492,1},
         {0x10B7,493,1},
         {0x10B8,494,1},
         {0x10B9,495,1},
         {0x10BA,496,1},
         {0x10BB,497,1},
         {0x10BC,498,1},
         {0x10BD,499,1},
         {0x10BE,500,1},
         {0x10BF,501,1},
         {0x10C0,502,1},
         {0x10C1,503,1},
         {0x10C2,504,1},
         {0x10C3,505,1},
         {0x10C4,506,1},
         {0x10C5,507,1},
         {0x10C7,508,1},
         {0x10CD,509,1},
         {0x13A0,510,1},
         {0x13A1,511,1},
         {0x13A2,512,1},
         {0x13A3,513,1},
         {0x13A4,514,1},
         {0x13A5,515,1},
         {0x13A6,516,1},
         {0x13A7,517,1},
         {0x13A8,518,1},
         {0x13A9,519,1},
         {0x13AA,520,1},
         {0x13AB,521,1},
         {0x13AC,522,1},
         {0x13AD,523,1},
         {0x13AE,524,1},
         {0x13AF,525,1},
         {0x13B0,526,1},
         {0x13B1,527,1},
         {0x13B2,528,1},
         {0x13B3,529,1},
         {0x13B4,530,1},
         {0x13B5,531,1},
         {0x13B6,532,1},
         {0x13B7,533,1},
         {0x13B8,534,1},
         {0x13B9,535,1},
         {0x13BA,536,1},
         {0x13BB,537,1},
         {0x13BC,538,1},
         {0x13BD,539,1},
         {0x13BE,540,1},
         {0x13BF,541,1},
         {0x13C0,542,1},
         {0x13C1,543,1},
         {0x13C2,544,1},
         {0x13C3,545,1},
         {0x13C4,546,1},
         {0x13C5,547,1},
         {0x13C6,548,1},
         {0x13C7,549,1},
         {0x13C8,550,1},
         {0x13C9,551,1},
         {0x13CA,552,1},
         {0x13CB,553,1},
         {0x13CC,554,1},
         {0x13CD,555,1},
         {0x13CE,556,1},
         {0x13CF,557,1},
         {0x13D0,558,1},
         {0x13D1,559,1},
         {0x13D2,560,1},
         {0x13D3,561,1},
         {0x13D4,562,1},
         {0x13D5,563,1},
         {0x13D6,564,1},
         {0x13D7,565,1},
         {0x13D8,566,1},
         {0x13D9,567,1},
         {0x13DA,568,1},
         {0x13DB,569,1},
         {0x13DC,570,1},
         {0x13DD,571,1},
         {0x13DE,572,1},
         {0x13DF,573,1},
         {0x13E0,574,1},
         {0x13E1,575,1},
         {0x13E2,576,1},
         {0x13E3,577,1},
         {0x13E4,578,1},
         {0x13E5,579,1},
         {0x13E6,580,1},
         {0x13E7,581,1},
         {0x13E8,582,1},
         {0x13E9,583,1},
         {0x13EA,584,1},
         {0x13EB,585,1},
         {0x13EC,586,1},
         {0x13ED,587,1},
         {0x13EE,588,1},
         {0x13EF,589,1},
         {0x13F0,590,1},
         {0x13F1,591,1},
         {0x13F2,592,1},
         {0x13F3,593,1},
         {0x13F4,594,1},
         {0x13F5,595,1},
         {0x1C90,596,1},
         {0x1C91,597,1},
         {0x1C92,598,1},
         {0x1C93,599,1},
         {0x1C94,600,1},
         {0x1C95,601,1},
         {0x1C96,602,1},
         {0x1C97,603,1},
         {0x1C98,604,1},
         {0x1C99,605,1},
         {0x1C9A,606,1},
         {0x1C9B,607,1},
         {0x1C9C,608,1},
         {0x1C9D,609,1},
         {0x1C9E,610,1},
         {0x1C9F,611,1},
         {0x1CA0,612,1},
         {0x1CA1,613,1},
         {0x1CA2,614,1},
         {0x1CA3,615,1},
         {0x1CA4,616,1},
         {0x1CA5,617,1},
         {0x1CA6,618,1},
         {0x1CA7,619,1},
         {0x1CA8,620,1},
         {0x1CA9,621,1},
         {0x1CAA,622,1},
         {0x1CAB,623,1},
         {0x1CAC,624,1},
         {0x1CAD,625,1},
         {0x1CAE,626,1},
         {0x1CAF,627,1},
         {0x1CB0,628,1},
         {0x1CB1,629,1},
         {0x1CB2,630,1},
         {0x1CB3,631,1},
         {0x1CB4,632,1},
         {0x1CB5,633,1},
         {0x1CB6,634,1},
         {0x1CB7,635,1},
         {0x1CB8,636,1},
         {0x1CB9,637,1},
         {0x1CBA,638,1},
         {0x1CBD,639,1},
         {0x1CBE,640,1},
         {0x1CBF,641,1},
         {0x1E00,642,1},
         {0x1E02,643,1},
         {0x1E04,644,1},
         {0x1E06,645,1},
         {0x1E08,646,1},
         {0x1E0A,647,1},
         {0x1E0C,648,1},
         {0x1E0E,649,1},
         {0x1E10,650,1},
         {0x1E12,651,1},
         {0x1E14,652,1},
         {0x1E16,653,1},
         {0x1E18,654,1},
         {0x1E1A,655,1},
         {0x1E1C,656,1},
         {0x1E1E,657,1},
         {0x1E20,658,1},
         {0x1E22,659,1},
         {0x1E24,660,1},
         {0x1E26,661,1},
         {0x1E28,662,1},
         {0x1E2A,663,1},
         {0x1E2C,664,1},
         {0x1E2E,665,1},
         {0x1E30,666,1},
         {0x1E32,667,1},
         {0x1E34,668,1},
         {0x1E36,669,1},
         {0x1E38,670,1},
         {0x1E3A,671,1},
         {0x1E3C,672,1},
         {0x1E3E,673,1},
         {0x1E40,674,1},
         {0x1E42,675,1},
         {0x1E44,676,1},
         {0x1E46,677,1},
         {0x1E48,678,1},
         {0x1E4A,679,1},
         {0x1E4C,680,1},
         {0x1E4E,681,1},
         {0x1E50,682,1},
         {0x1E52,683,1},
         {0x1E54,684,1},
         {0x1E56,685,1},
         {0x1E58,686,1},
         {0x1E5A,687,1},
         {0x1E5C,688,1},
         {0x1E5E,689,1},
         {0x1E60,690,1},
         {0x1E62,691,1},
         {0x1E64,692,1},
         {0x1E66,693,1},
         {0x1E68,694,1},
         {0x1E6A,695,1},
         {0x1E6C,696,1},
         {0x1E6E,697,1},
         {0x1E70,698,1},
         {0x1E72,699,1},
         {0x1E74,700,1},
         {0x1E76,701,1},
         {0x1E78,702,1},
         {0x1E7A,703,1},
         {0x1E7C,704,1},
         {0x1E7E,705,1},
         {0x1E80,706,1},
         {0x1E82,707,1},
         {0x1E84,708,1},
         {0x1E86,709,1},
         {0x1E88,710,1},
         {0x1E8A,711,1},
         {0x1E8C,712,1},
         {0x1E8E,713,1},
         {0x1E90,714,1},
         {0x1E92,715,1},
         {0x1E94,716,1},
         {0x1E9E,717,1},
         {0x1EA0,718,1},
         {0x1EA2,719,1},
         {0x1EA4,720,1},
         {0x1EA6,721,1},
         {0x1EA8,722,1},
         {0x1EAA,723,1},
         {0x1EAC,724,1},
         {0x1EAE,725,1},
         {0x1EB0,726,1},
         {0x1EB2,727,1},
         {0x1EB4,728,1},
         {0x1EB6,729,1},
         {0x1EB8,730,1},
         {0x1EBA,731,1},
         {0x1EBC,732,1},
         {0x1EBE,733,1},
         {0x1EC0,734,1},
         {0x1EC2,735,1},
         {0x1EC4,736,1},
         {0x1EC6,737,1},
         {0x1EC8,738,1},
         {0x1ECA,739,1},
         {0x1ECC,740,1},
         {0x1ECE,741,1},
         {0x1ED0,742,1},
         {0x1ED2,743,1},
         {0x1ED4,744,1},
         {0x1ED6,745,1},
         {0x1ED8,746,1},
         {0x1EDA,747,1},
         {0x1EDC,748,1},
         {0x1EDE,749,1},
         {0x1EE0,750,1},
         {0x1EE2,751,1},
         {0x1EE4,752,1},
         {0x1EE6,753,1},
         {0x1EE8,754,1},
         {0x1EEA,755,1},
         {0x1EEC,756,1},
         {0x1EEE,757,1},
         {0x1EF0,758,1},
         {0x1EF2,759,1},
         {0x1EF4,760,1},
         {0x1EF6,761,1},
         {0x1EF8,762,1},
         {0x1EFA,763,1},
         {0x1EFC,764,1},
         {0x1EFE,765,1},
         {0x1F08,766,1},
         {0x1F09,767,1},
         {0x1F0A,768,1},
         {0x1F0B,769,1},
         {0x1F0C,770,1},
         {0x1F0D,771,1},
         {0x1F0E,772,1},
         {0x1F0F,773,1},
         {0x1F18,774,1},
         {0x1F19,775,1},
         {0x1F1A,776,1},
         {0x1F1B,777,1},
         {0x1F1C,778,1},
         {0x1F1D,779,1},
         {0x1F28,780,1},
         {0x1F29,781,1},
         {0x1F2A,782,1},
         {0x1F2B,783,1},
         {0x1F2C,784,1},
         {0x1F2D,785,1},
         {0x1F2E,786,1},
         {0x1F2F,787,1},
         {0x1F38,788,1},
         {0x1F39,789,1},
         {0x1F3A,790,1},
         {0x1F3B,791,1},
         {0x1F3C,792,1},
         {0x1F3D,793,1},
         {0x1F3E,794,1},
         {0x1F3F,795,1},
         {0x1F48,796,1},
         {0x1F49,797,1},
         {0x1F4A,798,1},
         {0x1F4B,799,1},
         {0x1F4C,800,1},
         {0x1F4D,801,1},
         {0x1F59,802,1},
         {0x1F5B,803,1},
         {0x1F5D,804,1},
         {0x1F5F,805,1},
         {0x1F68,806,1},
         {0x1F69,807,1},
         {0x1F6A,808,1},
         {0x1F6B,809,1},
         {0x1F6C,810,1},
         {0x1F6D,811,1},
         {0x1F6E,812,1},
         {0x1F6F,813,1},
         {0x1F88,814,1},
         {0x1F89,815,1},
         {0x1F8A,816,1},
         {0x1F8B,817,1},
         {0x1F8C,818,1},
         {0x1F8D,819,1},
         {0x1F8E,820,1},
         {0x1F8F,821,1},
         {0x1F98,822,1},
         {0x1F99,823,1},
         {0x1F9A,824,1},
         {0x1F9B,825,1},
         {0x1F9C,826,1},
         {0x1F9D,827,1},
         {0x1F9E,828,1},
         {0x1F9F,829,1},
         {0x1FA8,830,1},
         {0x1FA9,831,1},
         {0x1FAA,832,1},
         {0x1FAB,833,1},
         {0x1FAC,834,1},
         {0x1FAD,835,1},
         {0x1FAE,836,1},
         {0x1FAF,837,1},
         {0x1FB8,838,1},
         {0x1FB9,839,1},
         {0x1FBA,840,1},
         {0x1FBB,841,1},
         {0x1FBC,842,1},
         {0x1FC8,843,1},
         {0x1FC9,844,1},
         {0x1FCA,845,1},
         {0x1FCB,846,1},
         {0x1FCC,847,1},
         {0x1FD8,848,1},
         {0x1FD9,849,1},
         {0x1FDA,850,1},
         {0x1FDB,851,1},
         {0x1FE8,852,1},
         {0x1FE9,853,1},
         {0x1FEA,854,1},
         {0x1FEB,855,1},
         {0x1FEC,856,1},
         {0x1FF8,857,1},
         {0x1FF9,858,1},
         {0x1FFA,859,1},
         {0x1FFB,860,1},
         {0x1FFC,861,1},
         {0x2126,862,1},
         {0x212A,863,1},
         {0x212B,864,1},
         {0x2132,865,1},
         {0x2160,866,1},
         {0x2161,867,1},
         {0x2162,868,1},
         {0x2163,869,1},
         {0x2164,870,1},
         {0x2165,871,1},
         {0x2166,872,1},
         {0x2167,873,1},
         {0x2168,874,1},
         {0x2169,875,1},
         {0x216A,876,1},
         {0x216B,877,1},
         {0x216C,878,1},
         {0x216D,879,1},
         {0x216E,880,1},
         {0x216F,881,1},
         {0x2183,882,1},
         {0x24B6,883,1},
         {0x24B7,884,1},
         {0x24B8,885,1},
         {0x24B9,886,1},
         {0x24BA,887,1},
         {0x24BB,888,1},
         {0x24BC,889,1},
         {0x24BD,890,1},
         {0x24BE,891,1},
         {0x24BF,892,1},
         {0x24C0,893,1},
         {0x24C1,894,1},
         {0x24C2,895,1},
         {0x24C3,896,1},
         {0x24C4,897,1},
         {0x24C5,898,1},
         {0x24C6,899,1},
         {0x24C7,900,1},
         {0x24C8,901,1},
         {0x24C9,902,1},
         {0x24CA,903,1},
         {0x24CB,904,1},
         {0x24CC,905,1},
         {0x24CD,906,1},
         {0x24CE,907,1},
         {0x24CF,908,1},
         {0x2C00,909,1},
         {0x2C01,910,1},
         {0x2C02,911,1},
         {0x2C03,912,1},
         {0x2C04,913,1},
         {0x2C05,914,1},
         {0x2C06,915,1},
         {0x2C07,916,1},
         {0x2C08,917,1},
         {0x2C09,918,1},
         {0x2C0A,919,1},
         {0x2C0B,920,1},
         {0x2C0C,921,1},
         {0x2C0D,922,1},
         {0x2C0E,923,1},
         {0x2C0F,924,1},
         {0x2C10,925,1},
         {0x2C11,926,1},
         {0x2C12,927,1},
         {0x2C13,928,1},
         {0x2C14,929,1},
         {0x2C15,930,1},
         {0x2C16,931,1},
         {0x2C17,932,1},
         {0x2C18,933,1},
         {0x2C19,934,1},
         {0x2C1A,935,1},
         {0x2C1B,936,1},
         {0x2C1C,937,1},
         {0x2C1D,938,1},
         {0x2C1E,939,1},
         {0x2C1F,940,1},
         {0x2C20,941,1},
         {0x2C21,942,1},
         {0x2C22,943,1},
         {0x2C23,944,1},
         {0x2C24,945,1},
         {0x2C25,946,1},
         {0x2C26,947,1},
         {0x2C27,948,1},
         {0x2C28,949,1},
         {0x2C29,950,1},
         {0x2C2A,951,1},
         {0x2C2B,952,1},
         {0x2C2C,953,1},
         {0x2C2D,954,1},
         {0x2C2E,955,1},
         {0x2C60,956,1},
         {0x2C62,957,1},
         {0x2C63,958,1},
         {0x2C64,959,1},
         {0x2C67,960,1},
         {0x2C69,961,1},
         {0x2C6B,962,1},
         {0x2C6D,963,1},
         {0x2C6E,964,1},
         {0x2C6F,965,1},
         {0x2C70,966,1},
         {0x2C72,967,1},
         {0x2C75,968,1},
         {0x2C7E,969,1},
         {0x2C7F,970,1},
         {0x2C80,971,1},
         {0x2C82,972,1},
         {0x2C84,973,1},
         {0x2C86,974,1},
         {0x2C88,975,1},
         {0x2C8A,976,1},
         {0x2C8C,977,1},
         {0x2C8E,978,1},
         {0x2C90,979,1},
         {0x2C92,980,1},
         {0x2C94,981,1},
         {0x2C96,982,1},
         {0x2C98,983,1},
         {0x2C9A,984,1},
         {0x2C9C,985,1},
         {0x2C9E,986,1},
         {0x2CA0,987,1},
         {0x2CA2,988,1},
         {0x2CA4,989,1},
         {0x2CA6,990,1},
         {0x2CA8,991,1},
         {0x2CAA,992,1},
         {0x2CAC,993,1},
         {0x2CAE,994,1},
         {0x2CB0,995,1},
         {0x2CB2,996,1},
         {0x2CB4,997,1},
         {0x2CB6,998,1},
         {0x2CB8,999,1},
         {0x2CBA,1000,1},
         {0x2CBC,1001,1},
         {0x2CBE,1002,1},
         {0x2CC0,1003,1},
         {0x2CC2,1004,1},
         {0x2CC4,1005,1},
         {0x2CC6,1006,1},
         {0x2CC8,1007,1},
         {0x2CCA,1008,1},
         {0x2CCC,1009,1},
         {0x2CCE,1010,1},
         {0x2CD0,1011,1},
         {0x2CD2,1012,1},
         {0x2CD4,1013,1},
         {0x2CD6,1014,1},
         {0x2CD8,1015,1},
         {0x2CDA,1016,1},
         {0x2CDC,1017,1},
         {0x2CDE,1018,1},
         {0x2CE0,1019,1},
         {0x2CE2,1020,1},
         {0x2CEB,1021,1},
         {0x2CED,1022,1},
         {0x2CF2,1023,1},
         {0xA640,1024,1},
         {0xA642,1025,1},
         {0xA644,1026,1},
         {0xA646,1027,1},
         {0xA648,1028,1},
         {0xA64A,1029,1},
         {0xA64C,1030,1},
         {0xA64E,1031,1},
         {0xA650,1032,1},
         {0xA652,1033,1},
         {0xA654,1034,1},
         {0xA656,1035,1},
         {0xA658,1036,1},
         {0xA65A,1037,1},
         {0xA65C,1038,1},
         {0xA65E,1039,1},
         {0xA660,1040,1},
         {0xA662,1041,1},
         {0xA664,1042,1},
         {0xA666,1043,1},
         {0xA668,1044,1},
         {0xA66A,1045,1},
         {0xA66C,1046,1},
         {0xA680,1047,1},
         {0xA682,1048,1},
         {0xA684,1049,1},
         {0xA686,1050,1},
         {0xA688,1051,1},
         {0xA68A,1052,1},
         {0xA68C,1053,1},
         {0xA68E,1054,1},
         {0xA690,1055,1},
         {0xA692,1056,1},
         {0xA694,1057,1},
         {0xA696,1058,1},
         {0xA698,1059,1},
         {0xA69A,1060,1},
         {0xA722,1061,1},
         {0xA724,1062,1},
         {0xA726,1063,1},
         {0xA728,1064,1},
         {0xA72A,1065,1},
         {0xA72C,1066,1},
         {0xA72E,1067,1},
         {0xA732,1068,1},
         {0xA734,1069,1},
         {0xA736,1070,1},
         {0xA738,1071,1},
         {0xA73A,1072,1},
         {0xA73C,1073,1},
         {0xA73E,1074,1},
         {0xA740,1075,1},
         {0xA742,1076,1},
         {0xA744,1077,1},
         {0xA746,1078,1},
         {0xA748,1079,1},
         {0xA74A,1080,1},
         {0xA74C,1081,1},
         {0xA74E,1082,1},
         {0xA750,1083,1},
         {0xA752,1084,1},
         {0xA754,1085,1},
         {0xA756,1086,1},
         {0xA758,1087,1},
         {0xA75A,1088,1},
         {0xA75C,1089,1},
         {0xA75E,1090,1},
         {0xA760,1091,1},
         {0xA762,1092,1},
         {0xA764,1093,1},
         {0xA766,1094,1},
         {0xA768,1095,1},
         {0xA76A,1096,1},
         {0xA76C,1097,1},
         {0xA76E,1098,1},
         {0xA779,1099,1},
         {0xA77B,1100,1},
         {0xA77D,1101,1},
         {0xA77E,1102,1},
         {0xA780,1103,1},
         {0xA782,1104,1},
         {0xA784,1105,1},
         {0xA786,1106,1},
         {0xA78B,1107,1},
         {0xA78D,1108,1},
         {0xA790,1109,1},
         {0xA792,1110,1},
         {0xA796,1111,1},
         {0xA798,1112,1},
         {0xA79A,1113,1},
         {0xA79C,1114,1},
         {0xA79E,1115,1},
         {0xA7A0,1116,1},
         {0xA7A2,1117,1},
         {0xA7A4,1118,1},
         {0xA7A6,1119,1},
         {0xA7A8,1120,1},
         {0xA7AA,1121,1},
         {0xA7AB,1122,1},
         {0xA7AC,1123,1},
         {0xA7AD,1124,1},
         {0xA7AE,1125,1},
         {0xA7B0,1126,1},
         {0xA7B1,1127,1},
         {0xA7B2,1128,1},
         {0xA7B3,1129,1},
         {0xA7B4,1130,1},
         {0xA7B6,1131,1},
         {0xA7B8,1132,1},
         {0xA7BA,1133,1},
         {0xA7BC,1134,1},
         {0xA7BE,1135,1},
         {0xA7C2,1136,1},
         {0xA7C4,1137,1},
         {0xA7C5,1138,1},
         {0xA7C6,1139,1},
         {0xA7C7,1140,1},
         {0xA7C9,1141,1},
         {0xA7F5,1142,1},
         {0xFF21,1143,1},
         {0xFF22,1144,1},
         {0xFF23,1145,1},
         {0xFF24,1146,1},
         {0xFF25,1147,1},
         {0xFF26,1148,1},
         {0xFF27,1149,1},
         {0xFF28,1150,1},
         {0xFF29,1151,1},
         {0xFF2A,1152,1},
         {0xFF2B,1153,1},
         {0xFF2C,1154,1},
         {0xFF2D,1155,1},
         {0xFF2E,1156,1},
         {0xFF2F,1157,1},
         {0xFF30,1158,1},
         {0xFF31,1159,1},
         {0xFF32,1160,1},
         {0xFF33,1161,1},
         {0xFF34,1162,1},
         {0xFF35,1163,1},
         {0xFF36,1164,1},
         {0xFF37,1165,1},
         {0xFF38,1166,1},
         {0xFF39,1167,1},
         {0xFF3A,1168,1},
         {0x10400,1169,1},
         {0x10401,1170,1},
         {0x10402,1171,1},
         {0x10403,1172,1},
         {0x10404,1173,1},
         {0x10405,1174,1},
         {0x10406,1175,1},
         {0x10407,1176,1},
         {0x10408,1177,1},
         {0x10409,1178,1},
         {0x1040A,1179,1},
         {0x1040B,1180,1},
         {0x1040C,1181,1},
         {0x1040D,1182,1},
         {0x1040E,1183,1},
         {0x1040F,1184,1},
         {0x10410,1185,1},
         {0x10411,1186,1},
         {0x10412,1187,1},
         {0x10413,1188,1},
         {0x10414,1189,1},
         {0x10415,1190,1},
         {0x10416,1191,1},
         {0x10417,1192,1},
         {0x10418,1193,1},
         {0x10419,1194,1},
         {0x1041A,1195,1},
         {0x1041B,1196,1},
         {0x1041C,1197,1},
         {0x1041D,1198,1},
         {0x1041E,1199,1},
         {0x1041F,1200,1},
         {0x10420,1201,1},
         {0x10421,1202,1},
         {0x10422,1203,1},
         {0x10423,1204,1},
         {0x10424,1205,1},
         {0x10425,1206,1},
         {0x10426,1207,1},
         {0x10427,1208,1},
         {0x104B0,1209,1},
         {0x104B1,1210,1},
         {0x104B2,1211,1},
         {0x104B3,1212,1},
         {0x104B4,1213,1},
         {0x104B5,1214,1},
         {0x104B6,1215,1},
         {0x104B7,1216,1},
         {0x104B8,1217,1},
         {0x104B9,1218,1},
         {0x104BA,1219,1},
         {0x104BB,1220,1},
         {0x104BC,1221,1},
         {0x104BD,1222,1},
         {0x104BE,1223,1},
         {0x104BF,1224,1},
         {0x104C0,1225,1},
         {0x104C1,1226,1},
         {0x104C2,1227,1},
         {0x104C3,1228,1},
         {0x104C4,1229,1},
         {0x104C5,1230,1},
         {0x104C6,1231,1},
         {0x104C7,1232,1},
         {0x104C8,1233,1},
         {0x104C9,1234,1},
         {0x104CA,1235,1},
         {0x104CB,1236,1},
         {0x104CC,1237,1},
         {0x104CD,1238,1},
         {0x104CE,1239,1},
         {0x104CF,1240,1},
         {0x104D0,1241,1},
         {0x104D1,1242,1},
         {0x104D2,1243,1},
         {0x104D3,1244,1},
         {0x10C80,1245,1},
         {0x10C81,1246,1},
         {0x10C82,1247,1},
         {0x10C83,1248,1},
         {0x10C84,1249,1},
         {0x10C85,1250,1},
         {0x10C86,1251,1},
         {0x10C87,1252,1},
         {0x10C88,1253,1},
         {0x10C89,1254,1},
         {0x10C8A,1255,1},
         {0x10C8B,1256,1},
         {0x10C8C,1257,1},
         {0x10C8D,1258,1},
         {0x10C8E,1259,1},
         {0x10C8F,1260,1},
         {0x10C90,1261,1},
         {0x10C91,1262,1},
         {0x10C92,1263,1},
         {0x10C93,1264,1},
         {0x10C94,1265,1},
         {0x10C95,1266,1},
         {0x10C96,1267,1},
         {0x10C97,1268,1},
         {0x10C98,1269,1},
         {0x10C99,1270,1},
         {0x10C9A,1271,1},
         {0x10C9B,1272,1},
         {0x10C9C,1273,1},
         {0x10C9D,1274,1},
         {0x10C9E,1275,1},
         {0x10C9F,1276,1},
         {0x10CA0,1277,1},
         {0x10CA1,1278,1},
         {0x10CA2,1279,1},
         {0x10CA3,1280,1},
         {0x10CA4,1281,1},
         {0x10CA5,1282,1},
         {0x10CA6,1283,1},
         {0x10CA7,1284,1},
         {0x10CA8,1285,1},
         {0x10CA9,1286,1},
         {0x10CAA,1287,1},
         {0x10CAB,1288,1},
         {0x10CAC,1289,1},
         {0x10CAD,1290,1},
         {0x10CAE,1291,1},
         {0x10CAF,1292,1},
         {0x10CB0,1293,1},
         {0x10CB1,1294,1},
         {0x10CB2,1295,1},
         {0x118A0,1296,1},
         {0x118A1,1297,1},
         {0x118A2,1298,1},
         {0x118A3,1299,1},
         {0x118A4,1300,1},
         {0x118A5,1301,1},
         {0x118A6,1302,1},
         {0x118A7,1303,1},
         {0x118A8,1304,1},
         {0x118A9,1305,1},
         {0x118AA,1306,1},
         {0x118AB,1307,1},
         {0x118AC,1308,1},
         {0x118AD,1309,1},
         {0x118AE,1310,1},
         {0x118AF,1311,1},
         {0x118B0,1312,1},
         {0x118B1,1313,1},
         {0x118B2,1314,1},
         {0x118B3,1315,1},
         {0x118B4,1316,1},
         {0x118B5,1317,1},
         {0x118B6,1318,1},
         {0x118B7,1319,1},
         {0x118B8,1320,1},
         {0x118B9,1321,1},
         {0x118BA,1322,1},
         {0x118BB,1323,1},
         {0x118BC,1324,1},
         {0x118BD,1325,1},
         {0x118BE,1326,1},
         {0x118BF,1327,1},
         {0x16E40,1328,1},
         {0x16E41,1329,1},
         {0x16E42,1330,1},
         {0x16E43,1331,1},
         {0x16E44,1332,1},
         {0x16E45,1333,1},
         {0x16E46,1334,1},
         {0x16E47,1335,1},
         {0x16E48,1336,1},
         {0x16E49,1337,1},
         {0x16E4A,1338,1},
         {0x16E4B,1339,1},
         {0x16E4C,1340,1},
         {0x16E4D,1341,1},
         {0x16E4E,1342,1},
         {0x16E4F,1343,1},
         {0x16E50,1344,1},
         {0x16E51,1345,1},
         {0x16E52,1346,1},
         {0x16E53,1347,1},
         {0x16E54,1348,1},
         {0x16E55,1349,1},
         {0x16E56,1350,1},
         {0x16E57,1351,1},
         {0x16E58,1352,1},
         {0x16E59,1353,1},
         {0x16E5A,1354,1},
         {0x16E5B,1355,1},
         {0x16E5C,1356,1},
         {0x16E5D,1357,1},
         {0x16E5E,1358,1},
         {0x16E5F,1359,1},
         {0x1E900,1360,1},
         {0x1E901,1361,1},
         {0x1E902,1362,1},
         {0x1E903,1363,1},
         {0x1E904,1364,1},
         {0x1E905,1365,1},
         {0x1E906,1366,1},
         {0x1E907,1367,1},
         {0x1E908,1368,1},
         {0x1E909,1369,1},
         {0x1E90A,1370,1},
         {0x1E90B,1371,1},
         {0x1E90C,1372,1},
         {0x1E90D,1373,1},
         {0x1E90E,1374,1},
         {0x1E90F,1375,1},
         {0x1E910,1376,1},
         {0x1E911,1377,1},
         {0x1E912,1378,1},
         {0x1E913,1379,1},
         {0x1E914,1380,1},
         {0x1E915,1381,1},
         {0x1E916,1382,1},
         {0x1E917,1383,1},
         {0x1E918,1384,1},
         {0x1E919,1385,1},
         {0x1E91A,1386,1},
         {0x1E91B,1387,1},
         {0x1E91C,1388,1},
         {0x1E91D,1389,1},
         {0x1E91E,1390,1},
         {0x1E91F,1391,1},
         {0x1E920,1392,1},
         {0x1E921,1393,1},
      };
      static const char32_t lowerValues[] = {
         0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,
         0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,
         0x79,0x7A,0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,
         0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,
         0xF6,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0x101,0x103,0x105,0x107,
         0x109,0x10B,0x10D,0x10F,0x111,0x113,0x115,0x117,0x119,0x11B,0x11D,0x11F,
         0x121,0x123,0x125,0x127,0x129,0x12B,0x12D,0x12F,0x69,0x307,0x133,0x135,
         0x137,0x13A,0x13C,0x13E,0x140,0x142,0x144,0x146,0x148,0x14B,0x14D,0x14F,
         0x151,0x153,0x155,0x157,0x159,0x15B,0x15D,0x15F,0x161,0x163,0x165,0x167,
         0x169,0x16B,0x16D,0x16F,0x171,0x173,0x175,0x177,0xFF,0x17A,0x17C,0x17E,
         0x253,0x183,0x185,0x254,0x188,0x256,0x257,0x18C,0x1DD,0x259,0x25B,0x192,
         0x260,0x263,0x269,0x268,0x199,0x26F,0x272,0x275,0x1A1,0x1A3,0x1A5,0x280,
         0x1A8,0x283,0x1AD,0x288,0x1B0,0x28A,0x28B,0x1B4,0x1B6,0x292,0x1B9,0x1BD,
         0x1C6,0x1C6,0x1C9,0x1C9,0x1CC,0x1CC,0x1CE,0x1D0,0x1D2,0x1D4,0x1D6,0x1D8,
         0x1DA,0x1DC,0x1DF,0x1E1,0x1E3,0x1E5,0x1E7,0x1E9,0x1EB,0x1ED,0x1EF,0x1F3,
         0x1F3,0x1F5,0x195,0x1BF,0x1F9,0x1FB,0x1FD,0x1FF,0x201,0x203,0x205,0x207,
         0x209,0x20B,0x20D,0x20F,0x211,0x213,0x215,0x217,0x219,0x21B,0x21D,0x21F,
         0x19E,0x223,0x225,0x227,0x229,0x22B,0x22D,0x22F,0x231,0x233,0x2C65,0x23C,
         0x19A,0x2C66,0x242,0x180,0x289,0x28C,0x247,0x249,0x24B,0x24D,0x24F,0x371,
         0x373,0x377,0x3F3,0x3AC,0x3AD,0x3AE,0x3AF,0x3CC,0x3CD,0x3CE,0x3B1,0x3B2,
         0x3B3,0x3B4,0x3B5,0x3B6,0x3B7,0x3B8,0x3B9,0x3BA,0x3BB,0x3BC,0x3BD,0x3BE,
         0x3BF,0x3C0,0x3C1,0x3C3,0x3C4,0x3C5,0x3C6,0x3C7,0x3C8,0x3C9,0x3CA,0x3CB,
         0x3D7,0x3D9,0x3DB,0x3DD,0x3DF,0x3E1,0x3E3,0x3E5,0x3E7,0x3E9,0x3EB,0x3ED,
         0x3EF,0x3B8,0x3F8,0x3F2,0x3FB,0x37B,0x37C,0x37D,0x450,0x451,0x452,0x453,
         0x454,0x455,0x456,0x457,0x458,0x459,0x45A,0x45B,0x45C,0x45D,0x45E,0x45F,
         0x430,0x431,0x432,0x433,0x434,0x435,0x436,0x437,0x438,0x439,0x43A,0x43B,
         0x43C,0x43D,0x43E,0x43F,0x440,0x441,0x442,0x443,0x444,0x445,0x446,0x447,
         0x448,0x449,0x44A,0x44B,0x44C,0x44D,0x44E,0x44F,0x461,0x463,0x465,0x467,
         0x469,0x46B,0x46D,0x46F,0x471,0x473,0x475,0x477,0x479,0x47B,0x47D,0x47F,
         0x481,0x48B,0x48D,0x48F,0x491,0x493,0x495,0x497,0x499,0x49B,0x49D,0x49F,
         0x4A1,0x4A3,0x4A5,0x4A7,0x4A9,0x4AB,0x4AD,0x4AF,0x4B1,0x4B3,0x4B5,0x4B7,
         0x4B9,0x4BB,0x4BD,0x4BF,0x4CF,0x4C2,0x4C4,0x4C6,0x4C8,0x4CA,0x4CC,0x4CE,
         0x4D1,0x4D3,0x4D5,0x4D7,0x4D9,0x4DB,0x4DD,0x4DF,0x4E1,0x4E3,0x4E5,0x4E7,
         0x4E9,0x4EB,0x4ED,0x4EF,0x4F1,0x4F3,0x4F5,0x4F7,0x4F9,0x4FB,0x4FD,0x4FF,
         0x501,0x503,0x505,0x507,0x509,0x50B,0x50D,0x50F,0x511,0x513,0x515,0x517,
         0x519,0x51B,0x51D,0x51F,0x521,0x523,0x525,0x527,0x529,0x52B,0x52D,0x52F,
         0x561,0x562,0x563,0x564,0x565,0x566,0x567,0x568,0x569,0x56A,0x56B,0x56C,
         0x56D,0x56E,0x56F,0x570,0x571,0x572,0x573,0x574,0x575,0x576,0x577,0x578,
         0x579,0x57A,0x57B,0x57C,0x57D,0x57E,0x57F,0x580,0x581,0x582,0x583,0x584,
         0x585,0x586,0x2D00,0x2D01,0x2D02,0x2D03,0x2D04,0x2D05,0x2D06,0x2D07,0x2D08,0x2D09,
         0x2D0A,0x2D0B,0x2D0C,0x2D0D,0x2D0E,0x2D0F,0x2D10,0x2D11,0x2D12,0x2D13,0x2D14,0x2D15,
         0x2D16,0x2D17,0x2D18,0x2D19,0x2D1A,0x2D1B,0x2D1C,0x2D1D,0x2D1E,0x2D1F,0x2D20,0x2D21,
         0x2D22,0x2D23,0x2D24,0x2D25,0x2D27,0x2D2D,0xAB70,0xAB71,0xAB72,0xAB73,0xAB74,0xAB75,
         0xAB76,0xAB77,0xAB78,0xAB79,0xAB7A,0xAB7B,0xAB7C,0xAB7D,0xAB7E,0xAB7F,0xAB80,0xAB81,
         0xAB82,0xAB83,0xAB84,0xAB85,0xAB86,0xAB87,0xAB88,0xAB89,0xAB8A,0xAB8B,0xAB8C,0xAB8D,
         0xAB8E,0xAB8F,0xAB90,0xAB91,0xAB92,0xAB93,0xAB94,0xAB95,0xAB96,0xAB97,0xAB98,0xAB99,
         0xAB9A,0xAB9B,0xAB9C,0xAB9D,0xAB9E,0xAB9F,0xABA0,0xABA1,0xABA2,0xABA3,0xABA4,0xABA5,
         0xABA6,0xABA7,0xABA8,0xABA9,0xABAA,0xABAB,0xABAC,0xABAD,0xABAE,0xABAF,0xABB0,0xABB1,
         0xABB2,0xABB3,0xABB4,0xABB5,0xABB6,0xABB7,0xABB8,0xABB9,0xABBA,0xABBB,0xABBC,0xABBD,
         0xABBE,0xABBF,0x13F8,0x13F9,0x13FA,0x13FB,0x13FC,0x13FD,0x10D0,0x10D1,0x10D2,0x10D3,
         0x10D4,0x10D5,0x10D6,0x10D7,0x10D8,0x10D9,0x10DA,0x10DB,0x10DC,0x10DD,0x10DE,0x10DF,
         0x10E0,0x10E1,0x10E2,0x10E3,0x10E4,0x10E5,0x10E6,0x10E7,0x10E8,0x10E9,0x10EA,0x10EB,
         0x10EC,0x10ED,0x10EE,0x10EF,0x10F0,0x10F1,0x10F2,0x10F3,0x10F4,0x10F5,0x10F6,0x10F7,
         0x10F8,0x10F9,0x10FA,0x10FD,0x10FE,0x10FF,0x1E01,0x1E03,0x1E05,0x1E07,0x1E09,0x1E0B,
         0x1E0D,0x1E0F,0x1E11,0x1E13,0x1E15,0x1E17,0x1E19,0x1E1B,0x1E1D,0x1E1F,0x1E21,0x1E23,
         0x1E25,0x1E27,0x1E29,0x1E2B,0x1E2D,0x1E2F,0x1E31,0x1E33,0x1E35,0x1E37,0x1E39,0x1E3B,
         0x1E3D,0x1E3F,0x1E41,0x1E43,0x1E45,0x1E47,0x1E49,0x1E4B,0x1E4D,0x1E4F,0x1E51,0x1E53,
         0x1E55,0x1E57,0x1E59,0x1E5B,0x1E5D,0x1E5F,0x1E61,0x1E63,0x1E65,0x1E67,0x1E69,0x1E6B,
         0x1E6D,0x1E6F,0x1E71,0x1E73,0x1E75,0x1E77,0x1E79,0x1E7B,0x1E7D,0x1E7F,0x1E81,0x1E83,
         0x1E85,0x1E87,0x1E89,0x1E8B,0x1E8D,0x1E8F,0x1E91,0x1E93,0x1E95,0xDF,0x1EA1,0x1EA3,
         0x1EA5,0x1EA7,0x1EA9,0x1EAB,0x1EAD,0x1EAF,0x1EB1,0x1EB3,0x1EB5,0x1EB7,0x1EB9,0x1EBB,
         0x1EBD,0x1EBF,0x1EC1,0x1EC3,0x1EC5,0x1EC7,0x1EC9,0x1ECB,0x1ECD,0x1ECF,0x1ED1,0x1ED3,
         0x1ED5,0x1ED7,0x1ED9,0x1EDB,0x1EDD,0x1EDF,0x1EE1,0x1EE3,0x1EE5,0x1EE7,0x1EE9,0x1EEB,
         0x1EED,0x1EEF,0x1EF1,0x1EF3,0x1EF5,0x1EF7,0x1EF9,0x1EFB,0x1EFD,0x1EFF,0x1F00,0x1F01,
         0x1F02,0x1F03,0x1F04,0x1F05,0x1F06,0x1F07,0x1F10,0x1F11,0x1F12,0x1F13,0x1F14,0x1F15,
         0x1F20,0x1F21,0x1F22,0x1F23,0x1F24,0x1F25,0x1F26,0x1F27,0x1F30,0x1F31,0x1F32,0x1F33,
         0x1F34,0x1F35,0x1F36,0x1F37,0x1F40,0x1F41,0x1F42,0x1F43,0x1F44,0x1F45,0x1F51,0x1F53,
         0x1F55,0x1F57,0x1F60,0x1F61,0x1F62,0x1F63,0x1F64,0x1F65,0x1F66,0x1F67,0x1F80,0x1F81,
         0x1F82,0x1F83,0x1F84,0x1F85,0x1F86,0x1F87,0x1F90,0x1F91,0x1F92,0x1F93,0x1F94,0x1F95,
         0x1F96,0x1F97,0x1FA0,0x1FA1,0x1FA2,0x1FA3,0x1FA4,0x1FA5,0x1FA6,0x1FA7,0x1FB0,0x1FB1,
         0x1F70,0x1F71,0x1FB3,0x1F72,0x1F73,0x1F74,0x1F75,0x1FC3,0x1FD0,0x1FD1,0x1F76,0x1F77,
         0x1FE0,0x1FE1,0x1F7A,0x1F7B,0x1FE5,0x1F78,0x1F79,0x1F7C,0x1F7D,0x1FF3,0x3C9,0x6B,
         0xE5,0x214E,0x2170,0x2171,0x2172,0x2173,0x2174,0x2175,0x2176,0x2177,0x2178,0x2179,
         0x217A,0x217B,0x217C,0x217D,0x217E,0x217F,0x2184,0x24D0,0x24D1,0x24D2,0x24D3,0x24D4,
         0x24D5,0x24D6,0x24D7,0x24D8,0x24D9,0x24DA,0x24DB,0x24DC,0x24DD,0x24DE,0x24DF,0x24E0,
         0x24E1,0x24E2,0x24E3,0x24E4,0x24E5,0x24E6,0x24E7,0x24E8,0x24E9,0x2C30,0x2C31,0x2C32,
         0x2C33,0x2C34,0x2C35,0x2C36,0x2C37,0x2C38,0x2C39,0x2C3A,0x2C3B,0x2C3C,0x2C3D,0x2C3E,
         0x2C3F,0x2C40,0x2C41,0x2C42,0x2C43,0x2C44,0x2C45,0x2C46,0x2C47,0x2C48,0x2C49,0x2C4A,
         0x2C4B,0x2C4C,0x2C4D,0x2C4E,0x2C4F,0x2C50,0x2C51,0x2C52,0x2C53,0x2C54,0x2C55,0x2C56,
         0x2C57,0x2C58,0x2C59,0x2C5A,0x2C5B,0x2C5C,0x2C5D,0x2C5E,0x2C61,0x26B,0x1D7D,0x27D,
         0x2C68,0x2C6A,0x2C6C,0x251,0x271,0x250,0x252,0x2C73,0x2C76,0x23F,0x240,0x2C81,
         0x2C83,0x2C85,0x2C87,0x2C89,0x2C8B,0x2C8D,0x2C8F,0x2C91,0x2C93,0x2C95,0x2C97,0x2C99,
         0x2C9B,0x2C9D,0x2C9F,0x2CA1,0x2CA3,0x2CA5,0x2CA7,0x2CA9,0x2CAB,0x2CAD,0x2CAF,0x2CB1,
         0x2CB3,0x2CB5,0x2CB7,0x2CB9,0x2CBB,0x2CBD,0x2CBF,0x2CC1,0x2CC3,0x2CC5,0x2CC7,0x2CC9,
         0x2CCB,0x2CCD,0x2CCF,0x2CD1,0x2CD3,0x2CD5,0x2CD7,0x2CD9,0x2CDB,0x2CDD,0x2CDF,0x2CE1,
         0x2CE3,0x2CEC,0x2CEE,0x2CF3,0xA641,0xA643,0xA645,0xA647,0xA649,0xA64B,0xA64D,0xA64F,
         0xA651,0xA653,0xA655,0xA657,0xA659,0xA65B,0xA65D,0xA65F,0xA661,0xA663,0xA665,0xA667,
         0xA669,0xA66B,0xA66D,0xA681,0xA683,0xA685,0xA687,0xA689,0xA68B,0xA68D,0xA68F,0xA691,
         0xA693,0xA695,0xA697,0xA699,0xA69B,0xA723,0xA725,0xA727,0xA729,0xA72B,0xA72D,0xA72F,
         0xA733,0xA735,0xA737,0xA739,0xA73B,0xA73D,0xA73F,0xA741,0xA743,0xA745,0xA747,0xA749,
         0xA74B,0xA74D,0xA74F,0xA751,0xA753,0xA755,0xA757,0xA759,0xA75B,0xA75D,0xA75F,0xA761,
         0xA763,0xA765,0xA767,0xA769,0xA76B,0xA76D,0xA76F,0xA77A,0xA77C,0x1D79,0xA77F,0xA781,
         0xA783,0xA785,0xA787,0xA78C,0x265,0xA791,0xA793,0xA797,0xA799,0xA79B,0xA79D,0xA79F,
         0xA7A1,0xA7A3,0xA7A5,0xA7A7,0xA7A9,0x266,0x25C,0x261,0x26C,0x26A,0x29E,0x287,
         0x29D,0xAB53,0xA7B5,0xA7B7,0xA7B9,0xA7BB,0xA7BD,0xA7BF,0xA7C3,0xA794,0x282,0x1D8E,
         0xA7C8,0xA7CA,0xA7F6,0xFF41,0xFF42,0xFF43,0xFF44,0xFF45,0xFF46,0xFF47,0xFF48,0xFF49,
         0xFF4A,0xFF4B,0xFF4C,0xFF4D,0xFF4E,0xFF4F,0xFF50,0xFF51,0xFF52,0xFF53,0xFF54,0xFF55,
         0xFF56,0xFF57,0xFF58,0xFF59,0xFF5A,0x10428,0x10429,0x1042A,0x1042B,0x1042C,0x1042D,0x1042E,
         0x1042F,0x10430,0x10431,0x10432,0x10433,0x10434,0x10435,0x10436,0x10437,0x10438,0x10439,0x1043A,
         0x1043B,0x1043C,0x1043D,0x1043E,0x1043F,0x10440,0x10441,0x10442,0x10443,0x10444,0x10445,0x10446,
         0x10447,0x10448,0x10449,0x1044A,0x1044B,0x1044C,0x1044D,0x1044E,0x1044F,0x104D8,0x104D9,0x104DA,
         0x104DB,0x104DC,0x104DD,0x104DE,0x104DF,0x104E0,0x104E1,0x104E2,0x104E3,0x104E4,0x104E5,0x104E6,
         0x104E7,0x104E8,0x104E9,0x104EA,0x104EB,0x104EC,0x104ED,0x104EE,0x104EF,0x104F0,0x104F1,0x104F2,
         0x104F3,0x104F4,0x104F5,0x104F6,0x104F7,0x104F8,0x104F9,0x104FA,0x104FB,0x10CC0,0x10CC1,0x10CC2,
         0x10CC3,0x10CC4,0x10CC5,0x10CC6,0x10CC7,0x10CC8,0x10CC9,0x10CCA,0x10CCB,0x10CCC,0x10CCD,0x10CCE,
         0x10CCF,0x10CD0,0x10CD1,0x10CD2,0x10CD3,0x10CD4,0x10CD5,0x10CD6,0x10CD7,0x10CD8,0x10CD9,0x10CDA,
         0x10CDB,0x10CDC,0x10CDD,0x10CDE,0x10CDF,0x10CE0,0x10CE1,0x10CE2,0x10CE3,0x10CE4,0x10CE5,0x10CE6,
         0x10CE7,0x10CE8,0x10CE9,0x10CEA,0x10CEB,0x10CEC,0x10CED,0x10CEE,0x10CEF,0x10CF0,0x10CF1,0x10CF2,
         0x118C0,0x118C1,0x118C2,0x118C3,0x118C4,0x118C5,0x118C6,0x118C7,0x118C8,0x118C9,0x118CA,0x118CB,
         0x118CC,0x118CD,0x118CE,0x118CF,0x118D0,0x118D1,0x118D2,0x118D3,0x118D4,0x118D5,0x118D6,0x118D7,
         0x118D8,0x118D9,0x118DA,0x118DB,0x118DC,0x118DD,0x118DE,0x118DF,0x16E60,0x16E61,0x16E62,0x16E63,
         0x16E64,0x16E65,0x16E66,0x16E67,0x16E68,0x16E69,0x16E6A,0x16E6B,0x16E6C,0x16E6D,0x16E6E,0x16E6F,
         0x16E70,0x16E71,0x16E72,0x16E73,0x16E74,0x16E75,0x16E76,0x16E77,0x16E78,0x16E79,0x16E7A,0x16E7B,
         0x16E7C,0x16E7D,0x16E7E,0x16E7F,0x1E922,0x1E923,0x1E924,0x1E925,0x1E926,0x1E927,0x1E928,0x1E929,
         0x1E92A,0x1E92B,0x1E92C,0x1E92D,0x1E92E,0x1E92F,0x1E930,0x1E931,0x1E932,0x1E933,0x1E934,0x1E935,
         0x1E936,0x1E937,0x1E938,0x1E939,0x1E93A,0x1E93B,0x1E93C,0x1E93D,0x1E93E,0x1E93F,0x1E940,0x1E941,
         0x1E942,0x1E943,
      };
      const Mapping* mappings=upper ? upperMap : lowerMap;
      const std::size_t count=upper ? sizeof(upperMap)/sizeof(upperMap[0]) : sizeof(lowerMap)/sizeof(lowerMap[0]);
      const char32_t* values=upper ? upperValues : lowerValues;
      std::wstring result;
      result.reserve(static_cast<std::size_t>(length()));
      for(int i=0;i<length();++i) {
         char32_t code=static_cast<char32_t>(m_str.c_str()[i]);
         if(code>=0xD800 && code<=0xDBFF && i+1<length()) {
            const char32_t next=static_cast<char32_t>(m_str.c_str()[i+1]);
            if(next>=0xDC00 && next<=0xDFFF) { code=0x10000+((code-0xD800)<<10)+(next-0xDC00); ++i; }
         }
         std::size_t first=0,last=count;
         while(first<last) { const std::size_t mid=first+(last-first)/2; if(mappings[mid].code<code) first=mid+1; else last=mid; }
         const bool found=first<count && mappings[first].code==code;
         const char32_t* mapped=found ? values+mappings[first].offset : &code;
         const unsigned size=found ? mappings[first].size : 1;
         for(unsigned j=0;j<size;++j) {
            char32_t ch=mapped[j];
            checked_size(result.size()+(ch>0xFFFF ? 2u : 1u));
            if(ch<=0xFFFF) result.push_back(static_cast<wchar_t>(ch));
            else { ch-=0x10000; result.push_back(static_cast<wchar_t>(0xD800+(ch>>10))); result.push_back(static_cast<wchar_t>(0xDC00+(ch&0x3FF))); }
         }
      }
      return QString(result);
   }

   UnicodeString m_str;  // Внутреннее представление
   bool m_null = true; // UnicodeString alone cannot distinguish null from empty.
   wchar_t m_empty_terminator = 0;

   // Exact positive binary rational, represented as decimal digits and the
   // position of the decimal point. No floating-point parsing or CRT rounding.
   static std::string binaryDecimal(unsigned long long mantissa, int exponent, int& point) {
      if(!mantissa) { point=1; return "0"; }
      while(exponent<0 && !(mantissa & 1)) { mantissa>>=1; ++exponent; }
      // Base 10^8 limbs keep exact midpoint conversion inexpensive on Win32.
      const unsigned radix=100000000;
      std::vector<unsigned> limbs;
      do { limbs.push_back(static_cast<unsigned>(mantissa%radix)); mantissa/=radix; } while(mantissa);
      const unsigned multiplier=exponent<0 ? 5 : 2;
      const int iterations=exponent<0 ? -exponent : exponent;
      for(int j=0;j<iterations;++j) {
         unsigned carry=0;
         for(std::size_t i=0;i<limbs.size();++i) {
            const unsigned product=limbs[i]*multiplier+carry;
            limbs[i]=product%radix; carry=product/radix;
         }
         if(carry) limbs.push_back(carry);
      }
      std::string digits=std::to_string(limbs.back());
      for(std::size_t i=limbs.size()-1;i>0;--i) {
         const std::string chunk=std::to_string(limbs[i-1]);
         digits.append(8-chunk.size(),'0'); digits+=chunk;
      }
      point=static_cast<int>(digits.size())+(exponent<0 ? exponent : 0);
      while(digits.size()>1 && digits.back()=='0') digits.pop_back();
      return digits;
   }
   static int compareDecimal(const std::string& a, int aPoint, const std::string& b, int bPoint) {
      if(aPoint!=bPoint) return aPoint<bPoint ? -1 : 1;
      const std::size_t count=(std::max)(a.size(),b.size());
      for(std::size_t i=0;i<count;++i) {
         const char x=i<a.size() ? a[i] : '0', y=i<b.size() ? b[i] : '0';
         if(x!=y) return x<y ? -1 : 1;
      }
      return 0;
   }
   // Qt's short-decimal fast path performs one rounded operation on an exact
   // positive integer and an exact power of ten; the sign is applied afterward.
   // Emulate its directed rounding with integer comparisons, avoiding Win32
   // extended-precision intermediate values and leaving the FPU mode unchanged.
   static unsigned long long qtParsedRounding(const std::string& digits, int point,
                                              unsigned long long nearest) {
      if(digits.size()>15) return nearest;
      const int count=static_cast<int>(digits.size());
      const int exponent=point-count;
      if(exponent < -22 || exponent > 37-count) return nearest;
      const System::Math::TRoundingMode mode=System::Math::GetRoundMode();
      if(mode==System::Math::rmNearest) return nearest;
      const unsigned long long hidden=1ULL<<52;
      const int biased=static_cast<int>(nearest>>52);
      const unsigned long long mantissa=(nearest&(hidden-1))+(biased ? hidden : 0);
      int exactPoint;
      const std::string exact=binaryDecimal(mantissa,biased ? biased-1075 : -1074,exactPoint);
      const int relation=compareDecimal(digits,point,exact,exactPoint);
      if(mode==System::Math::rmUp && relation>0) return nearest+1;
      if((mode==System::Math::rmDown || mode==System::Math::rmTruncate) && relation<0)
         return nearest-1;
      return nearest;
   }
   // Compare a normalized decimal with the rounding interval of a finite,
   // positive binary64. A zero result proves the correctly rounded answer.
   static int decimalInterval(const std::string& digits, int point, unsigned long long bits) {
      const unsigned long long hidden=1ULL<<52;
      const int biased=static_cast<int>(bits>>52);
      const unsigned long long mantissa=(bits&(hidden-1))+(biased ? hidden : 0);
      const int exponent=biased ? biased-1075 : -1074;
      const bool closer=mantissa==hidden && biased>1;
      const bool even=(mantissa&1)==0;
      int boundaryPoint;
      const std::string lower=binaryDecimal(closer ? 4*mantissa-1 : 2*mantissa-1,
                                           exponent-(closer ? 2 : 1),boundaryPoint);
      const int low=compareDecimal(digits,point,lower,boundaryPoint);
      if(low<0 || (low==0 && !even)) return -1;
      const std::string upper=binaryDecimal(2*mantissa+1,exponent-1,boundaryPoint);
      const int high=compareDecimal(digits,point,upper,boundaryPoint);
      return high>0 || (high==0 && !even) ? 1 : 0;
   }
   static std::string shortestDigits(double value, int& point) {
      if(value==0) { point=1; return "0"; }
      unsigned long long bits;
      std::memcpy(&bits,&value,sizeof(bits));
      const int biasedExponent=static_cast<int>((bits>>52)&0x7FF);
      const unsigned long long hidden=1ULL<<52;
      const unsigned long long mantissa=(bits&(hidden-1))+(biasedExponent ? hidden : 0);
      const int exponent=biasedExponent ? biasedExponent-1075 : -1074;
      const std::string exact=binaryDecimal(mantissa,exponent,point);
      // Midpoints between adjacent doubles bound all decimals which read back
      // as this value. Powers of two have a closer lower neighbor (except at
      // the normal/subnormal boundary). Even mantissas include both endpoints.
      const bool closerLower=mantissa==hidden && biasedExponent>1;
      int lowerPoint, upperPoint;
      const std::string lower=binaryDecimal(closerLower ? 4*mantissa-1 : 2*mantissa-1,
                                            exponent-(closerLower ? 2 : 1),lowerPoint);
      const std::string upper=binaryDecimal(2*mantissa+1,exponent-1,upperPoint);
      const bool inclusive=(mantissa&1)==0;
      const auto inside=[&](const std::string& candidate,int candidatePoint) {
         const int low=compareDecimal(candidate,candidatePoint,lower,lowerPoint);
         const int high=compareDecimal(candidate,candidatePoint,upper,upperPoint);
         return (low>0 || (inclusive && low==0)) && (high<0 || (inclusive && high==0));
      };
      for(std::size_t count=1;count<exact.size();++count) {
         const std::string down=exact.substr(0,count);
         std::string up=down;
         std::size_t i=up.size();
         while(i>0 && up[i-1]=='9') up[--i]='0';
         int upPoint=point;
         if(i>0) ++up[i-1]; else { up.insert(up.begin(),'1'); ++upPoint; }
         const bool downFits=inside(down,point), upFits=inside(up,upPoint);
         if(!downFits && !upFits) continue;
         // Among equally short decimals select the nearest, with decimal ties
         // to even, matching Qt's double-conversion SHORTEST mode.
         const bool roundUp=exact[count]>'5' || (exact[count]=='5' &&
            (exact.find_first_not_of('0',count+1)!=std::string::npos || ((down.back()-'0')&1)));
         const bool useUp=upFits && (!downFits || roundUp);
         std::string result=useUp ? up : down;
         if(useUp) point=upPoint;
         while(result.size()>1 && result.back()=='0') result.pop_back();
         return result;
      }
      return exact;
   }
   static std::string shortestDouble(double value, char format) {
      int point;
      std::string digits=shortestDigits(std::fabs(value),point);
      const bool fixed=format=='f' || format=='F';
      const bool scientific=format=='e' || format=='E';
      int cutoff=6;
      if(point>0) {
         cutoff=static_cast<int>(digits.size())+(point>100 ? 6 : 5);
         if(static_cast<int>(digits.size())>point) ++cutoff;
      }
      // Qt's shortest 'g' chooses its notation by the resulting text length.
      if(scientific || (!fixed && point!=static_cast<int>(digits.size()) && (point<=-4 || point>cutoff))) {
         if(digits.size()>1) digits.insert(1,1,'.');
         const int exponent=point-1;
         digits.push_back(format=='E' || format=='G' ? 'E' : 'e');
         digits.push_back(exponent<0 ? '-' : '+');
         const std::string exp=std::to_string(exponent<0 ? -exponent : exponent);
         if(exp.size()<2) digits.push_back('0');
         digits+=exp;
      } else if(point<=0) {
         digits.insert(0,static_cast<std::size_t>(-point),'0');
         digits.insert(0,"0.");
      } else if(static_cast<std::size_t>(point)<digits.size()) {
         digits.insert(static_cast<std::size_t>(point),1,'.');
      } else {
         digits.append(static_cast<std::size_t>(point)-digits.size(),'0');
      }
      if(value!=0 && std::signbit(value)) digits.insert(digits.begin(),'-');
      return digits;
   }

   enum PrintFlag { PrintAlternate=1, PrintZero=2, PrintLeft=4, PrintSpace=8, PrintPlus=16, PrintGroup=32 };
   static void groupPrintDigits(std::string& digits, std::size_t end) {
      while(end>3) { end-=3; digits.insert(end,1,','); }
   }
   static QString printInteger(unsigned long long magnitude, bool negative, bool signedConversion,
                               int base, bool upper, int precision, int width, unsigned flags) {
      std::string digits=number(magnitude,base).toUtf8();
      // Qt's signed conversion suppresses zero at precision 0; unsigned does not.
      if(signedConversion && magnitude==0 && precision==0) digits.clear();
      if(base==10 && (flags & PrintGroup)) groupPrintDigits(digits,digits.size());
      if(precision>0 && static_cast<std::size_t>(precision)>digits.size())
         digits.insert(0,static_cast<std::size_t>(precision)-digits.size(),'0');
      if(base==8 && (flags & PrintAlternate) && (digits.empty() || digits[0]!='0')) digits.insert(0,1,'0');
      const std::string sign=negative ? "-" : (flags & PrintPlus) ? "+" : (flags & PrintSpace) ? " " : "";
      // Qt 5.15 uses a lower-case base prefix even for %X.
      const std::string prefix=base==16 && (flags & PrintAlternate) ? "0x" : "";
      if((flags & PrintZero) && !(flags & PrintLeft) && precision<0 && width>0) {
         const std::size_t used=digits.size()+prefix.size()+(signedConversion ? sign.size() : 0);
         if(static_cast<std::size_t>(width)>used) digits.insert(0,static_cast<std::size_t>(width)-used,'0');
      }
      if(upper) for(char& ch:digits) if(ch>='a' && ch<='f') ch=static_cast<char>(ch-'a'+'A');
      checked_size(sign.size()+prefix.size()+digits.size());
      const std::string result=sign+prefix+digits;
      return fromLatin1(result.c_str(),checked_size(result.size()));
   }
   static QString printDouble(double value, char format, int precision, int width, unsigned flags) {
      if(precision<0) precision=6;
      if((format=='g' || format=='G') && precision==0) precision=1;
      std::string digits=number(value,format,precision).toUtf8();
      std::string sign;
      if(!digits.empty() && digits[0]=='-') { sign="-"; digits.erase(0,1); }
      else if(flags & PrintPlus) sign="+";
      else if(flags & PrintSpace) sign=" ";
      if(std::isfinite(value)) {
         std::size_t end=digits.find_first_of("eE");
         if(end==std::string::npos) end=digits.size();
         if(flags & PrintAlternate) {
            if(digits.find('.')==std::string::npos) { digits.insert(end,1,'.'); ++end; }
            if(format=='g' || format=='G') {
               // Qt 5.15's decimalForm counts leading fractional zeros too.
               // The zero before the point is added afterwards for values < 1.
               int significant=static_cast<int>(end)-1; // exclude the decimal point
               if(value!=0 && digits[0]=='0' && digits[1]=='.') --significant;
               if(precision>significant) digits.insert(end,static_cast<std::size_t>(precision-significant),'0');
            }
         }
         if((flags & PrintGroup) && digits.find_first_of("eE")==std::string::npos) {
            const std::size_t point=digits.find('.');
            groupPrintDigits(digits,point==std::string::npos ? digits.size() : point);
         }
         if((flags & PrintZero) && !(flags & PrintLeft) && width>0) {
            const std::size_t used=sign.size()+digits.size();
            if(static_cast<std::size_t>(width)>used) digits.insert(0,static_cast<std::size_t>(width)-used,'0');
         }
      }
      checked_size(sign.size()+digits.size());
      const std::string result=sign+digits;
      return fromLatin1(result.c_str(),checked_size(result.size()));
   }

public:

   // static

   QString& sprintf(const char* format, ...)
   {
      va_list args; va_start(args,format);
      try { vsprintf(format,args); va_end(args); return *this; }
      catch(...) { va_end(args); throw; }
   }
//--------------------------------------

   static QString asprintf(const char* format, ...) // Qt-style alias
   {
      va_list args; va_start(args,format);
      try { QString result=vasprintf(format,args); va_end(args); return result; }
      catch(...) { va_end(args); throw; }
   }
//--------------------------------------

   QString& vsprintf(const char* format, va_list args)
   {
      *this=vasprintf(format,args);
      return *this;
   }

   static QString vasprintf(const char* format, va_list args)
   {
      // Parse at the Unicode level: CRT printf counts bytes, uses the active
      // locale for wide text, and has different numeric rules on Win32/Win64.
      QString result("");
      if(!format) return result;
      enum Modifier { None, HH, H, L, LL, BigL, J, Z, T };
      const auto decimal=[](const char*& p) -> int {
         unsigned value=0;
         const unsigned limit=static_cast<unsigned>((std::numeric_limits<int>::max)());
         bool overflow=false;
         while(*p>='0' && *p<='9') {
            const unsigned digit=static_cast<unsigned>(*p++-'0');
            if(value>(limit-digit)/10) overflow=true;
            if(!overflow) value=value*10+digit;
         }
         return overflow || value>=limit ? 0 : static_cast<int>(value);
      };
      const char* p=format;
      while(*p) {
         const char* literal=p;
         while(*p && *p!='%') ++p;
         result+=fromUtf8(literal,checked_size(static_cast<std::size_t>(p-literal)));
         if(!*p) break;
         const char* escape=p++;
         if(*p=='%') { result+=QString('%'); ++p; continue; }
         unsigned flags=0;
         while(*p && std::strchr("#0- +'",*p)) {
            switch(*p++) {
               case '#': flags|=PrintAlternate; break;
               case '0': flags|=PrintZero; break;
               case '-': flags|=PrintLeft; break;
               case ' ': flags|=PrintSpace; break;
               case '+': flags|=PrintPlus; break;
               case '\'': flags|=PrintGroup; break;
            }
         }
         int width=-1, precision=-1;
         if(*p>='0' && *p<='9') width=decimal(p);
         else if(*p=='*') { ++p; width=va_arg(args,int); if(width<0) width=-1; }
         if(*p=='.') {
            ++p;
            if(*p>='0' && *p<='9') precision=decimal(p);
            else if(*p=='*') { ++p; precision=va_arg(args,int); if(precision<0) precision=-1; }
         }
         Modifier modifier=None;
         if(*p=='h') { ++p; modifier=H; if(*p=='h') { ++p; modifier=HH; } }
         else if(*p=='l') { ++p; modifier=L; if(*p=='l') { ++p; modifier=LL; } }
         else if(*p=='L') { ++p; modifier=BigL; }
         else if(*p=='j') { ++p; modifier=J; }
         else if(*p=='z' || *p=='Z') { ++p; modifier=Z; }
         else if(*p=='t') { ++p; modifier=T; }
         if(!*p) { result+=fromLatin1(escape); break; }
         const char conversion=*p;
         QString substitution("");
         if(conversion=='s') {
            if(modifier==L) {
               const unsigned short* source=va_arg(args,const unsigned short*);
               std::wstring units;
               if(source) for(std::size_t i=0;;++i) {
                  unsigned short unit;
                  std::memcpy(&unit,reinterpret_cast<const char*>(source)+i*sizeof(unit),sizeof(unit));
                  if(!unit) break;
                  checked_size(units.size()+1);
                  units.push_back(static_cast<wchar_t>(unit));
               }
               substitution=QString(units);
            } else substitution=fromUtf8(va_arg(args,const char*));
            if(precision>=0) substitution.truncate(precision);
         } else if(conversion=='c') {
            const int code=va_arg(args,int);
            substitution=QString(QChar(modifier==L ? static_cast<unsigned short>(code) : static_cast<unsigned char>(code)));
         } else if(conversion=='n') {
            const int count=result.length();
            switch(modifier) {
               case HH: *va_arg(args,signed char*)=static_cast<signed char>(count); break;
               case H: *va_arg(args,short*)=static_cast<short>(count); break;
               case L: *va_arg(args,long*)=count; break;
               case LL: *va_arg(args,long long*)=count; break;
               default: *va_arg(args,int*)=count; break;
            }
         } else if(conversion=='d' || conversion=='i') {
            long long value=0;
            switch(modifier) {
               case None: case HH: case H: case T: value=va_arg(args,int); break;
               case L: case J: value=va_arg(args,long); break;
               case LL: value=va_arg(args,long long); break;
               case Z: value=static_cast<long long>(va_arg(args,std::size_t)); break;
               default: break;
            }
            const unsigned long long bits=static_cast<unsigned long long>(value);
            substitution=printInteger(value<0 ? 0ULL-bits : bits,value<0,true,10,false,precision,width,flags);
         } else if(conversion=='u' || conversion=='o' || conversion=='x' || conversion=='X' || conversion=='p') {
            unsigned long long value=0;
            if(conversion=='p') {
               value=reinterpret_cast<std::size_t>(va_arg(args,void*)); flags|=PrintAlternate;
            } else switch(modifier) {
               case None: case HH: case H: value=va_arg(args,unsigned int); break;
               case L: value=va_arg(args,unsigned long); break;
               case LL: value=va_arg(args,unsigned long long); break;
               case Z: value=va_arg(args,std::size_t); break;
               default: break;
            }
            const int base=conversion=='o' ? 8 : conversion=='u' ? 10 : 16;
            substitution=printInteger(value,false,false,base,conversion=='X',precision,width,flags);
         } else if(std::strchr("eEfFgGaA",conversion)) {
            const double value=modifier==BigL ? static_cast<double>(va_arg(args,long double)) : va_arg(args,double);
            const char numberFormat=conversion=='a' ? 'f' : conversion=='A' ? 'F' : conversion;
            substitution=printDouble(value,numberFormat,precision,width,flags);
         } else {
            // Qt preserves malformed escapes and resumes at the unknown byte.
            result+=fromLatin1(escape,checked_size(static_cast<std::size_t>(p-escape)));
            continue;
         }
         ++p;
         if(width>substitution.length()) substitution=(flags & PrintLeft) ? substitution.leftJustified(width) : substitution.rightJustified(width);
         checked_size(static_cast<std::size_t>(result.length())+substitution.length());
         result+=substitution;
      }
      return result;
   }
//--------------------------------------
	
	static QString fromLocal8Bit(const char* str, int size = -1)
   {
      if(!str) return QString();
      if(size<0) size=checked_size(std::strlen(str));
      if(!size) return QString("");
      // Qt's Windows codec uses CP_ACP, independent of Delphi's mutable
      // DefaultSystemCodePage. Keep its invalid-tail and final-NUL behavior.
      const DWORD flags=MB_PRECOMPOSED|MB_ERR_INVALID_CHARS;
      int bytes=size, count=0; bool pending=false;
      for(;;) {
         count=::MultiByteToWideChar(CP_ACP,flags,str,bytes,nullptr,0);
         if(count) break;
         if(::GetLastError()!=ERROR_NO_UNICODE_TRANSLATION) return QString();
         while(bytes>1 && !str[bytes-1]) --bytes;
         if(bytes<=1 || pending) {
            std::string terminated(str,static_cast<std::size_t>(size));
            terminated.append(2,'\0');
            std::wstring result;
            const char* p=terminated.c_str();
            while(*p) {
               const char* next=::CharNextExA(CP_ACP,p,0);
               wchar_t units[2];
               const int length=::MultiByteToWideChar(CP_ACP,flags,p,static_cast<int>(next-p),units,2);
               if(length>0) result.push_back(units[0]);
               p=next;
            }
            return result.empty() ? QString() : QString(result);
         }
         pending=str[--bytes]!=0;
      }
      std::wstring decoded(static_cast<std::size_t>(count),L'\0');
      if(!::MultiByteToWideChar(CP_ACP,flags,str,bytes,&decoded[0],count)) return QString();
      if(!decoded.empty() && decoded.back()==0) decoded.pop_back();
      return QString(decoded);
   }
//--------------------------------------	

   static QString number(unsigned long long n, int base=10) {
      if(base<2 || base>36) base=10;
      char buffer[65]; char* end=buffer+sizeof(buffer); char* p=end;
      do { *--p="0123456789abcdefghijklmnopqrstuvwxyz"[n%base]; n/=base; } while(n);
      return fromLatin1(p,static_cast<int>(end-p));
   }
   static QString number(long long n, int base=10) {
      if(base<2 || base>36) base=10;
      const unsigned long long bits=static_cast<unsigned long long>(n);
      if(base==10 && n<0) return QString("-")+number(0ULL-bits,base);
      return number(bits,base);
   }
   static QString number(int n, int base=10) { return number(static_cast<long long>(n),base); }
   static QString number(unsigned int n, int base=10) { return number(static_cast<unsigned long long>(n),base); }
   static QString number(long n, int base=10) { return number(static_cast<long long>(n),base); }
   static QString number(unsigned long n, int base=10) { return number(static_cast<unsigned long long>(n),base); }
//------------------------------------------

   static QString number(double n, char format = 'g', int precision = 6)
   {
      const bool shortest=precision==-128; // QLocale::FloatingPointShortest in Qt 5.15
      if(precision<0 && !shortest) precision=6;
      if((format=='g'||format=='G') && precision==0) precision=1;
      const bool upperSpecial=format=='E' || format=='F' || format=='G';
      if(std::isnan(n)) return QString(upperSpecial ? "NAN" : "nan");
      if(std::isinf(n)) return QString(n<0 ? (upperSpecial ? "-INF" : "-inf") : (upperSpecial ? "INF" : "inf"));
      if(shortest) return fromStdString(shortestDouble(n,format));
      if(format=='f' || format=='F') {
         // Builder's printf/iostream switch large fixed values to scientific
         // notation. Convert the exact binary double to decimal instead.
         const bool negative=n!=0 && std::signbit(n);
         int exponent=0;
         const double fraction=std::frexp(std::fabs(n),&exponent);
         std::string digits=std::to_string(static_cast<unsigned long long>(std::ldexp(fraction,53)));
         exponent-=53;
         const int scale=exponent<0 ? -exponent : 0;
         const int multiplier=exponent<0 ? 5 : 2;
         const int iterations=exponent<0 ? -exponent : exponent;
         for(int j=0;j<iterations;++j) {
            int carry=0;
            for(std::size_t i=digits.size();i>0;--i) {
               int value=(digits[i-1]-'0')*multiplier+carry;
               digits[i-1]=static_cast<char>('0'+value%10);carry=value/10;
            }
            if(carry) digits.insert(digits.begin(),static_cast<char>('0'+carry));
         }
         if(digits.size()<=static_cast<std::size_t>(scale)) digits.insert(0,scale+1-digits.size(),'0');
         const std::size_t point=digits.size()-scale;
         const std::size_t keep=point+static_cast<std::size_t>(precision);
         checked_size(keep+2);
         bool round=false;
         if(keep<digits.size()) {
            round=digits[keep]>='5'; // Qt 5.15: exact halves round away from zero.
            digits.resize(keep);
         } else digits.append(keep-digits.size(),'0');
         if(round) {
            std::size_t i=digits.size();
            while(i>0 && digits[i-1]=='9') {digits[--i]='0';}
            if(i>0) ++digits[i-1]; else digits.insert(digits.begin(),'1');
         }
         if(precision>0) digits.insert(digits.size()-precision,1,'.');
         if(negative) digits.insert(digits.begin(),'-');
         return fromStdString(digits);
      }
       // Convert the exact binary value for e/g too: Builder's stream
       // formatter truncates precision differently on Win32 and Win64.
       const bool scientific = format=='e' || format=='E';
       const bool upper = format=='E' || format=='G';
       if(!scientific && precision==0) precision=1;
       if(scientific && precision>(std::numeric_limits<int>::max)()-10)
          throw std::length_error("QString formatted output too long");
       const bool negative=n!=0 && std::signbit(n);
       int exponent=0;
       const double fraction=std::frexp(std::fabs(n),&exponent);
       std::string digits=std::to_string(static_cast<unsigned long long>(std::ldexp(fraction,53)));
       exponent-=53;
       const int scale=exponent<0 ? -exponent : 0;
       const int multiplier=exponent<0 ? 5 : 2;
       const int iterations=exponent<0 ? -exponent : exponent;
       for(int j=0;j<iterations;++j) {
          int carry=0;
          for(std::size_t i=digits.size();i>0;--i) {
             const int value=(digits[i-1]-'0')*multiplier+carry;
             digits[i-1]=static_cast<char>('0'+value%10);carry=value/10;
          }
          if(carry) digits.insert(digits.begin(),static_cast<char>('0'+carry));
       }
       int decimalExponent=n==0 ? 0 : static_cast<int>(digits.size())-scale-1;
       const std::size_t significant=static_cast<std::size_t>(precision)+(scientific ? 1 : 0);
       if(significant<digits.size()) {
          bool round=digits[significant]>='5';
          digits.resize(significant);
          if(round) {
             std::size_t i=digits.size();
             while(i>0 && digits[i-1]=='9') digits[--i]='0';
             if(i>0) ++digits[i-1];
             else { digits.insert(digits.begin(),'1');digits.pop_back();++decimalExponent; }
          }
       }
       if(scientific) digits.append(significant-digits.size(),'0');
       else while(digits.size()>1 && digits.back()=='0') digits.pop_back();
       if(scientific || decimalExponent < -4 || decimalExponent >= precision) {
          if(digits.size()>1) digits.insert(1,1,'.');
          digits.push_back(upper ? 'E' : 'e');
          digits.push_back(decimalExponent<0 ? '-' : '+');
          std::string exp=std::to_string(decimalExponent<0 ? -decimalExponent : decimalExponent);
          if(exp.size()<2) digits.push_back('0');
          digits+=exp;
       } else if(decimalExponent<0) {
          digits.insert(0,static_cast<std::size_t>(-decimalExponent-1),'0');
          digits.insert(0,"0.");
       } else {
          const std::size_t point=static_cast<std::size_t>(decimalExponent)+1;
          if(digits.size()<point) digits.append(point-digits.size(),'0');
          else if(digits.size()>point) digits.insert(point,1,'.');
       }
       if(negative) digits.insert(digits.begin(),'-');
       return fromStdString(digits);
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------	

   static QString fromUtf8(const char* str, int size = -1)
   {
      if(!str) return QString();
      if(size<0) size=checked_size(std::strlen(str));
      const unsigned char* bytes=reinterpret_cast<const unsigned char*>(str);
      std::wstring result;
      result.reserve(static_cast<std::size_t>(size));
      int i=0;
      if(size>=3 && bytes[0]==0xEF && bytes[1]==0xBB && bytes[2]==0xBF) i=3;
      while(i<size) {
         const unsigned char first=bytes[i];
         if(first<0x80) { result.push_back(static_cast<wchar_t>(first)); ++i; continue; }
         const int count=first>=0xC2 && first<=0xDF ? 2 :
                         first>=0xE0 && first<=0xEF ? 3 :
                         first>=0xF0 && first<=0xF4 ? 4 : 0;
         unsigned code=count ? first & (0x7F >> count) : 0;
         bool valid=count!=0 && count<=size-i;
         for(int j=1;valid && j<count;++j) {
            const unsigned char next=bytes[i+j];
            valid=(next & 0xC0)==0x80;
            code=(code<<6) | (next & 0x3F);
         }
         const unsigned minimum=count==2 ? 0x80u : count==3 ? 0x800u : 0x10000u;
         valid=valid && code>=minimum && code<=0x10FFFF && !(code>=0xD800 && code<=0xDFFF);
         if(!valid) {
            // Qt 5.15 consumes one invalid byte per replacement character.
            result.push_back(static_cast<wchar_t>(0xFFFD)); ++i; continue;
         }
         i+=count;
         if(code<=0xFFFF) result.push_back(static_cast<wchar_t>(code));
         else {
            code-=0x10000;
            result.push_back(static_cast<wchar_t>(0xD800+(code>>10)));
            result.push_back(static_cast<wchar_t>(0xDC00+(code & 0x3FF)));
         }
      }
      return QString(result);
   }
//-----------------------------------

   static QString fromLatin1(const char* str, int size = -1)
   {
      if(!str) return QString();
      if(size < 0) size=checked_size(std::strlen(str));
      UnicodeString result; result.SetLength(size);
      for(int i=0;i<size;++i) result[i+1]=static_cast<unsigned char>(str[i]);
      return QString(result);
   }
//-----------------------------------

   static QString fromIso8859_1(const char* str, int size = -1)
   {
      return fromLatin1(str,size);
   }
//-----------------------------------

   // Статические методы для создания из std::string
   static QString fromStdString(const std::string& str)
   {
      return fromUtf8(str.c_str(), checked_size(str.length()));
   }
//-----------------------------------

   static QString fromStdWString(const std::wstring& str)
   {
      return fromStdU16String(std::u16string(str.begin(),str.end()));
   }
//-----------------------------------

   // Для обратной совместимости - конструктор уже есть, но метод для явного вызова
   static QString fromStdU16String(const std::u16string& str)
   {
      UnicodeString result;
      const bool swap=!str.empty() && str[0]==0xfffe;
      const std::size_t start=!str.empty() && (str[0]==0xfeff || swap) ? 1 : 0;
      result.SetLength(checked_size(str.size()-start));
      for(int i=0;i<result.Length();++i) {
         const unsigned ch=str[start+i];
         result[i+1]=static_cast<wchar_t>(swap ? (ch>>8)|((ch&0xff)<<8) : ch);
      }
      return QString(result);
   }
//-----------------------------------

   static QString fromStdU32String(const std::u32string& str)
   {
      std::wstring result;
      const bool swap=!str.empty() && str[0]==0xfffe0000;
      const std::size_t start=!str.empty() && (str[0]==0xfeff || swap) ? 1 : 0;
      for(std::size_t i=start;i<str.size();++i) {
         char32_t ch=str[i];
         if(swap) ch=(ch>>24)|((ch>>8)&0xff00)|((ch&0xff00)<<8)|((ch&0xff)<<24);
         if(ch>0x10FFFF || (ch>=0xD800 && ch<=0xDFFF)) ch=0xFFFD;
         if(ch<=0xFFFF) result.push_back(static_cast<wchar_t>(ch));
         else { ch-=0x10000; result.push_back(static_cast<wchar_t>(0xD800+(ch>>10))); result.push_back(static_cast<wchar_t>(0xDC00+(ch&0x3FF))); }
         checked_size(result.size());
      }
      return QString(result);
   }

//---------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Конструкторы
   QString() = default;
   QString(const QString& other) : m_str(other.m_str), m_null(other.m_null) {}
   QString(QString&& other) noexcept : m_str(std::move(other.m_str)), m_null(other.m_null) { other.m_null=true; }

   // Из разных типов строк
   QString(QChar ch) : m_str(UnicodeString(ch.unicode())), m_null(false) {}
   QString(QChar::SpecialCharacter ch) : QString(QChar(ch)) {}
   // QString(QChar) implicitly promotes char through int in Qt 5.15.
   QString(char ch) : QString(QChar(static_cast<int>(ch))) {}
   QString(wchar_t ch) : QString(QChar(ch)) {}
   QString(char16_t ch) : QString(QChar(ch)) {}
   QString(const char* utf8) : QString(fromUtf8(utf8)) {}
   QString(const wchar_t* utf16) : m_str(utf16), m_null(utf16==nullptr) {}
   // Explicit UnicodeString values are treated as non-null, including empty ones.
   QString(const UnicodeString& str) : m_str(str), m_null(false) {}
   QString(const std::string& utf8) : QString(fromUtf8(utf8.c_str(),checked_size(utf8.size()))) {}
   QString(const std::wstring& utf16) : m_str(utf16.data(), checked_size(utf16.size())), m_null(false) {}

   const wchar_t* constData() const { return m_str.c_str(); }
   const wchar_t* data() const { return m_str.c_str(); }

   // Из числовых типов
   QString(int n) : m_str(IntToStr(n)), m_null(false) {}
   QString(double d) : m_str(FloatToStr(d)), m_null(false) {}
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Операторы присваивания
   QString& operator=(const char* utf8) { return *this=fromUtf8(utf8); }
   QString& operator=(QChar ch) { return *this=QString(ch); }
   // Like Qt's ASCII overload, integral operands are converted to a Latin-1 byte.
   QString& operator=(char ch) { return *this=QChar(ch); }

   QString& operator=(const QString& other)
   {
      if(this != &other) { m_str = other.m_str; m_null=other.m_null; }
      return *this;
   }

   QString& operator=(QString&& other) noexcept
   {
      if(this != &other) { m_str = std::move(other.m_str); m_null=other.m_null; other.m_null=true; }
      return *this;
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Преобразование в другие типы
   operator UnicodeString() const { return m_str; }
//--------------------------------------

   UnicodeString toUnicodeString() const { return m_str; }
//--------------------------------------

   std::string toStdString() const
   {
      // UTF-16 -> UTF-8
      return toUtf8();
   }
//--------------------------------------

   std::wstring toStdWString() const
   {
      return std::wstring(m_str.c_str(), static_cast<std::size_t>(length()));
   }
//--------------------------------------

   const wchar_t* utf16() const { return m_str.c_str(); }
    wchar_t* data() {
       if(m_str.IsEmpty()) { m_null=false; m_empty_terminator=0; return &m_empty_terminator; }
       m_str.Unique(); return m_str.c_str();
    }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Базовые методы как в Qt
   int length() const { return m_str.Length(); }
   bool isEmpty() const { return m_str.IsEmpty(); }
   inline void clear() { m_str = L""; m_null=true; }
   
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Доступ к символам

   // at
   QChar at(int i) const
   {
      if(i < 0 || i >= length()) return QChar();
      return QChar(m_str[i + 1]);                  // UnicodeString индексируется с 1


   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // operator[]
   QChar operator[](int i) const { return at(i); }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   QCharRef operator[](int i)
   {
      if(i<0) throw std::out_of_range("QString::operator[] negative index");
      return QCharRef(m_str,i,&m_null);
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   QString section(QChar sep, int start, int end=-1, int flags=Qt::SectionDefault) const {
      return section(QString(sep),start,end,flags);
   }
   template<class T> typename std::enable_if<IsCharacterCode<T>::value,QString>::type
   section(T sep, int start, int end=-1, int flags=Qt::SectionDefault) const {
      return section(characterArgument(sep),start,end,flags);
   }

   QString section(const QString& sep, int start, int end = -1, int flags = Qt::SectionDefault) const
   {
      std::vector<QString> parts;
      if(!(flags & Qt::SectionCaseInsensitiveSeps) || sep.isEmpty()) parts=split(sep);
      else {
         const std::u32string needleKeys=sep.comparisonSearchKeys();
         std::u32string haystackKeys;
         if(sep.length()==1) {
            // Qt's one-unit separator search folds QChars independently.
            haystackKeys.reserve(static_cast<std::size_t>(length()));
            for(int i=0;i<length();++i) haystackKeys.push_back(comparisonFold(m_str.c_str()[i]));
         } else haystackKeys=comparisonSearchKeys();
         int from=0;
         for(;;) {
            const std::size_t found=haystackKeys.find(needleKeys,static_cast<std::size_t>(from));
            if(found==std::u32string::npos) { parts.push_back(mid(from)); break; }
            const int pos=static_cast<int>(found);
            parts.push_back(mid(from,pos-from));
            from=pos+sep.length();
         }
      }
      const int count=checked_size(parts.size());
      const bool skipEmpty=(flags & Qt::SectionSkipEmpty)!=0;
      int fields=count;
      if(skipEmpty) for(const QString& part:parts) if(part.isEmpty()) --fields;
      if(start<0) start+=fields;
      if(end<0) end+=fields;
      if(start>=count || end<0 || start>end) return QString();
      // Empty fields do not advance the logical index when skipped, but their
      // separators still belong to a selected range (including an open tail).
      int first=start, last=end;
      QString result;
      for(int i=0, field=0;i<count && field<=end;++i) {
         const QString& part=parts[static_cast<std::size_t>(i)];
         if(field>=start) {
            if(field==start) first=i;
            if(field==end) last=i;
            if(field>start && i>0) result+=sep;
            result+=part;
         }
         if(!skipEmpty || !part.isEmpty()) ++field;
      }
      if((flags & Qt::SectionIncludeLeadingSep) && first>0) result.prepend(sep);
      if((flags & Qt::SectionIncludeTrailingSep) && last<count-1) result+=sep;
      return result;
   }
//--------------------


//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Модификация строк
   // Keep QString conversion and braced arguments available. When both
   // QString and QChar require a conversion, the non-template QChar overload
   // wins; literal 0 instead prefers the standard conversion to const char*.
   // The default template parameter also preserves implicit numeric constructors.
   template<class = void>
   QString& append(const QString& other)
   {
      m_str += other.m_str;
      m_null=m_null && other.m_null;
      return *this;
   }
//--------------------

   QString& append(QChar ch) { return append(QString(ch)); }
   QString& append(const char* utf8) { return append(fromUtf8(utf8)); }

   template<class = void>
   QString& prepend(const QString& other)
   {
      if(other.isEmpty()) return *this;
      m_str = other.m_str + m_str;
      m_null=m_null && other.m_null;
      return *this;
   }
//--------------------

   QString& prepend(QChar ch) { return insert(0,ch); }
   QString& prepend(const char* utf8) { return prepend(fromUtf8(utf8)); }

   void resize(int size)
   {
      m_str.SetLength(size);
      m_null=false;
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Поиск
   int indexOf(const QString& str, int from = 0, Qt::CaseSensitivity cs=Qt::CaseSensitive) const
   {
      const int len=length(), n=str.length();
      if(from<0) from+=len;
      // Qt 5.15 checks the end position before dispatching one-unit needles
      // to its character search, which adjusts a negative start once more.
      const long long end=static_cast<long long>(from)+n;
      if(end<0 || end>len) return -1;
      if(n==0) return from;
      if(n==1) return indexOf(str.at(0),from,cs);
      if(from<0 || from>len || n>len-from) return -1;
      const PreparedSearch matcher(str.m_str.c_str(),n,cs);
      return matcher.forward(m_str.c_str(),from,len);
   }
   int indexOf(QChar ch, int from=0, Qt::CaseSensitivity cs=Qt::CaseSensitive) const {
      if(from<0) from=(std::max)(from+length(),0);
      for(int i=from;i<length();++i) if(equalCharacters(QChar(m_str.c_str()[i]),ch,cs)) return i;
      return -1;
   }
   template<class T> typename std::enable_if<IsCharacterCode<T>::value,int>::type
   indexOf(T ch, int from=0, Qt::CaseSensitivity cs=Qt::CaseSensitive) const { return indexOf(characterArgument(ch),from,cs); }
//--------------------

   int lastIndexOf(const QString& str, int from = -1, Qt::CaseSensitivity cs=Qt::CaseSensitive) const
   {
      const int len=length(), n=str.length();
      if(n==1) return lastIndexOf(str.at(0),from,cs);
      if(from<0) from+=len;
      if(from<0 || n>len) return -1;
      if(from==len && n==0) return from;
      if(from>=len) return -1;
      from=(std::min)(from,len-n);
      if(!n) return from;
      const PreparedSearch matcher(str.m_str.c_str(),n,cs,true);
      return matcher.backward(m_str.c_str(),from+n-1);
   }
//--------------------

   int lastIndexOf(QChar ch, int from=-1, Qt::CaseSensitivity cs=Qt::CaseSensitive) const {
      if(from<0) from+=length();
      if(from<0 || from>=length()) return -1;
      for(int i=from;i>=0;--i) if(equalCharacters(QChar(m_str.c_str()[i]),ch,cs)) return i;
      return -1;
   }
   template<class T> typename std::enable_if<IsCharacterCode<T>::value,int>::type
   lastIndexOf(T ch, int from=-1, Qt::CaseSensitivity cs=Qt::CaseSensitive) const { return lastIndexOf(characterArgument(ch),from,cs); }

   bool contains(QChar ch, Qt::CaseSensitivity cs=Qt::CaseSensitive) const { return indexOf(ch,0,cs)>=0; }
   template<class T> typename std::enable_if<IsCharacterCode<T>::value,bool>::type
   contains(T ch, Qt::CaseSensitivity cs=Qt::CaseSensitive) const { return contains(characterArgument(ch),cs); }

   bool contains(const QString& str, Qt::CaseSensitivity cs=Qt::CaseSensitive) const
   {
      return indexOf(str,0,cs)>=0;
   }
//--------------------
   bool startsWith(QChar ch, Qt::CaseSensitivity cs=Qt::CaseSensitive) const {
      return !isEmpty() && equalCharacters(at(0),ch,cs);
   }
   template<class T> typename std::enable_if<IsCharacterCode<T>::value,bool>::type
   startsWith(T ch, Qt::CaseSensitivity cs=Qt::CaseSensitive) const { return startsWith(characterArgument(ch),cs); }

   bool startsWith(const QString& str, Qt::CaseSensitivity cs=Qt::CaseSensitive) const
   {
      if(isNull()) return str.isNull(); // Qt 5.15's historical null-string rule.
      if(str.length()>length()) return false;
      return equalStringUnits(m_str.c_str(),str.m_str.c_str(),str.length(),cs);
   }
//--------------------

   bool endsWith(QChar ch, Qt::CaseSensitivity cs=Qt::CaseSensitive) const {
      return !isEmpty() && equalCharacters(at(length()-1),ch,cs);
   }
   template<class T> typename std::enable_if<IsCharacterCode<T>::value,bool>::type
   endsWith(T ch, Qt::CaseSensitivity cs=Qt::CaseSensitive) const { return endsWith(characterArgument(ch),cs); }

   bool endsWith(const QString& str, Qt::CaseSensitivity cs=Qt::CaseSensitive) const
   {
      if(isNull()) return str.isNull();
      if(str.length()>length()) return false;
      if(str.isEmpty()) return true;
      return equalStringUnits(m_str.c_str()+length()-str.length(),str.m_str.c_str(),str.length(),cs);
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Подстроки
   QString mid(int position, int n = -1) const
   {
      const int size=length();
      if(position>size) return QString();
      if(position<0) {
         // A range covering the whole string preserves its null/empty state.
         if(n<0 || static_cast<long long>(n)+position>=size) return *this;
         if(static_cast<long long>(n)+position<=0) return QString();
         n+=position; position=0;
      } else if(n<0 || n>size-position) n=size-position;
      if(position==0 && n==size) return *this;
      return QString(m_str.SubString(position+1,n));
   }
//--------------------

   QString left(int n) const
   {
      return mid(0, n);
   }
//--------------------

   QString right(int n) const
   {
      if(n<0 || n>=length()) return *this;
      return mid(length()-n,n);
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Разделение
   // Разделение - ОПТИМИЗИРОВАННАЯ ВЕРСИЯ
   std::vector<QString> split(QChar sep, Qt::SplitBehavior behavior=Qt::KeepEmptyParts,
                              Qt::CaseSensitivity cs=Qt::CaseSensitive) const {
      std::vector<QString> result;
      int from=0;
      for(;;) {
         const int pos=indexOf(sep,from,cs);
         const QString part=mid(from,pos<0 ? -1 : pos-from);
         if(behavior==Qt::KeepEmptyParts || !part.isEmpty()) result.push_back(part);
         if(pos<0) break;
         from=pos+1;
      }
      return result;
   }
   template<class T> typename std::enable_if<IsCharacterCode<T>::value,std::vector<QString>>::type
   split(T sep, Qt::SplitBehavior behavior=Qt::KeepEmptyParts, Qt::CaseSensitivity cs=Qt::CaseSensitive) const {
      return split(characterArgument(sep),behavior,cs);
   }

   std::vector<QString> split(const QString& sep, Qt::SplitBehavior behavior = Qt::KeepEmptyParts, Qt::CaseSensitivity cs=Qt::CaseSensitive) const
   {
      std::vector<QString> result;
      if(sep.isEmpty()) {
         if(behavior==Qt::KeepEmptyParts) result.push_back(mid(0,0));
         for(int i=0;i<length();++i) result.push_back(QString(at(i)));
         if(behavior==Qt::KeepEmptyParts) result.push_back(mid(length(),0));
         return result;
      }
      if(sep.length()==1) return split(sep.at(0),behavior,cs);
      const PreparedSearch matcher(sep.m_str.c_str(),sep.length(),cs);
      int from=0;
      for(;;) {
         const int pos=matcher.forward(m_str.c_str(),from,length());
         QString part=mid(from,pos<0 ? -1 : pos-from);
         if(behavior==Qt::KeepEmptyParts || !part.isEmpty()) result.push_back(part);
         if(pos<0) break;
         from=pos+sep.length();
      }
      return result;
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Регистр
   QString toUpper() const
   {
      if(isEmpty()) return *this;
      return convertedCase(true);
   }
//--------------------

   QString toLower() const
   {
      if(isEmpty()) return *this;
      return convertedCase(false);
   }
//--------------------

   QString toCaseFolded() const { return comparisonFolded(); }

   QString trimmed() const
   {
      if(isEmpty()) return *this;
      int first=0,last=length();
      while(first<last && at(first).isSpace()) ++first;
      while(last>first && at(last-1).isSpace()) --last;
      return mid(first,last-first);
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Примеры:
   // QString("  Hello   World  ").simplified();      // "Hello World"
   // QString("\t\n\nHello\n\t\nWorld\n").simplified(); // "Hello World"
   // QString("  \t\n  \t\n  ").simplified();         // ""
   // QString("NoSpacesHere").simplified();           // "NoSpacesHere" (быстрый путь)
   // QString("Single").simplified();                 // "Single"

   QString simplified() const
   {
      if(isEmpty()) return *this;
      UnicodeString result;
      bool space=false;
      for(int i=0;i<length();++i) {
         const QChar ch=at(i);
         if(ch.isSpace()) { space=!result.IsEmpty(); continue; }
         if(space) result+=L' ';
         result+=ch.unicode(); space=false;
      }
      return QString(result);
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Замена
   QString& replace(QChar before, const QString& after, Qt::CaseSensitivity cs=Qt::CaseSensitive)
   {
      // Qt's character overload folds individual UTF-16 units, including surrogates.
      // Snapshot the replacement before detaching: after may refer to *this.
      const QString replacement=after;
      std::wstring result;
      bool changed=false;
      for(int i=0;i<length();++i) {
         const wchar_t unit=m_str.c_str()[i];
         if(equalCharacters(QChar(unit),before,cs)) {
            if(!changed) { result.assign(m_str.c_str(),m_str.c_str()+i); changed=true; }
            checked_size(result.size()+static_cast<std::size_t>(replacement.length()));
            result.append(replacement.constData(),replacement.length());
         } else if(changed) {
            checked_size(result.size()+1);
            result.push_back(unit);
         }
      }
      if(changed) { m_str=UnicodeString(result.data(),checked_size(result.size())); m_null=false; }
      return *this;
   }
   QString& replace(QChar before, QChar after, Qt::CaseSensitivity cs=Qt::CaseSensitive) {
      return replace(before,QString(after),cs);
   }
   template<class T> typename std::enable_if<IsCharacterCode<T>::value,QString&>::type
   replace(T before, const QString& after, Qt::CaseSensitivity cs=Qt::CaseSensitive) {
      return replace(characterArgument(before),after,cs);
   }
   template<class Before,class After> typename std::enable_if<IsCharacterArgument<Before>::value && IsCharacterArgument<After>::value,QString&>::type
   replace(Before before, After after, Qt::CaseSensitivity cs=Qt::CaseSensitive) {
      return replace(characterArgument(before),characterArgument(after),cs);
   }

   QString& replace(const QString& before, const QString& after, Qt::CaseSensitivity cs)
   {
      if(before.isEmpty() && after.isEmpty()) return *this;
      // Arguments may alias *this. Search only the untouched original suffix;
      // inserted text is never searched again, including between Qt's batches.
      const QString needle=before, replacement=after, source=*this;
      const int needleLength=needle.length(), replacementLength=replacement.length();
      const bool insensitive=cs!=Qt::CaseSensitive;
      const std::u32string needleKeys=insensitive ? needle.comparisonSearchKeys() : std::u32string();
      std::u32string haystackKeys=insensitive ? source.comparisonSearchKeys() : std::u32string();
      std::wstring result;
      int from=0, batchCount=0;
      bool changed=false;
      for(;;) {
         int pos;
         if(!insensitive) pos=source.indexOf(needle,from);
         else {
            const std::size_t found=haystackKeys.find(needleKeys,static_cast<std::size_t>(from));
            pos=found==std::u32string::npos ? -1 : static_cast<int>(found);
         }
         if(pos<0) break;
         if(!changed) { result.reserve(static_cast<std::size_t>(source.length())); changed=true; }
         checked_size(result.size()+static_cast<std::size_t>(pos-from));
         if(pos>from) result.append(source.constData()+from,pos-from);
         checked_size(result.size()+static_cast<std::size_t>(replacementLength));
         if(replacementLength) result.append(replacement.constData(),replacementLength);
         from=pos+needleLength;
         if(!needleLength) {
            if(pos==source.length()) break;
            checked_size(result.size()+1);
            result.push_back(source.constData()[pos]);
            from=pos+1;
         }
         if(++batchCount==1024) {
            batchCount=0;
            // Qt resumes searching after applying 1024 replacements. Only the
            // first untouched low surrogate can acquire a different predecessor;
            // all other search keys in the original suffix remain valid.
            if(insensitive && needleLength && from<source.length()) {
               char32_t code=static_cast<char32_t>(source.constData()[from]);
               if(code>=0xDC00 && code<=0xDFFF) {
                  if(!result.empty() && result.back()>=0xD800 && result.back()<=0xDBFF)
                     code=0x10000+((static_cast<char32_t>(result.back())-0xD800)<<10)+(code-0xDC00);
                  haystackKeys[static_cast<std::size_t>(from)]=comparisonFold(code);
               }
            }
         }
      }
      if(changed) {
         checked_size(result.size()+static_cast<std::size_t>(source.length()-from));
         if(from<source.length()) result.append(source.constData()+from,source.length()-from);
         m_str=UnicodeString(result.data(),checked_size(result.size()));
         m_null=false;
      }
      return *this;
   }

   QString& replace(const QString& before, const QString& after)
   {
      return replace(before,after,Qt::CaseSensitive);
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Числовые преобразования
   int toInt(bool* ok = nullptr, int base=10) const
   {
      if(ok) *ok=false;
      if(base && (base<2 || base>36)) base=10;
      const std::string token=numericToken();
      std::size_t pos=0;
      const bool negative=!token.empty() && token[0]=='-';
      if(!token.empty() && (token[0]=='-' || token[0]=='+')) ++pos;
      if(pos==token.size()) return 0;
      if((base==0 || base==16) && token.size()-pos>=2 && token[pos]=='0' &&
         (token[pos+1]=='x' || token[pos+1]=='X')) { base=16; pos+=2; }
      else if(base==0) base=token[pos]=='0' ? 8 : 10;
      if(pos==token.size()) return 0;
      const unsigned limit=static_cast<unsigned>((std::numeric_limits<int>::max)())+(negative ? 1U : 0U);
      unsigned value=0;
      for(;pos<token.size();++pos) {
         const unsigned char ch=static_cast<unsigned char>(token[pos]);
         const unsigned digit=ch>='0' && ch<='9' ? ch-'0' :
            ch>='a' && ch<='z' ? ch-'a'+10 : ch>='A' && ch<='Z' ? ch-'A'+10 : 36;
         if(digit>=static_cast<unsigned>(base) || value>(limit-digit)/static_cast<unsigned>(base)) return 0;
         value=value*static_cast<unsigned>(base)+digit;
      }
      if(ok) *ok=true;
      if(negative && value==limit) return (std::numeric_limits<int>::min)();
      return negative ? -static_cast<int>(value) : static_cast<int>(value);
   }
//------------------------------------------

   double toDouble(bool* ok = nullptr) const
   {
      if(ok) *ok=false;
      const std::string str=numericToken();
      if(str.empty()) return 0;
      std::string lower=str;
      for(char& c:lower) if(c>='A' && c<='Z') c=static_cast<char>(c+'a'-'A');
      if(lower=="inf" || lower=="+inf" || lower=="-inf") {
         if(ok) *ok=true;
         return lower[0]=='-' ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
      }
      if(lower=="nan") { if(ok) *ok=true; return std::numeric_limits<double>::quiet_NaN(); }
      // Validate the whole decimal token before classifying a stream range error.
      // In particular, "1e999junk" must remain an invalid conversion, not infinity.
      std::size_t pos=0;
      const bool negative=str[0]=='-';
      if(str[pos]=='+' || str[pos]=='-') ++pos;
      long long digits=0, beforePoint=0, firstNonzero=-1;
      while(pos<str.size() && str[pos]>='0' && str[pos]<='9') {
         if(firstNonzero<0 && str[pos]!='0') firstNonzero=digits;
         ++digits; ++beforePoint; ++pos;
      }
      if(pos<str.size() && str[pos]=='.') {
         ++pos;
         while(pos<str.size() && str[pos]>='0' && str[pos]<='9') {
            if(firstNonzero<0 && str[pos]!='0') firstNonzero=digits;
            ++digits; ++pos;
         }
      }
      if(digits==0) return 0;
      long long exponent=0;
      if(pos<str.size() && (str[pos]=='e' || str[pos]=='E')) {
         ++pos; bool exponentNegative=false;
         if(pos<str.size() && (str[pos]=='+' || str[pos]=='-')) exponentNegative=str[pos++]=='-';
         const std::size_t exponentStart=pos;
         while(pos<str.size() && str[pos]>='0' && str[pos]<='9') {
            // Beyond any possible mantissa length; avoids integer overflow.
            if(exponent<1000000000000LL) exponent=exponent*10+(str[pos]-'0');
            ++pos;
         }
         if(pos==exponentStart) return 0;
         if(exponentNegative) exponent=-exponent;
      }
      if(pos!=str.size()) return 0;
      if(firstNonzero<0) { if(ok) *ok=true; return negative ? -0.0 : 0.0; }
      const long long decimalPoint=beforePoint-firstNonzero+exponent;
      if(decimalPoint>309) return negative ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
      if(decimalPoint<-323) return 0; // Qt reports nonzero underflow as +0, ok=false.
      std::string significant;
      for(std::size_t i=(str[0]=='+' || str[0]=='-') ? 1 : 0;i<str.size() && str[i]!='e' && str[i]!='E';++i)
         if(str[i]!='.') significant.push_back(str[i]);
      significant.erase(0,static_cast<std::size_t>(firstNonzero));
      while(significant.size()>1 && significant.back()=='0') significant.pop_back();
      const int point=static_cast<int>(decimalPoint);
      // The CRT is only an estimate. Exact decimal interval comparisons below
      // correct its rounding, even on C++Builder versions with inaccurate streams.
      std::istringstream stream(str); stream.imbue(std::locale::classic());
      double value=0; stream>>std::noskipws>>value;
      unsigned long long candidate;
      std::memcpy(&candidate,&value,sizeof(candidate));
      const unsigned long long maximum=0x7fefffffffffffffULL;
      candidate=(std::max)(1ULL,(std::min)(candidate&0x7fffffffffffffffULL,maximum));
      unsigned long long lo=1, hi=maximum;
      for(unsigned attempts=0;lo<=hi;++attempts) {
         const int relation=decimalInterval(significant,point,candidate);
         if(!relation) {
            candidate=qtParsedRounding(significant,point,candidate);
            if(negative) candidate|=1ULL<<63;
            std::memcpy(&value,&candidate,sizeof(value));
            if(ok) *ok=true;
            return value;
         }
         if(relation<0) {
            if(candidate==1) return 0;
            hi=candidate-1;
            candidate=attempts<4 ? hi : lo+(hi-lo)/2;
         } else {
            if(candidate==maximum) return negative ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
            lo=candidate+1;
            candidate=attempts<4 ? lo : lo+(hi-lo)/2;
         }
      }
      return 0;

   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   float toFloat(bool* ok = nullptr) const
   {
      bool tempOk;
      bool* okPtr = ok ? ok : &tempOk;

      *okPtr = false;
      if(m_str.IsEmpty()) return 0.0f;

      double d = toDouble(okPtr);
      if(!*okPtr) return std::isinf(d) ? static_cast<float>(d) : 0.0f;

      // Проверка на переполнение float
      const float maxFloat = (std::numeric_limits<float>::max)();
      const float minFloat = -(std::numeric_limits<float>::max)();

      if(std::isfinite(d) && (d > maxFloat || d < minFloat))
		{
         *okPtr = false;
         return d<0 ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
      }
      if(d!=0 && static_cast<float>(d)==0) { *okPtr=false; return 0.0f; }

      return static_cast<float>(d);
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   QString repeated(int times) const
   {
      if(isEmpty()) return *this;
      if(times <= 0) return QString();
      if(times == 1) return *this;

      int len = length();
      if(times>(std::numeric_limits<int>::max)()/len)
         throw std::length_error("QString repeated overflow");
      int totalLength = len * times;
      QString result;
      result.resize(totalLength);

      // Копируем строку times раз
      for(int i = 0; i < times; i++)
		{
         for(int j = 1; j <= len; j++)
			{
           result.m_str[i * len + j] = m_str[j];
         }
      }
      return result;
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   int count(QChar ch, Qt::CaseSensitivity cs=Qt::CaseSensitive) const {
      int result=0;
      for(int i=0;i<length();++i) if(equalCharacters(QChar(m_str.c_str()[i]),ch,cs)) ++result;
      return result;
   }
   template<class T> typename std::enable_if<IsCharacterCode<T>::value,int>::type
   count(T ch, Qt::CaseSensitivity cs=Qt::CaseSensitive) const { return count(characterArgument(ch),cs); }

   int count(const QString& str, Qt::CaseSensitivity cs=Qt::CaseSensitive) const
   {
      if(str.isEmpty()) {
         if(length()==(std::numeric_limits<int>::max)()) throw std::length_error("QString count overflow");
         return length()+1;
      }
      if(str.length()>length()) return 0;
      if(str.length()==1) return count(str.at(0),cs);
      const PreparedSearch matcher(str.m_str.c_str(),str.length(),cs);
      return matcher.occurrences(m_str.c_str(),length());
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   QString& remove(int position, int n)
   {
      if(position<0) position+=length();
      if(position >= 0 && position < length() && n > 0) { m_str.Delete(position + 1, n); m_null=false; }
      return *this;
   }
//------------------------------------------

   QString& remove(QChar ch, Qt::CaseSensitivity cs=Qt::CaseSensitive) { return replace(ch,QString(""),cs); }
   template<class T> typename std::enable_if<IsCharacterCode<T>::value,QString&>::type
   remove(T ch, Qt::CaseSensitivity cs=Qt::CaseSensitive) { return remove(characterArgument(ch),cs); }

   QString& remove(const QString& str)
   {
      if(!str.isEmpty()) replace(str, "");
      return *this;
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   template<class = void>
   QString& insert(int position, const QString& str)
   {
      if(position<0 || str.isEmpty()) return *this;
      checked_size(static_cast<std::size_t>((std::max)(position,length()))+str.length());
      // Snapshot first: str may be *this, including when padding is needed.
      const UnicodeString inserted=str.m_str;
      if(position>length()) {
         const int oldLength=length();
         m_str.SetLength(position);
         for(int i=oldLength;i<position;++i) m_str[i+1]=L' ';
      }
      m_str.Insert(inserted, position + 1);
      m_null=false;
      return *this;
   }
   QString& insert(int position, QChar ch) {
      if(position<0) position+=length();
      return insert(position,QString(ch));
   }
   QString& insert(int position, const char* utf8) { return insert(position,fromUtf8(utf8)); }
//---------------------------------------------------------------------------------------------------------------------------------------------------

private:
   static QString paddedArg(const QString& a, int fieldWidth, QChar fillChar) {
      if(fieldWidth==(std::numeric_limits<int>::min)()) throw std::length_error("QString field width overflow");
      const int width=fieldWidth<0 ? -fieldWidth : fieldWidth;
      if(width<=a.length()) return a;
      const QString pad=QString(fillChar).repeated(width-a.length());
      return fieldWidth<0 ? a+pad : pad+a;
   }
   QString replaceArg(const QString& replacement, const QString& localized) const {
      int lowest=100;
      for(int i=0;i<length();++i) if(at(i).unicode()==L'%') {
         int j=i+1; if(j<length() && at(j).unicode()==L'L') ++j;
         int n=0,digits=0;
         while(j<length() && digits<2 && at(j).digitValue()>=0) {n=n*10+at(j++).digitValue();++digits;}
         if(digits) lowest=(std::min)(lowest,n);
      }
      if(lowest==100) return *this;
      QString result("");
      for(int i=0;i<length();) {
         if(at(i).unicode()==L'%') {
            int j=i+1; const bool local=j<length() && at(j).unicode()==L'L'; if(local) ++j;
            int n=0,digits=0;
            while(j<length() && digits<2 && at(j).digitValue()>=0) {n=n*10+at(j++).digitValue();++digits;}
            if(digits && n==lowest) {result+=local ? localized : replacement;i=j;continue;}
         }
         result+=QString(at(i++));
      }
      return result;
   }
   static QString numericArgText(QString text, int fieldWidth, QChar fillChar, bool localized, bool group) {
      if(localized) {
         // This VCL adapter uses the current Windows/VCL numeric separators.
         // Qt's default locale on Windows obtains the same user settings.
         int first=text.length() && text.at(0).unicode()==L'-' ? 1 : 0;
         int end=first;
         while(end<text.length() && text.at(end).unicode()>=L'0' && text.at(end).unicode()<=L'9') ++end;
         for(int i=0;i<text.length();++i)
            if(text.at(i).unicode()==L'.') text[i]=QChar(System::Sysutils::FormatSettings.DecimalSeparator);
         if(group && end>first) {
            for(int i=end-3;i>first;i-=3) text.insert(i,QChar(System::Sysutils::FormatSettings.ThousandSeparator));
         }
      }
      // Qt's numeric zero padding precedes the magnitude, after its minus sign.
      // Negative field widths are padded by replaceArgEscapes on the right.
      if(fillChar.unicode()==L'0' && fieldWidth>text.length() &&
         text.length() && text.at(text.length()-1).unicode()>=L'0' && text.at(text.length()-1).unicode()<=L'9') {
         const int sign=text.at(0).unicode()==L'-' ? 1 : 0;
         text.insert(sign,QString(fillChar).repeated(fieldWidth-text.length()));
      }
      return paddedArg(text,fieldWidth,fillChar);
   }
public:
   QString arg(const QString& a, int fieldWidth=0, QChar fillChar=QChar(' ')) const {
      const QString value=paddedArg(a,fieldWidth,fillChar);
      return replaceArg(value,value);
   }
   QString arg(QChar a, int fieldWidth=0, QChar fillChar=QChar(' ')) const {
      return arg(QString(a),fieldWidth,fillChar);
   }
   QString arg(char a, int fieldWidth=0, QChar fillChar=QChar(' ')) const {
      return arg(QChar(a),fieldWidth,fillChar);
   }
   QString arg(long long a, int fieldWidth=0, int base=10, QChar fillChar=QChar(' ')) const {
      const QString value=number(a,base);
      return replaceArg(numericArgText(value,fieldWidth,fillChar,false,false),
                        numericArgText(value,fieldWidth,fillChar,true,base==10));
   }
   QString arg(unsigned long long a, int fieldWidth=0, int base=10, QChar fillChar=QChar(' ')) const {
      const QString value=number(a,base);
      return replaceArg(numericArgText(value,fieldWidth,fillChar,false,false),
                        numericArgText(value,fieldWidth,fillChar,true,base==10));
   }
   QString arg(int a, int width=0, int base=10, QChar fill=QChar(' ')) const { return arg(static_cast<long long>(a),width,base,fill); }
   QString arg(unsigned a, int width=0, int base=10, QChar fill=QChar(' ')) const { return arg(static_cast<unsigned long long>(a),width,base,fill); }
   QString arg(long a, int width=0, int base=10, QChar fill=QChar(' ')) const { return arg(static_cast<long long>(a),width,base,fill); }
   QString arg(unsigned long a, int width=0, int base=10, QChar fill=QChar(' ')) const { return arg(static_cast<unsigned long long>(a),width,base,fill); }
   QString arg(short a, int width=0, int base=10, QChar fill=QChar(' ')) const { return arg(static_cast<long long>(a),width,base,fill); }
   QString arg(unsigned short a, int width=0, int base=10, QChar fill=QChar(' ')) const { return arg(static_cast<unsigned long long>(a),width,base,fill); }
   QString arg(double a, int fieldWidth=0, char format='g', int precision=-1, QChar fillChar=QChar(' ')) const {
      if(format!='f' && format!='F' && format!='e' && format!='E' && format!='g' && format!='G')
         format=format>='A' && format<='Z' ? 'F' : 'f';
      const QString value=number(a,format,precision);
      return replaceArg(numericArgText(value,fieldWidth,fillChar,false,false),
                        numericArgText(value,fieldWidth,fillChar,true,true));
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Операторы
   QString operator+(QChar ch) const { QString result=*this; return result.append(ch); }
   QString operator+(char ch) const { return *this+QChar(ch); }
   QString operator+(const char* utf8) const { QString result=*this; return result.append(fromUtf8(utf8)); }
   friend QString operator+(QChar ch, const QString& str) { QString result(ch); return result.append(str); }
   friend QString operator+(char ch, const QString& str) { return QChar(ch)+str; }
   friend QString operator+(const char* utf8, const QString& str) { return QString::fromUtf8(utf8)+str; }

   QString operator+(const QString& other) const
   {
      QString result=*this; result.append(other); return result;
   }

   QString& operator+=(QChar ch) { return append(ch); }
   QString& operator+=(QChar::SpecialCharacter ch) { return append(QChar(ch)); }
   // Qt 5.15's ASCII overload wins for integral operands, including wide codes.
   QString& operator+=(char ch) { return append(QChar(ch)); }
   QString& operator+=(const char* utf8) { return append(fromUtf8(utf8)); }

   QString& operator+=(const QString& other)
   {
      return append(other);
   }

   bool operator==(const char* utf8) const { return *this==QString::fromUtf8(utf8); }
   friend bool operator==(const char* utf8, const QString& str) { return str==QString::fromUtf8(utf8); }
   bool operator!=(const char* utf8) const { return *this!=QString::fromUtf8(utf8); }
   friend bool operator!=(const char* utf8, const QString& str) { return str!=QString::fromUtf8(utf8); }
   bool operator<(const char* utf8) const { return *this<QString::fromUtf8(utf8); }
   friend bool operator<(const char* utf8, const QString& str) { return str>QString::fromUtf8(utf8); }
   bool operator>(const char* utf8) const { return *this>QString::fromUtf8(utf8); }
   friend bool operator>(const char* utf8, const QString& str) { return str<QString::fromUtf8(utf8); }
   bool operator<=(const char* utf8) const { return *this<=QString::fromUtf8(utf8); }
   friend bool operator<=(const char* utf8, const QString& str) { return str>=QString::fromUtf8(utf8); }
   bool operator>=(const char* utf8) const { return *this>=QString::fromUtf8(utf8); }
   friend bool operator>=(const char* utf8, const QString& str) { return str<=QString::fromUtf8(utf8); }

   bool operator==(QChar ch) const { return compareCharacter(ch)==0; }
   friend bool operator==(QChar ch, const QString& str) { return str==ch; }

   bool operator!=(QChar ch) const { return compareCharacter(ch)!=0; }
   friend bool operator!=(QChar ch, const QString& str) { return str!=ch; }

   bool operator<(QChar ch) const { return compareCharacter(ch)<0; }
   friend bool operator<(QChar ch, const QString& str) { return str>ch; }

   bool operator>(QChar ch) const { return compareCharacter(ch)>0; }
   friend bool operator>(QChar ch, const QString& str) { return str<ch; }

   bool operator<=(QChar ch) const { return compareCharacter(ch)<=0; }
   friend bool operator<=(QChar ch, const QString& str) { return str>=ch; }

   bool operator>=(QChar ch) const { return compareCharacter(ch)>=0; }
   friend bool operator>=(QChar ch, const QString& str) { return str<=ch; }

   template<class = void>
   bool operator==(const QString& other) const
   {
      return m_str == other.m_str;
   }

   template<class = void>
   bool operator!=(const QString& other) const
   {
      return m_str != other.m_str;
   }

   template<class = void>
   bool operator<(const QString& other) const
   {
      return m_str < other.m_str;
   }

   template<class = void>
   bool operator<=(const QString& other) const { return m_str <= other.m_str; }
   template<class = void>
   bool operator>=(const QString& other) const { return m_str >= other.m_str; }
   template<class = void>
   bool operator>(const QString& other) const { return m_str > other.m_str; }
//---------------------------------------------------------------------------------------------------------------------------------------------------

private:
   template<class T> struct IsStringOperandProxy : std::integral_constant<bool,
      !std::is_same<typename std::decay<T>::type,QString>::value &&
      !std::is_base_of<UnicodeString,typename std::decay<T>::type>::value &&
      !IsCharacterArgument<typename std::decay<T>::type>::value &&
      std::is_convertible<T,const QString&>::value &&
      !std::is_convertible<T,QChar>::value &&
      !std::is_convertible<T,const char*>::value> {};
   class StringOperand;
public:
   // Convert the left proxy only after overload resolution. Fixed right operands
   // preserve custom operators and reject unsupported expressions during SFINAE.
   friend QString operator+(const StringOperand& left, const QString& right);
   friend QString operator+(const StringOperand& left, QChar right);
   friend QString operator+(const StringOperand& left, char right);
   friend QString operator+(const StringOperand& left, const char* right);
   template<class> friend bool operator==(const StringOperand& left, const QString& right);
   friend bool operator==(const StringOperand& left, QChar right);
   friend bool operator==(const StringOperand& left, const char* right);
   template<class> friend bool operator!=(const StringOperand& left, const QString& right);
   friend bool operator!=(const StringOperand& left, QChar right);
   friend bool operator!=(const StringOperand& left, const char* right);
   template<class> friend bool operator<(const StringOperand& left, const QString& right);
   friend bool operator<(const StringOperand& left, QChar right);
   friend bool operator<(const StringOperand& left, const char* right);
   template<class> friend bool operator<=(const StringOperand& left, const QString& right);
   friend bool operator<=(const StringOperand& left, QChar right);
   friend bool operator<=(const StringOperand& left, const char* right);
   template<class> friend bool operator>(const StringOperand& left, const QString& right);
   friend bool operator>(const StringOperand& left, QChar right);
   friend bool operator>(const StringOperand& left, const char* right);
   template<class> friend bool operator>=(const StringOperand& left, const QString& right);
   friend bool operator>=(const StringOperand& left, QChar right);
   friend bool operator>=(const StringOperand& left, const char* right);

   std::string toUtf8() const
   {
      std::string result;
      result.reserve(static_cast<std::size_t>(length()));
      for(int i=0;i<length();++i) {
         unsigned code=static_cast<unsigned>(m_str.c_str()[i]);
         if(code>=0xD800 && code<=0xDBFF && i+1<length()) {
            const unsigned next=static_cast<unsigned>(m_str.c_str()[i+1]);
            if(next>=0xDC00 && next<=0xDFFF) { code=0x10000+((code-0xD800)<<10)+(next-0xDC00); ++i; }
         }
         if(code>=0xD800 && code<=0xDFFF) code='?';
         checked_size(result.size()+(code<0x80 ? 1u : code<0x800 ? 2u : code<0x10000 ? 3u : 4u));
         if(code<0x80) result.push_back(static_cast<char>(code));
         else if(code<0x800) {
            result.push_back(static_cast<char>(0xC0 | (code>>6)));
            result.push_back(static_cast<char>(0x80 | (code & 0x3F)));
         } else if(code<0x10000) {
            result.push_back(static_cast<char>(0xE0 | (code>>12)));
            result.push_back(static_cast<char>(0x80 | ((code>>6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (code & 0x3F)));
         } else {
            result.push_back(static_cast<char>(0xF0 | (code>>18)));
            result.push_back(static_cast<char>(0x80 | ((code>>12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((code>>6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (code & 0x3F)));
         }
      }
      return result;
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   std::u16string toStdU16String() const
   {
      const wchar_t* data = m_str.c_str();
      int len = length();
      std::u16string result;
      result.reserve(len);

      for(int i = 0; i < len; i++)
      {
         #ifdef _WIN32
         result.push_back(static_cast<char16_t>(data[i]));
         #else
         // Проверка на суррогатные пары для не-Windows платформ
         if(sizeof(wchar_t) == 4) result.push_back(static_cast<char16_t>(data[i] <= 0xFFFF ? data[i] : 0xFFFD));
         else result.push_back(static_cast<char16_t>(data[i]));
         #endif
      }
     return result;
   }
//----------------------------------------------

   std::u32string toStdU32String() const
   {
      const wchar_t* data = m_str.c_str();
      int len = length();
      std::u32string result;
      result.reserve(len);

     for(int i = 0; i < len; i++)
     {
        wchar_t ch = data[i];

        // Проверка на high surrogate (первый суррогат UTF-16)
        if(ch >= 0xD800 && ch <= 0xDBFF && i + 1 < len)
        {
            wchar_t next = data[i + 1];
            // Проверка на low surrogate
            if(next >= 0xDC00 && next <= 0xDFFF)
            {
               // Объединение суррогатной пары
               char32_t codePoint = 0x10000;
               codePoint += (static_cast<char32_t>(ch) - 0xD800) << 10;
               codePoint += static_cast<char32_t>(next) - 0xDC00;
               result.push_back(codePoint);
               i++; // Пропускаем low surrogate
               continue;
            }
         }

         // Обычный символ или непарный surrogate
         result.push_back(ch>=0xD800 && ch<=0xDFFF ? char32_t(0xFFFD) : static_cast<char32_t>(ch));
      }
      return result;
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Удалить n символов с конца
   void chop(int n)
	{
      if(n>0) resize(n>length() ? 0 : length()-n);
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Обрезать до позиции pos
   void truncate(int pos)
	{
      if(pos<length()) resize((std::max)(pos,0));
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   bool isNull() const
   {
      return m_null && m_str.IsEmpty();
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   QString& fill(QChar ch, int size = -1)
   {
      if(size < 0) size = length();
      m_str = UnicodeString::StringOfChar(ch.unicode(), size);
      m_null=false;
      return *this;
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   QString toHtmlEscaped() const
   {
      QString result = isEmpty() ? QString("") : *this;
      result.replace("&", "&amp;");
      result.replace("<", "&lt;");
      result.replace(">", "&gt;");
      result.replace("\"", "&quot;");
      return result;
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   QString leftJustified(int width, QChar fill = QChar(' '), bool truncate = false) const
   {
      if(length() >= width) return truncate ? left(width) : *this;
      return *this + QString(fill).repeated(width - length());
   }
//----------------------------------------------

   QString rightJustified(int width, QChar fill = QChar(' '), bool truncate = false) const
   {
      if(length() >= width) return truncate ? left(width) : *this;
      return QString(fill).repeated(width - length()) + *this;
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   // Метод capacity (приблизительный)
   int capacity() const
	{
      // UnicodeString не предоставляет capacity
      // Возвращаем текущую длину как приближение
      return m_str.Length();
   }
//---------------------------------------------------------------------------------------------------------------------------------------------------

   std::string toLocal8Bit() const
   {
      if(isEmpty()) return std::string();
      BOOL usedDefault=FALSE;
      const int count=::WideCharToMultiByte(CP_ACP,0,m_str.c_str(),length(),nullptr,0,nullptr,&usedDefault);
      if(!count) return std::string();
      std::string result(static_cast<std::size_t>(count),'\0');
      if(!::WideCharToMultiByte(CP_ACP,0,m_str.c_str(),length(),&result[0],count,nullptr,&usedDefault)) return std::string();
      return result;
   }
};
//---------------------------------------------------------------------------------------------------------------------------------------------------
//===================================================================================================================================================





// Implicit operator fallbacks intentionally require a conversion on the left.
// Keep UnicodeString on its native VCL operators, including its result type.
class QString::StringOperand {
   enum RightKind { StringRight, CharacterRight, Latin1Right, Utf8Right };
   const volatile void* source;
   QString (*appendSource)(const volatile void*,const void*,RightKind);
   bool (*compareSource)(const volatile void*,const void*,RightKind,unsigned);

   template<class T> static T&& restore(const volatile void* pointer) {
      typedef typename std::remove_reference<T>::type Source;
      return std::forward<T>(*static_cast<Source*>(const_cast<void*>(pointer)));
   }
   template<class Right> static bool compareValues(const QString& left,const Right& right,unsigned operation) {
      switch(operation) {
      case 0: return left.operator==(right);
      case 1: return left.operator!=(right);
      case 2: return left.operator<(right);
      case 3: return left.operator<=(right);
      case 4: return left.operator>(right);
      default: return left.operator>=(right);
      }
   }
   template<class T> static QString appendValue(const volatile void* source,const void* right,RightKind kind) {
      const QString& left=restore<T>(source);
      switch(kind) {
      case StringRight: return left.operator+(*static_cast<const QString*>(right));
      case CharacterRight: return left.operator+(*static_cast<const QChar*>(right));
      case Latin1Right: return left.operator+(*static_cast<const char*>(right));
      default: return left.operator+(*static_cast<const char* const*>(right));
      }
   }
   template<class T> static bool compareValue(const volatile void* source,const void* right,RightKind kind,unsigned operation) {
      const QString& left=restore<T>(source);
      switch(kind) {
      case StringRight: return compareValues(left,*static_cast<const QString*>(right),operation);
      case CharacterRight: return compareValues(left,*static_cast<const QChar*>(right),operation);
      default: return compareValues(left,*static_cast<const char* const*>(right),operation);
      }
   }
public:
   // The source operand lives until the full expression ends. Convert it inside
   // the operation, keeping lvalue/rvalue references and temporary values intact.
   // Function pointers erase only its type; no allocation or string copy is needed.
   // Reject incomplete class operands before instantiating traits. Keep this
   // in the constructor's immediate context so later definitions are reconsidered.
   template<class T, std::size_t = sizeof(typename std::decay<T>::type),
      typename std::enable_if<IsStringOperandProxy<T&&>::value,int>::type = 0>
   StringOperand(T&& value)
      : source(std::addressof(value)),appendSource(&appendValue<T>),compareSource(&compareValue<T>) {}
   QString add(const QString& right) const { return appendSource(source,&right,StringRight); }
   QString add(QChar right) const { return appendSource(source,&right,CharacterRight); }
   QString add(char right) const { return appendSource(source,&right,Latin1Right); }
   QString add(const char* right) const { return appendSource(source,&right,Utf8Right); }
   bool compare(const QString& right,unsigned op) const { return compareSource(source,&right,StringRight,op); }
   bool compare(QChar right,unsigned op) const { return compareSource(source,&right,CharacterRight,op); }
   bool compare(const char* right,unsigned op) const { return compareSource(source,&right,Utf8Right,op); }
};

inline QString operator+(const QString::StringOperand& left, const QString& right)
{ return left.add(right); }
inline QString operator+(const QString::StringOperand& left, QChar right)
{ return left.add(right); }
inline QString operator+(const QString::StringOperand& left, char right)
{ return left.add(right); }
inline QString operator+(const QString::StringOperand& left, const char* right)
{ return left.add(right); }
template<class = void> inline bool operator==(const QString::StringOperand& left, const QString& right)
{ return left.compare(right,0); }
inline bool operator==(const QString::StringOperand& left, QChar right)
{ return left.compare(right,0); }
inline bool operator==(const QString::StringOperand& left, const char* right)
{ return left.compare(right,0); }
template<class = void> inline bool operator!=(const QString::StringOperand& left, const QString& right)
{ return left.compare(right,1); }
inline bool operator!=(const QString::StringOperand& left, QChar right)
{ return left.compare(right,1); }
inline bool operator!=(const QString::StringOperand& left, const char* right)
{ return left.compare(right,1); }
template<class = void> inline bool operator<(const QString::StringOperand& left, const QString& right)
{ return left.compare(right,2); }
inline bool operator<(const QString::StringOperand& left, QChar right)
{ return left.compare(right,2); }
inline bool operator<(const QString::StringOperand& left, const char* right)
{ return left.compare(right,2); }
template<class = void> inline bool operator<=(const QString::StringOperand& left, const QString& right)
{ return left.compare(right,3); }
inline bool operator<=(const QString::StringOperand& left, QChar right)
{ return left.compare(right,3); }
inline bool operator<=(const QString::StringOperand& left, const char* right)
{ return left.compare(right,3); }
template<class = void> inline bool operator>(const QString::StringOperand& left, const QString& right)
{ return left.compare(right,4); }
inline bool operator>(const QString::StringOperand& left, QChar right)
{ return left.compare(right,4); }
inline bool operator>(const QString::StringOperand& left, const char* right)
{ return left.compare(right,4); }
template<class = void> inline bool operator>=(const QString::StringOperand& left, const QString& right)
{ return left.compare(right,5); }
inline bool operator>=(const QString::StringOperand& left, QChar right)
{ return left.compare(right,5); }
inline bool operator>=(const QString::StringOperand& left, const char* right)
{ return left.compare(right,5); }

class QStringList
{
private:
    std::vector<QString> m_data;

public:
    // ============ ТИПЫ (STL-совместимость) ============
   using value_type = QString;
   using size_type = std::size_t;
   using difference_type = std::ptrdiff_t;
   using reference = QString&;
   using const_reference = const QString&;
	using pointer = QString*;
   using const_pointer = const QString*;
   using iterator = std::vector<QString>::iterator;
   using const_iterator = std::vector<QString>::const_iterator;
   using reverse_iterator = std::vector<QString>::reverse_iterator;
   using const_reverse_iterator = std::vector<QString>::const_reverse_iterator;

   // ============ КОНСТРУКТОРЫ ============
   QStringList() = default;
   QStringList(const QStringList&) = default;
   QStringList(QStringList&&) noexcept = default;
   explicit QStringList(const std::vector<QString>& vec) : m_data(vec) {}
   explicit QStringList(std::vector<QString>&& vec) noexcept : m_data(std::move(vec)) {}

   QStringList(std::initializer_list<QString> init) : m_data(init) {}

   template<typename InputIt>
   QStringList(InputIt first, InputIt last) : m_data(first, last) {}

   // Из одного QString
   explicit QStringList(const QString& str) { m_data.push_back(str); }
   explicit QStringList(QString&& str) { m_data.push_back(std::move(str)); }

   // ============ ОПЕРАТОРЫ ПРИСВАИВАНИЯ ============
   QStringList& operator=(const QStringList&) = default;
   QStringList& operator=(QStringList&&) noexcept = default;

   QStringList& operator=(std::initializer_list<QString> init)
	{
      m_data = init;
      return *this;
   }

   // ============ STL-ИНТЕРФЕЙС ============
   iterator begin() noexcept { return m_data.begin(); }
   iterator end() noexcept { return m_data.end(); }
   const_iterator begin() const noexcept { return m_data.begin(); }
   const_iterator end() const noexcept { return m_data.end(); }
   const_iterator cbegin() const noexcept { return m_data.cbegin(); }
   const_iterator cend() const noexcept { return m_data.cend(); }

   reverse_iterator rbegin() noexcept { return m_data.rbegin(); }
   reverse_iterator rend() noexcept { return m_data.rend(); }
   const_reverse_iterator rbegin() const noexcept { return m_data.rbegin(); }
   const_reverse_iterator rend() const noexcept { return m_data.rend(); }
   const_reverse_iterator crbegin() const noexcept { return m_data.crbegin(); }
   const_reverse_iterator crend() const noexcept { return m_data.crend(); }

   size_type size() const noexcept { return m_data.size(); }
   size_type max_size() const noexcept { return m_data.max_size(); }
   bool empty() const noexcept { return m_data.empty(); }
   void reserve(size_type n) { m_data.reserve(n); }
   size_type capacity() const noexcept { return m_data.capacity(); }
   void shrink_to_fit() { m_data.shrink_to_fit(); }

   void clear() noexcept { m_data.clear(); }

   // ============ ДОСТУП К ЭЛЕМЕНТАМ ============
   reference operator[](size_type i) { return m_data[i]; }
   const_reference operator[](size_type i) const { return m_data[i]; }

   reference at(size_type i) { return m_data.at(i); }
   const_reference at(size_type i) const { return m_data.at(i); }

   reference front() { return m_data.front(); }
   const_reference front() const { return m_data.front(); }
   reference back() { return m_data.back(); }
   const_reference back() const { return m_data.back(); }

   pointer data() noexcept { return m_data.data(); }
   const_pointer data() const noexcept { return m_data.data(); }

   // ============ МОДИФИКАЦИЯ ============
   void push_back(const QString& value) { m_data.push_back(value); }
   void push_back(QString&& value) { m_data.push_back(std::move(value)); }

   void pop_back() { m_data.pop_back(); }

   template<typename... Args>
   reference emplace_back(Args&&... args)
	{
      m_data.emplace_back(std::forward<Args>(args)...);
      return m_data.back();
   }

   iterator insert(const_iterator pos, const QString& value)
	{
      return m_data.insert(pos, value);
   }

   iterator insert(const_iterator pos, QString&& value)
	{
      return m_data.insert(pos, std::move(value));
   }

   iterator insert(const_iterator pos, size_type count, const QString& value)
	{
      return m_data.insert(pos, count, value);
   }

   template<typename InputIt>
   iterator insert(const_iterator pos, InputIt first, InputIt last)
	{
      return m_data.insert(pos, first, last);
   }

   iterator insert(const_iterator pos, std::initializer_list<QString> init)
	{
      return m_data.insert(pos, init);
   }

   iterator erase(const_iterator pos)
	{
      return m_data.erase(pos);
   }

   iterator erase(const_iterator first, const_iterator last)
	{
      return m_data.erase(first, last);
   }

   void resize(size_type count)
	{
      m_data.resize(count);
   }

   void resize(size_type count, const QString& value)
	{
      m_data.resize(count, value);
   }

   void swap(QStringList& other) noexcept
	{
      m_data.swap(other.m_data);
   }

   // ============ Qt-МЕТОДЫ ============

   // Основной метод QStringList
   QString join(QChar separator) const { return join(QString(separator)); }
   template<class T> typename std::enable_if<QString::IsCharacterCode<T>::value,QString>::type
   join(T separator) const { return join(QString::characterArgument(separator)); }

   QString join(const QString& separator) const
   {
      if(empty()) return QString();
      if(size() == 1) return front().isEmpty() ? QString() : front();

      QString result;
      // Не используем reserve, так как его нет в UnicodeString
      // Просто собираем строку

      for(size_type i = 0; i < size(); i++)
		{
         if(i > 0) result.append(separator);        
         result.append((*this)[i]);
      }

      // Qt returns a null string whenever the joined length is zero.
      return result.isEmpty() ? QString() : result;
   }

   // Оператор добавления (Qt-стиль)
   QStringList& operator<<(const QString& str) 
	{
      push_back(str);
      return *this;
   }

   QStringList& operator<<(QString&& str)
	{
      push_back(std::move(str));
      return *this;
   }

   QStringList& operator<<(const QStringList& other)
	{
      if(this==&other) { const std::vector<QString> copy=m_data; m_data.insert(m_data.end(),copy.begin(),copy.end()); }
      else m_data.insert(m_data.end(), other.begin(), other.end());
      return *this;
   }

   // Проверка наличия
   bool contains(const QString& str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
	{
      if(cs == Qt::CaseSensitive) return std::find(begin(), end(), str) != end();
      else
		{
         QString foldedStr = str.comparisonFolded();
         return std::find_if(begin(), end(), [&](const QString& item) { return item.comparisonFolded() == foldedStr; }) != end();
      }
   }

   // Фильтрация
   QStringList filter(const QString& str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
	{
      QStringList result;
      if(cs == Qt::CaseSensitive)
		{
         std::copy_if(begin(), end(), std::back_inserter(result.m_data), [&](const QString& item) { return item.contains(str); });
      }
		else
		{
         const std::u32string needleKeys=str.comparisonSearchKeys();
         std::copy_if(begin(), end(), std::back_inserter(result.m_data), [&](const QString& item) {
            return item.comparisonSearchKeys().find(needleKeys)!=std::u32string::npos;
         });
      }
      return result;
   }

   // Замена в строках
   QStringList& replaceInStrings(const QString& before, const QString& after, Qt::CaseSensitivity cs = Qt::CaseSensitive)
	{
      const QString beforeCopy=before, afterCopy=after;
      for(auto& str : m_data)
		{
         str.replace(beforeCopy, afterCopy, cs);
      }
      return *this;
   }

   // Сортировка
   QStringList& sort(Qt::CaseSensitivity cs = Qt::CaseSensitive)
	{
      if(cs == Qt::CaseSensitive) std::sort(m_data.begin(), m_data.end());
      else
		{
         // Qt compares folded keys per UTF-16 unit, including surrogate context.
         std::sort(m_data.begin(), m_data.end(), [](const QString& a, const QString& b) { return a.comparisonSearchKeys() < b.comparisonSearchKeys(); });
      }
      return *this;
   }

   // Удобные геттеры (Qt-стиль)
   QString value(size_type i) const
	{
      return (i < size()) ? m_data[i] : QString();
   }

   reference first() { return at(0); }
   const_reference first() const { return at(0); }
   reference last() { return at(size()-1); }
   const_reference last() const { return at(size()-1); }

   // Удаление дубликатов
   int removeDuplicates()
	{
      // A tree bounds comparisons by O(n log n), even for colliding input hashes.
      // Equality/order include embedded NUL; null and empty strings compare equal.
      std::set<QString> seen;
      std::vector<QString> unique;
      unique.reserve(m_data.size());
      for(const auto& item:m_data) if(seen.insert(item).second) unique.push_back(item);
      const int removed=QString::checked_size(m_data.size()-unique.size());
      // Preserve first occurrences and avoid replacing storage when nothing was removed.
      if(removed) m_data.swap(unique);
      return removed;
   }

   // ============ КОНВЕРТАЦИЯ ============
   std::vector<QString> toStdVector() const & { return m_data; }
   std::vector<QString> toStdVector() && { return std::move(m_data); }

   std::vector<std::string> toStdStringVector() const
	{
      std::vector<std::string> result;
      result.reserve(size());
      for(const auto& s : *this)
		{
         result.push_back(s.toStdString());
      }
      return result;
   }

   // ============ ОПЕРАТОРЫ СРАВНЕНИЯ ============
   bool operator==(const QStringList& other) const
	{
      return m_data == other.m_data;
   }

   bool operator!=(const QStringList& other) const
	{
      return m_data != other.m_data;
   }

   bool operator<(const QStringList& other) const
	{
      return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
   }

   bool operator<=(const QStringList& other) const
	{
      return !(other < *this);
   }

   bool operator>(const QStringList& other) const
	{
      return other < *this;
   }

   bool operator>=(const QStringList& other) const
	{
      return !(*this < other);
   }

   // ============ ВСПОМОГАТЕЛЬНЫЕ ============
   friend void swap(QStringList& a, QStringList& b) noexcept
	{
      a.swap(b);
   }

   friend std::size_t hash_value(const QStringList& list)
	{
      std::size_t seed = list.size();

      // Своя функция хэширования QString вместо std::hash<QString>
      auto qstring_hash = [](const QString& str) -> 
		                    std::size_t 
		   		   		  {
           		   		   	const wchar_t* data = str.utf16();
           		   		   	int len = str.length();
           		   		   	std::size_t h = 0;

           		   		   	// Алгоритм djb2 (как в Qt qHash)
           		   		   	for(int i = 0; i < len; i++)
           		   		   	{
           		   		   		h = ((h << 5) + h) + static_cast<std::size_t>(data[i]); // h * 33 + c
           		   		   	}
           		   		   	return h;
           		   		   };

      for(const auto& s : list)
		{
         std::size_t h = qstring_hash(s);
         seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
      return seed;
   }

};

// Специализация std::hash для QStringList
namespace std
{
   template<>
   struct hash<QString>
	{
      std::size_t operator()(const QString& str) const noexcept
		{
         const wchar_t* data = str.utf16();
         int len = str.length();
         std::size_t h = 0;

         // Алгоритм djb2
         for(int i = 0; i < len; i++)
		   {
            h = ((h << 5) + h) + static_cast<std::size_t>(data[i]);
         }
         return h;
      }
   };

   template<>
   struct hash<QStringList>
	{
      std::size_t operator()(const QStringList& list) const noexcept
		{
         return hash_value(list);
      }
   };
}

// Глобальный оператор сложения
inline QStringList operator+(QStringList lhs, const QStringList& rhs) {
   lhs << rhs;
   return lhs;
}

inline QStringList operator+(QStringList lhs, const QString& rhs) {
   lhs << rhs;
   return lhs;
}

inline QStringList operator+(const QString& lhs, const QStringList& rhs) {
   QStringList result;
   result << lhs;
   result << rhs;
   return result;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
//===================================================================================================================================================


namespace Qt
{
   namespace Literals
	{

      // Для UTF-8 строк
      inline QString operator"" _qs(const char* str, size_t size)
      {
         if(size>static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::length_error("QString literal too long");
         return QString::fromUtf8(str, static_cast<int>(size));
      }

      // Для UTF-16 строк
      inline QString operator"" _qs(const char16_t* str, size_t size)
      {
         return QString::fromStdU16String(std::u16string(str,size));
   }

      // Для широких строк
      inline QString operator"" _qs(const wchar_t* str, size_t size)
      {
         return QString::fromStdWString(std::wstring(str,size));
   }

#ifdef __cpp_char8_t
      inline QString operator"" _qs(const char8_t* str, size_t size)
      {
         if(size>static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::length_error("QString literal too long");
         return QString::fromUtf8(reinterpret_cast<const char*>(str), static_cast<int>(size));
      }
#endif

      // Числовые литералы
      constexpr int operator"" _q(unsigned long long value) noexcept
      {
         return static_cast<int>(value);
      }

      constexpr float operator"" _qf(long double value) noexcept
      {
         return static_cast<float>(value);
      }

      constexpr double operator"" _qd(long double value) noexcept
      {
         return static_cast<double>(value);
      }

   } // namespace Literals
} // namespace Qt



#endif // QSTRING_NX_H
