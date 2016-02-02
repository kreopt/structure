#ifndef SERIALIZERS_JSON_HPP
#define SERIALIZERS_JSON_HPP

#include <binelpro/symbol.hpp>
#include "../structure.hpp"

namespace bp {
    namespace serializers {
        using namespace bp::literals;
        constexpr symbol::hash_type Json = "json"_hash;
    }
    template<>
    std::string structure::stringify<serializers::Json>() const;
    template<>
    void structure::parse<serializers::Json>(const std::string &_str);
}
#endif //SERIALIZERS_JSON_HPP
