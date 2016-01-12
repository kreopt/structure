#include "dcm_buf.hpp"

std::string serializers::dcm_buf::as_string() const {
    switch (type()) {
        case value_type::Int:
            return std::to_string(boost::get<serializable::int_t>(*boost::get<val_t>(*val_)));
        case value_type::Float:
            return std::to_string(boost::get<serializable::float_t>(*boost::get<val_t>(*val_)));
        case value_type::Bool:
            return std::to_string(boost::get<serializable::bool_t>(*boost::get<val_t>(*val_)));
        case value_type::String:
            return boost::get<std::string>(*boost::get<val_t>(*val_));
        default:
            return "";
    }
}

serializers::serializable::int_t serializers::dcm_buf::as_int() const {
    switch (type()) {
        case value_type::Int:
        case value_type::Float:
        case value_type::Bool:
            return boost::get<serializable::int_t>(*boost::get<val_t>(*val_));
        default:
            return 0;
    }
}

serializers::serializable::float_t serializers::dcm_buf::as_float() const {
    switch (type()) {
        case value_type::Int:
        case value_type::Float:
            return boost::get<serializable::float_t>(*boost::get<val_t>(*val_));
        default:
            return 0;
    }
}

bool serializers::dcm_buf::as_bool() const {
    switch (type()) {
        case value_type::Int:
            return boost::get<serializable::int_t>(*boost::get<val_t>(*val_)) != 0;
        case value_type::Bool:
            return boost::get<serializable::bool_t>(*boost::get<val_t>(*val_));
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
        return boost::get<arr_t>(*val_)->push_back(std::make_shared<_var_t>(std::make_shared<value>(_val)));
    } else {
        throw std::range_error("not an array");
    }
}

void serializers::dcm_buf::append(serializers::value &&_val) {
    if (type() == value_type::Array) {
        return boost::get<arr_t>(*val_)->emplace_back(std::make_shared<_var_t>(std::make_shared<value>(_val)));
    } else {
        throw std::range_error("not an array");
    }
}

serializers::serializer::ptr serializers::dcm_buf::at(const char *_key) {
    if (type() == value_type::Object) {
        return create((*boost::get<obj_t>(*val_))[bp::symbol(_key)]);
    } else {
        throw std::range_error("not an object");
    }
}

const serializers::serializer::ptr serializers::dcm_buf::at(const char *_key) const {
    if (type() == value_type::Object) {
        return create((*boost::get<obj_t>(*val_))[bp::symbol(_key)]);
    } else {
        throw std::range_error("not an object");
    }
}

bool serializers::dcm_buf::emplace(const std::string &_key, const serializers::value &_val) {
    if (type() == value_type::Object) {
        return boost::get<obj_t >(*val_)->emplace(bp::symbol(_key), std::make_shared<_var_t>(std::make_shared<value>(_val))).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool serializers::dcm_buf::emplace(const std::string &_key, serializers::value &&_val) {
    if (type() == value_type::Object) {
        return boost::get<obj_t >(*val_)->emplace(bp::symbol(_key), std::make_shared<_var_t>(std::make_shared<value>(_val))).second;
    } else {
        throw std::range_error("not an object");
    }
}

serializers::serializer::ptr serializers::dcm_buf::get(const std::string &_key,
                                                       const serializers::value &_default) const {
    if (type() == value_type::Object) {
        try {
            auto val = boost::get<obj_t>(*val_)->at(bp::symbol(_key));
            return create(val);
        } catch (std::out_of_range &e) {
            return create(std::make_shared<_var_t>(std::make_shared<value>(_default)));
        }
    } else {
        throw std::range_error("not an object");
    }
}

serializers::serializer::ptr serializers::dcm_buf::get(const std::string &_key, serializers::value &&_default) const {
    if (type() == value_type::Object) {
        try {
            auto val = boost::get<obj_t>(*val_)->at(bp::symbol(_key));
            return create(val);
        } catch (std::out_of_range &e) {
            return create(std::make_shared<_var_t>(std::make_shared<value>(_default)));
        }
    } else {
        throw std::range_error("not an object");
    }
}

serializers::serializer::ptr serializers::dcm_buf::set(serializers::value &&_val) {
    *val_ = std::make_shared<value>(_val);
    set_type(boost::apply_visitor(value_type_visitor(),_val));
    return shared_from_this();
}

serializers::serializer::ptr serializers::dcm_buf::set(const serializers::value &_val) {
    *val_ = std::make_shared<value>(_val);
    set_type(boost::apply_visitor(value_type_visitor(),_val));
    return shared_from_this();
}

serializers::serializer::ptr serializers::dcm_buf::set(const std::initializer_list<value> &_val) {
    *val_ = arr_t();
    // TODO: iterate over initializer list
    set_type(value_type::Array);
    return shared_from_this();
}

serializers::serializer::ptr serializers::dcm_buf::set(
        const std::initializer_list<std::pair<std::string, value>> &_val) {
    *val_ = obj_t();
    // TODO: iterate over initializer list
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

serializers::serializer::ptr serializers::dcm_buf::create(serializers::dcm_buf::var_t _obj) {
    return std::shared_ptr<dcm_buf>(new dcm_buf(_obj));
}

serializers::serializer::ptr serializers::dcm_buf::create() {
    obj_t obj;
    return create(std::make_shared<_var_t>(obj));
}

serializers::dcm_buf::dcm_buf(serializers::dcm_buf::var_t _obj) {
    val_ = _obj;
}
