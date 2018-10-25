#pragma once

#include "../ps8042.hpp"

namespace scancode_set2 {
  enum class Scancode {
    unknown,

    escape, backtick, capslock,
    tab, space, backspace, enter,

    lbracket, rbracket, backslash,
    semicolon, quote,
    comma, period, slash,

    lshift, rshift,
    lctrl, rctrl,
    lalt, ralt,
    lgui, rgui,

    numlock, scrolllock,

    f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,

    q, w, e, r, t, y, u, i, o, p,
    a, s, d, f, g, h, j, k, l,
    z, x, c, v, b, n, m,

    n1, n2, n3, n4, n5, n6, n7, n8, n9, n0,
    minus, equals,

    home, end,
    page_down, page_up,
    insert, del,
    printscrn, pause,

    left, up, right, down,

    kp_n1, kp_n2, kp_n3, kp_n4, kp_n5, kp_n6, kp_n7, kp_n8, kp_n9, kp_n0,
    kp_period, kp_enter, kp_plus, kp_minus, kp_asterisk, kp_slash,

    calculator, apps, myComputer, email,
    www_search, www_fav, www_home,
    www_refresh, www_stop,
    www_fwd, www_back,
    media_select,
    prev_track, next_track,
    play_pause, stop,
    mute, volume_down, volume_up,

    power, sleep, wake
  };

