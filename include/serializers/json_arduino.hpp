#ifndef FARMBRAIN_SERIALIZERS_JSON_HPP
#define FARMBRAIN_SERIALIZERS_JSON_HPP

#include "../structure.hpp"

namespace bp {
    namespace serializers {
        using namespace bp::literals;
        constexpr bp::hash_type JsonArduino = "json_arduino"_h;
    }
    template<>
    std::string structure::serialize<serializers::JsonArduino>() const;
    template<>
    bool structure::parse<serializers::JsonArduino>(const bp::string_view &_str);
}
#endif //FARMBRAIN_SERIALIZERS_JSON_HPP
