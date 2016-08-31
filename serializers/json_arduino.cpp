#include <sstream>
#include "structure.hpp"
#include "serializers/json_arduino.hpp"
#include <ArduinoJson.h>

namespace bp {
    const int MAX_JSON_BUFFER_SIZE = 1024;  //FIXME: does not work on grater buffers. why?
    using namespace serializable;

    /*
     * ArduinoJson does not manage string lifetime so we should keep strings
     * in memory until object is stringified. Pass user-managed string vector to builder to achieve correct result.
     * It should not be destoryed until json is dumped.
     * */
    void build_json(const structure& root, JsonVariant r, std::vector<std::string> &_string_manager) {
        auto tp = root.type();
        // TODO: throw errors on memory exhaust
        switch (tp) {
            case structure::value_type::Object: {
                auto &root_object = r.asObject();
                for (auto key: root.keys()) {
                    auto item = root.at(key);
                    auto newtp = item.type();
                    if (newtp==structure::value_type::Object) {
                        auto &obj = root_object.createNestedObject(key);
                        build_json(item, JsonVariant(std::ref(obj)), _string_manager);
                    } else if (newtp==structure::value_type::Array) {
                        auto &obj = root_object.createNestedArray(key);
                        build_json(item, JsonVariant(std::ref(obj)), _string_manager);
                    } else {
                        if (item.is_int()) {
                            root_object[key] = item.as_int();
                        } else if (item.is_float()) {
                            root_object[key] = item.as_float();
                        } else if (item.is_bool()) {
                            root_object[key] = item.as_bool();
                        } else if (item.is_string()) {
                            auto it = _string_manager.emplace(_string_manager.end(), item.as_string());
                            root_object[key] = it->c_str();
                        }
                    }

                }
                break;
            }
            case structure::value_type::Array: {
                auto &root_object = r.asArray();
                for (uint32_t i=0; i<root.size(); ++i) {
                    auto item = root.at(i);
                    auto newtp = item.type();
                    if (newtp==structure::value_type::Object) {
                        auto &obj = root_object.createNestedObject();
                        build_json(item, JsonVariant(std::ref(obj)), _string_manager);
                    } else if (newtp==structure::value_type::Array) {
                        auto &obj = root_object.createNestedArray();
                        build_json(item, JsonVariant(std::ref(obj)), _string_manager);
                    } else {
                        if (item.is_int()) {
                            root_object.add(item.as_int());
                        } else if (item.is_float()) {
                            root_object.add(item.as_float());
                        } else if (item.is_bool()) {
                            root_object.add(item.as_bool());
                        } else if (item.is_string()) {
                            auto it = _string_manager.emplace(_string_manager.end(), item.as_string());
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
    std::string structure::stringify<serializers::JsonArduino>() const {
        StaticJsonBuffer<MAX_JSON_BUFFER_SIZE> jsonBuffer;
        std::vector<std::string> string_manager;
        char str[MAX_JSON_BUFFER_SIZE];
        size_t len = 0;
        if (this->is_object()) {
            JsonObject &obj = jsonBuffer.createObject();
            build_json(*this, JsonVariant(std::ref(obj)), string_manager);
            len = obj.printTo(str, MAX_JSON_BUFFER_SIZE);
        } else if (this->is_array()) {
            JsonArray &arr = jsonBuffer.createArray();
            build_json(*this, JsonVariant(std::ref(arr)), string_manager);
            len = arr.printTo(str, MAX_JSON_BUFFER_SIZE);
        }
        string_manager.clear();
        if (len >= MAX_JSON_BUFFER_SIZE) {
            len = MAX_JSON_BUFFER_SIZE-1;
        }
        str[len] = 0;
        return std::string(str, len);
    };

    variant_ptr parse_variant(const JsonVariant &root) {

        if (root.is<JsonArray&>()) {
            array arr;
            auto &jarr = root.asArray();
            for (int i = 0, len=root.size(); i < len; ++i) {
                arr.push_back(parse_variant(jarr[i]));
            }
            return std::make_shared<tree>(arr);
        } else if (root.is<JsonObject&>()) {
            object obj;
            auto &jobj = root.asObject();
            for (auto kv: jobj) {
                obj.emplace(hash(kv.key), parse_variant(kv.value));
            }
            return std::make_shared<tree>(obj);
        } else if (root.is<bool>()) {
            return std::make_shared<tree>(root.as<bool>());
        } else if (root.is<double>()) {
            return std::make_shared<tree>(root.as<double>());
        } else if (root.is<long>()) {
            return std::make_shared<tree>(static_cast<serializable::int_t>(root.as<long>()));
        } else if (root.is<const char*>()) {
            return std::make_shared<tree>(std::string(root.as<const char*>()));
        }

        return std::make_shared<tree>(0);
    }

    template<>
    void structure::parse<serializers::JsonArduino>(const std::string &_str) {
        val_.reset();

//        JsonVariant root;
        StaticJsonBuffer<MAX_JSON_BUFFER_SIZE> jsonBuffer;


        JsonObject& root = jsonBuffer.parseObject(const_cast<char*>(_str.c_str()));
        if (!root.success()) {
            throw structure::parse_error("fail");
        }

//        if (root.isArray()) {
//            set_type(value_type::Array);
//        } else if (root.isObject()) {
            set_type(value_type::Object);
//        } else if (root.isBool()) {
//            set_type(value_type::Bool);
//        } else if (root.isDouble()) {
//            set_type(value_type::Float);
//        } else if (root.isIntegral()) {
//            set_type(value_type::Int);
//        } else if (root.isNull()) {
//            set_type(value_type::Null);
//        }
        try {
            val_ = parse_variant(JsonVariant(std::ref(root)));
        } catch (std::exception &_e) {
            throw structure::parse_error(_e.what());
        }
    };

}
