#include "lib/util/structure.hpp"
#include <gtest/gtest.h>
#include <string>
#include <algorithm>

using namespace bp::literals;

TEST(StructTest, constructors) {
    bp::structure s;
    s["a"] = 1;

    bp::structure s_copy(s);
    bp::structure s_deepcopy(std::move(s.deepcopy()));
    bp::structure s_move(std::move(s));
    s_copy["a"] = 3;

//    ASSERT_EQ(static_cast<bool>(s), false);
//    ASSERT_EQ(static_cast<bool>(s_copy), true);
//    ASSERT_EQ(static_cast<bool>(s_deepcopy), true);
//    ASSERT_EQ(static_cast<bool>(s_move), true);
    ASSERT_EQ(s_copy.at("a"_h).is_int(), true);
    ASSERT_EQ(s_deepcopy.at("a"_h).is_int(), true);
    ASSERT_EQ(s_move.at("a"_h).is_int(), true);
    ASSERT_EQ(s.is_null(), true);
    ASSERT_EQ(s_copy.is_object(), true);
    ASSERT_EQ(s_deepcopy.is_object(), true);
    ASSERT_EQ(s_move.is_object(), true);
    ASSERT_EQ(s_copy.at("a"_h).as<int>(), 3);
    ASSERT_EQ(s_deepcopy.at("a"_h).as<int>(), 1);
    ASSERT_EQ(s_move.at("a"_h).as<int>(), 3);

    {
        bp::structure s(1);
        ASSERT_TRUE(s.is_int()) << "check is int";
        ASSERT_EQ(s.as<int>(), 1);
    }
    {
        bp::structure s(1.0f);
        ASSERT_TRUE(s.is_float()) << "check is float";
        ASSERT_EQ(s.as<float>(), 1.0);
    }
    {
        bp::structure s("s");
        ASSERT_TRUE(s.is_string()) << "check is string";
        ASSERT_EQ(s.as<std::string>(), "s");
    }
    {
        bp::structure s {1, 2};
        ASSERT_TRUE(s.is_array()) << "check is array";
        ASSERT_EQ(s[0].as<int>(), 1);

    }
    {
        bp::structure s {{"a", "b"}};
        ASSERT_TRUE(s.is_object()) << "check is object";
        ASSERT_EQ(s["a"].as<std::string>(), "b");
    }
}

TEST(StructTest, object_access) {
    bp::structure s;
    s["a"] = 1;

#ifdef HAS_EXCEPTIONS
    ASSERT_NO_THROW(s.at("a"_h)) << "failed to access object key";
#endif
    ASSERT_EQ(s.type(), bp::structure::value_type::Object) << "structure type is not an object";
    ASSERT_EQ(s.at("a"_h).as<int>(), 1) << "bad value for .at()";
    ASSERT_EQ(s["a"].as<int>(), 1) << "bad value for operator[]";
    ASSERT_EQ(s.size(), 1) << "bad object size";

#ifdef HAS_EXCEPTIONS
    ASSERT_THROW(s["a"]["b"]=1, std::range_error) << "insertion to non-object does not throw";
#endif

    s.at("a"_h) = 2;
    ASSERT_EQ(s.at("a"_h).as<int>(), 2) << "bad value after change";

#ifdef HAS_EXCEPTIONS
    ASSERT_NO_THROW(s["b"] = 2) << "exception on insert into object";
    ASSERT_EQ(s.size(), 2) << "bad size afer insertion";
#endif
}

TEST(StructTest, object_access_get) {
    bp::structure s;
    s["a"] = 1;

    ASSERT_EQ(s.get<std::string>("b"_h), std::string(""));
    ASSERT_EQ(s.get<int>("b"_h, 1), 1);
    ASSERT_EQ(s.get<int>("a"_h), 1);
    ASSERT_EQ(s.get<int>("a"_h, 2), 1);
}

TEST(StructTest, object_erase) {
    bp::structure s;
    s["a"] = 1;
#ifdef USE_EXCEPTIONS
    ASSERT_THROW(s.at("a"_h).erase("b"_h), std::range_error) << "erase non-object does not throw";
#endif
    s.erase("a"_h);
    ASSERT_EQ(s.size(), 0) << "Size after erasing does not decreased";
    ASSERT_EQ(s.type(), bp::structure::value_type::Object) << "empty object does not remains object";
}

