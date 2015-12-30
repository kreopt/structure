#include "dcm_buf.hpp"

std::string serializers::dcm_buf::as_string() const {
    switch (type()) {
        case value_type::Int:
            return std::to_string(boost::get<serializable::int_t>(*val_));
        case value_type::Float:
            return std::to_string(boost::get<serializable::float_t>(*val_));
        case value_type::Bool:
            return std::to_string(boost::get<serializable::bool_t>(*val_));
        case value_type::String:
            return boost::get<std::string>(*val_);
        default:
            return "";
    }
}

serializers::serializable::int_t serializers::dcm_buf::as_int() const {
    switch (type()) {
        case value_type::Int:
        case value_type::Float:
        case value_type::Bool:
            return boost::get<serializable::int_t>(*val_);
        default:
            return 0;
    }
}

serializers::serializable::float_t serializers::dcm_buf::as_float() const {
    switch (type()) {
        case value_type::Int:
        case value_type::Float:
            return boost::get<serializable::float_t>(*val_);
        default:
            return 0;
    }
}

bool serializers::dcm_buf::as_bool() const {
    switch (type()) {
        case value_type::Int:
            return boost::get<serializable::int_t>(*val_) != 0;
        case value_type::Bool:
            return boost::get<serializable::bool_t>(*val_);
        default:
            return false;
    }
}

size_t serializers::dcm_buf::size() const {
    return type() == value_type::Array ? boost::get<arr_t>(*val_)->size() : 0;
}

serializers::serializer::ptr serializers::dcm_buf::at(int index) {
    if (type() == value_type::Array) {
        return create((*boost::get<arr_t>(*val_))[index]);
    } else {
        throw std::range_error("not an array");
    }
}

const serializers::serializer::ptr serializers::dcm_buf::at(int index) const {
    if (type() == value_type::Array) {
        return create((*boost::get<arr_t>(*val_))[index]);
    } else {
        throw std::range_error("not an array");
    }
}

void serializers::dcm_buf::append(const serializers::value &_val) {
    if (type() == value_type::Array) {
        return boost::get<arr_t>(*val_)->push_back(std::make_shared<value>(_val));
    } else {
        throw std::range_error("not an array");
    }
}

void serializers::dcm_buf::append(serializers::value &&_val) {
    if (type() == value_type::Array) {
        return boost::get<arr_t>(*val_)->emplace_back(std::make_shared<value>(_val));
    } else {
        throw std::range_error("not an array");
    }
}

serializers::serializer::ptr serializers::dcm_buf::at(const char *_key) {
    if (type() == value_type::Object) {
        return create((*boost::get<obj_t>(*val_))[_key]);
    } else {
        throw std::range_error("not an object");
    }
}

const serializers::serializer::ptr serializers::dcm_buf::at(const char *_key) const {
    if (type() == value_type::Object) {
        return create((*boost::get<obj_t>(*val_))[interproc::symbol(_key)]);
    } else {
        throw std::range_error("not an object");
    }
}

bool serializers::dcm_buf::emplace(const std::string &_key, const serializers::value &_val) {
    if (type() == value_type::Object) {
        return boost::get<obj_t >(*val_)->emplace(interproc::symbol(_key), std::make_shared<value>(_val)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool serializers::dcm_buf::emplace(const std::string &_key, serializers::value &&_val) {
    if (type() == value_type::Object) {
        return boost::get<obj_t >(*val_)->emplace(interproc::symbol(_key), std::make_shared<value>(_val)).second;
    } else {
        throw std::range_error("not an object");
    }
}

serializers::serializer::ptr serializers::dcm_buf::get(const std::string &_key,
                                                       const serializers::value &_default) const {
    if (type() == value_type::Object) {
        try {
            auto val = boost::get<obj_t>(*val_)->at(interproc::symbol(_key));
            return create(val);
        } catch (std::out_of_range &e) {
            return create(std::make_shared<value>(_default));
        }
    } else {
        throw std::range_error("not an object");
    }
}

serializers::serializer::ptr serializers::dcm_buf::get(const std::string &_key, serializers::value &&_default) const {
    if (type() == value_type::Object) {
        try {
            auto val = boost::get<obj_t>(*val_)->at(interproc::symbol(_key));
            return create(val);
        } catch (std::out_of_range &e) {
            return create(std::make_shared<value>(_default));
        }
    } else {
        throw std::range_error("not an object");
    }
}

serializers::serializer::ptr serializers::dcm_buf::set(serializers::value &&_val) {
    *val_ = _val;
    set_type(value_type_visitor(_val));
    return shared_from_this();
}

serializers::serializer::ptr serializers::dcm_buf::set(const serializers::value &_val) {
    *val_ = _val;
    set_type(value_type_visitor(_val));
    return shared_from_this();
}

serializers::serializer::ptr serializers::dcm_buf::set(const std::initializer_list<value> &_val) {
    *val_ = _val;
    set_type(value_type::Array);
    return shared_from_this();
}

serializers::serializer::ptr serializers::dcm_buf::set(
        const std::initializer_list<std::pair<std::string, value>> &_val) {
    *val_ = _val;
    set_type(value_type::Object);
    return shared_from_this();
}

std::string serializers::dcm_buf::stringify() const {
    // entry ::= <type>[len]<data>;
    // object_entry ::= <key><val_entry>
    return "";
}

void serializers::dcm_buf::parse(const std::string &_str) {

}
