#include "dcm_buf.hpp"

std::string bp::serializers::dcm_buf::as_string() const {
    switch (type()) {
        case value_type::Int:     return std::to_string(get_value<serializable::int_t>());
        case value_type::Float:   return std::to_string(get_value<serializable::float_t>());
        case value_type::Bool:    return std::to_string(get_value<serializable::bool_t>());
        case value_type::CString: return get_value<serializable::c_str_t>();
        case value_type::String:  return get_value<std::string>();
        default: return "";
    }
}

bp::serializable::int_t bp::serializers::dcm_buf::as_int() const {
    switch (type()) {
        case value_type::Int:
        case value_type::Float:
        case value_type::Bool:
            return get_value<serializable::int_t>();
        default:
            return 0;
    }
}

bp::serializable::float_t bp::serializers::dcm_buf::as_float() const {
    switch (type()) {
        case value_type::Int:
        case value_type::Float:
            return get_value<serializable::float_t>();
        default:
            return 0;
    }
}

bool bp::serializers::dcm_buf::as_bool() const {
    switch (type()) {
        case value_type::Int:
            return get_value<serializable::int_t>() != 0;
        case value_type::Bool:
            return get_value<serializable::bool_t>();
        default:
            return false;
    }
}

size_t bp::serializers::dcm_buf::size() const {
    return type() == value_type::Array ? get_variant<arr_t>()->size() : 0;
}

bp::serializer::ptr bp::serializers::dcm_buf::at(int index) {
    if (type() == value_type::Array) {
        return create((*get_variant<arr_t>())[index]);
    } else {
        throw std::range_error("not an array");
    }
}

const bp::serializer::ptr bp::serializers::dcm_buf::at(int index) const {
    if (type() == value_type::Array) {
        return create((*get_variant<arr_t>())[index]);
    } else {
        throw std::range_error("not an array");
    }
}

void bp::serializers::dcm_buf::append(const bp::serializer::value &_val) {
    initialize_if_null(value_type::Array);
    if (type() == value_type::Array) {
        return get_variant<arr_t>()->push_back(std::make_shared<_var_t>(_val));
    } else {
        throw std::range_error("not an array");
    }
}

void bp::serializers::dcm_buf::append(bp::serializer::value &&_val) {
    initialize_if_null(value_type::Array);
    if (type() == value_type::Array) {
        return get_variant<arr_t>()->emplace_back(std::make_shared<_var_t>(_val));
    } else {
        throw std::range_error("not an array");
    }
}

bool bp::serializers::dcm_buf::append(const std::initializer_list<value> &_val) {
    initialize_if_null(value_type::Array);
    if (type() == value_type::Array) {
        // TODO: iterate over initializer list
        arr_t arr = std::make_shared<_arr_t>();
        get_variant<arr_t>()->push_back(std::make_shared<_var_t>(arr));
        return true;
    } else {
        throw std::range_error("not an array");
    }
}

bool bp::serializers::dcm_buf::append(const std::initializer_list<std::pair<std::string, value>> &_val) {
    initialize_if_null(value_type::Array);
    if (type() == value_type::Array) {
        // TODO: iterate over initializer list
        arr_t arr = std::make_shared<_arr_t>();
        get_variant<arr_t>()->push_back(std::make_shared<_var_t>(arr));
        return true;
    } else {
        throw std::range_error("not an array");
    }
}

bp::serializer::ptr bp::serializers::dcm_buf::at(const char *_key) {
    if (type() == value_type::Object) {
        return create(get_variant<arr_t>()->at(bp::symbol(_key)));
    } else {
        throw std::range_error("not an object");
    }
}