  constexpr const char* scancodeName(Scancode c) {
    if (c == Scancode::unknown)
          return "unknown";

    if (c == Scancode::scrolllock)
      return "scrolllock";
    if (c == Scancode::numlock)
      return "numlock";
    if (c == Scancode::capslock)
      return "capslock";

    if (c == Scancode::escape)
      return "escape";
    if (c == Scancode::tab)
      return "tab";
    if (c == Scancode::backspace)
      return "backspace";
    if (c == Scancode::enter)
      return "enter";

    if (c == Scancode::lshift)
      return "lshift";
    if (c == Scancode::rshift)
      return "rshift";
    if (c == Scancode::lctrl)
      return "lctrl";
    if (c == Scancode::rctrl)
      return "rctrl";
    if (c == Scancode::lalt)
      return "lalt";
    if (c == Scancode::ralt)
      return "ralt";
    if (c == Scancode::lgui)
      return "lgui";
    if (c == Scancode::rgui)
      return "rgui";

    if (c == Scancode::home)
      return "home";
    if (c == Scancode::end)
      return "end";
    if (c == Scancode::page_down)
      return "page down";
    if (c == Scancode::page_up)
      return "page up";
    if (c == Scancode::insert)
      return "insert";
    if (c == Scancode::del)
      return "del";
    if (c == Scancode::printscrn)
      return "print screen";
    if (c == Scancode::pause)
      return "pause";

    if (c == Scancode::left)
      return "left";
    if (c == Scancode::up)
      return "up";
    if (c == Scancode::right)
      return "right";
    if (c == Scancode::down)
      return "down";

    if (c == Scancode::q)
      return "q";
    if (c == Scancode::w)
      return "w";
    if (c == Scancode::e)
      return "e";
    if (c == Scancode::r)
      return "r";
    if (c == Scancode::t)
      return "t";
    if (c == Scancode::y)
      return "y";
    if (c == Scancode::u)
      return "u";
    if (c == Scancode::i)
      return "i";
    if (c == Scancode::o)
      return "o";
    if (c == Scancode::p)
      return "p";
    if (c == Scancode::a)
      return "a";
    if (c == Scancode::s)
      return "s";
    if (c == Scancode::d)
      return "d";
    if (c == Scancode::f)
      return "f";
    if (c == Scancode::g)
      return "g";
    if (c == Scancode::h)
      return "h";
    if (c == Scancode::j)
      return "j";
    if (c == Scancode::k)
      return "k";
    if (c == Scancode::l)
      return "l";
    if (c == Scancode::z)
      return "z";
    if (c == Scancode::x)
      return "x";
    if (c == Scancode::c)
      return "c";
    if (c == Scancode::v)
      return "v";
    if (c == Scancode::b)
      return "b";
    if (c == Scancode::n)
      return "n";
    if (c == Scancode::m)
      return "m";

    if (c == Scancode::f1)
      return "F1";
    if (c == Scancode::f2)
      return "F2";
    if (c == Scancode::f3)
      return "F3";
    if (c == Scancode::f4)
      return "F4";
    if (c == Scancode::f5)
      return "F5";
    if (c == Scancode::f6)
      return "F6";
    if (c == Scancode::f7)
      return "F7";
    if (c == Scancode::f8)
      return "F8";
    if (c == Scancode::f9)
      return "F9";
    if (c == Scancode::f10)
      return "F10";
    if (c == Scancode::f11)
      return "F11";
    if (c == Scancode::f12)
      return "F12";

    if (c == Scancode::n1)
      return "1";
    if (c == Scancode::n2)
      return "2";
    if (c == Scancode::n3)
      return "3";
    if (c == Scancode::n4)
      return "4";
    if (c == Scancode::n5)
      return "5";
    if (c == Scancode::n6)
      return "6";
    if (c == Scancode::n7)
      return "7";
    if (c == Scancode::n8)
      return "8";
    if (c == Scancode::n9)
      return "9";
    if (c == Scancode::n0)
      return "0";

    if (c == Scancode::kp_n1)
      return "(kp) 1";
    if (c == Scancode::kp_n2)
      return "(kp) 2";
    if (c == Scancode::kp_n3)
      return "(kp) 3";
    if (c == Scancode::kp_n4)
      return "(kp) 4";
    if (c == Scancode::kp_n5)
      return "(kp) 5";
    if (c == Scancode::kp_n6)
      return "(kp) 6";
    if (c == Scancode::kp_n7)
      return "(kp) 7";
    if (c == Scancode::kp_n8)
      return "(kp) 8";
    if (c == Scancode::kp_n9)
      return "(kp) 9";
    if (c == Scancode::kp_n0)
      return "(kp) 0";

    if (c == Scancode::kp_period)
      return "(kp) .";
    if (c == Scancode::kp_enter)
      return "(kp) enter";
    if (c == Scancode::kp_plus)
      return "(kp) +";
    if (c == Scancode::kp_minus)
      return "(kp) -";
    if (c == Scancode::kp_asterisk)
      return "(kp) *";
    if (c == Scancode::kp_slash)
      return "(kp) /";

    if (c == Scancode::space)
      return " ";
    if (c == Scancode::minus)
      return "-";
    if (c == Scancode::equals)
      return "=";
    if (c == Scancode::backslash)
      return "\\";
    if (c == Scancode::lbracket)
      return "[";
    if (c == Scancode::rbracket)
      return "]";
    if (c == Scancode::semicolon)
      return ";";
    if (c == Scancode::quote)
      return "\'";
    if (c == Scancode::comma)
      return ",";
    if (c == Scancode::period)
      return ".";
    if (c == Scancode::slash)
      return "/";
    if (c == Scancode::backtick)
      return "`";

    if (c == Scancode::calculator)
      return "calculator";
    if (c == Scancode::apps)
      return "apps";
    if (c == Scancode::myComputer)
      return "My Computer";
    if (c == Scancode::email)
      return "email";

    if (c == Scancode::www_search)
      return "www search";
    if (c == Scancode::www_fav)
      return "www fav";
    if (c == Scancode::www_home)
      return "www home";
    if (c == Scancode::www_refresh)
      return "www refresh";
    if (c == Scancode::www_stop)
      return "www stop";
    if (c == Scancode::www_fwd)
      return "www forward";
    if (c == Scancode::www_back)
      return "www back";

    if (c == Scancode::media_select)
      return "media select";
    if (c == Scancode::prev_track)
      return "previous track";
    if (c == Scancode::next_track)
      return "next track";
    if (c == Scancode::play_pause)
      return "play/pause";
    if (c == Scancode::stop)
      return "stop";
    if (c == Scancode::mute)
      return "mute";
    if (c == Scancode::volume_down)
      return "volume down";
    if (c == Scancode::volume_up)
      return "volume up";

    if (c == Scancode::power)
      return "power";
    if (c == Scancode::sleep)
      return "sleep";
    if (c == Scancode::wake)
      return "wake";

    return "unknown";
  }

