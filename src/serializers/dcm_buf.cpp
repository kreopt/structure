#include "structure.hpp"
#include "serializers/dcm_buf.hpp"

using size_block = uint32_t;

namespace bp {
    using namespace serializable;

    template<>
    std::string structure::stringify<serializers::Dcm>() const {
        // entry ::= <type>[len]<data>;
        // object_entry ::= <key><val_entry>

        std::string r;
        auto tp = type();
        value_type ntp = tp;
        r += char(ntp);
        size_block sz;
        switch (tp) {
            case value_type::Object: {
                auto val = get_variant<bp::serializable::object_ptr>(val_);
                sz = static_cast<size_block>(val->size());
                r.append(reinterpret_cast<char *>(&sz), sizeof(size_block));

                for (auto item: *val) {
                    size_block key_size = static_cast<size_block >(item.first.name().size());
                    r.append(reinterpret_cast<const char *>(&key_size), sizeof(size_block));
                    r.append(item.first.name());
                    r.append(structure(item.second).stringify<serializers::Dcm>());
                }
                break;
            }
            case value_type::Array: {
                sz = static_cast<size_block>(this->size());
                r.append(reinterpret_cast<char *>(&sz), sizeof(size_block));
                for (uint i = 0; i < sz; i++) {
                    r.append(this->at(i).stringify<serializers::Dcm>());
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
                    auto val = get_value<serializable::symbol>(val_);
                    size_block key_size = static_cast<size_block >(val.name().size());
                    r.append(reinterpret_cast<const char *>(&key_size), sizeof(size_block));
                    r.append(val.name());
                }
                break;
            }
        }
        return r;
    };

    variant_ptr parse_variant(std::string::const_iterator &_it) {

        structure::value_type tp = static_cast<structure::value_type >(_it[0]);
        size_block sz = 0;
        _it++;

        switch (tp) {
            case structure::value_type::Object:
            case structure::value_type::Array:
            case structure::value_type::String:
            case structure::value_type::Symbol:
                sz = reinterpret_cast<const size_block *>(&(*_it))[0];
                _it += sizeof(size_block);
                break;
            default:
                break;
        }

        switch (tp) {
            case structure::value_type::Object: {
                object_ptr obj = std::make_shared<object>();
                for (uint i = 0; i < sz; i++) {
                    size_block key_size = reinterpret_cast<const size_block*>(&(*_it))[0];
                    _it+=sizeof(size_block);
                    symbol key(std::string(_it, _it+key_size));
                    _it += key_size;
                    obj->emplace(key, parse_variant(_it));
                }
                return std::make_shared<variant>(obj);
            }
            case structure::value_type::Array: {
                array_ptr obj = std::make_shared<array>();
                for (uint i = 0; i < sz; i++) {
                    obj->emplace_back(parse_variant(_it));
                }
                return std::make_shared<variant>(obj);
            }
            case structure::value_type::String: {
                value_ptr obj = std::make_shared<value>(std::string(_it, _it + sz));
                _it += sz;
                return std::make_shared<variant>(obj);
            }
            case structure::value_type::Int: {
                value_ptr obj = std::make_shared<value>(
                        reinterpret_cast<const serializable::int_t *>(&(*_it))[0]);
                _it += sizeof(serializable::int_t);
                return std::make_shared<variant>(obj);
            }
            case structure::value_type::Float: {
                value_ptr obj = std::make_shared<value>(
                        reinterpret_cast<const serializable::float_t *>(&(*_it))[0]);
                _it += sizeof(serializable::float_t);
                return std::make_shared<variant>(obj);
            }
            case structure::value_type::Bool: {
                value_ptr obj = std::make_shared<value>(*_it ? true : false);
                _it += 1;
                return std::make_shared<variant>(obj);
            }
            case structure::value_type::Symbol: {
                symbol key(std::string(_it, _it+sz));
                _it += sz;
                value_ptr obj = std::make_shared<value>(key);
                return std::make_shared<variant>(obj);
            }
            case structure::value_type::Null: {
                return std::make_shared<variant>(nullptr);
            }
        }
        return std::make_shared<variant>(nullptr);
    }

    template<>
    void structure::parse<serializers::Dcm>(const std::string &_str) {
        val_.reset();
        auto it = _str.begin();
        value_type tp = static_cast<value_type >(*it);
        set_type(tp);
        try {
            val_ = parse_variant(it);
        } catch (std::exception &_e) {
            throw bp::structure::parse_error(_e.what());
        }
    };
}