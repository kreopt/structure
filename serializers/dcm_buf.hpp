#ifndef SERIALIZERS_DCM_BUF_HPP
#define SERIALIZERS_DCM_BUF_HPP

namespace bp {
    namespace serializers {
        using namespace bp::literals;
        constexpr symbol_t Dcm = "dcm"_sym;
    }
    template<>
    std::string serializer::stringify<serializers::Dcm>() const;
    template<>
    void serializer::parse<serializers::Dcm>(const std::string &_str);
}

#endif //SERIALIZERS_DCM_BUF_HPP