  constexpr char scancodeToChar(Scancode c) {
    if (c == Scancode::unknown)
      return '\0';

    if (c == Scancode::q)
      return 'q';
    if (c == Scancode::w)
      return 'w';
    if (c == Scancode::e)
      return 'e';
    if (c == Scancode::r)
      return 'r';
    if (c == Scancode::t)
      return 't';
    if (c == Scancode::y)
      return 'y';
    if (c == Scancode::u)
      return 'u';
    if (c == Scancode::i)
      return 'i';
    if (c == Scancode::o)
      return 'o';
    if (c == Scancode::p)
      return 'p';
    if (c == Scancode::a)
      return 'a';
    if (c == Scancode::s)
      return 's';
    if (c == Scancode::d)
      return 'd';
    if (c == Scancode::f)
      return 'f';
    if (c == Scancode::g)
      return 'g';
    if (c == Scancode::h)
      return 'h';
    if (c == Scancode::j)
      return 'j';
    if (c == Scancode::k)
      return 'k';
    if (c == Scancode::l)
      return 'l';
    if (c == Scancode::z)
      return 'z';
    if (c == Scancode::x)
      return 'x';
    if (c == Scancode::c)
      return 'c';
    if (c == Scancode::v)
      return 'v';
    if (c == Scancode::b)
      return 'b';
    if (c == Scancode::n)
      return 'n';
    if (c == Scancode::m)
      return 'm';

    if (c == Scancode::n1)
      return '1';
    if (c == Scancode::n2)
      return '2';
    if (c == Scancode::n3)
      return '3';
    if (c == Scancode::n4)
      return '4';
    if (c == Scancode::n5)
      return '5';
    if (c == Scancode::n6)
      return '6';
    if (c == Scancode::n7)
      return '7';
    if (c == Scancode::n8)
      return '8';
    if (c == Scancode::n9)
      return '9';
    if (c == Scancode::n0)
      return '0';

    if (c == Scancode::kp_n1)
      return '1';
    if (c == Scancode::kp_n2)
      return '2';
    if (c == Scancode::kp_n3)
      return '3';
    if (c == Scancode::kp_n4)
      return '4';
    if (c == Scancode::kp_n5)
      return '5';
    if (c == Scancode::kp_n6)
      return '6';
    if (c == Scancode::kp_n7)
      return '7';
    if (c == Scancode::kp_n8)
      return '8';
    if (c == Scancode::kp_n9)
      return '9';
    if (c == Scancode::kp_n0)
      return '0';

    if (c == Scancode::space)
      return ' ';
    if (c == Scancode::minus)
      return '-';
    if (c == Scancode::equals)
      return '=';
    if (c == Scancode::backslash)
      return '\\';
    if (c == Scancode::lbracket)
      return '[';
    if (c == Scancode::rbracket)
      return ']';
    if (c == Scancode::semicolon)
      return ';';
    if (c == Scancode::quote)
      return '\'';
    if (c == Scancode::comma)
      return ',';
    if (c == Scancode::period)
      return '.';
    if (c == Scancode::slash)
      return '/';
    if (c == Scancode::backtick)
      return '`';

    return '\0';
  }

  constexpr uint8_t releaseKeyScancode = 0xf0;
  constexpr uint8_t extraKeyScancode = 0xe0;

