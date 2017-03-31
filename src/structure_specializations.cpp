#include "structure.hpp"

namespace bp {
    template<>
    std::string structure::as<std::string>() const {
        switch (type()) {
            case value_type::Int:
                return bp::to_string(val_->as<int>());
            case value_type::Float:
                return bp::to_string(val_->as<float>());
            case value_type::Bool:
                return val_->as<bool>() ? "true" : "false";
            case value_type::String:
                return val_->as<std::string>();
            default:
                return "";
        }
    }

    template<>
    int32_t structure::as<int32_t>() const {
        return as_int<int32_t>();
    }
    template<>
    uint32_t structure::as<uint32_t>() const {
        return as_int<uint32_t>();
    }
    template<>
    int16_t structure::as<int16_t>() const {
        return as_int<int16_t>();
    }
    template<>
    uint16_t structure::as<uint16_t>() const {
        return as_int<uint16_t>();
    }
    template<>
    int8_t structure::as<int8_t>() const {
        return as_int<int8_t>();
    }
    template<>
    uint8_t structure::as<uint8_t>() const {
        return as_int<uint8_t>();
    }
#ifdef SPARK
    template<>
    int structure::as<int>() const {
        return as_int<int>();
    }
#endif

    template<>
    float structure::as<float>() const {
        switch (type()) {
            case value_type::Int:
                return val_->as<int>();
            case value_type::Float:
                return val_->as<float>();
            default:
                return 0;
        }
    }

    template<>
    bool structure::as<bool>() const {
        switch (type()) {
            case value_type::Int:
                return val_->as<int>() != 0;
            case value_type::Bool:
                return val_->as<bool>();
            case value_type::String: {
                auto s = val_->as<std::string>();
                return !(s == "false" || s == "0" || s.empty());
            }
            default:
                return false;
        }
    }
}