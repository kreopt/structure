#include "structure.hpp"

bp::structure::structure(bp::structure::variant_ptr _obj) : val_(_obj) {
    if (_obj) {
        set_type(boost::apply_visitor(variant_visitor(), *val_));
    } else {
        initialize_if_null(value_type::Null);
        set_type(value_type::Null);
    }
}

bp::serializable::string_t bp::structure::as_string() const {
    switch (type()) {
        case value_type::Int:     return std::to_string(get_value<serializable::int_t>(val_));
        case value_type::Float:   return std::to_string(get_value<serializable::float_t>(val_));
        case value_type::Bool:    return get_value<serializable::bool_t>(val_)?"true":"false";
        case value_type::String:  return get_value<std::string>(val_);
        case value_type::Symbol:  return get_value<serializable::symbol>(val_).name();
        default: return "";
    }
}

bp::serializable::int_t bp::structure::as_int() const {
    switch (type()) {
        case value_type::Int:   return get_value<serializable::int_t>(val_);
        case value_type::Float: return static_cast<serializable::int_t>(get_value<serializable::float_t>(val_));
        case value_type::Bool:  return static_cast<serializable::int_t>(get_value<serializable::bool_t>(val_));
        default:
            return 0;
    }
}

bp::serializable::float_t bp::structure::as_float() const {
    switch (type()) {
        case value_type::Int:   return get_value<serializable::int_t>(val_);
        case value_type::Float: return get_value<serializable::float_t>(val_);
        default:
            return 0;
    }
}

bp::serializable::bool_t bp::structure::as_bool() const {
    switch (type()) {
        case value_type::Int:  return get_value<serializable::int_t>(val_) != 0;
        case value_type::Bool: return get_value<serializable::bool_t>(val_);
        default:
            return false;
    }
}

bp::serializable::symbol bp::structure::as_symbol() const {
    switch (type()) {
        case value_type::Symbol:
            return get_value<serializable::symbol>(val_);
        case value_type::String:
            return bp::symbol(get_value<serializable::string_t >(val_));
        default:
            return ""_sym;
    }
}

size_t bp::structure::size() const {
    if (is_array()) return get_variant<array_ptr>(val_)->size();
    if (is_object()) return get_variant<object_ptr>(val_)->size();
    return 0;
}

bool bp::structure::append(const std::initializer_list<variant_t> &_val) {
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

bool bp::structure::append(const std::initializer_list<std::pair<std::string, variant_t>> &_val) {
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

void bp::structure::initialize_if_null(bp::structure::value_type _type) {
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

bp::structure bp::structure::operator[] (int index) {
    if (type() == value_type::Array) {
        return bp::structure((*get_variant<array_ptr>(val_))[index]);
    } else {
        throw std::range_error("not an array");
    }
}

bp::structure bp::structure::operator[](const bp::symbol &_key) {
    if (type() != value_type::Object ) {
        emplace(_key, nullptr);
    }
    auto var = get_variant<object_ptr>(val_);

    if (!var->count(_key)) {
        var->emplace(_key, nullptr);
    }
    return bp::structure(var->at(_key));
}

void bp::structure::erase(const bp::symbol &_key) {
    if (type() == value_type::Object) {
         get_variant<object_ptr>(val_)->erase(_key);
    } else {
        throw std::range_error("not an object");
    }
}

bp::structure bp::structure::at(int index) const {
    if (type() == value_type::Array) {
        return bp::structure((*get_variant<array_ptr>(val_))[index]);
    } else {
        throw std::range_error("not an array");
    }
}

bp::structure bp::structure::at(const symbol &_key) const {
    if (type() == value_type::Object) {
        return bp::structure(get_variant<object_ptr>(val_)->at(_key));
    } else {
        throw std::range_error("not an object");
    }
}

bp::structure &bp::structure::operator=(const std::initializer_list<variant_t> &_val) {
    initialize_val<array_t>();
    for (auto item: _val) {
        this->append(item);
    }
    set_type(value_type::Array);
    return *this;
}

bp::structure &bp::structure::operator=(const std::initializer_list<std::pair<std::string, variant_t>> &_val) {
    initialize_val<object_t>();

    for (auto item: _val) {
        this->emplace(item.first, item.second);
    }
    set_type(value_type::Object);
    return *this;
}

bp::structure &bp::structure::operator=(const std::initializer_list<std::pair<std::string, structure>> &_val) {
    initialize_val<object_t>();

    for (auto item: _val) {
        this->emplace(item.first, *item.second.data());
    }
    set_type(value_type::Object);
    return *this;
}

bp::structure &bp::structure::operator=(const structure::ptr &_str) {
    operator=(*_str);
    return *this;
}

bp::structure &bp::structure::operator=(structure::ptr &&_str) {
    operator=(*_str);
    _str.reset();
    return *this;
}

bp::structure &bp::structure::operator=(const structure &_str) {
    if (&_str==this) return *this;
    this->value_type_ = _str.value_type_;
    this->val_ = clone_variant(_str.val_);
    return *this;
}

bp::structure &bp::structure::operator=(structure &&_str) {
    this->val_ =std::move(_str.val_);
    this->value_type_ = std::move(_str.value_type_);
    _str.val_ = nullptr;
    _str.value_type_ = value_type::Null;
    return *this;
}

bool bp::structure::emplace(const std::initializer_list<std::pair<bp::symbol, variant_t>> &_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        for (auto &entry: _val) {
            emplace(entry.first, entry.second);
        }
        return true; //
    } else {
        throw std::range_error("not an object");
    }
}

//bool bp::structure::emplace(const std::initializer_list<std::pair<bp::symbol, structure>> &_val) {
//    initialize_if_null(value_type::Object);
//    if (type() == value_type::Object) {
//        for (auto &entry: _val) {
//            emplace(entry.first, *entry.second.data());
//        }
//        return true; //
//    } else {
//        throw std::range_error("not an object");
//    }
//}

bool bp::structure::emplace(symbol &&_key, const bp::structure & _str) {
    if (_str) {
        return emplace(std::forward<symbol>(_key), *_str.data());
    } else  {
        return emplace(std::forward<symbol>(_key), nullptr);
    }
}