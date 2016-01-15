#include "serializer.hpp"
#include "dcm_buf.hpp"

using size_block = uint32_t;

namespace bp {

    template<>
    std::string serializer::stringify<serializers::type::Dcm>() const {
        // entry ::= <type>[len]<data>;
        // object_entry ::= <key><val_entry>

        std::string r;
        auto tp = type();
        value_type ntp = tp;
        r += char(ntp);
        size_block sz;
        switch (tp) {
            case value_type::Object: {
                auto val = get_variant<object_ptr>(val_);
                sz = static_cast<size_block>(val->size());
                r.append(reinterpret_cast<char *>(&sz), sizeof(size_block));
                for (auto item: *val) {
                    size_block key_size = static_cast<size_block >(sizeof(item.first.name));
                    r.append(reinterpret_cast<const char *>(&item.first.hash), sizeof(symbol_t::hash_type));
                    r.append(reinterpret_cast<const char *>(&key_size), sizeof(size_block));
                    r.append(reinterpret_cast<const char *>(&item.first.name), key_size);
                    r.append(create(item.second)->stringify<serializers::type::Dcm>());
                }
                break;
            }
            case value_type::Array: {
                sz = static_cast<size_block>(this->size());
                r.append(reinterpret_cast<char *>(&sz), sizeof(size_block));
                for (int i = 0; i < sz; i++) {
                    r.append(this->at(i)->stringify<serializers::type::Dcm>());
                }
                break;
            }
            default: {
                if (tp == value_type::String) {
                    auto val = get_value<std::string>(val_);
                    sz = static_cast<size_block>(val.size());
                    r.append(reinterpret_cast<char *>(&sz), sizeof(size_block));
                    r.append(val);
                } else if (tp == value_type::Int) {
                    auto val = get_value<serializable::int_t>(val_);
                    r.append(reinterpret_cast<const char *>(&val), sizeof(serializable::int_t));
                } else if (tp == value_type::Float) {
                    auto val = get_value<serializable::float_t>(val_);
                    r.append(reinterpret_cast<const char *>(&val), sizeof(serializable::float_t));
                } else if (tp == value_type::Bool) {
                    auto val = get_value<serializable::bool_t>(val_);
                    r.append(val ? '\1' : '\0', 1);
                } else if (is_symbol()) {
                    auto val = get_value<serializable::symbol_t>(val_);
                    size_block key_size = static_cast<size_block >(sizeof(val.name));
                    r.append(reinterpret_cast<const char *>(&val.hash), sizeof(symbol_t::hash_type));
                    r.append(reinterpret_cast<const char *>(&key_size), sizeof(size_block));
                    r.append(reinterpret_cast<const char *>(&val.name), key_size);
                }
                break;
            }
        }
        return r;
    };

    serializer::variant_ptr parse_variant(std::string::const_iterator &_it) {

        serializer::value_type tp = static_cast<serializer::value_type >(_it[0]);
        size_block sz;
        _it++;

        switch (tp) {
            case serializer::value_type::Object:
            case serializer::value_type::Array:
            case serializer::value_type::String:
                sz = reinterpret_cast<const size_block *>(&(*_it))[0];
                _it += sizeof(size_block);
                break;
            default:
                break;
        }

        switch (tp) {
            case serializer::value_type::Object: {
                serializer::object_ptr obj = std::make_shared<serializer::object_t>();
                for (int i = 0; i < sz; i++) {
                    symbol_t::hash_type hash = reinterpret_cast<const symbol_t::hash_type*>(&(*_it))[0];
                    _it+=sizeof(symbol_t::hash_type);
                    size_block key_size = reinterpret_cast<const size_block*>(&(*_it))[0];
                    _it+=sizeof(size_block);
                    symbol_t key(std::string(_it, _it+key_size).c_str());
                    _it += key_size;
                    obj->emplace(key, parse_variant(_it));
                }
                return std::make_shared<serializer::variant_t>(obj);
            }
            case serializer::value_type::Array: {
                serializer::array_ptr obj = std::make_shared<serializer::array_t>();
                for (int i = 0; i < sz; i++) {
                    obj->emplace_back(parse_variant(_it));
                }
                return std::make_shared<serializer::variant_t>(obj);
            }
            case serializer::value_type::String: {
                serializer::value_ptr obj = std::make_shared<serializer::value>(std::string(_it, _it + sz));
                _it += sz;
                return std::make_shared<serializer::variant_t>(obj);
            }
            case serializer::value_type::Int: {
                serializer::value_ptr obj = std::make_shared<serializer::value>(
                        reinterpret_cast<const serializable::int_t *>(&(*_it))[0]);
                _it += sizeof(serializable::int_t);
                return std::make_shared<serializer::variant_t>(obj);
            }
            case serializer::value_type::Float: {
                serializer::value_ptr obj = std::make_shared<serializer::value>(
                        reinterpret_cast<const serializable::float_t *>(&(*_it))[0]);
                _it += sizeof(serializable::float_t);
                return std::make_shared<serializer::variant_t>(obj);
            }
            case serializer::value_type::Bool: {
                serializer::value_ptr obj = std::make_shared<serializer::value>(*_it ? true : false);
                _it += 1;
                return std::make_shared<serializer::variant_t>(obj);
            }

        }
    }

    template<>
    void serializer::parse<serializers::type::Dcm>(const std::string &_str) {
        val_.reset();
        auto it = _str.begin();
        value_type tp = static_cast<value_type >(*it);
        set_type(tp);
        val_ = parse_variant(it);
    };
}