TEST(StructTest, object_emplace) {
    bp::structure s, s1;
    s1["a"] = 1;

    s.emplace("1", 1);
    s.emplace("str", s1.deepcopy());
    s.emplace("a", {1, 2, 3});
    s.emplace("o", {{"a", 1}});
    s.emplace({{"oa", 1},
               {"ob", 2}});
    s.emplace({{"as", s1}});
    s.at("as"_h).at("a"_h) = 2;

    ASSERT_EQ(s.at("1"_h).as<int>(), 1);
#ifdef USE_EXCEPTIONS
    ASSERT_NO_THROW(s.at("str"_h).at("a"_h));
#endif
    ASSERT_EQ(s.at("str"_h).at("a"_h).as<int>(), 1);
    ASSERT_EQ(s.at("a"_h).is_array(), true);
    ASSERT_EQ(s.at("a"_h).size(), 3);
#ifdef USE_EXCEPTIONS
    ASSERT_NO_THROW(s.at("a"_h)[0]);
#endif
    ASSERT_EQ(s.at("a"_h)[0].as<int>(), 1);
    ASSERT_EQ(s.at("o"_h).is_object(), true);
    ASSERT_EQ(s.at("o"_h).size(), 1);
#ifdef USE_EXCEPTIONS
    ASSERT_NO_THROW(s.at("o"_h).at("a"_h));
#endif
    ASSERT_EQ(s.at("o"_h).at("a"_h).as<int>(), 1);
    ASSERT_EQ(s.at("as"_h).at("a"_h).as<int>(), 2);

    ASSERT_EQ(s.at("oa"_h).as<int>(), 1);
    ASSERT_EQ(s.at("ob"_h).as<int>(), 2);

    ASSERT_EQ(s.size(), 7);
//    ASSERT_EQ(s.size(), 5);
    bp::structure s2({{"a", 1}, {"b", 2}});
    s.merge(s2);
#ifdef USE_EXCEPTIONS
    ASSERT_THROW(s.merge({1,2,3}));
#endif
    ASSERT_TRUE(s.has_key("b"_h));
    ASSERT_EQ(s.at("a"_h).as<int>(), 1);
    ASSERT_EQ(s.at("b"_h).as<int>(), 2);
}

TEST(StructTest, data_types) {
    bp::structure s;
    s["a"] = 1;
    auto v = s.at("a"_h);
    ASSERT_EQ(v.as<int>(), 1) << "as<int>()(int)";
    ASSERT_EQ(v.as<std::string>(), std::string("1")) << "as<std::string>()(int)";
    ASSERT_EQ(v.as<bool>(), true) << "as<bool>()(int!=0)";
    ASSERT_EQ(v.as<float>(), 1.0) << "as<float>()(int)";
    v = s.at("a"_h);
    s["a"] = 0;
    ASSERT_EQ(v.as<bool>(), false) << "as<bool>()(int==0)";

    s["a"] = true;
    v = s.at("a"_h);
    ASSERT_EQ(v.as<int>(), 1) << "as<int>()(bool)";
    ASSERT_EQ(v.as<std::string>(), std::string("true")) << "as<std::string>()(bool==true)";
    ASSERT_EQ(v.as<bool>(), true) << "as<bool>()(bool==true)";
    ASSERT_EQ(v.as<float>(), 0) << "as<float>()(bool)";
    s["a"] = false;
    ASSERT_EQ(v.as<bool>(), false) << "as<bool>()(bool==false)";
    ASSERT_EQ(v.as<std::string>(), std::string("false")) << "as<std::string>()(bool==false)";

    s["a"] = 1.0f;
    v = s.at("a"_h);
    ASSERT_EQ(v.as<int>(), 1) << "as<int>()(float)";
    ASSERT_EQ(v.as<std::string>(), std::to_string(1.0)) << "as<std::string>()(float)";
    ASSERT_EQ(v.as<bool>(), false) << "as<bool>()(float)";
    ASSERT_EQ(v.as<float>(), 1.0) << "as<float>()(float)";

    s["a"] = "str";
    v = s.at("a"_h);
    ASSERT_EQ(v.as<int>(), 0) << "as<int>()(string)";
    ASSERT_EQ(v.as<std::string>(), std::string("str")) << "as<std::string>()(string)";
    ASSERT_EQ(v.as<bool>(), false) << "as<bool>()(string)";
    ASSERT_EQ(v.as<float>(), 0) << "as<float>()(string)";
}

TEST(StructTest, array_access) {
    bp::structure s;
    s["a"] = {1, 2, 3};
    ASSERT_EQ(s.at("a"_h).size(), 3);
    ASSERT_EQ(s.at("a"_h)[0].as<int>(), 1);
    ASSERT_EQ(s.at("a"_h)[1].as<int>(), 2);
    ASSERT_EQ(s.at("a"_h)[2].as<int>(), 3);
    ASSERT_EQ(s["a"][0].as<int>(), 1);
    ASSERT_EQ(s["a"][1].as<int>(), 2);
    ASSERT_EQ(s["a"][2].as<int>(), 3);

    s.create_array("b");
    ASSERT_TRUE(s["b"].clear());
//    ASSERT_NO_THROW(s.erase("b"_h));
    ASSERT_TRUE(s.at("b"_h).empty());
    ASSERT_TRUE(s.erase("b"_h));
}

