#include "structure.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace bp::literals;

TEST(StructTest, constructors) {
    bp::structure s;
    s["a"] = 1;

    bp::structure s_copy(s);
    bp::structure s_deepcopy(std::move(s.deepcopy()));
    bp::structure s_move(std::move(s));
    s_copy["a"]=3;

    ASSERT_EQ(static_cast<bool>(s), false);
    ASSERT_EQ(static_cast<bool>(s_copy), true);
    ASSERT_EQ(static_cast<bool>(s_deepcopy), true);
    ASSERT_EQ(static_cast<bool>(s_move), true);
    ASSERT_EQ(s_copy.at("a").is_int(), true);
    ASSERT_EQ(s_deepcopy.at("a").is_int(), true);
    ASSERT_EQ(s_move.at("a").is_int(), true);
    ASSERT_EQ(s.is_null(), true);
    ASSERT_EQ(s_copy.is_object(), true);
    ASSERT_EQ(s_deepcopy.is_object(), true);
    ASSERT_EQ(s_move.is_object(), true);
    ASSERT_EQ(s_copy.at("a").as_int(), 3);
    ASSERT_EQ(s_deepcopy.at("a").as_int(), 1);
    ASSERT_EQ(s_move.at("a").as_int(), 3);
}

TEST(StructTest, object_access) {
    bp::structure s;
    s["a"] = 1;

    ASSERT_NO_THROW(s.at("a")) << "failed to access object key";
    ASSERT_EQ(s.type(), bp::structure::value_type::Object) << "structure type is not an object";
    ASSERT_EQ(s.at("a").as_int(), 1) << "bad value for .at()";
    ASSERT_EQ(s["a"].as_int(), 1) << "bad value for operator[]";
    ASSERT_EQ(s.size(), 1) << "bad object size";

    ASSERT_THROW(s["a"_sym]["b"_sym]=1, std::range_error) << "insertion to non-object does not throw";

    s.at("a")=2;
    ASSERT_EQ(s.at("a").as_int(), 2)  << "bad value after change";

    ASSERT_NO_THROW(s["b"]=2) << "exception on insert into object";
    ASSERT_EQ(s.size(), 2) << "bad size afer insertion";
}

TEST(StructTest, object_access_get) {
    bp::structure s;
    s["a"] = 1;

    ASSERT_EQ(s.get("b").as_string(), std::string(""));
    ASSERT_EQ(s.get("b", 1).as_int(), 1);
    ASSERT_EQ(s.get("a"), 1);
    ASSERT_EQ(s.get("a", 2), 1);
}

TEST(StructTest, object_erase) {
    bp::structure s;
    s["a"] = 1;

    ASSERT_THROW(s.at("a").erase("b"), std::range_error)  << "erase non-object does not throw";
    s.erase("a");
    ASSERT_EQ(s.size(), 0) << "Size after erasing does not decreased";
    ASSERT_EQ(s.type(), bp::structure::value_type::Object)  << "empty object does not remains object";
}

TEST(StructTest, object_emplace) {
    bp::structure s, s1;
    s1["a"] = 1;

    s.emplace("1", 1);
    s.emplace("str", s1.deepcopy());
    s.emplace("a", {1,2,3});
    s.emplace("o", {{"a",1}});
    s.emplace({{"oa", 1}, {"ob", 2}});
    s.emplace({{"as", s1}});
    s.at("as").at("a")=2;

    ASSERT_EQ(s.at("1").as_int(), 1);
    ASSERT_NO_THROW(s.at("str").at("a"));
    ASSERT_EQ(s.at("str").at("a").as_int(), 1);
    ASSERT_EQ(s.at("a").is_array(), true);
    ASSERT_EQ(s.at("a").size(), 3);
    ASSERT_NO_THROW(s.at("a")[0]);
    ASSERT_EQ(s.at("a")[0], 1);
    ASSERT_EQ(s.at("o").is_object(), true);
    ASSERT_EQ(s.at("o").size(), 1);
    ASSERT_NO_THROW(s.at("o").at("a"));
    ASSERT_EQ(s.at("o").at("a").as_int(), 1);
    ASSERT_EQ(s.at("as").at("a").as_int(), 2);

    ASSERT_EQ(s.at("oa").as_int(), 1);
    ASSERT_EQ(s.at("ob").as_int(), 2);

    ASSERT_EQ(s.size(), 7);
}

