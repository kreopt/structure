#ifndef SERIALIZERS_DCM_BUF_HPP
#define SERIALIZERS_DCM_BUF_HPP

#include <unordered_map>
#include <binelpro/symbol.hpp>
#include "serializer.hpp"
namespace bp {

    template<typename T, typename U, typename... Us>
    struct is_any_of : std::integral_constant<bool,
                    std::conditional<std::is_same<T,U>::value, std::true_type, is_any_of<T,Us...>>::type::value>
    { };

    template<typename T, typename U>
    struct is_any_of<T,U> : std::is_same<T,U>::type { };

    using size_block = uint32_t;

    namespace serializers {
        class dcm_buf : public bp::serializer, public std::enable_shared_from_this<dcm_buf> {
            // TODO: move to base class
            class _var_t;

            using var_t = std::shared_ptr<_var_t>;
            using val_t = std::shared_ptr<value>;
            using _obj_t = std::unordered_map<bp::symbol_t, var_t>;
            using obj_t = std::shared_ptr<_obj_t>;
            using _arr_t = std::vector<var_t>;
            using arr_t = std::shared_ptr<_arr_t>;

            class _var_t : public boost::variant<val_t, obj_t, arr_t> {
            public:
                _var_t(const val_t &_val) : boost::variant<val_t, obj_t, arr_t>(_val) { }
                _var_t(val_t &&_val) : boost::variant<val_t, obj_t, arr_t>(_val) { }
                _var_t(const obj_t &_val) : boost::variant<val_t, obj_t, arr_t>(_val) { }
                _var_t(obj_t &&_val) : boost::variant<val_t, obj_t, arr_t>(_val) { }
                _var_t(const arr_t &_val) : boost::variant<val_t, obj_t, arr_t>(_val) { }
                _var_t(arr_t &&_val) : boost::variant<val_t, obj_t, arr_t>(_val) { }
                _var_t(const value &_val) : boost::variant<val_t, obj_t, arr_t>(std::make_shared<value>(_val)){}
                _var_t(value &&_val) : boost::variant<val_t, obj_t, arr_t>(std::make_shared<value>(_val)){}
                _var_t(const _obj_t &_val) : boost::variant<val_t, obj_t, arr_t>(std::make_shared<_obj_t>(_val)){}
                _var_t(_obj_t &&_val) : boost::variant<val_t, obj_t, arr_t>(std::make_shared<_obj_t>(_val)){}
                _var_t(const _arr_t &_val) : boost::variant<val_t, obj_t, arr_t>(std::make_shared<_arr_t>(_val)){}
                _var_t(_arr_t &&_val) : boost::variant<val_t, obj_t, arr_t>(std::make_shared<_arr_t>(_val)){}
            };

            struct variant_visitor : public boost::static_visitor<bp::serializer::value_type> {
                value_type operator()(val_t _val) const {
                    if (_val) {
                        return boost::apply_visitor(value_type_visitor(), *_val);
                    } else {
                        return value_type::Null;
                    }
                }

                value_type operator()(obj_t _val) const { return value_type::Object; }

                value_type operator()(arr_t _val) const { return value_type::Array; }
            };

            var_t val_;

            dcm_buf(var_t _obj);

            static serializer::ptr create(var_t _obj);

            template <typename V, class = typename std::enable_if<bp::is_any_of<V, arr_t, obj_t, val_t>::value>::type>
            V get_variant() const {
                return boost::get<V>(*val_);
            }

            template <typename V>
            V get_value() const {
                return boost::get<V>(*get_variant<val_t>());
            }

            void initialize_if_null(bp::serializer::value_type _type);

            template <typename V, class = typename std::enable_if<bp::is_any_of<V, _arr_t, _obj_t>::value>::type>
            void initialize_val() {
                if (val_) {
                    *val_ = std::make_shared<V>();
                } else {
                    val_ = std::make_shared<_var_t>(std::make_shared<V>());
                }
            }

        public:
            static serializer::ptr create();

            virtual std::string as_string() const override;

            virtual serializable::int_t as_int() const override;

            virtual serializable::float_t as_float() const override;

            virtual bool as_bool() const override;

            virtual size_t size() const override;

            virtual serializer::ptr at(int index) override;

            virtual const serializer::ptr at(int index) const override;

            virtual void append(const value &_val) override;

            virtual void append(value &&_val) override;


            virtual bool append(const std::initializer_list<value> &_val) override;

            virtual bool append(const std::initializer_list<std::pair<std::string, value>> &_val) override;

            virtual bool emplace(const std::string &_key, const std::initializer_list<value> &_val) override;

            virtual bool emplace(const std::string &_key,
                                 const std::initializer_list<std::pair<std::string, value>> &_val) override;

            virtual serializer::ptr at(const char *_key) override;

            virtual const serializer::ptr at(const char *_key) const override;

            virtual bool emplace(const std::string &_key, const value &_val) override;

            virtual bool emplace(const std::string &_key, value &&_val) override;

            virtual serializer::ptr get(const std::string &_key, const value &_default) const override;

            virtual serializer::ptr get(const std::string &_key, value &&_default) const override;

            virtual serializer::ptr set(value &&_val) override;

            virtual serializer::ptr set(const value &_val) override;

            virtual serializer::ptr set(const std::initializer_list<value> &_val) override;

            virtual serializer::ptr set(const std::initializer_list<std::pair<std::string, value>> &_val) override;

            virtual std::string stringify() const override;

            virtual void parse(const std::string &_str) override;
        };
    }
}

#endif //SERIALIZERS_DCM_BUF_HPP
