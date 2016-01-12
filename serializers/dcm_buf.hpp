#ifndef SERIALIZERS_DCM_BUF_HPP
#define SERIALIZERS_DCM_BUF_HPP

#include <unordered_map>
#include <binelpro/symbol.hpp>
#include "serializer.hpp"

namespace serializers {
    class dcm_buf : public serializers::serializer, public std::enable_shared_from_this<dcm_buf>{

        class _var_t;
        using var_t = std::shared_ptr<_var_t>;
        using val_t = std::shared_ptr<value>;
        using _obj_t = std::unordered_map<bp::symbol_t, var_t>;
        using obj_t = std::shared_ptr<_obj_t>;
        using _arr_t = std::vector<var_t>;
        using arr_t = std::shared_ptr<_arr_t>;

        class _var_t : public boost::variant<val_t, obj_t, arr_t> {
        public:
            _var_t(const val_t &_val) : boost::variant<val_t, obj_t, arr_t>(_val){}
            _var_t(val_t &&_val) : boost::variant<val_t, obj_t, arr_t>(_val){}
            _var_t(const obj_t &_val) : boost::variant<val_t, obj_t, arr_t>(_val){}
            _var_t(obj_t &&_val) : boost::variant<val_t, obj_t, arr_t>(_val){}
            _var_t(const arr_t &_val) : boost::variant<val_t, obj_t, arr_t>(_val){}
            _var_t(arr_t &&_val) : boost::variant<val_t, obj_t, arr_t>(_val){}
        };

        struct variant_visitor : public boost::static_visitor<serializers::serializer::value_type> {
            value_type operator()(val_t _val) const { return boost::apply_visitor(value_type_visitor(), *_val); }
            value_type operator()(obj_t _val) const { return value_type::Object; }
            value_type operator()(arr_t _val) const { return value_type::Array; }
        };

        var_t val_;

        dcm_buf(var_t _obj);

        static serializer::ptr create(var_t _obj);

    public:
        static serializer::ptr create();
        static serializer::ptr create_object();
        static serializer::ptr create_array();

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


#endif //SERIALIZERS_DCM_BUF_HPP
