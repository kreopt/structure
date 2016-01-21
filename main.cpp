#include <iostream>
#include <chrono>
#include "serializer.hpp"
#include "serializers/json.hpp"
#include "serializers/dcm_buf.hpp"

using namespace std;
using namespace literals::string_literals;
using namespace bp::literals;
int main() {
    auto tp = std::chrono::high_resolution_clock::now();
//    for (int i=0; i<1000; i++) {
        auto s = bp::serializer::create();

            s->emplace("cmd"_sym, "caps"_sym);
        s->emplace("i", 1);
        s->emplace("s", "string");
        s->emplace("b", true);
        s->emplace("f", 1.1);
        s->emplace("a", {1, "a", 1.1, true});
//        s->at("a")->append(39);
//        s->emplace("o", {{"aa", 1}, {"bb", 1}});

    for (const auto key: s->keys()) {
        std::cout << key << std::endl;
    }

    for (const auto item: s->as_object()) {
        std::cout << item.first.name << " " << item.second->as_string() << std::endl;
    }

    for (const auto item: s->at("a")->as_array()) {
        std::cout << item->as_string() << std::endl;
    }

    std::cout << "---" << std::endl;
//    std::cout << s->get("s", ":(")->as_string() << std::endl;

        std::string buf = s->stringify<bp::serializers::Dcm>();
        auto ps = bp::serializer::create();
        ps->parse<bp::serializers::Dcm>(buf);
        std::string buf1 = ps->stringify<bp::serializers::Dcm>();
//    }
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - tp).count() << "mcs" << std::endl;
    std::cout << buf << std::endl;
    std::cout << ps->get("cmd"_sym, ""_sym)->as_symbol().hash << std::endl;
    std::cout << buf1 << std::endl;

    return 0;
}