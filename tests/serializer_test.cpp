#include "lib/util/structure.hpp"
#include "lib/util/serializers/json_arduino.hpp"
#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include <json/json.h>

using namespace bp::literals;

constexpr bp::hash_type SerializerType = bp::serializers::JsonArduino;
TEST(JsonTest, serializer) {
    bp::structure s;
    s.emplace("1", 1);
    s.emplace("str", "bla");
    s.emplace("a", {1,2,3});
    s.emplace("o", {{"a",1}});
    s.emplace({{"oa", 1}, {"ob", 2}});


    auto json = s.serialize<SerializerType>();

    Json::Value root;
#ifdef HAS_EXCEPTIONS
    try {
#endif
        std::stringstream ss(json);
        ss >> root;
#ifdef HAS_EXCEPTIONS
    } catch (Json::Exception &_e) {
        throw bp::structure::parse_error(_e.what());
        FAIL();
    }
#else
    if (ss.fail()) {
        FAIL();
    }
#endif

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
    ASSERT_EQ(s.get<int>("1"_h, 0), 1);
    ASSERT_EQ(s.get<std::string>("str"_h, ""), "bla");
    ASSERT_EQ(s.get<int>("oa"_h, 0), 1);
    ASSERT_EQ(s.get<int>("ob"_h, 0), 2);
    ASSERT_TRUE(s["a"].is_array());
    ASSERT_EQ(s["a"].size(), 3);
    ASSERT_EQ(s["a"][0].as<int>(), 1);
    ASSERT_TRUE(s["o"].is_object());
    ASSERT_EQ(s["o"].get<int>("a"_h, 0), 1);

    auto pkt = bp::structure::create_from_string<bp::serializers::JsonArduino>("{\"cmd\":\"status\"}");
    ASSERT_TRUE(pkt.has_key("cmd"_h));
    ASSERT_EQ(pkt.get<std::string>("cmd"_h), "status");
}