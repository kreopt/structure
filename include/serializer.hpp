#ifndef BP_SERIALIZER_HPP
#define BP_SERIALIZER_HPP

#include <string>
#include <iterator>
#include <unordered_map>
#include <boost/variant.hpp>
#include <binelpro/symbol.hpp>
#include <binelpro/util.hpp>

namespace bp {
    using namespace bp::literals;

    namespace serializable {
        using int_t = int32_t;
        using float_t = double;
        using bool_t = bool;
        using symbol_t = bp::symbol_t;
        using string_t = std::string;
    }

    namespace serializers {
        enum class type: symbol_t::hash_type {
            Dcm = "dcmbuf"_sym,
            Json = "json"_sym
        };
    }

    class serializer : public std::enable_shared_from_this<serializer>{
    public:
        using ptr = std::shared_ptr<serializer>;

        enum class value_type: char {
            Int = 'i',
            Float = 'f',
            String = 's',
            Bool = 'b',
            Null = 'n',
            Object = 'o',
            Array = 'a',
            Symbol = 'S'
        };

        using value = boost::variant<
                serializable::int_t,
                serializable::float_t,
                serializable::bool_t,
                serializable::symbol_t,
                serializable::string_t>;
        using value_ptr = std::shared_ptr<value>;

        class variant_t;
        using variant_ptr = std::shared_ptr<variant_t>;

        using object_t = std::unordered_map<bp::symbol_t, variant_ptr>;
        using object_ptr = std::shared_ptr<object_t>;

        using array_t = std::vector<variant_ptr>;
        using array_ptr = std::shared_ptr<array_t>;

