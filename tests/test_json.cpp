#include "structure.hpp"
#ifdef ARDUINO_JSON
#include "serializers/json_arduino.hpp"
#else
#include "serializers/json.hpp"
#endif
#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include <json/json.h>

using namespace bp::literals;

#ifdef ARDUINO_JSON
constexpr bp::symbol::hash_type SerializerType = bp::serializers::JsonArduino;
#else
constexpr bp::symbol::hash_type SerializerType = bp::serializers::Json;
#endif
TEST(JsonTest, serializer) {
    bp::structure s;
    s.emplace("1", 1);
    s.emplace("str", "bla");
    s.emplace("a", {1,2,3});
    s.emplace("o", {{"a",1}});
    s.emplace({{"oa", 1}, {"ob", 2}});


    auto json = s.stringify<SerializerType>();

    Json::Value root;
    try {
        std::stringstream ss(json);
        ss >> root;
    } catch (Json::Exception &_e) {
        throw bp::structure::parse_error(_e.what());
        FAIL();
    }

    std::cout << json << std::endl;
    ASSERT_TRUE(root.isObject());
    ASSERT_EQ(root.get("1", 0), 1);
    ASSERT_EQ(root.get("str", ""), "bla");
    ASSERT_EQ(root.get("oa", 0), 1);
    ASSERT_EQ(root.get("ob", 0), 2);
    ASSERT_TRUE(root["a"].isArray());
    ASSERT_EQ(root["a"].size(), 3);
    ASSERT_EQ(root["a"][0], 1);
    ASSERT_TRUE(root["o"].isObject());
    ASSERT_EQ(root["o"].get("a", 0), 1);
}

TEST(StructTest, parser) {
    auto s = bp::structure::create_from_string<SerializerType>("{\"ob\":2,\"1\":1,\"str\":\"bla\",\"o\":{\"a\":1},\"oa\":1,\"a\":[1,2,3]}");

    ASSERT_TRUE(s.is_object());
    ASSERT_EQ(s.get("1", 0).as_int(), 1);
    ASSERT_EQ(s.get("str", std::string("")).as_string(), "bla");
    ASSERT_EQ(s.get("oa", 0).as_int(), 1);
    ASSERT_EQ(s.get("ob", 0).as_int(), 2);
    ASSERT_TRUE(s["a"].is_array());
    ASSERT_EQ(s["a"].size(), 3);
    ASSERT_EQ(s["a"][0].as_int(), 1);
    ASSERT_TRUE(s["o"].is_object());
    ASSERT_EQ(s["o"].get("a", 0).as_int(), 1);
}