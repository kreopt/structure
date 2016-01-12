#include <sstream>
#include "json.hpp"

std::string serializers::json::as_string() const {
    return root_->asString();
}

serializers::serializable::int_t serializers::json::as_int() const {
    return root_->asInt64();
}

double serializers::json::as_float() const {
    return root_->asDouble();
}

bool serializers::json::as_bool() const {
    return root_->asBool();
}

serializers::json::json() : value_type_(serializers::serializer::value_type::Null) {
    root_ = new Json::Value();
}

serializers::json::json(Json::Value &_val) : value_type_(serializers::serializer::value_type::Null),
                                                   root_(&_val), ref_(true)
{

}


serializers::serializer::ptr serializers::json::create() {
    return std::shared_ptr<json>(new json());
}

serializers::serializer::ptr serializers::json::create(Json::Value &_val) {
    return std::shared_ptr<json>(new json(_val));
}


struct SerializableVisitor : public boost::static_visitor<Json::Value> {
    Json::Value operator()(serializers::serializable::int_t _val) const { return Json::Value(Json::Int(_val)); }
    Json::Value operator()(serializers::serializable::float_t _val) const { return Json::Value(_val); }
    Json::Value operator()(serializers::serializable::c_str_t _val) const { return Json::Value(_val); }
    Json::Value operator()(serializers::serializable::bool_t _val) const { return Json::Value(_val); }
    Json::Value operator()(const std::string & _val) const { return Json::Value(_val); }
};

serializers::serializer::ptr serializers::json::get(const std::string &_key, const value &_default) const {
    auto jval = root_->get(_key, boost::apply_visitor(SerializableVisitor(), _default));
    return create(jval);
}

serializers::serializer::ptr serializers::json::get(const std::string &_key, value &&_default) const {
    auto jval = root_->get(_key, boost::apply_visitor(SerializableVisitor(), _default));
    return create(jval);
}

void serializers::json::parse(const std::string &_str) {
    std::stringstream ss(_str);
    ss >> *root_;
}

std::string serializers::json::stringify() const {
    return root_->toStyledString();
}

size_t serializers::json::size() const {
    return 0;
}

serializers::serializer::ptr serializers::json::at(int index) {
    return create((*root_)[index]);
}

const serializers::serializer::ptr serializers::json::at(int index) const {
    return create((*root_)[index]);
}

serializers::serializer::ptr serializers::json::at(const char *_key) {
    return create((*root_)[_key]);
}

const serializers::serializer::ptr serializers::json::at(const char *_key) const {
    return create((*root_)[_key]);
}

serializers::serializer::ptr serializers::json::set(serializers::value &&_val) {
    *root_ = boost::apply_visitor(SerializableVisitor(), _val);
    return std::shared_ptr<serializer>();
}

serializers::serializer::ptr serializers::json::set(const serializers::value &_val) {
    *root_ = boost::apply_visitor(SerializableVisitor(), _val);
    return shared_from_this();
}

serializers::serializer::ptr serializers::json::set(const std::initializer_list<value> &_val) {
    for (auto val: _val) {
        root_->append(boost::apply_visitor(SerializableVisitor(), val));
    }
    return shared_from_this();
}

serializers::serializer::ptr serializers::json::set(const std::initializer_list<std::pair<std::string, value>> &_val) {
    *root_ = Json::Value();
    for (auto pair: _val) {
        (*root_)[pair.first] = boost::apply_visitor(SerializableVisitor(), pair.second);
    }
    return shared_from_this();
}

serializers::json::operator serializer::ptr() {
    return shared_from_this();
}

void serializers::json::append(const serializers::value &_val) {
    // TODO:
}

void serializers::json::append(serializers::value &&_val) {
    // TODO:
}

bool serializers::json::emplace(const std::string &_key, const serializers::value &_val) {
    // TODO:
    return false;
}

bool serializers::json::emplace(const std::string &_key, serializers::value &&_val) {
    // TODO:
    return false;
}

serializers::json::~json() {
    if (!ref_) {
        delete root_;
    }
}

bool serializers::json::append(const std::initializer_list<value> &_val) {
    return false;
}

bool serializers::json::append(const std::initializer_list<std::pair<std::string, value>> &_val) {
    return false;
}

bool serializers::json::emplace(const std::string &_key, const std::initializer_list<value> &_val) {
    return false;
}

bool serializers::json::emplace(const std::string &_key,
                                const std::initializer_list<std::pair<std::string, value>> &_val) {
    return false;
}
