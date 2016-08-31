#include "structure.hpp"

using namespace bp::serializable;
// OBJECT KEYS

bp::structure::object_key_iterator::object_key_iterator(const structure *_object) : object_(_object) {
    if (object_ && object_->is_object()) {
        it_ = get_variant<object_ptr>(object_->val_)->begin();
    }
}

bp::structure::object_key_iterator::object_key_iterator(const bp::structure::object_key_iterator&_it)
        : object_(_it.object_), it_(_it.it_), end_(_it.end_) {}

bp::structure::object_key_iterator &bp::structure::object_key_iterator::operator++() {
    ++it_;
    return *this;
}

bp::structure::object_key_iterator bp::structure::object_key_iterator::operator++(int) {object_key_iterator tmp(*this); operator++(); return tmp;}

bool bp::structure::object_key_iterator::operator==(const object_key_iterator& _rhs) const {
    if (!_rhs.object_) {
        return it_==end_;
    }
    return it_==_rhs.it_;
}

const char *bp::structure::object_key_iterator::operator*() {
    if (!object_ || it_==end_) {
        return nullptr;
    }
    return it_->first;
}

bool bp::structure::object_key_iterator::operator!=(const object_key_iterator& _rhs) const {
    if (!_rhs.object_) {
        return it_!=end_;
    }
    return it_!=_rhs.it_;
}

bp::structure::object_key_iterator &
bp::structure::object_key_iterator::operator=(const bp::structure::object_key_iterator &_rhs) {
    object_ = _rhs.object_;
    it_ = _rhs.it_;
    end_ = _rhs.end_;
    return *this;
}

// OBJECT ITEMS

bp::structure::object_iterator::object_iterator(const structure *_object) : object_(_object) {
    if (object_ && object_->is_object()) {
        it_ = get_variant<object_ptr>(object_->val_)->begin();
        end_ = get_variant<object_ptr>(object_->val_)->end();
    }
}

bp::structure::object_iterator::object_iterator(const object_iterator&_it)
        : object_(_it.object_), it_(_it.it_), end_(_it.end_) {}

bp::structure::object_iterator &bp::structure::object_iterator::operator++() {
    ++it_;
    return *this;
}

bp::structure::object_iterator bp::structure::object_iterator::operator++(int) {object_iterator tmp(*this); operator++(); return tmp;}

bool bp::structure::object_iterator::operator==(const bp::structure::object_iterator& _rhs) const {
    if (!_rhs.object_) {
        return it_==end_;
    }
    return it_==_rhs.it_;
}

std::pair<object::key_type, bp::structure> bp::structure::object_iterator::operator*() {
    if (!object_ || it_==end_) {
        return std::make_pair(""_sym, bp::structure());
    }
    return std::make_pair(it_->first, bp::structure(it_->second));
}

bool bp::structure::object_iterator::operator!=(const bp::structure::object_iterator& _rhs) const {
    if (!_rhs.object_) {
        return it_!=end_;
    }
    return it_!=_rhs.it_;
}

bp::structure::object_iterator &
bp::structure::object_iterator::operator=(const bp::structure::object_iterator &_rhs) {
    object_ = _rhs.object_;
    it_ = _rhs.it_;
    end_ = _rhs.end_;
    return *this;
}

// ARRAY ITEMS

bp::structure::array_iterator::array_iterator(const structure *_object) : object_(_object) {
//    index = 0;
//    size = 0;
//    if (object_ && object_.is_array()) {
//        size = object_.size();
//    }
    if (object_ && object_->is_array()) {
        it_ = get_variant<array_ptr>(object_->val_)->begin();
        end_ = get_variant<array_ptr>(object_->val_)->end();
    }
}

bp::structure::array_iterator::array_iterator(const bp::structure::array_iterator &_it)
        : object_(_it.object_), it_(_it.it_), end_(_it.end_)/*, index(_it.index), size(_it.size) */{}

bp::structure::array_iterator &bp::structure::array_iterator::operator++() {
    if (it_!=end_) {
        ++it_;
    }
    return *this;
}

bp::structure::array_iterator bp::structure::array_iterator::operator++(int) {array_iterator tmp(*this); operator++(); return tmp;}

bool bp::structure::array_iterator::operator==(const bp::structure::array_iterator& _rhs) const {
    if (!_rhs.object_) {
        return it_==end_;
    }
    return it_==_rhs.it_;
}

bp::structure bp::structure::array_iterator::operator*() {
    if (!object_) {
        return bp::structure();
    }
    return bp::structure(*it_);
}

bool bp::structure::array_iterator::operator!=(const bp::structure::array_iterator& _rhs) const {
    return !operator==(_rhs);
}

bp::structure::array_iterator &bp::structure::array_iterator::operator=(const bp::structure::array_iterator &_rhs) {
    object_ = _rhs.object_;
    it_ = _rhs.it_;
    end_ = _rhs.end_;
    return *this;
}
