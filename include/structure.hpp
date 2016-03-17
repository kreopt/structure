#ifndef BP_SERIALIZER_HPP
#define BP_SERIALIZER_HPP

#include <string>
#include <iterator>
#include <unordered_map>
#include <vector>
#include <boost/variant.hpp>
#include <binelpro/symbol.hpp>
#include <binelpro/util.hpp>

namespace bp {
    using namespace bp::literals;

    namespace serializable {
        using int_t = int32_t;
        using float_t = double;
        using bool_t = bool;
        using symbol = bp::symbol;
        using string_t = std::string;
    }

    class structure {
    public:
        using ptr = std::shared_ptr<structure>;

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
                serializable::symbol,
                serializable::string_t>;
        using value_ptr = std::shared_ptr<value>;

        class variant_t;
        using variant_ptr = std::shared_ptr<variant_t>;

        using object_t = std::unordered_map<bp::symbol, variant_ptr>;
        using object_ptr = std::shared_ptr<object_t>;

        using array_t = std::vector<variant_ptr>;
        using array_ptr = std::shared_ptr<array_t>;

        class variant_t : public boost::variant<value_ptr, object_ptr, array_ptr> {
        public:
            variant_t() : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(0)) { }
            variant_t(const serializable::int_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)) { }
            variant_t(const serializable::float_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)) { }
            variant_t(const serializable::bool_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)) { }
            variant_t(const serializable::symbol &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)) { }
            variant_t(const serializable::string_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(_val)) { }
            variant_t(const char* _val) : boost::variant<value_ptr, object_ptr, array_ptr>(_val?std::make_shared<value>(std::string(_val)):nullptr) { }

            template <typename ValueType,
                    class = typename std::enable_if<
                            bp::is_any_of<
                                    std::remove_cv_t<std::remove_reference_t<ValueType>>,
                                    value, object_t, array_t>::value
                    >::type>
            variant_t(const std::shared_ptr<ValueType> &_val) :
                    boost::variant<value_ptr, object_ptr, array_ptr>(_val){}

            template <typename ValueType,
                    class = typename std::enable_if<
                            bp::is_any_of<
                                    std::remove_cv_t<std::remove_reference_t<ValueType>>,
                                    value, object_t, array_t>::value
                    >::type>
            variant_t(std::shared_ptr<ValueType> &&_val) :
                    boost::variant<value_ptr, object_ptr, array_ptr>(std::forward<std::shared_ptr<ValueType>>(_val)){}

            template <typename ValueType,
                    class = typename std::enable_if<
                            bp::is_any_of<
                                    std::remove_cv_t<std::remove_reference_t<ValueType>>,
                                    value, object_t, array_t>::value
                    >::type>
            variant_t(ValueType &&_val) :
                    boost::variant<value_ptr, object_ptr, array_ptr>(
                            std::make_shared<std::remove_reference_t<ValueType>>(std::forward<ValueType>(_val))
                    ){}
        };

    private:
        void initialize_if_null(bp::structure::value_type _type);

        template <typename V, class = typename std::enable_if<bp::is_any_of<V, array_t, object_t>::value>::type>
        void initialize_val() {
            if (val_) {
                *val_ = std::make_shared<V>();
            } else {
                val_ = std::make_shared<variant_t>(std::make_shared<V>());
            }
        }

        inline void set_type(value_type _type) {value_type_ = _type;};

        struct value_type_visitor : public boost::static_visitor<value_type> {
            value_type operator()(serializable::int_t _val) const { return value_type::Int; }
            value_type operator()(serializable::int_t &&_val) const { return value_type::Int; }
            value_type operator()(serializable::float_t _val) const { return value_type::Float; }
            value_type operator()(serializable::float_t &&_val) const { return value_type::Float; }
            value_type operator()(serializable::bool_t _val) const { return value_type::Bool; }
            value_type operator()(serializable::bool_t &&_val) const { return value_type::Bool; }
            value_type operator()(const serializable::symbol &_val) const { return value_type::Symbol; }
            value_type operator()(serializable::symbol &&_val) const { return value_type::Symbol; }
            value_type operator()(const serializable::string_t & _val) const { return value_type::String; }
            value_type operator()(serializable::string_t && _val) const { return value_type::String; }
        };
        struct variant_visitor : public boost::static_visitor<bp::structure::value_type> {
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

        variant_ptr clone_variant(const variant_ptr &_ptr) {
            auto type = boost::apply_visitor(variant_visitor(), *_ptr);
            variant_ptr res;
            switch (type) {
                case value_type::Object: {
                    auto obj = std::make_shared<object_t>();
                    for (auto entry: *get_variant<object_ptr>(_ptr)) {
                        obj->emplace(entry.first, clone_variant(entry.second));
                    }
                    res = std::make_shared<variant_t>(obj);
                    break;
                }
                case value_type::Array: {
                    auto arr = std::make_shared<array_t>();
                    for (auto entry: *get_variant<array_ptr>(_ptr)) {
                        arr->emplace_back(clone_variant(entry));
                    }
                    res = std::make_shared<variant_t>(arr);

                    break;
                }
                default: {
                    res = std::make_shared<variant_t>(*_ptr);
                }
            }
            return res;
        }
    public:
        structure(bp::structure::variant_ptr _obj = nullptr);
        structure(const structure& _s) {
            operator=(_s);
        }
        structure(structure &&_s){
            operator=(std::move(_s));
        }

        operator bool() const {
            return static_cast<bool>(val_);
        }

        variant_ptr data() const {return val_;}

        structure::ptr clone() const {
            return std::make_shared<structure>(*this);
        }

        serializable::string_t  as_string() const;
        serializable::int_t     as_int() const;
        serializable::float_t   as_float() const;
        serializable::bool_t    as_bool() const;
        serializable::symbol    as_symbol() const;

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

        structure  operator[] (int index);

        structure at(int index) const;

        template<typename ValType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<ValType>, variant_t>::value>::type>
        void append(ValType &&_val) {
            initialize_if_null(value_type::Array);
            if (type() == value_type::Array) {
                return get_variant<array_ptr>(val_)->push_back(std::make_shared<variant_t>(std::forward<ValType>(_val)));
            } else {
                throw std::range_error("not an array");
            }
        }
        bool append(const std::initializer_list<variant_t> &_val);
        bool append(const std::initializer_list<std::pair<std::string, variant_t>> &_val);

        // objects:
        structure operator [](const symbol &_key);
        structure operator [](const char* _key) {return operator[](bp::symbol(_key));}

        structure at(const symbol &_key) const;

        void erase(const symbol &_key);

        template <typename KeyType, typename ValType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<KeyType>, bp::symbol>::value>::type,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<ValType>, variant_t>::value>::type>
        bool emplace(KeyType &&_key, ValType &&_val) {
            initialize_if_null(value_type::Object);
            if (type() == value_type::Object) {
                return get_variant<object_ptr>(val_)->
                        emplace(std::forward<KeyType>(_key), std::make_shared<variant_t>(std::forward<ValType>(_val))).second;
            } else {
                throw std::range_error("not an object");
            }
        };

        template <typename KeyType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<KeyType>, bp::symbol>::value>::type>
        bool emplace(KeyType &&_key, const std::initializer_list<variant_t> &_val) {
            initialize_if_null(value_type::Object);
            if (type() == value_type::Object) {
                array_ptr obj = std::make_shared<array_t>();
                for (auto item: _val) {
                    obj->push_back(std::make_shared<variant_t>(item));
                }
                return get_variant<object_ptr>(val_)->emplace(std::forward<KeyType>(_key), std::make_shared<variant_t>(obj)).second;
            } else {
                throw std::range_error("not an object");
            }
        };

        template <typename KeyType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<KeyType>, bp::symbol>::value>::type>
        bool emplace(KeyType &&_key, const std::initializer_list<std::pair<bp::symbol, variant_t>> &_val = nullptr) {
            initialize_if_null(value_type::Object);
            if (type() == value_type::Object) {
                object_ptr obj = std::make_shared<object_t>();
                for (auto item: _val) {
                    obj->emplace(item.first, std::make_shared<variant_t>(item.second));
                }
                return get_variant<object_ptr>(val_)->emplace(std::forward<KeyType>(_key), std::make_shared<variant_t>(obj)).second;
            } else {
                throw std::range_error("not an object");
            }
        };

        template <typename KeyType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<KeyType>, bp::symbol>::value>::type>
        bool emplace(KeyType &&_key, const std::initializer_list<std::pair<bp::symbol, bp::structure>> &_val = nullptr) {
            initialize_if_null(value_type::Object);
            if (type() == value_type::Object) {
                object_ptr obj = std::make_shared<object_t>();
                for (auto item: _val) {
                    obj->emplace(item.first, std::make_shared<variant_t>(item.second));
                }
                return get_variant<object_ptr>(val_)->emplace(std::forward<KeyType>(_key), std::make_shared<variant_t>(obj)).second;
            } else {
                throw std::range_error("not an object");
            }
        };

        bool emplace(const std::initializer_list<std::pair<bp::symbol, variant_t>> &_val);

//        bool emplace(const std::initializer_list<std::pair<bp::symbol, structure>> &_val);

        bool emplace(symbol &&_key, const bp::structure & _str);


        template<typename ValueType,
                class = typename std::enable_if<std::is_convertible<std::remove_cv_t<std::remove_reference_t<ValueType>>, variant_t>::value>::type>
        structure get(const bp::symbol &_key, ValueType && _default) const {
            if (type() == value_type::Object) {
                try {
                    auto val = get_variant<object_ptr>(val_)->at(_key);
                    return bp::structure(val);
                } catch (std::out_of_range &e) {
                    return bp::structure(std::make_shared<variant_t>(std::forward<ValueType>(_default)));
                }
            } else {
                throw std::range_error("not an object");
            }
        };
        inline structure get(const bp::symbol &_key) const {
            return get(_key, variant_t(nullptr));
        };

        structure& operator=(const structure::ptr &_str);
        structure& operator=(structure::ptr &&_str);
        structure& operator=(const structure &_str);
        structure& operator=(structure &&_str);
        structure& operator=(const std::initializer_list<variant_t> &_val) ;   // array
        structure& operator=(const std::initializer_list<std::pair<std::string, variant_t>> &_val) ;   // object
//        structure& operator=(const std::initializer_list<std::pair<std::string, structure>> &_val) ;   // object

        template<typename ValType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<ValType>, variant_t>::value>::type>
        structure& operator=(ValType &&_val) {
            initialize_if_null(value_type::Object);
            if (val_) {
                *val_ = _val;
            } else {
                val_ = std::make_shared<variant_t>(std::forward<ValType>(_val));
            }
            set_type(boost::apply_visitor(variant_visitor(), *val_));
            return *this;
        }

        template<typename ValType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<ValType>, variant_t>::value>::type>
        inline structure& set(ValType &&_val) {
            operator=(std::forward<ValType>(_val));
            return *this;
        };
        inline structure& set(structure && _str) {
            operator=(_str);
            return *this;
        };
        inline structure& set(const structure & _str) {
            operator=(_str);
            return *this;
        };
        inline structure& set(const std::initializer_list<variant_t> &_val) {
            operator=(_val);
            return *this;
        };   // array
        inline structure& set(const std::initializer_list<std::pair<std::string, variant_t>> &_val) {
            operator=(_val);
            return *this;
        };
//        inline structure& set(const std::initializer_list<std::pair<std::string, structure>> &_val) {
//            operator=(_val);
//            return *this;
//        };
        // object

        //

        template <symbol::hash_type serializer_type>
        std::string stringify() const {return "";};

        template <symbol::hash_type serializer_type>
        void parse(const std::string &_str) {};

        template <symbol::hash_type serializer_type>
        static structure create_from_string(const std::string &_s){
            structure s;
            s.parse<serializer_type>(_s);
            return s;
        };

        // ITERATORS

        class object_key_iterator : public std::iterator<std::input_iterator_tag, const char*> {
            const structure &object_;
            object_t::iterator    it;
            object_t::iterator    end;
        public:
            object_key_iterator(const structure &_object);
            object_key_iterator(const object_key_iterator&_it);
            object_key_iterator & operator++();
            object_key_iterator operator++(int);
            bool operator==(const object_key_iterator& rhs);
            bool operator!=(const object_key_iterator& rhs);
            const char * operator*();
        };

        // TODO: methods to conform forward iterator interface
        class object_iterator : public std::iterator<std::forward_iterator_tag, structure> {
            const structure &object_;
            object_t::iterator    it;
            object_t::iterator    end;
        public:
            object_iterator(const structure &_object);
            object_iterator(const object_iterator&_it);
            object_iterator & operator++();
            object_iterator operator++(int);
            bool operator==(const object_iterator& rhs);
            bool operator!=(const object_iterator& rhs);
            std::pair<bp::structure::object_t::key_type, bp::structure>  operator*();

            /////
//            reference operator*() const;
//            object_iterator operator++(int);
        };

        // TODO: methods to conform random access iterator interface
        class array_iterator : public std::iterator<std::random_access_iterator_tag, structure> {
            const structure &object_;
            size_t  index;
            size_t  size;
        public:
            array_iterator(const structure &_object);
            array_iterator(const array_iterator&_it);
            array_iterator & operator++();
            array_iterator operator++(int);
            bool operator==(const array_iterator& rhs);
            bool operator!=(const array_iterator& rhs);
            bp::structure  operator*();

            /////
//            array_iterator& operator--(); //prefix increment
//            array_iterator operator--(int); //postfix decrement
//            friend bool operator<(const array_iterator&, const array_iterator&);
//            friend bool operator>(const array_iterator&, const array_iterator&);
//            friend bool operator<=(const array_iterator&, const array_iterator&);
//            friend bool operator>=(const array_iterator&, const array_iterator&);
//
//            array_iterator& operator+=(size_t);
//            friend array_iterator operator+(const array_iterator&, size_t);
//            friend array_iterator operator+(size_t, const array_iterator&);
//            array_iterator& operator-=(size_t);
//            friend array_iterator operator-(const array_iterator&, size_t);
//            friend difference_type operator-(array_iterator, array_iterator);
//
//            reference operator[](size_t) const;
        };

    private:
        template <typename iterator_type>
        class iterable {
            iterator_type it_;
        public:
            iterable(const iterator_type &_it) : it_(_it){}
            iterable(const iterable &_it) : it_(_it.it_) {}
            iterator_type begin() const {return it_;}
            iterator_type end() const { return iterator_type(structure());}
        };
        template <typename iterator_type>
        iterable<iterator_type> get_iterable() {
            return iterable<iterator_type>(iterator_type(*this));
        }
    public:

        iterable<object_key_iterator> keys() {return this->get_iterable<object_key_iterator>();}
        iterable<object_iterator> as_object() {return this->get_iterable<object_iterator>();}
        iterable<array_iterator> as_array() {return this->get_iterable<array_iterator>();}

        const iterable<object_key_iterator> keys() const {return static_cast<const structure*>(this)->keys();}
        const iterable<object_iterator> as_object() const {return static_cast<const structure*>(this)->as_object();}
        const iterable<array_iterator> as_array() const {return static_cast<const structure*>(this)->as_array();}
        // EXCEPTIONS

        class structure_error : public std::runtime_error {
        public:
            structure_error(const char* _msg) : std::runtime_error(_msg){}
        };
        class parse_error : public structure_error {
        public:
            parse_error(const char* _msg) : structure_error(_msg){}
        };

    private:
        value_type value_type_ = value_type::Null;
        variant_ptr val_;
    };
}


#endif //BP_SERIALIZER_HPP
