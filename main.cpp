#include <iostream>
#include <chrono>
#include "structure.hpp"
#include "serializers/json.hpp"
#include "serializers/dcm_buf.hpp"

using namespace std;
using namespace literals::string_literals;
using namespace bp::literals;

int main() {
    auto s = bp::structure(/*{
                                               {"event"_sym, "test"},
                                               {"data"_sym, {
                                                   {"a", 1},
                                                   {"b", 2}
                                               }}
                                       }*/);

    s.emplace("event", "caps");
    bp::structure ss(s);
    bp::serializable::variant var;
    ss.at("event") = "test";
    ss.get("event", "");
    ss.emplace({
                       {"struct", *s.data()}
               });
    s.emplace("data"_sym, {
            {"mount"_sym, "dd"},
            {"caps"_sym, "cc"},
            {"endpoint"_sym, "ep"}});
    s.emplace({
                      {"a", "b"},
                      {"c", "d"}
              });


    std::string buf = s.stringify<bp::serializers::Json>();
    std::cout << buf << std::endl <<
        ss.stringify<bp::serializers::Json>() << std::endl <<
        static_cast<char>(s.get("d").type()) << std::endl;

    return 0;
}