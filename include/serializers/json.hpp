#ifndef SERIALIZERS_JSON_HPP
#define SERIALIZERS_JSON_HPP

#include <binelpro/symbol.hpp>
#include "../structure.hpp"

namespace bp {
    namespace serializers {
        using namespace bp::literals;
        constexpr bp::hash_type Json = "json"_h;
    }
    template<>
    std::string structure::serialize<serializers::Json>() const;
    template<>
    bool structure::parse<serializers::Json>(const std::string &_str);
}
#endif //SERIALIZERS_JSON_HPP
