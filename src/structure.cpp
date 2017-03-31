#include "structure.hpp"

using namespace bp::serializable;

struct variant_visitor {
    using result_type = bp::structure::value_type;

    bp::structure::value_type operator()(value_ptr _val) const {
        if (_val) {
            return bp::visit(bp::value_type_visitor(), *_val);
        } else {
            return bp::structure::value_type::Null;
        }
    }

    bp::structure::value_type operator()(object_ptr) const { return bp::structure::value_type::Object; }

    bp::structure::value_type operator()(array_ptr) const { return bp::structure::value_type::Array; }
};

tree_ptr clone_variant(const tree_ptr &_ptr) {
    auto type = bp::visit(variant_visitor(), *_ptr);
    tree_ptr res;
    switch (type) {
        case bp::structure::value_type::Object: {
            auto obj = std::make_shared<bp::serializable::object>();
            for (const auto &entry: *_ptr->as_object()) {
                obj->emplace(entry.first, clone_variant(entry.second));
            }
            res = std::make_shared<bp::serializable::tree>(obj);
            break;
        }
        case bp::structure::value_type::Array: {
            auto arr = std::make_shared<bp::serializable::array>();
            for (const auto &entry: *_ptr->as_array()) {
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

//bp::structure::structure(bp::serializable::tree _obj) :
//        value_type_(bp::visit(variant_visitor(), _obj)),
//        val_(std::make_shared<bp::serializable::tree>(_obj)) {
//}

bp::structure::structure(bp::serializable::tree_ptr _obj) : value_type_(value_type::Null), val_(_obj) {
    if (_obj) {
        value_type_ = bp::visit(variant_visitor(), *val_);
    } else {
        initialize_if_null(value_type::Null);
        value_type_ = value_type::Null;
    }
}
//
//bp::serializable::string_t bp::structure::as_string() const {
//    switch (type()) {
//        case value_type::Int:
//            return bp::to_string(val_->as<serializable::int_t>());
//        case value_type::Float:
//            return bp::to_string(val_->as<serializable::float_t>());
//        case value_type::Bool:
//            return val_->as<serializable::bool_t>() ? "true" : "false";
//        case value_type::String:
//            return val_->as<std::string>();
//        default:
//            return "";
//    }
//}
//
//bp::serializable::int_t bp::structure::as_int() const {
//    switch (type()) {
//        case value_type::Int:
//            return val_->as<serializable::int_t>();
//        case value_type::Float:
//            return static_cast<serializable::int_t>(val_->as<serializable::float_t>());
//        case value_type::Bool:
//            return static_cast<serializable::int_t>(val_->as<serializable::bool_t>());
//        default:
//            return 0;
//    }
//}
//
//bp::serializable::float_t bp::structure::as_float() const {
//    switch (type()) {
//        case value_type::Int:
//            return val_->as<serializable::int_t>();
//        case value_type::Float:
//            return val_->as<serializable::float_t>();
//        default:
//            return 0;
//    }
//}
//
//bp::serializable::bool_t bp::structure::as_bool() const {
//    switch (type()) {
//        case value_type::Int:
//            return val_->as<serializable::int_t>() != 0;
//        case value_type::Bool:
//            return val_->as<serializable::bool_t>();
//        default:
//            return false;
//    }
//}

size_t bp::structure::size() const {
    if (is_array()) return val_->as_array()->size();
    if (is_object()) return val_->as_object()->size();
    return 0;
}

void bp::structure::initialize_if_null(bp::structure::value_type _type) {
    if (is_null()) {
        switch (_type) {
            case value_type::Object:
                *val_ = std::make_shared<object >();
                break;
            case value_type::Array:
                *val_ = std::make_shared<array >();
                break;
            default:
                break;
        }
        value_type_ = _type;
    }
}

bp::structure bp::structure::operator[](int index) ENABLE_IF_HAS_EXCEPTIONS(throw (type_error)) {
    if (is_array()) {
        return val_->as_array()->operator[](static_cast<size_t>(index % size()));
    } else {
#ifdef HAS_EXCEPTIONS
        throw type_error("not an array");
#endif
        return bp::structure();
    }
}

bp::structure bp::structure::operator[](const bp::hash_type &_key) ENABLE_IF_HAS_EXCEPTIONS(throw (type_error)) {
    if (is_null()) {
        emplace(_key, nullptr);
    } else {
        if (type() != value_type::Object) {
#ifdef HAS_EXCEPTIONS
            throw type_error("not an object");
#else
            emplace(_key, nullptr);
#endif
        }
    }
    return val_->as_object()->emplace(_key, std::make_shared<tree>(nullptr)).first->second;
}

bool bp::structure::erase(const bp::hash_type &_key) ENABLE_IF_HAS_EXCEPTIONS(throw (type_error, std::out_of_range)) {
    if (is_object()) {
        val_->as_object()->erase(_key);
        return true;
    } else {
#ifdef HAS_EXCEPTIONS
        throw type_error("not an object");
#endif
        return false;
    }
}

bp::structure bp::structure::at(const bp::hash_type &_key) const ENABLE_IF_HAS_EXCEPTIONS(throw (type_error, std::out_of_range)) {
    if (is_object()) {
        auto val = val_->as_object().get();
#ifndef HAS_EXCEPTIONS
        if (val->find(_key) == val->end()) {
            return bp::structure();
        }
#endif
        return val->at(_key);
    } else {
#ifdef HAS_EXCEPTIONS
        throw type_error("not an object");
#endif
        return bp::structure();
    }
}
bp::structure bp::structure::at(int _key) const ENABLE_IF_HAS_EXCEPTIONS(throw (type_error, std::out_of_range)) {
    if (is_array()) {
        auto val = val_->as_array().get();
#ifndef HAS_EXCEPTIONS
        if (val->size() <= _key) {
            return bp::structure();
        }
#endif
        return val->operator[](static_cast<size_t>(_key % size()));
    } else {
#ifdef HAS_EXCEPTIONS
        throw type_error("not an array");
#endif
        return bp::structure();
    }
}

bp::structure &bp::structure::operator=(const structure &_str) {
    if (&_str == this) return *this;
    this->value_type_ = _str.value_type_;
    *this->val_ = *_str.val_;
    return *this;
}

bp::structure &bp::structure::operator=(structure &&_str) {
    if (!this->val_) {
        this->val_ = _str.val_;
    } else {
        if (!_str.val_) {
            this->val_.reset();
        } else {
            // TODO: check why it necessary?
            if (bp::holds_alternative<object_ptr>(*_str.val_)) {
                *this->val_ = _str.val_->as_object();
            } else if (bp::holds_alternative<array_ptr>(*_str.val_)) {
                *this->val_ = _str.val_->as_array();
            } else {
                *this->val_ = _str.val_->as_value();
            }
        }
    }
    this->value_type_ = _str.value_type_;
    _str.val_ = nullptr;
    _str.value_type_ = value_type::Null;
    return *this;
}

bool bp::structure::emplace(const std::initializer_list<std::pair<bp::hash_type, bp::structure>> &_val) {
    if (!emplace_init()) { return false; }
    for (const auto &entry: _val) {
        emplace(entry.first, entry.second);
    }
    return true; //
}

bool bp::structure::emplace(const bp::hash_type &_key, const bp::structure &_str) {
    if (!emplace_init()) { return false; }
    if (!_str.is_null()) {
        return emplace_resolve(_key, *_str.val_);
    } else {
        return emplace_resolve(_key, serializable::tree());
    }
}

bool bp::structure::emplace(const std::initializer_list<std::pair<bp::string_view, bp::structure>> &_val) {
    if (!emplace_init()) { return false; }
    for (const auto &entry: _val) {
        emplace(entry.first, entry.second);
    }
    return true; //
}

bool bp::structure::emplace(const bp::string_view &_key, const bp::structure &_str) {
    return emplace(bp::symbol(_key).to_hash(), _str);
}

bp::structure::value_type bp::structure::get_variant_type(const bp::serializable::tree &_var) const {
    return bp::visit(variant_visitor(), _var);
}

bp::structure::structure(const structure &_s) : value_type_(_s.value_type_), val_(_s.val_) {

}

bp::structure::structure(structure &&_s) : value_type_(std::move(_s.value_type_)), val_(std::move(_s.val_)) {
    _s.value_type_ = value_type::Null;
    _s.val_ = nullptr;
}

bp::structure bp::structure::deepcopy() const {
    return structure(clone_variant(val_));
}

bp::structure::structure(const std::initializer_list<bp::serializable::tree> &_val) :
        value_type_(value_type::Array),
        val_(std::make_shared<serializable::tree>(std::make_shared<array>())) {
    for (const auto &item: _val) {
        this->append(bp::structure(std::make_shared<bp::serializable::tree>(item)));
    }

}

bp::structure::structure(const std::initializer_list<std::pair<bp::string_view, bp::structure>> &_val) :
        value_type_(value_type::Object),
        val_(std::make_shared<serializable::tree>(std::make_shared<object>())) {
    for (const auto &item: _val) {
        this->emplace_resolve(item.first, std::move(*item.second.val_));
    }
}

bp::structure::structure(const std::initializer_list<std::pair<bp::hash_type, bp::structure>> &_val):
        value_type_(value_type::Object),
        val_(std::make_shared<serializable::tree>(std::make_shared<object>())) {
    for (const auto &item: _val) {
        this->emplace_resolve(item.first, std::move(*item.second.val_));
    }
}

bool bp::structure::has_key(const bp::hash_type &_key) const ENABLE_IF_HAS_EXCEPTIONS(throw (type_error)) {
    if (is_object()) {
        return val_->as_object()->count(_key) > 0;
    }
    return false;
}

bool bp::structure::operator==(const bp::structure &_s) const {
    if (value_type_ != _s.value_type_) return false;
    if (val_ == _s.val_) return true;

    bool res = false;

    if (is_object()) {
        for (const auto &item: this->as_object()) {
            if (!_s.has_key(item.first)) {
                res = false;
                break;
            }
            res = (item.second == _s.at(item.first));
            if (!res) break;
        }
    } else if (is_array()) {
        int i = 0;
        for (const auto &item: this->as_array()) {
            res = (item == _s.at(i));
            if (!res) break;
            ++i;
        }
    } else {
        //FIXME: do not depend on build type
#ifndef TESTING
        // TODO: ireate over allowed types and match is<>->as<>
        if (is_int()) {
            res = as<int>() == _s.as<int>();
        } else if (is_float()) {
            res = as<float>() == _s.as<float>();
        } else if (is_bool()) {
            res = as<bool>() == _s.as<bool>();
        } else if (is_string()) {
            res = as<std::string>() == _s.as<std::string>();
        } else if (is_null()) {
            res = true;
        }
#else
        res = (*val_ == *_s.val_);
#endif
    }

    return res;
}

bp::structure::structure(bp::structure::value_type _t) : structure() {
    initialize_if_null(_t);
}

bp::structure::structure() : value_type_(value_type::Null),
                             val_(std::make_shared<serializable::tree>(nullptr)) {}

bool bp::structure::clear() {
    if (is_array()) {
        val_->as_array()->clear();
        return true;
    } else {
#ifdef HAS_EXCEPTIONS
        throw type_error("not an array");
#endif
        return false;
    }
}
//bp::structure(std::make_shared<bp::serializable::tree>(item))
bool bp::structure::append(const bp::structure &_val) {
    initialize_if_null(value_type::Array);
    if (is_array()) {
        val_->as_array()->push_back(_val.val_);
    } else {
#ifdef HAS_EXCEPTIONS
        throw type_error("not an array");
#endif
        return false;
    }
    return true;
}

bool bp::structure::create_object(const bp::string_view &_s) {
    return this->emplace(_s, structure(value_type::Object));
}

bool bp::structure::create_array(const bp::string_view&_s) {
    return this->emplace(_s, structure(value_type::Array));
}

bool bp::structure::create_object(const bp::hash_type &_s) {
    return this->emplace(_s, structure(value_type::Object));
}

bool bp::structure::create_array(const bp::hash_type &_s) {
    return this->emplace(_s, structure(value_type::Array));
}

bool bp::structure::empty() const {
    return static_cast<bool>(size()==0);
}

void bp::structure::reset() {
    value_type_ = value_type::Null;
    val_.reset(new serializable::tree(nullptr));
}

bp::structure& bp::structure::merge(const bp::structure &_s) ENABLE_IF_HAS_EXCEPTIONS(throw (type_error)) {

    if ((is_object() || is_null()) && _s.is_object()) {
        for (const auto& kv: _s.as_object() ) {
            operator[](kv.first) = kv.second.deepcopy();
        }
    } else {
#ifdef HAS_EXCEPTIONS
        throw type_error("types mismatch");
#endif
    }
    return *this;
}

bp::structure bp::structure::create_object() {
    return structure(value_type::Object);
}

bp::structure bp::structure::create_array() {
    return structure(value_type::Array);
}
