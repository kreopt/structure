#include "structure.hpp"

using namespace bp::serializable;

struct value_type_visitor {
    using result_type = bp::structure::value_type;
    bp::structure::value_type operator()(bp::serializable::int_t _val) const { return bp::structure::value_type::Int; }
    bp::structure::value_type operator()(bp::serializable::int_t &&_val) const { return bp::structure::value_type::Int; }
    bp::structure::value_type operator()(bp::serializable::float_t _val) const { return bp::structure::value_type::Float; }
    bp::structure::value_type operator()(bp::serializable::float_t &&_val) const { return bp::structure::value_type::Float; }
    bp::structure::value_type operator()(bp::serializable::bool_t _val) const { return bp::structure::value_type::Bool; }
    bp::structure::value_type operator()(bp::serializable::bool_t &&_val) const { return bp::structure::value_type::Bool; }
    bp::structure::value_type operator()(const bp::serializable::symbol &_val) const { return bp::structure::value_type::Symbol; }
    bp::structure::value_type operator()(bp::serializable::symbol &&_val) const { return bp::structure::value_type::Symbol; }
    bp::structure::value_type operator()(const bp::serializable::string_t & _val) const { return bp::structure::value_type::String; }
    bp::structure::value_type operator()(bp::serializable::string_t && _val) const { return bp::structure::value_type::String; }
};

struct variant_visitor {
    using result_type = bp::structure::value_type;

    bp::structure::value_type operator()(value_ptr _val) const {
        if (_val) {
            return bp::visit(value_type_visitor(), *_val);
        } else {
            return bp::structure::value_type::Null;
        }
    }

    bp::structure::value_type operator()(object_ptr _val) const { return bp::structure::value_type::Object; }

    bp::structure::value_type operator()(array_ptr _val) const { return bp::structure::value_type::Array; }
};

variant_ptr clone_variant(const variant_ptr &_ptr) {
    auto type = bp::visit(variant_visitor(), *_ptr);
    variant_ptr res;
    switch (type) {
        case bp::structure::value_type::Object: {
            auto obj = std::make_shared<bp::serializable::object>();
            for (auto entry: *bp::serializable::get_variant<bp::serializable::object_ptr>(_ptr)) {
                obj->emplace(entry.first, clone_variant(entry.second));
            }
            res = std::make_shared<bp::serializable::tree>(obj);
            break;
        }
        case bp::structure::value_type::Array: {
            auto arr = std::make_shared<bp::serializable::array>();
            for (auto entry: *bp::serializable::get_variant<bp::serializable::array_ptr>(_ptr)) {
                arr->emplace_back(clone_variant(entry));
            }
            res = std::make_shared<bp::serializable::tree>(arr);

            break;
        }
        default: {
            res = std::make_shared<bp::serializable::tree>(*_ptr);
        }
    }
    return res;
}

bp::structure::structure(bp::serializable::variant_ptr _obj) : value_type_(value_type::Null), val_(_obj) {
    if (_obj) {
        set_type(bp::visit(variant_visitor(), *val_));
    } else {
        initialize_if_null(value_type::Null);
        set_type(value_type::Null);
    }
}

bp::serializable::string_t bp::structure::as_string() const {
    switch (type()) {
        case value_type::Int:     return std::to_string(serializable::get_value<serializable::int_t>(val_));
        case value_type::Float:   return std::to_string(serializable::get_value<serializable::float_t>(val_));
        case value_type::Bool:    return serializable::get_value<serializable::bool_t>(val_)?"true":"false";
        case value_type::String:  return serializable::get_value<std::string>(val_);
        case value_type::Symbol:  return serializable::get_value<serializable::symbol>(val_).name();
        default: return "";
    }
}

bp::serializable::int_t bp::structure::as_int() const {
    switch (type()) {
        case value_type::Int:   return serializable::get_value<serializable::int_t>(val_);
        case value_type::Float: return static_cast<serializable::int_t>(serializable::get_value<serializable::float_t>(val_));
        case value_type::Bool:  return static_cast<serializable::int_t>(serializable::get_value<serializable::bool_t>(val_));
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

bool bp::structure::append(const std::initializer_list<bp::serializable::tree> &_val) {
    initialize_if_null(value_type::Array);
    if (type() == value_type::Array) {
        array_ptr arr = std::make_shared<array>();
        for (auto item: _val) {
            arr->push_back(std::make_shared<bp::serializable::tree>(item));
        }
        get_variant<array_ptr>(val_)->push_back(std::make_shared<bp::serializable::tree>(arr));
        return true;
    } else {
        throw std::range_error("not an array");
    }
}

bool bp::structure::append(const std::initializer_list<std::pair<std::string, bp::serializable::tree>> &_val) {
    initialize_if_null(value_type::Array);
    if (type() == value_type::Array) {
        object_ptr obj = std::make_shared<object>();
        for (auto item: _val) {
            obj->emplace(bp::symbol(item.first), std::make_shared<bp::serializable::tree>(item.second));
        }
        get_variant<array_ptr>(val_)->push_back(std::make_shared<bp::serializable::tree>(obj));
        return true;
    } else {
        throw std::range_error("not an array");
    }
}

void bp::structure::initialize_if_null(bp::structure::value_type _type) {
    if (type()==value_type::Null) {
        switch (_type) {
            case value_type::Object:
                initialize_val<object>();
                break;
            case value_type::Array:
                initialize_val<array>();
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

bp::structure &bp::structure::operator=(const std::initializer_list<bp::serializable::tree> &_val) {
    initialize_val<array>();
    set_type(value_type::Array);
    for (auto item: _val) {
        this->append(item);
    }
    return *this;
}

bp::structure &bp::structure::operator=(const std::initializer_list<std::pair<std::string, bp::serializable::tree>> &_val) {
    initialize_val<object>();
    set_type(value_type::Object);
    for (auto item: _val) {
        this->emplace(item.first, item.second);
    }
    return *this;
}

bp::structure &bp::structure::operator=(const std::initializer_list<std::pair<std::string, structure>> &_val) {
    initialize_val<object>();

    for (auto item: _val) {
        this->emplace(item.first, *item.second.data());
    }
    set_type(value_type::Object);
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

bool bp::structure::emplace(const std::initializer_list<std::pair<bp::symbol, bp::serializable::tree>> &_val) {
    emplace_init();
    for (auto &entry: _val) {
        emplace(entry.first, entry.second);
    }
    return true; //
}

bool bp::structure::emplace(const std::initializer_list<std::pair<bp::symbol, structure>> &_val) {
    emplace_init();
    for (auto &entry: _val) {
        emplace(entry.first, *entry.second.data());
    }
    return true;
}

bool bp::structure::emplace(symbol &&_key, const bp::structure & _str) {
    if (_str) {
        return emplace(std::forward<symbol>(_key), *_str.data());
    } else  {
        return emplace(std::forward<symbol>(_key), nullptr);
    }
}

bp::structure::value_type bp::structure::get_variant_type(const bp::serializable::tree &_var) const {
    return bp::visit(variant_visitor(), _var);
}

bp::structure::structure(const structure& _s) : value_type_(_s.value_type_), val_(_s.val_) {

}

bp::structure::structure(structure &&_s) : value_type_(std::move(_s.value_type_)), val_(std::move(_s.val_)) {
    _s.value_type_ = value_type::Null;
    _s.val_ = nullptr;
}

bp::structure bp::structure::deepcopy() const {
    if (val_) {
        return structure(clone_variant(val_));
    }
    return structure();
}