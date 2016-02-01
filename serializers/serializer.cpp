#include "serializer.hpp"

bp::serializer::serializer(bp::serializer::variant_ptr _obj) : val_(_obj) {
    if (_obj) {
        set_type(boost::apply_visitor(variant_visitor(), *val_));
    } else {
        set_type(value_type::Null);
    }
}

bp::serializer::ptr bp::serializer::create(bp::serializer::variant_ptr _obj){
    return std::shared_ptr<serializer>(new serializer(_obj));
}

bp::serializable::string_t bp::serializer::as_string() const {
    switch (type()) {
        case value_type::Int:     return std::to_string(get_value<serializable::int_t>(val_));
        case value_type::Float:   return std::to_string(get_value<serializable::float_t>(val_));
        case value_type::Bool:    return std::to_string(get_value<serializable::bool_t>(val_));
        case value_type::String:  return get_value<std::string>(val_);
        case value_type::Symbol:  return std::string(get_value<serializable::symbol>(val_).name);
        default: return "";
    }
}

bp::serializable::int_t bp::serializer::as_int() const {
    switch (type()) {
        case value_type::Int:   return get_value<serializable::int_t>(val_);
        case value_type::Float: return static_cast<serializable::int_t>(get_value<serializable::float_t>(val_));
        case value_type::Bool:  return static_cast<serializable::int_t>(get_value<serializable::bool_t>(val_));
        default:
            return 0;
    }
}

bp::serializable::float_t bp::serializer::as_float() const {
    switch (type()) {
        case value_type::Int:   return get_value<serializable::int_t>(val_);
        case value_type::Float: return get_value<serializable::float_t>(val_);
        default:
            return 0;
    }
}

bp::serializable::bool_t bp::serializer::as_bool() const {
    switch (type()) {
        case value_type::Int:  return get_value<serializable::int_t>(val_) != 0;
        case value_type::Bool: return get_value<serializable::bool_t>(val_);
        default:
            return false;
    }
}

bp::serializable::symbol bp::serializer::as_symbol() const {
    switch (type()) {
        case value_type::Symbol:
            return get_value<serializable::symbol>(val_);
        case value_type::String:
            return bp::symbol(get_value<serializable::string_t >(val_));
        default:
            return ""_sym;
    }
}

size_t bp::serializer::size() const {
    if (is_array()) return get_variant<array_ptr>(val_)->size();
    if (is_object()) return get_variant<object_ptr>(val_)->size();
    return 0;
}

bp::serializer::ptr bp::serializer::at(int index) {
    if (type() == value_type::Array) {
        return create((*get_variant<array_ptr>(val_))[index]);
    } else {
        throw std::range_error("not an array");
    }
}

const bp::serializer::ptr bp::serializer::at(int index) const {
    if (type() == value_type::Array) {
        return create((*get_variant<array_ptr>(val_))[index]);
    } else {
        throw std::range_error("not an array");
    }
}

void bp::serializer::append(const variant_t &_val) {
    initialize_if_null(value_type::Array);
    if (type() == value_type::Array) {
        return get_variant<array_ptr>(val_)->push_back(std::make_shared<variant_t>(_val));
    } else {
        throw std::range_error("not an array");
    }
}

void bp::serializer::append(variant_t &&_val) {
    initialize_if_null(value_type::Array);
    if (type() == value_type::Array) {
        return get_variant<array_ptr>(val_)->emplace_back(std::make_shared<variant_t>(_val));
    } else {
        throw std::range_error("not an array");
    }
}

bool bp::serializer::append(const std::initializer_list<variant_t> &_val) {
    initialize_if_null(value_type::Array);
    if (type() == value_type::Array) {
        array_ptr arr = std::make_shared<array_t>();
        for (auto item: _val) {
            arr->push_back(std::make_shared<variant_t>(item));
        }
        get_variant<array_ptr>(val_)->push_back(std::make_shared<variant_t>(arr));
        return true;
    } else {
        throw std::range_error("not an array");
    }
}

