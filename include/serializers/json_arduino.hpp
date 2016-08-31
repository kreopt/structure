#ifndef FARMBRAIN_SERIALIZERS_JSON_HPP
#define FARMBRAIN_SERIALIZERS_JSON_HPP

#include "structure.hpp"

namespace bp {
    namespace serializers {
        using namespace bp::literals;
        constexpr bp::symbol::hash_type JsonArduino = "json_arduino"_hash;
    }
    template<>
    std::string structure::stringify<serializers::JsonArduino>() const;
    template<>
    void structure::parse<serializers::JsonArduino>(const std::string &_str);
}
#endif //FARMBRAIN_SERIALIZERS_JSON_HPP
