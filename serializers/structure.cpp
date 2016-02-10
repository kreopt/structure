#include "structure.hpp"

bp::structure::structure(bp::structure::variant_ptr _obj) : val_(_obj) {
    if (_obj) {
        set_type(boost::apply_visitor(variant_visitor(), *val_));
    } else {
        set_type(value_type::Null);
    }
}

bp::structure::ptr bp::structure::create(bp::structure::variant_ptr _obj){
    return std::shared_ptr<structure>(new structure(_obj));
}

bp::serializable::string_t bp::structure::as_string() const {
    switch (type()) {
        case value_type::Int:     return std::to_string(get_value<serializable::int_t>(val_));
        case value_type::Float:   return std::to_string(get_value<serializable::float_t>(val_));
        case value_type::Bool:    return std::to_string(get_value<serializable::bool_t>(val_));
        case value_type::String:  return get_value<std::string>(val_);
        case value_type::Symbol:  return std::string(get_value<serializable::symbol>(val_).name);
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

bp::structure::ptr bp::structure::operator[](int index) {
    if (type() == value_type::Array) {
        return create((*get_variant<array_ptr>(val_))[index]);
    } else {
        throw std::range_error("not an array");
    }
}

const bp::structure::ptr bp::structure::operator[](int index) const {
    return static_cast<const structure*>(this)->operator[](index);
}

bp::structure::ptr bp::structure::operator[](const bp::symbol &_key) {
    if (type() == value_type::Object) {
        return create(get_variant<object_ptr>(val_)->at(_key));
    } else {
        throw std::range_error("not an object");
    }
}

const bp::structure::ptr bp::structure::operator[](const bp::symbol &_key) const {
    return static_cast<const structure*>(this)->operator[](_key);
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