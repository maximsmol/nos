#pragma once

namespace typ {
  template<class T>
  struct type_ {using type = T;};

  template<class T, T v>
  struct const_ {constexpr static T val = v;};

  struct false_ : const_<bool, false> {};
  struct true_  : const_<bool, true> {};

  template<bool b>
  struct not_ : const_<bool, !b> {};


  namespace detail {
    template<class T>
    struct id : type_<T> {};
  }

  template<class T>
  using id = typename detail::id<T>::type;


  namespace signed_ {
    namespace detail {
      template<class T> struct pred : false_ {};
      template<> struct pred<signed char> : true_ {};
      template<> struct pred<short>       : true_ {};
      template<> struct pred<int>         : true_ {};
      template<> struct pred<long>        : true_ {};
      template<> struct pred<long long>   : true_ {};
    }

    template<class T>
    constexpr static bool pred = detail::pred<T>::val;


    namespace detail {
      template<class T> struct make {};
      template<> struct make<unsigned char>      : type_<signed char> {};
      template<> struct make<         char>      : type_<signed char> {};
      template<> struct make<unsigned short>     : type_<short>       {};
      template<> struct make<unsigned int>       : type_<int>         {};
      template<> struct make<unsigned long>      : type_<long>        {};
      template<> struct make<unsigned long long> : type_<long long>   {};

      template<> struct make<signed char> : type_<signed char> {};
      template<> struct make<short>       : type_<short>       {};
      template<> struct make<int>         : type_<int>         {};
      template<> struct make<long>        : type_<long>        {};
      template<> struct make<long long>   : type_<long long>   {};
    }

    template<class T>
    using make = typename detail::make<T>::type;
  }

  namespace unsigned_ {
    namespace detail {
      template<class T> struct pred : false_ {};
      template<> struct pred<unsigned char>      : true_ {};
      template<> struct pred<unsigned short>     : true_ {};
      template<> struct pred<unsigned int>       : true_ {};
      template<> struct pred<unsigned long>      : true_ {};
      template<> struct pred<unsigned long long> : true_ {};
    }

    template<class T>
    constexpr static bool pred = detail::pred<T>::val;


    namespace detail {
      template<class T> struct make {};
      template<> struct make<signed char> : type_<unsigned char>      {};
      template<> struct make<char>        : type_<unsigned char>      {};
      template<> struct make<short>       : type_<unsigned short>     {};
      template<> struct make<int>         : type_<unsigned int>       {};
      template<> struct make<long>        : type_<unsigned long>      {};
      template<> struct make<long long>   : type_<unsigned long long> {};

      template<> struct make<unsigned char>      : type_<unsigned char>      {};
      template<> struct make<unsigned short>     : type_<unsigned short>     {};
      template<> struct make<unsigned int>       : type_<unsigned int>       {};
      template<> struct make<unsigned long>      : type_<unsigned long>      {};
      template<> struct make<unsigned long long> : type_<unsigned long long> {};
    }

    template<class T>
    using make = typename detail::make<T>::type;
  }


  namespace detail {
    template<bool b, class T = void>
    struct enable_if {};

    template<class T>
    struct enable_if<true, T> : type_<T> {};
  }

  template<bool b, class T = void>
  using enable_if = typename detail::enable_if<b, T>::type;
}
