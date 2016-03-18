#include "structure.hpp"
#include <gtest/gtest.h>

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
    // TODO
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
    //TODO
}

TEST(StructTest, array_insert) {
    //TODO
}

TEST(StructTest, assignment) {
    // TODO
}

TEST(StructTest, array_iterator) {
    // TODO
}

TEST(StructTest, object_iterator) {
    // TODO
}

TEST(StructTest, object_key_iterator) {
    // TODO
}

TEST(StructTest, operator_bool) {
    // TODO
}