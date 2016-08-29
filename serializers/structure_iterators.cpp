#include "structure.hpp"

using namespace bp::serializable;
// OBJECT KEYS

bp::structure::object_key_iterator::object_key_iterator(const structure *_object) : object_(_object) {
    if (object_ && object_->is_object()) {
        it = get_variant<object_ptr>(object_->val_)->begin();
    }
}

bp::structure::object_key_iterator::object_key_iterator(const bp::structure::object_key_iterator&_it)
        : object_(_it.object_), it(_it.it), end(_it.end) {}

bp::structure::object_key_iterator &bp::structure::object_key_iterator::operator++() {
    ++it;
    return *this;
}

bp::structure::object_key_iterator bp::structure::object_key_iterator::operator++(int) {object_key_iterator tmp(*this); operator++(); return tmp;}

bool bp::structure::object_key_iterator::operator==(const object_key_iterator& rhs) const {
    if (!rhs.object_) {
        return it==end;
    }
    return it==rhs.it;
}

const char *bp::structure::object_key_iterator::operator*() {
    if (!object_ || it==end) {
        return nullptr;
    }
    return it->first;
}

bool bp::structure::object_key_iterator::operator!=(const object_key_iterator& rhs) const {
    if (!rhs.object_) {
        return it!=end;
    }
    return it!=rhs.it;
}

// OBJECT ITEMS

bp::structure::object_iterator::object_iterator(const structure *_object) : object_(_object) {
    if (object_ && object_->is_object()) {
        it = get_variant<object_ptr>(object_->val_)->begin();
        end = get_variant<object_ptr>(object_->val_)->end();
    }
}

bp::structure::object_iterator::object_iterator(const object_iterator&_it)
        : object_(_it.object_), it(_it.it), end(_it.end) {}

bp::structure::object_iterator &bp::structure::object_iterator::operator++() {
    ++it;
    return *this;
}

bp::structure::object_iterator bp::structure::object_iterator::operator++(int) {object_iterator tmp(*this); operator++(); return tmp;}

bool bp::structure::object_iterator::operator==(const bp::structure::object_iterator& rhs) const {
    if (!rhs.object_) {
        return it==end;
    }
    return it==rhs.it;
}

std::pair<object::key_type, bp::structure> bp::structure::object_iterator::operator*() {
    if (!object_ || it==end) {
        return std::make_pair(""_sym, bp::structure());
    }
    return std::make_pair(it->first, bp::structure(it->second));
}

bool bp::structure::object_iterator::operator!=(const bp::structure::object_iterator& rhs) const {
    if (!rhs.object_) {
        return it!=end;
    }
    return it!=rhs.it;
}

// ARRAY ITEMS

bp::structure::array_iterator::array_iterator(const structure *_object) : object_(_object) {
//    index = 0;
//    size = 0;
//    if (object_ && object_.is_array()) {
//        size = object_.size();
//    }
    if (object_ && object_->is_array()) {
        it = get_variant<array_ptr>(object_->val_)->begin();
        end = get_variant<array_ptr>(object_->val_)->end();
    }
}

bp::structure::array_iterator::array_iterator(const bp::structure::array_iterator &_it)
        : object_(_it.object_), it(_it.it), end(_it.end)/*, index(_it.index), size(_it.size) */{}

bp::structure::array_iterator &bp::structure::array_iterator::operator++() {
    if (it!=end) {
        ++it;
    }
    return *this;
}

bp::structure::array_iterator bp::structure::array_iterator::operator++(int) {array_iterator tmp(*this); operator++(); return tmp;}

bool bp::structure::array_iterator::operator==(const bp::structure::array_iterator& rhs) const {
    if (!rhs.object_) {
        return it==end;
    }
    return it==rhs.it;
}

bp::structure bp::structure::array_iterator::operator*() {
    if (!object_) {
        return bp::structure();
    }
    return bp::structure(*it);
}

bool bp::structure::array_iterator::operator!=(const bp::structure::array_iterator& rhs) const {
    return !operator==(rhs);
}