TEST(StructTest, data_types) {
    bp::structure s;
    s["a"] = 1;
    auto v = s.at("a");
    ASSERT_EQ(v.as_int(), 1)  << "as_int(int)";
    ASSERT_EQ(v.as_string(), std::string("1"))  << "as_string(int)";
    ASSERT_EQ(v.as_bool(), true)  << "as_bool(int!=0)";
    ASSERT_EQ(v.as_float(), 1.0)  << "as_float(int)";
    v = s.at("a");
    s["a"] = 0;
    ASSERT_EQ(v.as_bool(), false)  << "as_bool(int==0)";

    s["a"] = true;
    v = s.at("a");
    ASSERT_EQ(v.as_int(), 1)  << "as_int(bool)";
    ASSERT_EQ(v.as_string(), std::string("true"))  << "as_string(bool==true)";
    ASSERT_EQ(v.as_bool(), true)  << "as_bool(bool==true)";
    ASSERT_EQ(v.as_float(), 0)  << "as_float(bool)";
    s["a"] = false;
    ASSERT_EQ(v.as_bool(), false)  << "as_bool(bool==false)";
    ASSERT_EQ(v.as_string(), std::string("false"))  << "as_string(bool==false)";

    s["a"] = 1.0;
    v = s.at("a");
    ASSERT_EQ(v.as_int(), 1)  << "as_int(float)";
    ASSERT_EQ(v.as_string(), std::to_string(1.0))  << "as_string(float)";
    ASSERT_EQ(v.as_bool(), false)  << "as_bool(float)";
    ASSERT_EQ(v.as_float(), 1.0)  << "as_float(float)";

    s["a"] = "str";
    v = s.at("a");
    ASSERT_EQ(v.as_int(), 0)  << "as_int(string)";
    ASSERT_EQ(v.as_string(), std::string("str"))  << "as_string(string)";
    ASSERT_EQ(v.as_bool(), false)  << "as_bool(string)";
    ASSERT_EQ(v.as_float(), 0)  << "as_float(string)";
}

TEST(StructTest, array_access) {
    bp::structure s;
    s["a"]={1,2,3};
    ASSERT_EQ(s.at("a").size(), 3);
    ASSERT_EQ(s.at("a")[0].as_int(), 1);
    ASSERT_EQ(s.at("a")[1].as_int(), 2);
    ASSERT_EQ(s.at("a")[2].as_int(), 3);
    ASSERT_EQ(s["a"][0].as_int(), 1);
    ASSERT_EQ(s["a"][1].as_int(), 2);
    ASSERT_EQ(s["a"][2].as_int(), 3);
}

TEST(StructTest, array_insert) {
    bp::structure s;
    s["a"]={1,2,3};
    ASSERT_NO_THROW(s["a"].append(4));
    ASSERT_THROW(s.append({2,3,4}), std::range_error);
    ASSERT_EQ(s["a"].size(), 4);
    ASSERT_EQ(s["a"][3].as_int(), 4);

    s["a"].append({2,3});
    ASSERT_TRUE(s["a"][4].is_array());
    ASSERT_EQ(s["a"][4][0].as_int(), 2);
    ASSERT_EQ(s["a"][4][1].as_int(), 3);
    s["a"].append({{"a",3}});
    ASSERT_TRUE(s["a"][5].is_object());
    ASSERT_EQ(s["a"][5]["a"].as_int(), 3);
}

TEST(StructTest, assignment) {
    bp::structure s;
    ASSERT_TRUE(s.is_null()) << "check is null";
    s = 1;
    ASSERT_TRUE(s.is_int()) << "check is int";
    ASSERT_EQ(s.as_int(), 1);
    s = 1.0;
    ASSERT_TRUE(s.is_float()) << "check is float";
    ASSERT_EQ(s.as_float(), 1.0);
    s = "s";
    ASSERT_TRUE(s.is_string()) << "check is string";
    ASSERT_EQ(s.as_string(), "s");
    s = {1,2};
    ASSERT_TRUE(s.is_array()) << "check is array";
    ASSERT_EQ(s[0].as_int(), 1);
    s = {{"a", "b"}};
    ASSERT_TRUE(s.is_object()) << "check is object";
    ASSERT_EQ(s["a"].as_string(), "b");
}

TEST(StructTest, array_iterator) {
    bp::structure s;
    s={1,2,3};
    uint i=1;
    auto arr_it = s.as_array();
    ASSERT_TRUE(arr_it.begin() != arr_it.end());
    for (auto v: s.as_array()) {
        ASSERT_EQ(i, v.as_int());
        i++;
    }
}

TEST(StructTest, object_iterator) {
    bp::structure s;
    s={{"a",1},{"b",2},{"c",3}};
    std::unordered_map<std::string, uint> kvs{{"a",1},{"b",2},{"c",3}};
    auto obj_it = s.as_object();
    ASSERT_TRUE(obj_it.begin() != obj_it.end());
    for (auto kv: s.as_object()) {
        ASSERT_TRUE(kvs.count(kv.first)>0);
        ASSERT_EQ(kvs[kv.first.name()], kv.second.as_int());
    }
}

TEST(StructTest, object_key_iterator) {
    bp::structure s;
    s={{"a",1},{"b",2},{"c",3}};
    std::vector<std::string> keys{"a","b","c"};
    auto obj_it = s.keys();
    ASSERT_TRUE(obj_it.begin() != obj_it.end());
    for (auto k: s.keys()) {
        ASSERT_TRUE(std::find(keys.begin(), keys.end(), k)!=keys.end());
    }
    for (const auto k: s.keys()) {
        ASSERT_TRUE(std::find(keys.begin(), keys.end(), k)!=keys.end());
    }
}

TEST(StructTest, operator_bool) {
    bp::structure s;
    ASSERT_FALSE(static_cast<bool>(s));
    s = 1;
    ASSERT_TRUE(static_cast<bool>(s));
}