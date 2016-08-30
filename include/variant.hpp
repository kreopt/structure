#ifndef HG_VARIANT_H
#define HG_VARIANT_H

//TODO:
// Better compile-time checks (eg, ensure types are unique)
// Recursive variant
// Reference members (?)
// Better namespace safety (mostly done now)
// Clean up implementation
// Add helper visitors for copy/move/assign (especially assign) etc
// (ie- intended to be used outside the class,
//  *not* the variant_detail::MoveVisitor etc which are only
//  useful within the implementation)
// Complete test suite

#include <type_traits>
#include <cassert>
#include <tuple>
#include <utility>
#include <cstring>
#include <limits>
#include <type_traits>
#include <cinttypes>

#ifdef USE_BOOST_VARIANT
#include <boost/variant.hpp>
#else
#include "mapbox/variant.hpp"
#endif

namespace bp {
#ifdef USE_BOOST_VARIANT
    template <typename ...Types>
    using variant = boost::variant<Types...>;
#else
    template <typename ...Types>
    using variant = mapbox::util::variant<Types...>;
#endif

    template<typename Visitor, typename Variant>
    typename std::remove_reference<Visitor>::type::result_type
    visit(Visitor &&visitor, Variant &&variant) {
#ifndef USE_BOOST_VARIANT
        return mapbox::util::apply_visitor(visitor, variant);
#else
        return boost::apply_visitor(visitor, variant);
#endif
    }

    template<class T, class... Types>
    constexpr bool holds_alternative(const bp::variant<Types...> &v) {
#ifndef USE_BOOST_VARIANT
        return v.template is<T>();
#else
        return v.type() == typeid(T);
#endif
    };

    template<typename T, typename... Types>
    T &get(bp::variant<Types...> &v) {
#ifndef USE_BOOST_VARIANT
        return v.template get<T>();
#else
        return boost::get<T>(v);
#endif
    };

    template<class T, class... Types>
    T &&get(bp::variant<Types...> &&v) {
#ifndef USE_BOOST_VARIANT
        return v.template get<T>();
#else
        return boost::get<T>(v);
#endif
    };

    template<class T, class... Types>
    const T &get(const bp::variant<Types...> &v) {
#ifndef USE_BOOST_VARIANT
        return v.template get<T>();
#else
        return boost::get<T>(v);
#endif
    };

    template<class T, class... Types>
    const T &&get(const bp::variant<Types...> &&v) {
#ifndef USE_BOOST_VARIANT
        return v.template get<T>();
#else
        return boost::get<T>(v);
#endif
    };

} //namespace hg

#endif //HG_VARIANT_H