  constexpr Scancode regularKeysMap[] = {
    Scancode::unknown,     // 0x00
    Scancode::f9,          // 0x01
    Scancode::unknown,     // 0x02
    Scancode::f5,          // 0x03
    Scancode::f3,          // 0x04
    Scancode::f1,          // 0x05
    Scancode::f2,          // 0x06
    Scancode::f12,         // 0x07
    Scancode::unknown,     // 0x08
    Scancode::f10,         // 0x09
    Scancode::f8,          // 0x0a
    Scancode::f6,          // 0x0b
    Scancode::f4,          // 0x0c
    Scancode::tab,         // 0x0d
    Scancode::backtick,    // 0x0e
    Scancode::unknown,     // 0x0f
    Scancode::unknown,     // 0x10
    Scancode::lalt,        // 0x11
    Scancode::lshift,      // 0x12
    Scancode::unknown,     // 0x13
    Scancode::lctrl,       // 0x14
    Scancode::q,           // 0x15
    Scancode::n1,          // 0x16
    Scancode::unknown,     // 0x17
    Scancode::unknown,     // 0x18
    Scancode::unknown,     // 0x19
    Scancode::z,           // 0x1a
    Scancode::s,           // 0x1b
    Scancode::a,           // 0x1c
    Scancode::w,           // 0x1d
    Scancode::n2,          // 0x1e
    Scancode::unknown,     // 0x1f
    Scancode::unknown,     // 0x20
    Scancode::c,           // 0x21
    Scancode::x,           // 0x22
    Scancode::d,           // 0x23
    Scancode::e,           // 0x24
    Scancode::n4,          // 0x25
    Scancode::n3,          // 0x26
    Scancode::unknown,     // 0x27
    Scancode::unknown,     // 0x28
    Scancode::space,       // 0x29
    Scancode::v,           // 0x2a
    Scancode::f,           // 0x2b
    Scancode::t,           // 0x2c
    Scancode::r,           // 0x2d
    Scancode::n5,          // 0x2e
    Scancode::unknown,     // 0x2f
    Scancode::unknown,     // 0x30
    Scancode::n,           // 0x31
    Scancode::b,           // 0x32
    Scancode::h,           // 0x33
    Scancode::g,           // 0x34
    Scancode::y,           // 0x35
    Scancode::n6,          // 0x36
    Scancode::unknown,     // 0x37
    Scancode::unknown,     // 0x38
    Scancode::unknown,     // 0x39
    Scancode::m,           // 0x3a
    Scancode::j,           // 0x3b
    Scancode::u,           // 0x3c
    Scancode::n7,          // 0x3d
    Scancode::n8,          // 0x3e
    Scancode::unknown,     // 0x3f
    Scancode::unknown,     // 0x40
    Scancode::comma,       // 0x41
    Scancode::k,           // 0x42
    Scancode::i,           // 0x43
    Scancode::o,           // 0x44
    Scancode::n0,          // 0x45
    Scancode::n9,          // 0x46
    Scancode::unknown,     // 0x47
    Scancode::unknown,     // 0x48
    Scancode::period,      // 0x49
    Scancode::slash,       // 0x4a
    Scancode::l,           // 0x4b
    Scancode::semicolon,   // 0x4c
    Scancode::p,           // 0x4d
    Scancode::minus,       // 0x4e
    Scancode::unknown,     // 0x4f
    Scancode::unknown,     // 0x50
    Scancode::unknown,     // 0x51
    Scancode::quote,       // 0x52
    Scancode::unknown,     // 0x53
    Scancode::lbracket,    // 0x54
    Scancode::equals,      // 0x55
    Scancode::unknown,     // 0x56
    Scancode::unknown,     // 0x57
    Scancode::capslock,    // 0x58
    Scancode::rshift,      // 0x59
    Scancode::enter,       // 0x5a
    Scancode::rbracket,    // 0x5b
    Scancode::unknown,     // 0x5c
    Scancode::backslash,   // 0x5d
    Scancode::unknown,     // 0x5e
    Scancode::unknown,     // 0x5f
    Scancode::unknown,     // 0x60
    Scancode::unknown,     // 0x61
    Scancode::unknown,     // 0x62
    Scancode::unknown,     // 0x63
    Scancode::unknown,     // 0x64
    Scancode::unknown,     // 0x65
    Scancode::backspace,   // 0x66
    Scancode::unknown,     // 0x67
    Scancode::unknown,     // 0x68
    Scancode::kp_n1,       // 0x69
    Scancode::unknown,     // 0x6a
    Scancode::kp_n4,       // 0x6b
    Scancode::kp_n7,       // 0x6c
    Scancode::unknown,     // 0x6d
    Scancode::unknown,     // 0x6e
    Scancode::unknown,     // 0x6f
    Scancode::kp_n0,       // 0x70
    Scancode::kp_period,   // 0x71
    Scancode::kp_n2,       // 0x72
    Scancode::kp_n5,       // 0x73
    Scancode::kp_n6,       // 0x74
    Scancode::kp_n8,       // 0x75
    Scancode::escape,      // 0x76
    Scancode::numlock,     // 0x77
    Scancode::f11,         // 0x78
    Scancode::kp_plus,     // 0x79
    Scancode::kp_n3,       // 0x7a
    Scancode::kp_minus,    // 0x7b
    Scancode::kp_asterisk, // 0x7c
    Scancode::kp_n9,       // 0x7d
    Scancode::scrolllock,  // 0x7e
    Scancode::unknown,     // 0x7f
    Scancode::unknown,     // 0x80
    Scancode::unknown,     // 0x81
    Scancode::unknown,     // 0x82
    Scancode::f7           // 0x83
  };

