#include <sstream>
#include "json.hpp"

std::string bp::serializers::json::as_string() const {
    return root_->asString();
}

bp::serializable::int_t bp::serializers::json::as_int() const {
    return root_->asInt64();
}

double bp::serializers::json::as_float() const {
    return root_->asDouble();
}

bool bp::serializers::json::as_bool() const {
    return root_->asBool();
}

bp::serializers::json::json() : value_type_(bp::serializer::value_type::Null) {
    root_ = new Json::Value();
}

bp::serializers::json::json(Json::Value &_val) : value_type_(bp::serializer::value_type::Null),
                                                   root_(&_val), ref_(true)
{

}


bp::serializer::ptr bp::serializers::json::create() {
    return std::shared_ptr<json>(new json());
}

bp::serializer::ptr bp::serializers::json::create(Json::Value &_val) {
    return std::shared_ptr<json>(new json(_val));
}


struct SerializableVisitor : public boost::static_visitor<Json::Value> {
    Json::Value operator()(bp::serializable::int_t _val) const { return Json::Value(Json::Int(_val)); }
    Json::Value operator()(bp::serializable::float_t _val) const { return Json::Value(_val); }
    Json::Value operator()(bp::serializable::c_str_t _val) const { return Json::Value(_val); }
    Json::Value operator()(bp::serializable::bool_t _val) const { return Json::Value(_val); }
    Json::Value operator()(const std::string & _val) const { return Json::Value(_val); }
};

bp::serializer::ptr bp::serializers::json::get(const std::string &_key, const value &_default) const {
    auto jval = root_->get(_key, boost::apply_visitor(SerializableVisitor(), _default));
    return create(jval);
}

bp::serializer::ptr bp::serializers::json::get(const std::string &_key, value &&_default) const {
    auto jval = root_->get(_key, boost::apply_visitor(SerializableVisitor(), _default));
    return create(jval);
}

void bp::serializers::json::parse(const std::string &_str) {
    std::stringstream ss(_str);
    ss >> *root_;
}

std::string bp::serializers::json::stringify() const {
    return root_->toStyledString();
}

size_t bp::serializers::json::size() const {
    return 0;
}

bp::serializer::ptr bp::serializers::json::at(int index) {
    return create((*root_)[index]);
}

const bp::serializer::ptr bp::serializers::json::at(int index) const {
    return create((*root_)[index]);
}

bp::serializer::ptr bp::serializers::json::at(const char *_key) {
    return create((*root_)[_key]);
}

const bp::serializer::ptr bp::serializers::json::at(const char *_key) const {
    return create((*root_)[_key]);
}

bp::serializer::ptr bp::serializers::json::set(bp::serializer::value &&_val) {
    *root_ = boost::apply_visitor(SerializableVisitor(), _val);
    return std::shared_ptr<serializer>();
}

bp::serializer::ptr bp::serializers::json::set(const bp::serializer::value &_val) {
    *root_ = boost::apply_visitor(SerializableVisitor(), _val);
    return shared_from_this();
}

bp::serializer::ptr bp::serializers::json::set(const std::initializer_list<value> &_val) {
    for (auto val: _val) {
        root_->append(boost::apply_visitor(SerializableVisitor(), val));
    }
    return shared_from_this();
}

bp::serializer::ptr bp::serializers::json::set(const std::initializer_list<std::pair<std::string, value>> &_val) {
    *root_ = Json::Value();
    for (auto pair: _val) {
        (*root_)[pair.first] = boost::apply_visitor(SerializableVisitor(), pair.second);
    }
    return shared_from_this();
}

bp::serializers::json::operator serializer::ptr() {
    return shared_from_this();
}

void bp::serializers::json::append(const bp::serializer::value &_val) {
    // TODO:
}

void bp::serializers::json::append(bp::serializer::value &&_val) {
    // TODO:
}

bool bp::serializers::json::emplace(const std::string &_key, const bp::serializer::value &_val) {
    // TODO:
    return false;
}

bool bp::serializers::json::emplace(const std::string &_key, bp::serializer::value &&_val) {
    // TODO:
    return false;
}

bp::serializers::json::~json() {
    if (!ref_) {
        delete root_;
    }
}

bool bp::serializers::json::append(const std::initializer_list<value> &_val) {
    return false;
}

bool bp::serializers::json::append(const std::initializer_list<std::pair<std::string, value>> &_val) {
    return false;
}

bool bp::serializers::json::emplace(const std::string &_key, const std::initializer_list<value> &_val) {
    return false;
}

bool bp::serializers::json::emplace(const std::string &_key,
                                const std::initializer_list<std::pair<std::string, value>> &_val) {
    return false;
}
