#ifndef SERIALIZERS_DCM_BUF_HPP
#define SERIALIZERS_DCM_BUF_HPP

#include <binelpro/symbol.hpp>
#include "../serializer.hpp"

namespace bp {
    namespace serializers {
        using namespace bp::literals;
        constexpr symbol::hash_type Dcm = "dcm"_hash;
    }
    template<>
    std::string serializer::stringify<serializers::Dcm>() const;
    template<>
    void serializer::parse<serializers::Dcm>(const std::string &_str);
}

#endif //SERIALIZERS_DCM_BUF_HPP
