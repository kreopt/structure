#ifndef SERIALIZERS_DCM_BUF_HPP
#define SERIALIZERS_DCM_BUF_HPP

#include <binelpro/symbol.hpp>
#include "../structure.hpp"

namespace bp {
    namespace serializers {
        using namespace bp::literals;
        constexpr symbol::hash_type Dcm = "dcm"_hash;
    }
    template<>
    std::string structure::stringify<serializers::Dcm>() const;
    template<>
    void structure::parse<serializers::Dcm>(const std::string &_str);
}

#endif //SERIALIZERS_DCM_BUF_HPP
