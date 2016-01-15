#include <sstream>
#include <json/json.h>
#include "serializer.hpp"

namespace bp {

    Json::Value build_json(serializer::ptr root) {
        Json::Value r;
        auto tp = root->type();
        switch (tp) {
            case serializer::value_type::Object: {
                for (auto item: root->keys()) {
                    r[item.name]=build_json(root->at(item));
                }
                break;
            }
            case serializer::value_type::Array: {
                for (int i=0; i<root->size(); ++i) {
                    r.append(build_json(root->at(i)));
                }
                break;
            }
            default: {
                if (root->is_string()) {
                    r = root->as_string();
                } else if (root->is_int()) {
                    r = root->as_int();
                } else if (root->is_float()) {
                    r = root->as_float();
                } else if (root->is_bool()) {
                    r = root->as_bool();
                } else if (root->is_symbol()) {
                    r = root->as_string();
                }
                break;
            }
        }
        return r;
    }


    template<>
    std::string serializer::stringify<serializers::type::Json>() const {
        serializer::ptr root = std::const_pointer_cast<bp::serializer>(shared_from_this());
        return build_json(root).toStyledString();
    };

    serializer::variant_ptr parse_variant(const Json::Value &root) {

        if (root.isArray()) {
            bp::serializer::array_t arr;
            for (int i = root.size()-1; i>=0; --i) {
                arr.push_back(parse_variant(root[i]));
            }
            return std::make_shared<bp::serializer::variant_t>(arr);
        } else if (root.isObject()) {
            bp::serializer::object_t obj;
            for (auto key: root.getMemberNames()) {
                obj.emplace(bp::symbol(key), parse_variant(root[key]));
            }
            return std::make_shared<bp::serializer::variant_t>(obj);
        } else if (root.isBool()) {
            return std::make_shared<bp::serializer::variant_t>(root.asBool());
        } else if (root.isDouble()) {
            return std::make_shared<bp::serializer::variant_t>(root.asDouble());
        } else if (root.isIntegral()) {
            return std::make_shared<bp::serializer::variant_t>(static_cast<serializable::int_t>(root.asLargestInt()));
        } else if (root.isNull()) {
            return std::make_shared<bp::serializer::variant_t>(0);
        } else if (root.isString()) {
            return std::make_shared<bp::serializer::variant_t>(root.asString());
        }
    }

    template<>
    void serializer::parse<serializers::type::Json>(const std::string &_str) {
        val_.reset();

        Json::Value root;
        std::stringstream ss(_str);
        ss >> root;

        if (root.isArray()) {
            set_type(value_type::Array);
        } else if (root.isObject()) {
            set_type(value_type::Object);
        } else if (root.isBool()) {
            set_type(value_type::Bool);
        } else if (root.isDouble()) {
            set_type(value_type::Float);
        } else if (root.isIntegral()) {
            set_type(value_type::Int);
        } else if (root.isNull()) {
            set_type(value_type::Null);
        } else if (root.isString()) {
            set_type(value_type::String);
        }
        val_ = parse_variant(root);
    };

}