TEST(StructTest, array_insert) {
    bp::structure s;
    s["a"] = {1, 2, 3};
#ifdef USE_EXCEPTIONS
    ASSERT_NO_THROW(s["a"].append(4));
    ASSERT_THROW(s.append({2, 3, 4}), std::range_error);
    ASSERT_EQ(s["a"][3].as<int>(), 4);
    ASSERT_EQ(s["a"].size(), 4);


    s["a"].append({2, 3});
    ASSERT_TRUE(s["a"][4].is_array());
    ASSERT_EQ(s["a"][4][0].as<int>(), 2);
    ASSERT_EQ(s["a"][4][1].as<int>(), 3);
    s["a"].append({{"a", 3}});
    ASSERT_TRUE(s["a"][5].is_object());
    ASSERT_EQ(s["a"][5]["a"].as<int>(), 3);
#endif
}

TEST(StructTest, assignment) {
    bp::structure s;
    ASSERT_TRUE(s.is_null()) << "check is null";
    s = 1;
    ASSERT_TRUE(s.is_int()) << "check is int";
    ASSERT_EQ(s.as<int>(), 1);
    s = 1.0f;
    ASSERT_TRUE(s.is_float()) << "check is float";
    ASSERT_EQ(s.as<float>(), 1.0);
    s = "s";
    ASSERT_TRUE(s.is_string()) << "check is string";
    ASSERT_EQ(s.as<std::string>(), "s");
    s = {1, 2};
    ASSERT_TRUE(s.is_array()) << "check is array";
    ASSERT_EQ(s[0].as<int>(), 1);
    s = {{"a", "b"}};
    ASSERT_TRUE(s.is_object()) << "check is object";
    ASSERT_EQ(s["a"].as<std::string>(), "b");

    s.at("a"_h) = bp::structure({{"b", 1}});
    ASSERT_TRUE(s.at("a"_h).is_object());
#ifdef HAS_EXCEPTIONS
    ASSERT_NO_THROW(s.at("a"_h).at("b"_h));
#endif
    ASSERT_EQ(s.at("a"_h).at("b"_h).as<int>(), 1);

    bp::structure s0 = bp::structure::create_object();
    bp::structure s1 = bp::structure::create_object();
    s1.create_array("test");
    s0["test"]=s1;
    auto s2 = s0.at("test"_h);
    ASSERT_EQ(s2.type(), s1.type());

}

TEST(StructTest, array_iterator) {
    bp::structure s;
    s = {1, 2, 3};
    uint i = 1;
    auto arr_it = s.as_array();
    ASSERT_TRUE(arr_it.begin() != arr_it.end());
    for (auto v: s.as_array()) {
        ASSERT_EQ(i, v.as<int>());
        i++;
    }
}

TEST(StructTest, object_iterator) {
    bp::structure s;
    s = {{"a", 1},
         {"b", 2},
         {"c", 3}};
    std::map<std::string, uint> kvs{{"a", 1},
                                  {"b", 2},
                                  {"c", 3}};
    auto obj_it = s.as_object();
    ASSERT_TRUE(obj_it.begin() != obj_it.end());
    for (auto kv: s.as_object()) {
        ASSERT_TRUE(kvs.count(bp::sym_name(kv.first)) > 0);
        ASSERT_EQ(kvs[bp::sym_name(kv.first)], kv.second.as<int>());
    }
}

TEST(StructTest, object_key_iterator) {
    bp::structure s;
    s = {{"a", 1},
         {"b", 2},
         {"c", 3}};
    std::vector<std::string> keys{"a", "b", "c"};
    auto obj_it = s.keys();
    ASSERT_TRUE(obj_it.begin() != obj_it.end());
    for (auto k: s.keys()) {
        ASSERT_TRUE(std::find(keys.begin(), keys.end(), bp::sym_name(k)) != keys.end());
    }
    for (const auto k: s.keys()) {
        ASSERT_TRUE(std::find(keys.begin(), keys.end(), bp::sym_name(k)) != keys.end());
    }
}

TEST(StructTest, equality) {
    bp::structure s;
    s.emplace("1", 1);
    s.emplace("a", {1, 2, 3});
    s.emplace("o", {{"a", 1}});
    s.emplace({{"oa", 1},
               {"ob", 2}});

    bp::structure s1 = s.deepcopy();
    bp::structure s2;

    s.emplace("1", 1);
    s.emplace("a", {1, 2, 3});
    s.emplace("o", {{"a", 2}});
    s.emplace({{"oa", 1},
               {"ob", 2}});

    ASSERT_TRUE(s == s1);
    ASSERT_TRUE(s != s2);
}

TEST(StructTest, getters) {
    bp::structure s;
    s.emplace("i", 1);
    s.emplace("f", 1.0f);
    s.emplace("b", true);
    s.emplace("s", "s");

    ASSERT_EQ(s.get<int>("i"_h), s.at("i"_h).as<int>());
    ASSERT_EQ(s.get<float>("f"_h), s.at("f"_h).as<float>());
    ASSERT_EQ(s.get<bool>("b"_h), s.at("b"_h).as<bool>());
    ASSERT_EQ(s.get<std::string>("s"_h), s.at("s"_h).as<std::string>());
    ASSERT_EQ(s.get<std::string>("s"_h), "s");
}