const bp::serializer::ptr bp::serializers::dcm_buf::at(const char *_key) const {
    if (type() == value_type::Object) {
        return create(get_variant<obj_t>()->at(bp::symbol(_key)));
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializers::dcm_buf::emplace(const std::string &_key, const bp::serializer::value &_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        return get_variant<obj_t>()->emplace(bp::symbol(_key), std::make_shared<_var_t>(_val)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializers::dcm_buf::emplace(const std::string &_key, bp::serializer::value &&_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        return get_variant<obj_t>()->emplace(bp::symbol(_key), std::make_shared<_var_t>(_val)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializers::dcm_buf::emplace(const std::string &_key, const std::initializer_list<value> &_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        arr_t obj = std::make_shared<_arr_t>();
        for (auto item: _val) {
            obj->push_back(std::make_shared<_var_t>(item));
        }
        return get_variant<obj_t>()->emplace(bp::symbol(_key), std::make_shared<_var_t>(obj)).second;
    } else {
        throw std::range_error("not an object");
    }
}

bool bp::serializers::dcm_buf::emplace(const std::string &_key,
                                   const std::initializer_list<std::pair<std::string, value>> &_val) {
    initialize_if_null(value_type::Object);
    if (type() == value_type::Object) {
        obj_t obj = std::make_shared<_obj_t>();
        for (auto item: _val) {
            obj->emplace(bp::symbol(item.first), std::make_shared<_var_t>(item.second));
        }
        return get_variant<obj_t>()->emplace(bp::symbol(_key), std::make_shared<_var_t>(obj)).second;
    } else {
        throw std::range_error("not an object");
    }
}


bp::serializer::ptr bp::serializers::dcm_buf::get(const std::string &_key,
                                                       const bp::serializer::value &_default) const {
    if (type() == value_type::Object) {
        try {
            auto val = get_variant<obj_t>()->at(bp::symbol(_key));
            return create(val);
        } catch (std::out_of_range &e) {
            return create(std::make_shared<_var_t>(_default));
        }
    } else {
        throw std::range_error("not an object");
    }
}

bp::serializer::ptr bp::serializers::dcm_buf::get(const std::string &_key, bp::serializer::value &&_default) const {
    if (type() == value_type::Object) {
        try {
            auto val = get_variant<obj_t>()->at(bp::symbol(_key));
            return create(val);
        } catch (std::out_of_range &e) {
            return create(std::make_shared<_var_t>(_default));
        }
    } else {
        throw std::range_error("not an object");
    }
}

bp::serializer::ptr bp::serializers::dcm_buf::set(bp::serializer::value &&_val) {
    initialize_if_null(value_type::Object);
    if (val_) {
        *val_ = std::make_shared<value>(_val);
    } else {
        val_ = std::make_shared<_var_t>(_val);
    }
    set_type(boost::apply_visitor(value_type_visitor(),_val));
    return shared_from_this();
}

bp::serializer::ptr bp::serializers::dcm_buf::set(const bp::serializer::value &_val) {
    if (val_) {
        *val_ = std::make_shared<value>(_val);
    } else {
        val_ = std::make_shared<_var_t>(_val);
    }
    set_type(boost::apply_visitor(value_type_visitor(),_val));
    return shared_from_this();
}

bp::serializer::ptr bp::serializers::dcm_buf::set(const std::initializer_list<value> &_val) {
    initialize_val<_arr_t>();
    for (auto item: _val) {
        this->append(item);
    }
    set_type(value_type::Array);
    return shared_from_this();
}

bp::serializer::ptr bp::serializers::dcm_buf::set(
        const std::initializer_list<std::pair<std::string, value>> &_val) {
    initialize_val<_obj_t>();

    for (auto item: _val) {
        this->emplace(item.first, item.second);
    }
    set_type(value_type::Object);
    return shared_from_this();
}

std::string bp::serializers::dcm_buf::stringify() const {
    // entry ::= <type>[len]<data>;
    // object_entry ::= <key><val_entry>

    std::string r;
    auto tp = type();
    r+=char(tp);
    size_block sz;
    switch (tp){
        case value_type::Object: {
            auto val = get_variant<obj_t>();
            sz = static_cast<size_block>(val->size());
            r.append(reinterpret_cast<char *>(&sz), sizeof(size_block));
            for (auto item: *val) {
                r.append(reinterpret_cast<const char *>(&item.first), sizeof(bp::symbol_t));
                r.append(create(item.second)->stringify());
            }
            break;
        }
        case value_type::Array: {
            sz = static_cast<size_block>(this->size());
            r.append(reinterpret_cast<char *>(&sz), sizeof(size_block));
            for (int i = 0; i < sz; i++) {
                r.append(this->at(i)->stringify());
            }
            break;
        }
        default: {
            if (tp == value_type::String || tp == value_type::Bytes) {
                auto val = get_value<std::string>();
                sz = static_cast<size_block>(val.size());
                r.append(reinterpret_cast<char *>(&sz), sizeof(size_block));
                r.append(val);
            } else if (tp == value_type::CString) {
                auto val = get_value<serializable::c_str_t>();
                sz = static_cast<size_block>(strlen(val));
                const char *s = reinterpret_cast<const char *>(&sz);
                r.append(s, sizeof(size_block));
                r.append(val, sz);
            } else if (tp == value_type::Int) {
                auto val = get_value<serializable::int_t>();
                r.append(reinterpret_cast<const char *>(&val), sizeof(serializable::int_t));
            } else if (tp == value_type::Float) {
                auto val = get_value<serializable::float_t>();
                r.append(reinterpret_cast<const char *>(&val), sizeof(serializable::float_t));
            } else if (tp == value_type::Bool) {
                auto val = get_value<serializable::bool_t>();
                r.append(val ? '\1' : '\0', 1);
            }
            break;
        }
    }
    return r;
}

void bp::serializers::dcm_buf::parse(const std::string &_str) {

}

bp::serializer::ptr bp::serializers::dcm_buf::create(bp::serializers::dcm_buf::var_t _obj) {
    return std::shared_ptr<dcm_buf>(new dcm_buf(_obj));
}

bp::serializer::ptr bp::serializers::dcm_buf::create() {
    val_t obj;
    return create(std::make_shared<_var_t>(obj));
}

bp::serializers::dcm_buf::dcm_buf(bp::serializers::dcm_buf::var_t _obj) : val_(_obj) {
    if (_obj) {
        set_type(boost::apply_visitor(variant_visitor(), *val_));
    } else {
        set_type(value_type::Null);
    }
}

void bp::serializers::dcm_buf::initialize_if_null(bp::serializer::value_type _type) {
    if (type()==value_type::Null) {
        switch (_type) {
            case value_type::Object:
                initialize_val<_obj_t>();
                break;
            case value_type::Array:
                initialize_val<_arr_t>();
                break;
            default:
                break;
        }
        set_type(_type);
    }
}
