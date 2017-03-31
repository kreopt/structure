#include <sstream>
#include <jsoncpp/json/json.h>
#include "structure.hpp"
#include "serializers/json.hpp"

namespace bp {
    using namespace serializable;

    Json::Value build_json(const structure& root) {
        Json::Value r;
        auto tp = root.type();
        switch (tp) {
            case structure::value_type::Object: {
                for (auto item: root.keys()) {
                    r[item]=build_json(root.at(item));
                }
                break;
            }
            case structure::value_type::Array: {
                for (uint32_t i=0; i<root.size(); ++i) {
                    r.append(build_json(root.at(i)));
                }
                break;
            }
            default: {
                if (root.is_string()) {
                    r = root.as<std::string>();
                } else if (root.is_int()) {
                    r = root.as<int>();
                } else if (root.is_float()) {
                    r = root.as<float>();
                } else if (root.is_bool()) {
                    r = root.as<bool>();
                }
                break;
            }
        }
        return r;
    }


    template<>
    std::string structure::serialize<serializers::Json>() const {
        return build_json(*this).toStyledString();
    };

    tree_ptr parse_variant(const Json::Value &root) {

        if (root.isArray()) {
            array arr;
            for (int i = 0, len=root.size(); i < len; ++i)  {
                arr.push_back(parse_variant(root[i]));
            }
            return std::make_shared<tree>(arr);
        } else if (root.isObject()) {
            object obj;
            for (auto key: root.getMemberNames()) {
                obj.emplace(bp::symbol(key).to_hash(), parse_variant(root[key]));
            }
            return std::make_shared<tree>(obj);
        } else if (root.isBool()) {
            return std::make_shared<tree>(root.asBool());
        } else if (root.isDouble()) {
            return std::make_shared<tree>(static_cast<float>(root.asDouble()));
        } else if (root.isIntegral()) {
            return std::make_shared<tree>(static_cast<serializable::int_t>(root.asLargestInt()));
        } else if (root.isNull()) {
            return std::make_shared<tree>(0);
        } else if (root.isString()) {
            return std::make_shared<tree>(root.asString());
        }
        return std::make_shared<tree>(0);
    }

    template<>
    bool structure::parse<serializers::Json>(const std::string &_str) {
        val_.reset();

        Json::Value root;
        try {
            std::stringstream ss(_str);
            ss >> root;
        } catch (Json::Exception &_e) {
            throw bp::structure::parse_error(_e.what());
        }

        if (root.isArray()) {
            value_type_ = value_type::Array;
        } else if (root.isObject()) {
            value_type_ = value_type::Object;
        } else if (root.isBool()) {
            value_type_ = value_type::Bool;
        } else if (root.isDouble()) {
            value_type_ = value_type::Float;
        } else if (root.isIntegral()) {
            value_type_ = value_type::Int;
        } else if (root.isNull()) {
            value_type_ = value_type::Null;
        } else if (root.isString()) {
            value_type_ = value_type::String;
        }
        try {
            val_ = parse_variant(root);
        } catch (std::exception &_e) {
            throw bp::structure::parse_error(_e.what());
        }
        return true;
    };

}