  constexpr Scancode extraKeysMap[] = {
    Scancode::unknown,     // 0x00
    Scancode::unknown,     // 0x01
    Scancode::unknown,     // 0x02
    Scancode::unknown,     // 0x03
    Scancode::unknown,     // 0x04
    Scancode::unknown,     // 0x05
    Scancode::unknown,     // 0x06
    Scancode::unknown,     // 0x07
    Scancode::unknown,     // 0x08
    Scancode::unknown,     // 0x09
    Scancode::unknown,     // 0x0a
    Scancode::unknown,     // 0x0b
    Scancode::unknown,     // 0x0c
    Scancode::unknown,     // 0x0d
    Scancode::unknown,     // 0x0e
    Scancode::unknown,     // 0x0f
    Scancode::www_search,  // 0x10
    Scancode::ralt,        // 0x11
    Scancode::unknown,     // 0x12
    Scancode::unknown,     // 0x13
    Scancode::rctrl,       // 0x14
    Scancode::prev_track,  // 0x15
    Scancode::unknown,     // 0x16
    Scancode::unknown,     // 0x17
    Scancode::www_fav,     // 0x18
    Scancode::unknown,     // 0x19
    Scancode::unknown,     // 0x1a
    Scancode::unknown,     // 0x1b
    Scancode::unknown,     // 0x1c
    Scancode::unknown,     // 0x1d
    Scancode::unknown,     // 0x1e
    Scancode::lgui,        // 0x1f
    Scancode::www_refresh, // 0x20
    Scancode::volume_down, // 0x21
    Scancode::unknown,     // 0x22
    Scancode::mute,        // 0x23
    Scancode::unknown,     // 0x24
    Scancode::unknown,     // 0x25
    Scancode::unknown,     // 0x26
    Scancode::rgui,        // 0x27
    Scancode::www_stop,    // 0x28
    Scancode::unknown,     // 0x29
    Scancode::unknown,     // 0x2a
    Scancode::calculator,  // 0x2b
    Scancode::unknown,     // 0x2c
    Scancode::unknown,     // 0x2d
    Scancode::unknown,     // 0x2e
    Scancode::apps,        // 0x2f
    Scancode::www_fwd,     // 0x30
    Scancode::unknown,     // 0x31
    Scancode::volume_up,   // 0x32
    Scancode::unknown,     // 0x33
    Scancode::play_pause,  // 0x34
    Scancode::unknown,     // 0x35
    Scancode::unknown,     // 0x36
    Scancode::power,       // 0x37
    Scancode::www_back,    // 0x38
    Scancode::unknown,     // 0x39
    Scancode::www_home,    // 0x3a
    Scancode::stop,        // 0x3b
    Scancode::unknown,     // 0x3c
    Scancode::unknown,     // 0x3d
    Scancode::unknown,     // 0x3e
    Scancode::sleep,       // 0x3f
    Scancode::myComputer,  // 0x40
    Scancode::unknown,     // 0x41
    Scancode::unknown,     // 0x42
    Scancode::unknown,     // 0x43
    Scancode::unknown,     // 0x44
    Scancode::unknown,     // 0x45
    Scancode::unknown,     // 0x46
    Scancode::unknown,     // 0x47
    Scancode::email,       // 0x48
    Scancode::unknown,     // 0x49
    Scancode::kp_slash,    // 0x4a
    Scancode::unknown,     // 0x4b
    Scancode::unknown,     // 0x4c
    Scancode::next_track,  // 0x4d
    Scancode::unknown,     // 0x4e
    Scancode::unknown,     // 0x4f
    Scancode::media_select,// 0x50
    Scancode::unknown,     // 0x51
    Scancode::unknown,     // 0x52
    Scancode::unknown,     // 0x53
    Scancode::unknown,     // 0x54
    Scancode::unknown,     // 0x55
    Scancode::unknown,     // 0x56
    Scancode::unknown,     // 0x57
    Scancode::unknown,     // 0x58
    Scancode::unknown,     // 0x59
    Scancode::kp_enter,    // 0x5a
    Scancode::unknown,     // 0x5b
    Scancode::unknown,     // 0x5c
    Scancode::unknown,     // 0x5d
    Scancode::wake,        // 0x5e
    Scancode::unknown,     // 0x5f
    Scancode::unknown,     // 0x60
    Scancode::unknown,     // 0x61
    Scancode::unknown,     // 0x62
    Scancode::unknown,     // 0x63
    Scancode::unknown,     // 0x64
    Scancode::unknown,     // 0x65
    Scancode::unknown,     // 0x66
    Scancode::unknown,     // 0x67
    Scancode::unknown,     // 0x68
    Scancode::end,         // 0x69
    Scancode::unknown,     // 0x6a
    Scancode::left,        // 0x6b
    Scancode::home,        // 0x6c
    Scancode::unknown,     // 0x6d
    Scancode::unknown,     // 0x6e
    Scancode::unknown,     // 0x6f
    Scancode::insert,      // 0x70
    Scancode::del,         // 0x71
    Scancode::down,        // 0x72
    Scancode::unknown,     // 0x73
    Scancode::right,       // 0x74
    Scancode::up,          // 0x75
    Scancode::unknown,     // 0x76
    Scancode::unknown,     // 0x77
    Scancode::unknown,     // 0x78
    Scancode::unknown,     // 0x79
    Scancode::page_down,   // 0x7a
    Scancode::unknown,     // 0x7b
    Scancode::unknown,     // 0x7c
    Scancode::page_up      // 0x7d
  };

