#include "../structure.hpp"
#include "json_arduino.hpp"
#include "../../json/ArduinoJson.h"
#include "../../unilog.h"
USE_LOGGER(ulog)

namespace bp {
//    const int MAX_JSON_BUFFER_SIZE = 2048;
    const int MAX_JSON_BUFFER_SIZE = 1536;
    using namespace serializable;

    int keys_count(const structure& root);

    int item_keys_count(const bp::structure &item) {
        uint8_t cnt = 0;
        if (item.is_object() || item.is_array()) {
            cnt += keys_count(item);
        } else if (item.is_string()) {
            ++cnt;
        }
        return cnt;
    }
    // counts all keys in tree. needed by json builder to reserve necessary amount of data to avoid reallocation
    int keys_count(const structure& root) {
        uint8_t cnt = 0;
        // TODO: throw errors on memory exhaust
        switch (root.type()) {
            case structure::value_type::Object: {
                for (const auto &key: root.keys()) {
                    cnt += 1 + item_keys_count(root.at(key));
                }
                break;
            }
            case structure::value_type::Array: {
                for (size_t i=0, sz=root.size(); i<sz; ++i) {
                    cnt += item_keys_count(root.at(i));
                }
                break;
            }
            default: {
                break;
            }
        }
        return cnt;
    }

    /*
     * ArduinoJson does not manage string lifetime so we should keep strings
     * in memory until object is stringified. Pass user-managed string vector to builder to achieve correct result.
     * It should not be destoryed until json is dumped.
     * */
    void build_json(const structure& root, JsonVariant &r, std::vector<std::string> &_string_manager) {
        // TODO: throw errors on memory exhaust
        switch (root.type()) {
            case structure::value_type::Object: {
                auto &root_object = r.asObject();
                for (const auto &key: root.keys()) {
                    auto kit = _string_manager.emplace(_string_manager.end(), std::string(bp::symbol::name(key)));
                    auto item = root.at(key);
                    if (item.is_object()) {
                        auto &obj = root_object.createNestedObject(kit->c_str());
                        JsonVariant var;
                        var.set(obj);
                        build_json(item, var, _string_manager);
                    } else if (item.is_array()) {
                        auto &obj = root_object.createNestedArray(kit->c_str());
                        JsonVariant var;
                        var.set(obj);
                        build_json(item, var, _string_manager);
                    } else {
                        if (item.is_int()) {
                            root_object[kit->c_str()] = item.as<int>();
                        } else if (item.is_float()) {
                            root_object[kit->c_str()] = item.as<float>();
                        } else if (item.is_bool()) {
                            root_object[kit->c_str()] = item.as<bool>();
                        } else if (item.is_string()) {
                            auto it = _string_manager.emplace(_string_manager.end(), item.as<std::string>());
                            root_object[kit->c_str()] = it->c_str();
                        }
                    }

                }
                break;
            }
            case structure::value_type::Array: {
                auto &root_object = r.asArray();
                for (size_t i=0, sz=root.size(); i<sz; ++i) {
                    auto item = root.at(i);
                    if (item.is_object()) {
                        auto &obj = root_object.createNestedObject();
                        JsonVariant var;
                        var.set(obj);
                        build_json(item, var, _string_manager);
                    } else if (item.is_array()) {
                        auto &obj = root_object.createNestedArray();
                        JsonVariant var;
                        var.set(obj);
                        build_json(item, var, _string_manager);
                    } else {
                        if (item.is_int()) {
                            root_object.add(item.as<int>());
                        } else if (item.is_float()) {
                            root_object.add(item.as<float>());
                        } else if (item.is_bool()) {
                            root_object.add(item.as<bool>());
                        } else if (item.is_string()) {
                            auto it = _string_manager.emplace(_string_manager.end(), item.as<std::string>());
                            root_object.add(it->c_str());
                        }
                    }

                }
                break;
            }
            default: {

                break;
            }
        }
    }


    template<>
    std::string structure::serialize<serializers::JsonArduino>() const {
        //ulog.info(ulog.logen, "JSON Free memory: %ld bytes %d", System.freeMemory(), this->has_key("cmd"_h));
//        StaticJsonBuffer<MAX_JSON_BUFFER_SIZE> jsonBuffer;
        DynamicJsonBuffer jsonBuffer;
        std::vector<std::string> string_manager;
        auto sz = static_cast<size_t>(keys_count(*this));
        string_manager.reserve(sz);

        char str[MAX_JSON_BUFFER_SIZE];
        size_t len = 0;
        if (this->is_object()) {
            JsonObject &obj = jsonBuffer.createObject();
            JsonVariant var;
            var.set(obj);
            build_json(*this, var, string_manager);
            len = obj.printTo(str, MAX_JSON_BUFFER_SIZE);
        } else if (this->is_array()) {
            JsonArray &arr = jsonBuffer.createArray();
            JsonVariant var;
            var.set(arr);
            build_json(*this, var, string_manager);
            len = arr.printTo(str, MAX_JSON_BUFFER_SIZE);
        }
        string_manager.clear();
        if (len >= MAX_JSON_BUFFER_SIZE) {
            len = MAX_JSON_BUFFER_SIZE-1;
        }
        str[len] = 0;
        return std::string(str, len);
    };

    tree_ptr parse_variant(const JsonVariant &root) {

        if (root.is<JsonArray&>()) {
            array arr;
            auto &jarr = root.asArray();
            for (size_t i = 0, len=root.size(); i < len; ++i) {
                arr.push_back(parse_variant(jarr[i]));
            }
            return std::make_shared<tree>(arr);
        } else if (root.is<JsonObject&>()) {
            object obj;
            auto &jobj = root.asObject();
            for (const auto &kv: jobj) {
                obj.emplace(bp::symbol(std::string(kv.key)).to_hash(), parse_variant(kv.value));
            }
            return std::make_shared<tree>(obj);
        } else if (root.is<bool>()) {
            return std::make_shared<tree>(root.as<bool>());
        } else if (root.is<double>()) {
            return std::make_shared<tree>(static_cast<float>(root.as<double>()));
        } else if (root.is<long>()) {
            return std::make_shared<tree>(static_cast<int>(root.as<int>()));
        } else if (root.is<const char*>()) {
            return std::make_shared<tree>(std::string(root.as<const char*>()));
        }

        return std::make_shared<tree>(static_cast<int>(0));
    }

    template<>
    bool structure::parse<serializers::JsonArduino>(const bp::string_view &_str) {
        val_.reset();

//        JsonVariant root;
        StaticJsonBuffer<MAX_JSON_BUFFER_SIZE> jsonBuffer;

        std::string s(_str.data(), _str.size());
        JsonObject& root = jsonBuffer.parseObject(const_cast<char*>(s.c_str()));
        if (!root.success()) {
#ifdef HAS_EXCEPTIONS
            throw structure::parse_error("fail");
#endif
            return false;
        }

//        if (root.isArray()) {
//            set_type(value_type::Array);
//        } else if (root.isObject()) {
            value_type_ = value_type::Object;
//        } else if (root.isBool()) {
//            set_type(value_type::Bool);
//        } else if (root.isDouble()) {
//            set_type(value_type::Float);
//        } else if (root.isIntegral()) {
//            set_type(value_type::Int);
//        } else if (root.isNull()) {
//            set_type(value_type::Null);
//        }
#ifdef HAS_EXCEPTIONS
        try {
#endif
        JsonVariant var;
        var.set(root);
            val_ = parse_variant(var);
#ifdef HAS_EXCEPTIONS
        } catch (std::exception &_e) {
            throw structure::parse_error(_e.what());
        }
#endif
        return true;
    };

}
