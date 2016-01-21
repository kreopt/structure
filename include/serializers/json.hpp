#ifndef SERIALIZERS_JSON_HPP
#define SERIALIZERS_JSON_HPP

#include <binelpro/symbol.hpp>
#include "../serializer.hpp"

namespace bp {
    namespace serializers {
        using namespace bp::literals;
        constexpr symbol_t Json = "json"_sym;
    }
    template<>
    std::string serializer::stringify<serializers::Json>() const;
    template<>
    void serializer::parse<serializers::Json>(const std::string &_str);
}
#endif //SERIALIZERS_JSON_HPP