        class variant_t : public boost::variant<value_ptr, object_ptr, array_ptr> {
        public:
            variant_t() : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(0)) { }
            variant_t(const serializable::int_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)) { }
            variant_t(const serializable::float_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)) { }
            variant_t(const serializable::bool_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)) { }
            variant_t(const serializable::symbol_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)) { }
            variant_t(const serializable::string_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)) { }
            variant_t(const char* _val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(std::string(_val))) { }

            variant_t(const value_ptr &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(_val) { }
            variant_t(value_ptr &&_val) : boost::variant<value_ptr, object_ptr, array_ptr>(_val) { }
            variant_t(const object_ptr &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(_val) { }
            variant_t(object_ptr &&_val) : boost::variant<value_ptr, object_ptr, array_ptr>(_val) { }
            variant_t(const array_ptr &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(_val) { }
            variant_t(array_ptr &&_val) : boost::variant<value_ptr, object_ptr, array_ptr>(_val) { }

            variant_t(const value &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)){}
            variant_t(value &&_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)){}
            variant_t(const object_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<object_t>(_val)){}
            variant_t(object_t &&_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<object_t>(_val)){}
            variant_t(const array_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<array_t>(_val)){}
            variant_t(array_t &&_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<array_t>(_val)){}
        };

    private:
        value_type value_type_ = value_type::Null;

        void initialize_if_null(bp::serializer::value_type _type);

        template <typename V, class = typename std::enable_if<bp::is_any_of<V, array_t, object_t>::value>::type>
        void initialize_val() {
            if (val_) {
                *val_ = std::make_shared<V>();
            } else {
                val_ = std::make_shared<variant_t>(std::make_shared<V>());
            }
        }

        serializer(bp::serializer::variant_ptr _obj = nullptr);
    protected:
        variant_ptr val_;

        inline void set_type(value_type _type) {value_type_ = _type;};
        struct value_type_visitor : public boost::static_visitor<value_type> {
            value_type operator()(serializable::int_t _val) const { return value_type::Int; }
            value_type operator()(serializable::int_t &&_val) const { return value_type::Int; }
            value_type operator()(serializable::float_t _val) const { return value_type::Float; }
            value_type operator()(serializable::float_t &&_val) const { return value_type::Float; }
            value_type operator()(serializable::bool_t _val) const { return value_type::Bool; }
            value_type operator()(serializable::bool_t &&_val) const { return value_type::Bool; }
            value_type operator()(const serializable::symbol_t &_val) const { return value_type::Symbol; }
            value_type operator()(serializable::symbol_t &&_val) const { return value_type::Symbol; }
            value_type operator()(const serializable::string_t & _val) const { return value_type::String; }
            value_type operator()(serializable::string_t && _val) const { return value_type::String; }
        };
        struct variant_visitor : public boost::static_visitor<bp::serializer::value_type> {
            value_type operator()(value_ptr _val) const {
                if (_val) {
                    return boost::apply_visitor(value_type_visitor(), *_val);
                } else {
                    return value_type::Null;
                }
            }

            value_type operator()(object_ptr _val) const { return value_type::Object; }

            value_type operator()(array_ptr _val) const { return value_type::Array; }
        };

        template <typename V, class = typename std::enable_if<bp::is_any_of<V, array_ptr, object_ptr, value_ptr>::value>::type>
        V get_variant(variant_ptr _val) const {
            return boost::get<V>(*_val);
        }

        template <typename V>
        V get_value(variant_ptr _val) const {
            return boost::get<V>(*get_variant<value_ptr>(_val));
        }
    public:
        serializable::string_t  as_string() const;
        serializable::int_t     as_int() const;
        serializable::float_t   as_float() const;
        serializable::bool_t    as_bool() const;
        serializable::symbol_t  as_symbol() const;

        inline bool is_int() const { return value_type_ == value_type::Int; }
        inline bool is_float() const {return value_type_ == value_type::Float; }
        inline bool is_bool() const { return value_type_ == value_type::Bool; }
        inline bool is_string() const { return value_type_ == value_type::String; }
        inline bool is_null() const { return value_type_ == value_type::Null; }
        inline bool is_array() const { return value_type_ == value_type::Array; }
        inline bool is_object() const { return value_type_ == value_type::Object; }
        inline bool is_symbol() const { return value_type_ == value_type::Symbol; }

        inline value_type type() const { return value_type_; };

        // arrays:
        size_t size() const;
        serializer::ptr at(int index);
        const serializer::ptr at(int index) const;
        void append(const variant_t &_val);
        void append(variant_t &&_val);
        bool append(const std::initializer_list<variant_t> &_val);
        bool append(const std::initializer_list<std::pair<std::string, variant_t>> &_val);

        // objects:
        serializer::ptr at(const char *_key);
        const serializer::ptr at(const char *_key) const;
        serializer::ptr at(const std::string &_key);
        const serializer::ptr at(const std::string &_key) const;
        serializer::ptr at(const symbol_t &_key);
        const serializer::ptr at(const symbol_t &_key) const;

        bool emplace(const std::string &_key, const variant_t &_val) ;
        bool emplace(const std::string &_key, variant_t &&_val) ;
        bool emplace(const std::string &_key, const std::initializer_list<variant_t> &_val) ;
        bool emplace(const std::string &_key, const std::initializer_list<std::pair<bp::symbol_t, variant_t>> &_val) ;
        bool emplace(const symbol_t &_key, const variant_t &_val) ;
        bool emplace(const symbol_t &_key, variant_t &&_val) ;
        bool emplace(const symbol_t &_key, const std::initializer_list<variant_t> &_val) ;
        bool emplace(const symbol_t &_key, const std::initializer_list<std::pair<bp::symbol_t, variant_t>> &_val) ;
        bool emplace(symbol_t &&_key, const variant_t &_val) ;
        bool emplace(symbol_t &&_key, variant_t &&_val) ;
        bool emplace(symbol_t &&_key, const std::initializer_list<variant_t> &_val) ;
        bool emplace(symbol_t &&_key, const std::initializer_list<std::pair<bp::symbol_t, variant_t>> &_val) ;
        bool emplace(const char* _key, const variant_t &_val) ;
        bool emplace(const char* _key, variant_t &&_val) ;
        bool emplace(const char* _key, const std::initializer_list<variant_t> &_val) ;
        bool emplace(const char* _key, const std::initializer_list<std::pair<bp::symbol_t, variant_t>> &_val) ;

        serializer::ptr get(const std::string &_key, const variant_t &_default) const ;
        serializer::ptr get(const std::string &_key, variant_t &&_default) const ;
        serializer::ptr get(const symbol_t &_key, const variant_t &_default) const ;
        serializer::ptr get(const symbol_t &_key, variant_t &&_default) const ;
        serializer::ptr get(const char* _key, const variant_t &_default) const ;
        serializer::ptr get(const char* _key, variant_t &&_default) const ;

        serializer::ptr set(variant_t &&_val) ;
        serializer::ptr set(const variant_t &_val) ;  // atom
        serializer::ptr set(const std::initializer_list<variant_t> &_val) ;   // array
        serializer::ptr set(const std::initializer_list<std::pair<std::string, variant_t>> &_val) ;   // object

        //

        template <symbol_t::hash_type serializer_type>
        std::string stringify() const {return std::string();};

        template <symbol_t::hash_type serializer_type>
        void parse(const std::string &_str) {};

        static serializer::ptr create(variant_ptr _obj = nullptr);

        // ITERATORS

        class object_key_iterator : public std::iterator<std::input_iterator_tag, const char*> {
            const serializer::ptr object_;
            object_t::iterator    it;
            object_t::iterator    end;
        public:
            object_key_iterator(const serializer::ptr &_object);
            object_key_iterator(const object_key_iterator&_it);
            object_key_iterator & operator++();
            object_key_iterator operator++(int);
            bool operator==(const object_key_iterator& rhs);
            bool operator!=(const object_key_iterator& rhs);
            const char * operator*();
        };
        class object_iterator : public std::iterator<std::forward_iterator_tag, serializer::ptr> {
            const serializer::ptr object_;
            object_t::iterator    it;
            object_t::iterator    end;
        public:
            object_iterator(const serializer::ptr &_object);
            object_iterator(const object_iterator&_it);
            object_iterator & operator++();
            object_iterator operator++(int);
            bool operator==(const object_iterator& rhs);
            bool operator!=(const object_iterator& rhs);
            std::pair<bp::serializer::object_t::key_type, bp::serializer::ptr>  operator*();
        };

        class array_iterator : public std::iterator<std::random_access_iterator_tag, serializer::ptr> {
            const serializer::ptr object_;
            size_t  index;
            size_t  size;
        public:
            array_iterator(const serializer::ptr &_object);
            array_iterator(const array_iterator&_it);
            array_iterator & operator++();
            array_iterator operator++(int);
            bool operator==(const array_iterator& rhs);
            bool operator!=(const array_iterator& rhs);
            bp::serializer::ptr  operator*();
        };

        template <typename iterator_type>
        class iterable {
            iterator_type it_;
        public:
            iterable(const iterator_type &_it) : it_(_it){}
            iterable(const iterable &_it) : it_(_it.it_) {}
            iterator_type begin() {return it_;}
            iterator_type end() { return iterator_type(nullptr);}
        };
        template <typename iterator_type>
        iterable<iterator_type> get_iterable() {
            return iterable<iterator_type>(iterator_type(this->shared_from_this()));
        }

        iterable<object_key_iterator> keys() {return this->get_iterable<object_key_iterator>();}
        iterable<object_iterator> as_object() {return this->get_iterable<object_iterator>();}
        iterable<array_iterator> as_array() {return this->get_iterable<array_iterator>();}
    };
}


#endif //BP_SERIALIZER_HPP