  constexpr uint8_t printscrn_press[] = {
    extraKeyScancode, 0x12, extraKeyScancode, 0x7c
  };
  constexpr uint8_t printscrn_release[] = {
    extraKeyScancode, releaseKeyScancode, 0x7c, extraKeyScancode, releaseKeyScancode, 0x12
  };
  constexpr uint8_t pause_press[] = {
    0xe1, 0x14, 0x77, 0xe1, releaseKeyScancode, 0x14, releaseKeyScancode, 0x77
  };

  enum class KbdEventType {
    Press, Release
  };
  struct KbdEvent {
    public:
      Scancode code;
      KbdEventType type;
    private:
  };

  bool isSequence(const ps8042::Controller& c, const uint8_t seq[], int i0, int seq_size);
  bool isSequence(const ps8042::Controller& c, const uint8_t seq[], int i0, int seq_size) {
    for (int i = i0; i < seq_size; ++i)
      if (c.poll() != seq[i])
        return false;
    return true;
  }

  KbdEvent pollKbd(const ps8042::Controller& c);
  KbdEvent pollKbd(const ps8042::Controller& c) {
    KbdEvent res = {
      Scancode::unknown,
      KbdEventType::Press
    };

    // possible sequences:
    // extraKeyScancode printscrn_press...
    // extraKeyScancode releaseKeyScancode printscrn_release...
    // extraKeyScancode releaseKeyScancode extraKey
    // extraKeyScancode extraKey
    // pause_press...
    // releaseKeyScancode regularKey
    // regularKey

    uint8_t keyByte = c.poll();
    if (keyByte == extraKeyScancode) { // extraKeyScancode
      keyByte = c.poll();

      if (keyByte == printscrn_press[1]) { // extraKeyScancode printscrn_press...
        if (isSequence(c, printscrn_press, 2, sizeof(printscrn_press)/sizeof(uint8_t)))
          res.code = Scancode::printscrn;
      }
      else {
        if (keyByte == releaseKeyScancode) { // extraKeyScancode releaseKeyScancode
          res.type = KbdEventType::Release;

          if (keyByte == printscrn_release[2]) { // extraKeyScancode releaseKeyScancode printscrn_release...
            if (isSequence(c, printscrn_release, 3, sizeof(printscrn_release)/sizeof(uint8_t)))
              res.code = Scancode::printscrn;
          }
          else // extraKeyScancode releaseKeyScancode extraKey
            res.code = extraKeysMap[c.poll()];
        }
        else // extraKeyScancode extraKey
          res.code = extraKeysMap[keyByte];
      }
    }
    else if (keyByte == pause_press[0]) { // pause_press...
      if (isSequence(c, pause_press, 1, sizeof(pause_press)/sizeof(uint8_t)))
        res.code = Scancode::pause;
    }
    else {
      if (keyByte == releaseKeyScancode) { // releaseKeyScancode regularKey
        res.type = KbdEventType::Release;
        keyByte = c.poll();
      }

      // regularKey
      res.code = regularKeysMap[keyByte];
    }

    return res;
  }
}
