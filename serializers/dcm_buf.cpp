#include "dcm_buf.hpp"

std::string serializers::dcm_buf::as_string() const {
    return std::string();
}

serializers::serializable::int_t serializers::dcm_buf::as_int() const {
    switch (type()) {
        case value_type::Int:
        case value_type::Float:
        case value_type::Bool:
            return boost::get<serializable::int_t>(val_);
        default:
            return 0;
    }
}

serializers::serializable::float_t serializers::dcm_buf::as_float() const {
    switch (type()) {
        case value_type::Int:
        case value_type::Float:
            return boost::get<serializable::float_t>(val_);
        default:
            return 0;
    }
}

bool serializers::dcm_buf::as_bool() const {
    switch (type()) {
        case value_type::Int:
            return boost::get<serializable::int_t>(val_) != 0;
        case value_type::Bool:
            return boost::get<serializable::bool_t>(val_);
        default:
            return false;
    }
}

size_t serializers::dcm_buf::size() const {
    return type() == value_type::Array ? boost::get<arr_t>(val_) : 0;
}

serializers::serializer::ptr serializers::dcm_buf::at(int index) {
    if (type() == value_type::Array) {
        return boost::get<arr_t>(val_)[index];
    } else {
        throw std::range_error("not an array");
    }
    return std::shared_ptr<serializer>();
}

const serializers::serializer::ptr serializers::dcm_buf::at(int index) const {
    return std::shared_ptr<serializer>();
}

void serializers::dcm_buf::append(const serializers::value &_val) {

}

void serializers::dcm_buf::append(serializers::value &&_val) {

}

serializers::serializer::ptr serializers::dcm_buf::at(const char *_key) {
    return std::shared_ptr<serializer>();
}

const serializers::serializer::ptr serializers::dcm_buf::at(const char *_key) const {
    return std::shared_ptr<serializer>();
}

bool serializers::dcm_buf::emplace(const std::string &_key, const serializers::value &_val) {
    return false;
}

bool serializers::dcm_buf::emplace(const std::string &_key, serializers::value &&_val) {
    return false;
}

serializers::serializer::ptr serializers::dcm_buf::get(const std::string &_key,
                                                       const serializers::value &_default) const {
    return std::shared_ptr<serializer>();
}

serializers::serializer::ptr serializers::dcm_buf::get(const std::string &_key, serializers::value &&_default) const {
    return std::shared_ptr<serializer>();
}

serializers::serializer::ptr serializers::dcm_buf::set(serializers::value &&_val) {
    return std::shared_ptr<serializer>();
}

serializers::serializer::ptr serializers::dcm_buf::set(const serializers::value &_val) {
    return std::shared_ptr<serializer>();
}

serializers::serializer::ptr serializers::dcm_buf::set(const std::initializer_list<value> &_val) {
    return std::shared_ptr<serializer>();
}

serializers::serializer::ptr serializers::dcm_buf::set(
        const std::initializer_list<std::pair<std::string, value>> &_val) {
    return std::shared_ptr<serializer>();
}

std::string serializers::dcm_buf::stringify() const {
    return __cxx11::basic_string < char, char_traits < _CharT >, allocator < _CharT >> ();
}

void serializers::dcm_buf::parse(const std::string &_str) {

}