bool bp::serializer::append(const std::initializer_list<std::pair<std::string, variant_t>> &_val) {
    initialize_if_null(value_type::Array);
    if (type() == value_type::Array) {
        object_ptr obj = std::make_shared<object_t>();
        for (auto item: _val) {
            obj->emplace(bp::symbol(item.first), std::make_shared<variant_t>(item.second));
        }
        get_variant<array_ptr>(val_)->push_back(std::make_shared<variant_t>(obj));
        return true;
    } else {
        throw std::range_error("not an array");
    }
}

bp::serializer::ptr bp::serializer::at(const char *_key) {
    return at(bp::symbol(_key));
}

const bp::serializer::ptr bp::serializer::at(const char *_key) const {
    return at(bp::symbol(_key));
}

bp::serializer::ptr bp::serializer::at(const std::string &_key) {
    return at(bp::symbol(_key));
}

const bp::serializer::ptr bp::serializer::at(const std::string &_key) const {
    return at(bp::symbol(_key));
}

bp::serializer::ptr bp::serializer::at(const bp::symbol &_key) {
    if (type() == value_type::Object) {
        return create(get_variant<object_ptr>(val_)->at(_key));
    } else {
        throw std::range_error("not an object");
    }
}

const bp::serializer::ptr bp::serializer::at(const bp::symbol &_key) const {
    if (type() == value_type::Object) {
        return create(get_variant<object_ptr>(val_)->at(_key));
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializer::emplace(const bp::symbol &_key, const bp::serializer::variant_t &_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        return get_variant<object_ptr>(val_)->emplace(_key, std::make_shared<variant_t>(_val)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializer::emplace(const bp::symbol &_key, bp::serializer::variant_t &&_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        return get_variant<object_ptr>(val_)->emplace(_key, std::make_shared<variant_t>(_val)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializer::emplace(const bp::symbol &_key, const std::initializer_list<variant_t> &_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        array_ptr obj = std::make_shared<array_t>();
        for (auto item: _val) {
            obj->push_back(std::make_shared<variant_t>(item));
        }
        return get_variant<object_ptr>(val_)->emplace(_key, std::make_shared<variant_t>(obj)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializer::emplace(const bp::symbol &_key,
                             const std::initializer_list<std::pair<bp::symbol, variant_t>> &_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        object_ptr obj = std::make_shared<object_t>();
        for (auto item: _val) {
            obj->emplace(item.first, std::make_shared<variant_t>(item.second));
        }
        return get_variant<object_ptr>(val_)->emplace(_key, std::make_shared<variant_t>(obj)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializer::emplace(bp::symbol &&_key, const bp::serializer::variant_t &_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        return get_variant<object_ptr>(val_)->emplace(_key, std::make_shared<variant_t>(_val)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializer::emplace(bp::symbol &&_key, bp::serializer::variant_t &&_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        return get_variant<object_ptr>(val_)->emplace(_key, std::make_shared<variant_t>(_val)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializer::emplace(bp::symbol &&_key, const std::initializer_list<variant_t> &_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        array_ptr obj = std::make_shared<array_t>();
        for (auto item: _val) {
            obj->push_back(std::make_shared<variant_t>(item));
        }
        return get_variant<object_ptr>(val_)->emplace(_key, std::make_shared<variant_t>(obj)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializer::emplace(bp::symbol &&_key,
                             const std::initializer_list<std::pair<bp::symbol, variant_t>> &_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        object_ptr obj = std::make_shared<object_t>();
        for (auto item: _val) {
            obj->emplace(item.first, std::make_shared<variant_t>(item.second));
        }
        return get_variant<object_ptr>(val_)->emplace(_key, std::make_shared<variant_t>(obj)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializer::emplace(const char *_key, const bp::serializer::variant_t &_val) {
    return emplace(bp::symbol(_key), _val);
}

bool bp::serializer::emplace(const char *_key, bp::serializer::variant_t &&_val) {
    return emplace(bp::symbol(_key), _val);
}

bool bp::serializer::emplace(const char *_key, const std::initializer_list<variant_t> &_val) {
    return emplace(bp::symbol(_key), _val);
}

bool bp::serializer::emplace(const char *_key,
                             const std::initializer_list<std::pair<bp::symbol, variant_t>> &_val) {
    return emplace(bp::symbol(_key), _val);
}

bool bp::serializer::emplace(const std::string &_key, const bp::serializer::variant_t &_val) {
    return emplace(bp::symbol(_key), _val);
}

bool bp::serializer::emplace(const std::string &_key, bp::serializer::variant_t &&_val) {
    return emplace(bp::symbol(_key), _val);
}

bool bp::serializer::emplace(const std::string &_key, const std::initializer_list<variant_t> &_val) {
    return emplace(bp::symbol(_key), _val);
}

bool bp::serializer::emplace(const std::string &_key,
                             const std::initializer_list<std::pair<bp::symbol, variant_t>> &_val) {
    return emplace(bp::symbol(_key), _val);
}

bp::serializer::ptr bp::serializer::get(const bp::symbol &_key,
                                        const bp::serializer::variant_t &_default) const {
    if (type() == value_type::Object) {
        try {
            auto val = get_variant<object_ptr>(val_)->at(_key);
            return create(val);
        } catch (std::out_of_range &e) {
            return create(std::make_shared<variant_t>(_default));
        }
    } else {
        throw std::range_error("not an object");
    }
}

bp::serializer::ptr bp::serializer::get(const bp::symbol &_key,
                                        bp::serializer::variant_t &&_default) const {
    if (type() == value_type::Object) {
        try {
            auto val = get_variant<object_ptr>(val_)->at(_key);
            return create(val);
        } catch (std::out_of_range &e) {
            return create(std::make_shared<variant_t>(_default));
        }
    } else {
        throw std::range_error("not an object");
    }
}

bp::serializer::ptr bp::serializer::get(const char *_key, const bp::serializer::variant_t &_default) const {
    return get(bp::symbol(_key), _default);
}

bp::serializer::ptr bp::serializer::get(const char *_key, bp::serializer::variant_t &&_default) const {
    return get(bp::symbol(_key), _default);
}

bp::serializer::ptr bp::serializer::get(const std::string &_key,
                                                  const bp::serializer::variant_t &_default) const {
    return get(bp::symbol(_key), _default);
}

bp::serializer::ptr bp::serializer::get(const std::string &_key, bp::serializer::variant_t &&_default) const {
    return get(bp::symbol(_key), _default);
}

bp::serializer::ptr bp::serializer::set(bp::serializer::variant_t &&_val) {
    initialize_if_null(value_type::Object);
    if (val_) {
        *val_ = _val;
    } else {
        val_ = std::make_shared<variant_t>(_val);
    }
    set_type(boost::apply_visitor(variant_visitor(), _val));
    return shared_from_this();
}

bp::serializer::ptr bp::serializer::set(const bp::serializer::variant_t &_val) {
    if (val_) {
        *val_ = _val;
    } else {
        val_ = std::make_shared<variant_t>(_val);
    }
    set_type(boost::apply_visitor(variant_visitor(),_val));
    return shared_from_this();
}

bp::serializer::ptr bp::serializer::set(const std::initializer_list<variant_t> &_val) {
    initialize_val<array_t>();
    for (auto item: _val) {
        this->append(item);
    }
    set_type(value_type::Array);
    return shared_from_this();
}

bp::serializer::ptr bp::serializer::set(
        const std::initializer_list<std::pair<std::string, variant_t>> &_val) {
    initialize_val<object_t>();

    for (auto item: _val) {
        this->emplace(item.first, item.second);
    }
    set_type(value_type::Object);
    return shared_from_this();
}

void bp::serializer::initialize_if_null(bp::serializer::value_type _type) {
    if (type()==value_type::Null) {
        switch (_type) {
            case value_type::Object:
                initialize_val<object_t>();
                break;
            case value_type::Array:
                initialize_val<array_t>();
                break;
            default:
                break;
        }
        set_type(_type);
    }
}