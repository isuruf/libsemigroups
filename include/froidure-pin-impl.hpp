//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// This file contains implementations of the methods for the FroidurePin class.

// TODO(now):   double check "noexcept" usage!
// TODO(later): make const member functions thread-safe

#ifndef LIBSEMIGROUPS_INCLUDE_FROIDURE_PIN_IMPL_HPP_
#define LIBSEMIGROUPS_INCLUDE_FROIDURE_PIN_IMPL_HPP_

#define TEMPLATE                    \
  template <typename TElementType,  \
            typename TElementHash,  \
            typename TElementEqual, \
            class TTraits>
#define FROIDURE_PIN \
  FroidurePin<TElementType, TElementHash, TElementEqual, TTraits>

#define VOID TEMPLATE void
#define INLINE_VOID TEMPLATE inline void
#define SIZE_T TEMPLATE size_t
#define BOOL TEMPLATE bool
#define ELEMENT_INDEX_TYPE TEMPLATE element_index_type
#define LETTER_TYPE TEMPLATE letter_type
#define CONST_REFERENCE TEMPLATE typename FROIDURE_PIN::const_reference
#define ELEMENT_TYPE TEMPLATE typename FROIDURE_PIN::element_type
#define WORD_TYPE TEMPLATE word_type

namespace libsemigroups {

  // using enumerate_index_type = FroidurePinBase::size_type;
  using element_index_type = FroidurePinBase::element_index_type;

  ////////////////////////////////////////////////////////////////////////
  // FroidurePin - constructors + destructor - public
  ////////////////////////////////////////////////////////////////////////

  TEMPLATE
  FROIDURE_PIN::FroidurePin(std::vector<element_type> const* gens)
      : _batch_size(8192),
        _degree(UNDEFINED),
        _duplicate_gens(),
        _elements(),
        _enumerate_order(),
        _final(),
        _first(),
        _found_one(false),
        _gens(),
        _id(),
        _idempotents(),
        _idempotents_found(false),
        _is_idempotent(),
        _left(gens->size()),
        _length(),
        _lenindex(),
        _letter_to_pos(),
        _map(),
        _max_threads(std::thread::hardware_concurrency()),
        _nr(0),
        _nrgens(gens->size()),
        _nr_rules(0),
        _pos(0),
        _pos_one(0),
        _prefix(),
        _reduced(gens->size()),
        _relation_gen(0),
        _relation_pos(UNDEFINED),
        _right(gens->size()),
        _sorted(),
        _suffix(),
        _tmp_product(),
        _wordlen(0) {  // (length of the current word) - 1
    if (_nrgens == 0) {
      throw LIBSEMIGROUPS_EXCEPTION("no generators given");
    }
#ifdef LIBSEMIGROUPS_STATS
    _nr_products = 0;
#endif
    _right.set_default_value(UNDEFINED);
    // FIXME inclusion of the next line makes test FroidurePin of BMats 01
    // extremely slow (~50ms to ~10s!!!!)
    // reserve(_nrgens);
    _degree = internal_degree()(this->to_internal_const((*gens)[0]));

    for (size_t i = 0; i < _nrgens; ++i) {
      size_t degree = internal_degree()(this->to_internal_const((*gens)[i]));
      if (degree != _degree) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "generator " + internal::to_string(i) + " has degree "
            + internal::to_string(degree) + " but should have degree "
            + internal::to_string(_degree));
      }
    }
    for (const_reference x : *gens) {
      _gens.push_back(this->internal_copy(this->to_internal_const(x)));
    }

    _tmp_product = one()(_gens[0]);
    _id          = one()(_gens[0]);
    _lenindex.push_back(0);

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
    _map.set_empty_key(this->empty_key(_id));
#endif

    // add the generators
    for (letter_type i = 0; i < _nrgens; i++) {
      auto it = _map.find(_gens[i]);
      if (it != _map.end()) {  // duplicate generator
        _letter_to_pos.push_back(it->second);
        _nr_rules++;
        _duplicate_gens.emplace_back(i, _first[it->second]);
        // i.e. _gens[i] = _gens[_first[it->second]]
        // _first maps from element_index_type -> letter_type :)
      } else {
        is_one(_gens[i], _nr);
        _elements.push_back(_gens[i]);
        // Note that every non-duplicate generator is *really* stored in
        // _elements, and so must be deleted from _elements but not _gens.
        _first.push_back(i);
        _final.push_back(i);
        _enumerate_order.push_back(_nr);
        _letter_to_pos.push_back(_nr);
        _length.push_back(1);
        _map.emplace(_elements.back(), _nr);
        _prefix.push_back(UNDEFINED);
        // TODO _prefix.push_back(_nr) and get rid of _letter_to_pos, and
        // the extra clause in the enumerate method!
        _suffix.push_back(UNDEFINED);
        _nr++;
      }
    }
    expand(_nr);
    _lenindex.push_back(_enumerate_order.size());
  }

  TEMPLATE
  FROIDURE_PIN::FroidurePin(std::vector<element_type> const& gens)
      : FroidurePin(&gens) {}

  TEMPLATE
  FROIDURE_PIN::FroidurePin(std::initializer_list<element_type> gens)
      : FroidurePin(std::vector<element_type>(gens)) {}

  TEMPLATE
  FROIDURE_PIN::FroidurePin(FroidurePin const& S)
      : _batch_size(S._batch_size),
        _degree(S._degree),
        _duplicate_gens(S._duplicate_gens),
        _elements(),
        _enumerate_order(S._enumerate_order),
        _final(S._final),
        _first(S._first),
        _found_one(S._found_one),
        _gens(),
        _id(this->internal_copy(S._id)),
        _idempotents(S._idempotents),
        _idempotents_found(S._idempotents_found),
        _is_idempotent(S._is_idempotent),
        _left(S._left),
        _length(S._length),
        _lenindex(S._lenindex),
        _letter_to_pos(S._letter_to_pos),
        _max_threads(S._max_threads),
        _nr(S._nr),
        _nrgens(S._nrgens),
        _nr_rules(S._nr_rules),
        _pos(S._pos),
        _pos_one(S._pos_one),
        _prefix(S._prefix),
        _reduced(S._reduced),
        _relation_gen(S._relation_gen),
        _relation_pos(S._relation_pos),
        _right(S._right),
        _sorted(),  // TODO S this if set
        _suffix(S._suffix),
        _wordlen(S._wordlen) {
#ifdef LIBSEMIGROUPS_STATS
    _nr_products = 0;
#endif
    _elements.reserve(_nr);

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
    _map.set_empty_key(this->empty_key(_id));
    _map.resize(_nr);
#else
    _map.reserve(_nr);
#endif
    _tmp_product = this->internal_copy(S._id);

    element_index_type i = 0;
    for (internal_const_reference x : S._elements) {
      auto y = this->internal_copy(x);
      _elements.push_back(y);
      _map.emplace(y, i++);
    }
    copy_gens();
  }

  TEMPLATE
  FROIDURE_PIN::~FroidurePin() {
    this->internal_free(_tmp_product);
    this->internal_free(_id);

    // delete those generators not in _elements, i.e. the duplicate ones
    for (auto& x : _duplicate_gens) {
      this->internal_free(_gens[x.first]);
    }
    for (auto& x : _elements) {
      this->internal_free(x);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePin - constructor - private
  ////////////////////////////////////////////////////////////////////////

  // Partial copy.
  // \p copy a semigroup
  // \p coll a collection of additional generators
  //
  // This is a constructor for a semigroup generated by the generators of the
  // FroidurePin copy and the (possibly) additional generators coll.
  //
  // The relevant parts of the data structure of copy are copied and
  // \c this will be corrupt unless add_generators or closure is called
  // subsequently. This is why this method is private.
  //
  // The same effect can be obtained by copying copy using the copy
  // constructor and then calling add_generators or closure. However,
  // this constructor avoids copying those parts of the data structure of
  // copy that add_generators invalidates anyway. If copy has not been
  // enumerated at all, then these two routes for adding more generators are
  // equivalent.
  //
  // <add_generators> or <closure> should usually be called after this.
  TEMPLATE
  FROIDURE_PIN::FroidurePin(FroidurePin const&               S,
                            std::vector<element_type> const* coll)
      : _batch_size(S._batch_size),
        _degree(S._degree),  // copy for comparison in add_generators
        _duplicate_gens(S._duplicate_gens),
        _elements(),
        _found_one(S._found_one),  // copy in case degree doesn't change in
                                   // add_generators
        _gens(),
        _idempotents(S._idempotents),
        _idempotents_found(S._idempotents_found),
        _is_idempotent(S._is_idempotent),
        _left(S._left),
        _letter_to_pos(S._letter_to_pos),
        _max_threads(S._max_threads),
        _nr(S._nr),
        _nrgens(S._nrgens),
        _nr_rules(0),
        _pos(S._pos),
        _pos_one(S._pos_one),  // copy in case degree doesn't change in
                               // add_generators
        _reduced(S._reduced),
        _relation_gen(0),
        _relation_pos(UNDEFINED),
        _right(S._right),
        _sorted(),
        _wordlen(0) {
    LIBSEMIGROUPS_ASSERT(!coll->empty());
    LIBSEMIGROUPS_ASSERT(internal_degree()(coll->at(0)) >= S.degree());

#ifdef LIBSEMIGROUPS_DEBUG
    for (const_reference x : *coll) {
      LIBSEMIGROUPS_ASSERT(internal_degree()(x)
                           == internal_degree()((*coll)[0]));
    }
#endif
#ifdef LIBSEMIGROUPS_STATS
    _nr_products = 0;
#endif
    _elements.reserve(S._nr);

    // the following are required for assignment to specific positions in
    // add_generators
    _final.resize(S._nr, 0);
    _first.resize(S._nr, 0);
    _length.resize(S._nr, 0);
    _prefix.resize(S._nr, 0);
    _suffix.resize(S._nr, 0);

    size_t deg_plus = internal_degree()(coll->at(0)) - S.degree();

    if (deg_plus != 0) {
      _degree += deg_plus;
      _found_one = false;
      _pos_one   = 0;
    }

    _lenindex.push_back(0);
    _lenindex.push_back(S._lenindex[1]);
    _enumerate_order.reserve(S._nr);

    // add the distinct old generators to new _enumerate_order
    for (enumerate_index_type i = 0; i < S._lenindex[1]; i++) {
      _enumerate_order.push_back(S._enumerate_order[i]);
      _final[_enumerate_order[i]]  = S._final[S._enumerate_order[i]];
      _first[_enumerate_order[i]]  = S._first[S._enumerate_order[i]];
      _prefix[_enumerate_order[i]] = UNDEFINED;
      _suffix[_enumerate_order[i]] = UNDEFINED;
      _length[_enumerate_order[i]] = 1;
    }

    _id          = one()(this->to_internal(coll->at(0)));
    _tmp_product = this->internal_copy(_id);

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
    _map.set_empty_key(this->empty_key(_id));
    _map.resize(S._nr);
#else
    _map.reserve(S._nr);
#endif

    element_index_type i = 0;
    for (internal_const_reference x : S._elements) {
      auto y = this->internal_copy(x);
      increase_degree_by()(y, deg_plus);
      _elements.push_back(y);
      _map.emplace(y, i);
      is_one(y, i++);
    }
    copy_gens();  // copy the old generators
    // Now this is ready to have add_generators or closure called on it
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePin - methods - public
  ////////////////////////////////////////////////////////////////////////

  ELEMENT_INDEX_TYPE FROIDURE_PIN::word_to_pos(word_type const& w) const {
    // w is a word in the generators (i.e. a vector of letter_type's)
    if (w.size() == 0) {
      throw LIBSEMIGROUPS_EXCEPTION("the given word has length 0");
    }
    for (auto x : w) {
      validate_letter_index(x);
    }
    element_index_type out = _letter_to_pos[w[0]];
    for (auto it = w.cbegin() + 1; it < w.cend() && out != UNDEFINED; ++it) {
      out = _right.get(out, _letter_to_pos[*it]);
    }
    return out;
  }

  ELEMENT_TYPE FROIDURE_PIN::word_to_element(word_type const& w) const {
    element_index_type pos = word_to_pos(w);
    if (pos != UNDEFINED) {
      // Return a copy
      return this->external_copy(_elements[pos]);
    }
    // word_to_pos is always known for generators (i.e. when w.size() == 1),
    // and word_to_pos verifies that w is valid.
    LIBSEMIGROUPS_ASSERT(w.size() > 1);
    LIBSEMIGROUPS_ASSERT(w[0] < nr_generators() && w[1] < nr_generators());
    element_type prod = this->external_copy(_tmp_product);
    product()(this->to_internal(prod), _gens[w[0]], _gens[w[1]]);
    for (auto it = w.begin() + 2; it < w.end(); ++it) {
      LIBSEMIGROUPS_ASSERT(*it < nr_generators());
      swap()(_tmp_product, this->to_internal(prod));
      product()(this->to_internal(prod), _tmp_product, _gens[*it]);
    }
    return prod;
  }

  BOOL FROIDURE_PIN::equal_to(word_type const& u, word_type const& v) const {
    element_index_type u_pos = word_to_pos(u);  // validates u
    element_index_type v_pos = word_to_pos(v);  // validates v
    if (finished() || (u_pos != UNDEFINED && v_pos != UNDEFINED)) {
      LIBSEMIGROUPS_ASSERT(u_pos != UNDEFINED);
      LIBSEMIGROUPS_ASSERT(v_pos != UNDEFINED);
      return u_pos == v_pos;
    } else {
      element_type uu  = word_to_element(u);
      element_type vv  = word_to_element(v);
      auto         res = (uu == vv);
      this->external_free(uu);
      this->external_free(vv);
      return res;
    }
  }

  SIZE_T FROIDURE_PIN::current_max_word_length() const noexcept {
    if (finished()) {
      return _lenindex.size() - 2;
    } else if (_nr > _lenindex.back()) {
      return _lenindex.size();
    } else {
      return _lenindex.size() - 1;
    }
  }

  SIZE_T FROIDURE_PIN::degree() const noexcept {
    return _degree;
  }

  SIZE_T FROIDURE_PIN::nr_generators() const noexcept {
    return _gens.size();
  }

  CONST_REFERENCE FROIDURE_PIN::generator(letter_type pos) const {
    validate_letter_index(pos);
    return this->to_external_const(_gens[pos]);
  }

  BOOL FROIDURE_PIN::finished_impl() const {
    return (_pos >= _nr);
  }

  BOOL FROIDURE_PIN::is_begun() const noexcept {
    LIBSEMIGROUPS_ASSERT(_lenindex.size() > 1);
    return (_pos >= _lenindex[1]);
  }

  ELEMENT_INDEX_TYPE FROIDURE_PIN::current_position(const_reference x) const {
    if (internal_degree()(this->to_internal_const(x)) != _degree) {
      return UNDEFINED;
    }

    auto it = _map.find(this->to_internal_const(x));
    return (it == _map.end() ? UNDEFINED : it->second);
  }

  SIZE_T FROIDURE_PIN::current_size() const noexcept {
    return _elements.size();
  }

  SIZE_T FROIDURE_PIN::current_nr_rules() const noexcept {
    return _nr_rules;
  }

  ELEMENT_INDEX_TYPE
  FROIDURE_PIN::prefix(element_index_type pos) const {
    validate_element_index(pos);
    return _prefix[pos];
  }

  ELEMENT_INDEX_TYPE
  FROIDURE_PIN::suffix(element_index_type pos) const {
    validate_element_index(pos);
    return _suffix[pos];
  }

  LETTER_TYPE
  FROIDURE_PIN::first_letter(element_index_type pos) const {
    validate_element_index(pos);
    return _first[pos];
  }

  LETTER_TYPE
  FROIDURE_PIN::final_letter(element_index_type pos) const {
    validate_element_index(pos);
    return _final[pos];
  }

  SIZE_T FROIDURE_PIN::batch_size() const noexcept {
    return _batch_size;
  }
  SIZE_T FROIDURE_PIN::length_const(element_index_type pos) const {
    validate_element_index(pos);
    return _length[pos];
  }

  SIZE_T FROIDURE_PIN::length_non_const(element_index_type pos) {
    if (pos >= _nr) {
      enumerate();
    }
    return length_const(pos);
  }

  ELEMENT_INDEX_TYPE
  FROIDURE_PIN::product_by_reduction(element_index_type i,
                                     element_index_type j) const {
    validate_element_index(i);
    validate_element_index(j);

    if (length_const(i) <= length_const(j)) {
      while (i != UNDEFINED) {
        j = _left.get(j, _final[i]);
        i = _prefix[i];
      }
      return j;
    } else {
      while (j != UNDEFINED) {
        i = _right.get(i, _first[j]);
        j = _suffix[j];
      }
      return i;
    }
  }

  ELEMENT_INDEX_TYPE
  FROIDURE_PIN::fast_product(element_index_type i, element_index_type j) const {
    validate_element_index(i);
    validate_element_index(j);
    if (length_const(i) < 2 * complexity()(_tmp_product)
        || length_const(j) < 2 * complexity()(_tmp_product)) {
      return product_by_reduction(i, j);
    } else {
      product()(_tmp_product, _elements[i], _elements[j]);
      return _map.find(_tmp_product)->second;
    }
  }

  ELEMENT_INDEX_TYPE FROIDURE_PIN::letter_to_pos(letter_type i) const {
    validate_letter_index(i);
    return _letter_to_pos[i];
  }

  SIZE_T FROIDURE_PIN::nr_idempotents() {
    init_idempotents();
    return _idempotents.size();
  }

  BOOL FROIDURE_PIN::is_idempotent(element_index_type pos) {
    validate_element_index(pos);
    init_idempotents();
    return _is_idempotent[pos];
  }

  SIZE_T FROIDURE_PIN::nr_rules() {
    enumerate();
    return _nr_rules;
  }

  VOID FROIDURE_PIN::set_batch_size(size_t batch_size) noexcept {
    _batch_size = batch_size;
  }

  VOID FROIDURE_PIN::reserve(size_t n) {
    // Since the FroidurePin we are enumerating is bounded in size by the
    // maximum value of an element_index_t, we cast the argument here to this
    // integer type.
    element_index_type nn = static_cast<element_index_type>(n);
    _elements.reserve(nn);
    _final.reserve(nn);
    _first.reserve(nn);
    _enumerate_order.reserve(nn);
    _left.reserve(nn);
    _length.reserve(nn);

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
    _map.resize(nn);
#else
    _map.reserve(nn);
#endif

    _prefix.reserve(nn);
    _reduced.reserve(nn);
    _right.reserve(nn);
    _suffix.reserve(nn);
  }

  SIZE_T FROIDURE_PIN::size() {
    enumerate();
    return _elements.size();
  }

  BOOL FROIDURE_PIN::contains(const_reference x) {
    return (position(x) != UNDEFINED);
  }

  ELEMENT_INDEX_TYPE FROIDURE_PIN::position(const_reference x) {
    if (internal_degree()(this->to_internal_const(x)) != _degree) {
      return UNDEFINED;
    }

    while (true) {
      auto it = _map.find(this->to_internal_const(x));
      if (it != _map.end()) {
        return it->second;
      }
      if (finished()) {
        return UNDEFINED;
      }
      enumerate(_nr + 1);
      // _nr + 1 means we enumerate _batch_size more elements
    }
  }

  ELEMENT_INDEX_TYPE FROIDURE_PIN::sorted_position(const_reference x) {
    return position_to_sorted_position(position(x));
  }

  ELEMENT_INDEX_TYPE
  FROIDURE_PIN::position_to_sorted_position(element_index_type pos) {
    enumerate(LIMIT_MAX);
    if (pos >= _nr) {
      return UNDEFINED;
    }
    init_sorted();
    return _sorted[pos].second;
  }

  CONST_REFERENCE FROIDURE_PIN::at(element_index_type pos) {
    enumerate(pos + 1);
    return this->to_external_const(_elements.at(pos));
  }

  CONST_REFERENCE FROIDURE_PIN::operator[](element_index_type pos) const {
    LIBSEMIGROUPS_ASSERT(pos < _elements.size());
    return this->to_external_const(_elements[pos]);
  }

  CONST_REFERENCE FROIDURE_PIN::sorted_at(element_index_type pos) {
    init_sorted();
    return this->to_external_const(_sorted.at(pos).first);
  }

  ELEMENT_INDEX_TYPE FROIDURE_PIN::right(element_index_type i, letter_type j) {
    enumerate();
    return _right.get(i, j);
  }

  ELEMENT_INDEX_TYPE FROIDURE_PIN::left(element_index_type i, letter_type j) {
    enumerate();
    return _left.get(i, j);
  }

  VOID FROIDURE_PIN::minimal_factorisation(word_type&         word,
                                           element_index_type pos) {
    if (pos >= _nr && !finished()) {
      enumerate(pos + 1);
    }
    validate_element_index(pos);
    word.clear();
    while (pos != UNDEFINED) {
      word.push_back(_first[pos]);
      pos = _suffix[pos];
    }
  }

  WORD_TYPE
  FROIDURE_PIN::minimal_factorisation(element_index_type pos) {
    word_type word;
    minimal_factorisation(word, pos);
    return word;
  }

  WORD_TYPE FROIDURE_PIN::minimal_factorisation(const_reference x) {
    element_index_type pos = this->position(x);
    if (pos == UNDEFINED) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "the argument is not an element of the semigroup");
    }
    return minimal_factorisation(pos);
  }

  VOID FROIDURE_PIN::factorisation(word_type& word, element_index_type pos) {
    minimal_factorisation(word, pos);
  }

  WORD_TYPE FROIDURE_PIN::factorisation(element_index_type pos) {
    return minimal_factorisation(pos);
  }

  WORD_TYPE FROIDURE_PIN::factorisation(const_reference x) {
    return minimal_factorisation(x);
  }

  VOID FROIDURE_PIN::reset_next_relation() noexcept {
    _relation_pos = UNDEFINED;
    _relation_gen = 0;
  }

  VOID FROIDURE_PIN::next_relation(word_type& relation) {
    if (!finished()) {
      enumerate();
    }

    relation.clear();

    if (_relation_pos == _nr) {  // no more relations
      return;
    }

    if (_relation_pos != UNDEFINED) {
      while (_relation_pos < _nr) {
        while (_relation_gen < _nrgens) {
          if (!_reduced.get(_enumerate_order[_relation_pos], _relation_gen)
              && (_relation_pos < _lenindex[1]
                  || _reduced.get(_suffix[_enumerate_order[_relation_pos]],
                                  _relation_gen))) {
            relation.push_back(_enumerate_order[_relation_pos]);
            relation.push_back(_relation_gen);
            relation.push_back(
                _right.get(_enumerate_order[_relation_pos], _relation_gen));
            break;
          }
          _relation_gen++;
        }
        if (_relation_gen == _nrgens) {  // then relation is empty
          _relation_gen = 0;
          _relation_pos++;
        } else {
          break;
        }
      }
      _relation_gen++;
    } else {
      // duplicate generators
      if (_relation_gen < _duplicate_gens.size()) {
        relation.push_back(_duplicate_gens[_relation_gen].first);
        relation.push_back(_duplicate_gens[_relation_gen].second);
        _relation_gen++;
      } else {
        _relation_gen = 0;
        _relation_pos++;
        next_relation(relation);
      }
    }
  }

  VOID FROIDURE_PIN::enumerate(size_t limit64) {
    run(limit64);
  }

  VOID FROIDURE_PIN::run() {
    run(LIMIT_MAX);
  }

  VOID FROIDURE_PIN::run(size_t limit64) {
    std::lock_guard<std::mutex> lg(_mtx);
    if (_pos >= _nr || limit64 <= _nr || stopped()) {
      return;
    }
    // Ensure that limit isn't too big
    size_type limit = static_cast<size_type>(limit64);

    if (LIMIT_MAX - _batch_size > _nr) {
      limit = std::max(limit, _nr + _batch_size);
    } else {  // _batch_size is very big for some reason
      limit = _batch_size;
    }

    REPORT("limit = ", limit);
    internal::Timer timer;
    size_t          tid = REPORTER.thread_id(std::this_thread::get_id());

    // product the generators by every generator
    if (_pos < _lenindex[1]) {
      size_type nr_shorter_elements = _nr;
      while (_pos < _lenindex[1]) {
        element_index_type i = _enumerate_order[_pos];
        for (letter_type j = 0; j != _nrgens; ++j) {
          product()(_tmp_product, _elements[i], _gens[j], tid);
#ifdef LIBSEMIGROUPS_STATS
          _nr_products++;
#endif
          auto it = _map.find(_tmp_product);

          if (it != _map.end()) {
            _right.set(i, j, it->second);
            _nr_rules++;
          } else {
            is_one(_tmp_product, _nr);
            _elements.push_back(this->internal_copy(_tmp_product));
            _first.push_back(_first[i]);
            _final.push_back(j);
            _enumerate_order.push_back(_nr);
            _length.push_back(2);
            _map.emplace(_elements.back(), _nr);
            _prefix.push_back(i);
            _reduced.set(i, j, true);
            _right.set(i, j, _nr);
            _suffix.push_back(_letter_to_pos[j]);
            _nr++;
          }
        }
        _pos++;
      }
      for (enumerate_index_type i = 0; i != _pos; ++i) {
        letter_type b = _final[_enumerate_order[i]];
        for (letter_type j = 0; j != _nrgens; ++j) {
          _left.set(_enumerate_order[i], j, _right.get(_letter_to_pos[j], b));
        }
      }
      _wordlen++;
      expand(_nr - nr_shorter_elements);
      _lenindex.push_back(_enumerate_order.size());
    }

    // product the words of length > 1 by every generator
    bool stop = (_nr >= limit || stopped());

    while (_pos != _nr && !stop) {
      size_type nr_shorter_elements = _nr;
      while (_pos != _lenindex[_wordlen + 1] && !stop) {
        element_index_type i = _enumerate_order[_pos];
        letter_type        b = _first[i];
        element_index_type s = _suffix[i];
        for (letter_type j = 0; j != _nrgens; ++j) {
          if (!_reduced.get(s, j)) {
            element_index_type r = _right.get(s, j);
            if (_found_one && r == _pos_one) {
              _right.set(i, j, _letter_to_pos[b]);
            } else if (_prefix[r] != UNDEFINED) {  // r is not a generator
              _right.set(i, j, _right.get(_left.get(_prefix[r], b), _final[r]));
            } else {
              _right.set(i, j, _right.get(_letter_to_pos[b], _final[r]));
            }
          } else {
            product()(_tmp_product, _elements[i], _gens[j], tid);
#ifdef LIBSEMIGROUPS_STATS
            _nr_products++;
#endif
            auto it = _map.find(_tmp_product);

            if (it != _map.end()) {
              _right.set(i, j, it->second);
              _nr_rules++;
            } else {
              is_one(_tmp_product, _nr);
              _elements.push_back(this->internal_copy(_tmp_product));
              _first.push_back(b);
              _final.push_back(j);
              _length.push_back(_wordlen + 2);
              _map.emplace(_elements.back(), _nr);
              _prefix.push_back(i);
              _reduced.set(i, j, true);
              _right.set(i, j, _nr);
              _suffix.push_back(_right.get(s, j));
              _enumerate_order.push_back(_nr);
              _nr++;
              stop = (_nr >= limit || stopped());
            }
          }
        }  // finished applying gens to <_elements.at(_pos)>
        _pos++;
      }  // finished words of length <wordlen> + 1
      expand(_nr - nr_shorter_elements);

      if (_pos > _nr || _pos == _lenindex[_wordlen + 1]) {
        for (enumerate_index_type i = _lenindex[_wordlen]; i != _pos; ++i) {
          element_index_type p = _prefix[_enumerate_order[i]];
          letter_type        b = _final[_enumerate_order[i]];
          for (letter_type j = 0; j != _nrgens; ++j) {
            _left.set(_enumerate_order[i], j, _right.get(_left.get(p, j), b));
          }
        }
        _wordlen++;
        _lenindex.push_back(_enumerate_order.size());
      }
      REPORT("found ",
             _nr,
             " elements, ",
             _nr_rules,
             " rules, max word length ",
             current_max_word_length());
    }
    REPORT("elapsed time = ", timer);
    report_why_we_stopped();
#ifdef LIBSEMIGROUPS_STATS
    REPORT("number of products = ", _nr_products);
#endif
  }

  VOID FROIDURE_PIN::add_generator(element_type const& x) {
    add_generators({x});
  }

  TEMPLATE
  template <class TCollection>
  void FROIDURE_PIN::add_generators(TCollection const& coll) {
    static_assert(!std::is_pointer<TCollection>::value,
                  "TCollection should not be a pointer");
    if (coll.size() == 0) {
      return;
    }
    for (auto it = coll.begin(); it < coll.end(); ++it) {
      element_index_type degree
          = internal_degree()(this->to_internal_const(*it));
      if (degree != _degree) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "new generator " + internal::to_string(it - coll.begin())
            + " has degree " + internal::to_string(degree)
            + " but should have degree " + internal::to_string(_degree));
      }
    }
    internal::Timer timer;
    size_t          tid = REPORTER.thread_id(std::this_thread::get_id());

    // get some parameters from the old semigroup
    letter_type old_nrgens  = _nrgens;
    size_type   old_nr      = _nr;
    size_type   nr_old_left = _pos;

    // erase the old index
    _enumerate_order.erase(_enumerate_order.begin() + _lenindex[1],
                           _enumerate_order.end());

    // old_new[i] indicates if we have seen _elements.at(i) yet in new.
    std::vector<bool> old_new;
    old_new.clear();
    old_new.resize(old_nr, false);
    for (letter_type i = 0; i < _letter_to_pos.size(); i++) {
      old_new[_letter_to_pos[i]] = true;
    }

    // add the new generators to new _gens, _elements, and _enumerate_order
    for (const_reference x : coll) {
      auto it = _map.find(this->to_internal_const(x));
      if (it == _map.end()) {  // new generator
        _gens.push_back(this->internal_copy(this->to_internal_const(x)));
        _elements.push_back(_gens.back());
        _map.emplace(_gens.back(), _nr);

        _first.push_back(_gens.size() - 1);
        _final.push_back(_gens.size() - 1);

        _letter_to_pos.push_back(_nr);
        _enumerate_order.push_back(_nr);

        is_one(this->to_internal_const(x), _nr);
        _prefix.push_back(UNDEFINED);
        _suffix.push_back(UNDEFINED);
        _length.push_back(1);
        _nr++;
      } else if (_letter_to_pos[_first[it->second]] == it->second) {
        _gens.push_back(this->internal_copy(this->to_internal_const(x)));
        // x is one of the existing generators
        _duplicate_gens.push_back(
            std::make_pair(_gens.size() - 1, _first[it->second]));
        // _gens[_gens.size() - 1] = _gens[_first[it->second])]
        // since _first maps element_index_type -> letter_type
        _letter_to_pos.push_back(it->second);
      } else {
        // x is an old element that will now be a generator
        _gens.push_back(_elements[it->second]);
        _letter_to_pos.push_back(it->second);
        _enumerate_order.push_back(it->second);

        _first[it->second]  = _gens.size() - 1;
        _final[it->second]  = _gens.size() - 1;
        _prefix[it->second] = UNDEFINED;
        _suffix[it->second] = UNDEFINED;
        _length[it->second] = UNDEFINED;

        old_new[it->second] = true;
      }
    }

    // reset the data structure
    _idempotents_found = false;
    _nr_rules          = _duplicate_gens.size();
    _pos               = 0;
    _wordlen           = 0;
    _nrgens            = _gens.size();
    _lenindex.clear();
    _lenindex.push_back(0);
    _lenindex.push_back(_nrgens - _duplicate_gens.size());

    // Add columns for new generators
    // FIXME isn't this a bit wasteful, we could recycle the old _reduced, to
    // avoid reallocation
    _reduced = internal::RecVec<bool>(
        _nrgens, _reduced.nr_rows() + _nrgens - old_nrgens);
    _left.add_cols(_nrgens - _left.nr_cols());
    _right.add_cols(_nrgens - _right.nr_cols());

    // Add rows in for newly added generators
    _left.add_rows(_nrgens - old_nrgens);
    _right.add_rows(_nrgens - old_nrgens);

    size_type nr_shorter_elements;

    // Repeat until we have multiplied all of the elements of <old> up to the
    // old value of _pos by all of the (new and old) generators

    while (nr_old_left > 0) {
      nr_shorter_elements = _nr;
      while (_pos < _lenindex[_wordlen + 1] && nr_old_left > 0) {
        element_index_type i = _enumerate_order[_pos];  // position in _elements
        letter_type        b = _first[i];
        element_index_type s = _suffix[i];
        if (_right.get(i, 0) != UNDEFINED) {
          nr_old_left--;
          // _elements[i] is in old semigroup, and its descendants are
          // known
          for (letter_type j = 0; j < old_nrgens; j++) {
            element_index_type k = _right.get(i, j);
            if (!old_new[k]) {  // it's new!
              is_one(_elements[k], k);
              _first[k]  = _first[i];
              _final[k]  = j;
              _length[k] = _wordlen + 2;
              _prefix[k] = i;
              _reduced.set(i, j, true);
              if (_wordlen == 0) {
                _suffix[k] = _letter_to_pos[j];
              } else {
                _suffix[k] = _right.get(s, j);
              }
              _enumerate_order.push_back(k);
              old_new[k] = true;
            } else if (s == UNDEFINED || _reduced.get(s, j)) {
              // this clause could be removed if _nr_rules wasn't necessary
              _nr_rules++;
            }
          }
          for (letter_type j = old_nrgens; j < _nrgens; j++) {
            closure_update(i, j, b, s, old_nr, tid, old_new);
          }
        } else {
          // _elements[i] is either not in old, or it is in old but its
          // descendants are not known
          for (letter_type j = 0; j < _nrgens; j++) {
            closure_update(i, j, b, s, old_nr, tid, old_new);
          }
        }
        _pos++;
      }  // finished words of length <wordlen> + 1

      expand(_nr - nr_shorter_elements);
      if (_pos > _nr || _pos == _lenindex[_wordlen + 1]) {
        if (_wordlen == 0) {
          for (enumerate_index_type i = 0; i < _pos; i++) {
            size_t b = _final[_enumerate_order[i]];
            for (letter_type j = 0; j < _nrgens; j++) {
              // TODO(JDM) reuse old info here!
              _left.set(
                  _enumerate_order[i], j, _right.get(_letter_to_pos[j], b));
            }
          }
        } else {
          for (enumerate_index_type i = _lenindex[_wordlen]; i < _pos; i++) {
            element_index_type p = _prefix[_enumerate_order[i]];
            letter_type        b = _final[_enumerate_order[i]];
            for (letter_type j = 0; j < _nrgens; j++) {
              // TODO(JDM) reuse old info here!
              _left.set(_enumerate_order[i], j, _right.get(_left.get(p, j), b));
            }
          }
        }
        _lenindex.push_back(_enumerate_order.size());
        _wordlen++;
      }
      REPORT("found ",
             _nr,
             " elements, ",
             _nr_rules,
             " rules, max word length ",
             current_max_word_length());
    }
    if (is_begun()) {
      REPORT("elapsed time = ", timer);
    }
    report_why_we_stopped();
  }

  VOID
  FROIDURE_PIN::add_generators(std::initializer_list<const_element_type> coll) {
    add_generators<std::initializer_list<const_element_type>>(coll);
  }

  TEMPLATE
  template <class TCollection>
  FROIDURE_PIN*
  FROIDURE_PIN::copy_add_generators(TCollection const& coll) const {
    static_assert(!std::is_pointer<TCollection>::value,
                  "TCollection should not be a pointer");
    if (coll.size() == 0) {
      return new FroidurePin(*this);
    } else {
      // Partially copy
      FroidurePin* out = new FroidurePin(*this, &coll);
      out->add_generators(coll);
      return out;
    }
  }

  TEMPLATE
  template <class TCollection>
  void FROIDURE_PIN::closure(TCollection const& coll) {
    static_assert(!std::is_pointer<TCollection>::value,
                  "TCollection should not be a pointer");
    if (coll.size() == 0) {
      return;
    } else {
      for (const_reference x : coll) {
        if (!contains(x)) {
          add_generators({x});
        }
      }
    }
  }

  VOID FROIDURE_PIN::closure(std::initializer_list<const_element_type> coll) {
    closure<std::initializer_list<const_element_type>>(coll);
  }

  TEMPLATE
  template <class TCollection>
  FROIDURE_PIN* FROIDURE_PIN::copy_closure(TCollection const& coll) {
    static_assert(!std::is_pointer<TCollection>::value,
                  "TCollection should not be a pointer");
    if (coll.size() == 0) {
      return new FroidurePin(*this);
    } else {
      // The next line is required so that when we call the closure method on
      // out, the partial copy contains enough information to all membership
      // testing without a call to enumerate (which will fail because the
      // partial copy does not contain enough data to run enumerate).
      this->enumerate(LIMIT_MAX);
      // Partially copy
      FroidurePin* out = new FroidurePin(*this, &coll);
      out->closure(coll);
      return out;
    }
  }

  VOID FROIDURE_PIN::set_max_threads(size_t nr_threads) noexcept {
    // TODO check noexcept is ok
    unsigned int n
        = static_cast<unsigned int>(nr_threads == 0 ? 1 : nr_threads);
    _max_threads = std::min(n, std::thread::hardware_concurrency());
  }

  BOOL FROIDURE_PIN::is_monoid() {
    run();
    return _found_one;
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePin - validation methods - private
  ////////////////////////////////////////////////////////////////////////

  VOID FROIDURE_PIN::validate_element_index(element_index_type i) const {
    if (i >= _nr) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "there are only " + internal::to_string(_nr) + " elements"
          + (finished() ? "" : " enumerated so far") + ", but index "
          + internal::to_string(i) + " was given");
    }
  }

  VOID FROIDURE_PIN::validate_letter_index(letter_type i) const {
    if (i >= nr_generators()) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "there are only  " + internal::to_string(nr_generators())
          + " generators, not " + internal::to_string(i));
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePin - enumeration methods - private
  ////////////////////////////////////////////////////////////////////////

  // Expand the data structures in the semigroup with space for nr elements
  INLINE_VOID FROIDURE_PIN::expand(size_type nr) {
    _left.add_rows(nr);
    _reduced.add_rows(nr);
    _right.add_rows(nr);
  }

  // Check if an element is the identity, x should be in the position pos
  // of _elements.
  // FIXME noexcept should depend on whether or not internal_equal_to throws.
  INLINE_VOID FROIDURE_PIN::is_one(internal_const_element_type x,
                                   element_index_type          pos) noexcept {
    if (!_found_one && internal_equal_to()(x, _id)) {
      _pos_one   = pos;
      _found_one = true;
    }
  }

  // _nrgens, _duplicates_gens, _letter_to_pos, and _elements must all be
  // initialised for this to work, and _gens must point to an empty vector.
  VOID FROIDURE_PIN::copy_gens() {
    LIBSEMIGROUPS_ASSERT(_gens.empty());
    _gens.resize(_nrgens);
    std::vector<bool> seen(_nrgens, false);
    // really copy duplicate gens from _elements
    for (std::pair<letter_type, letter_type> const& x : _duplicate_gens) {
      // The degree of everything in _elements has already been increased (if
      // it needs to be at all), and so we do not need to increase the degree
      // in the copy below.
      _gens[x.first] = this->internal_copy(_elements[_letter_to_pos[x.second]]);
      seen[x.first]  = true;
    }
    // the non-duplicate gens are already in _elements, so don't really copy
    for (letter_type i = 0; i < _nrgens; i++) {
      if (!seen[i]) {
        _gens[i] = _elements[_letter_to_pos[i]];
      }
    }
  }

  VOID FROIDURE_PIN::closure_update(element_index_type i,
                                    letter_type        j,
                                    letter_type        b,
                                    element_index_type s,
                                    size_type          old_nr,
                                    size_t const&      tid,
                                    std::vector<bool>& old_new) {
    if (_wordlen != 0 && !_reduced.get(s, j)) {
      element_index_type r = _right.get(s, j);
      if (_found_one && r == _pos_one) {
        _right.set(i, j, _letter_to_pos[b]);
      } else if (_prefix[r] != UNDEFINED) {
        _right.set(i, j, _right.get(_left.get(_prefix[r], b), _final[r]));
      } else {
        _right.set(i, j, _right.get(_letter_to_pos[b], _final[r]));
      }
    } else {
      product()(_tmp_product, _elements[i], _gens[j], tid);
      auto it = _map.find(_tmp_product);
      if (it == _map.end()) {  // it's new!
        is_one(_tmp_product, _nr);
        _elements.push_back(this->internal_copy(_tmp_product));
        _first.push_back(b);
        _final.push_back(j);
        _length.push_back(_wordlen + 2);
        _map.emplace(_elements.back(), _nr);
        _prefix.push_back(i);
        _reduced.set(i, j, true);
        _right.set(i, j, _nr);
        if (_wordlen == 0) {
          _suffix.push_back(_letter_to_pos[j]);
        } else {
          _suffix.push_back(_right.get(s, j));
        }
        _enumerate_order.push_back(_nr);
        _nr++;
      } else if (it->second < old_nr && !old_new[it->second]) {
        // we didn't process it yet!
        is_one(_tmp_product, it->second);
        _first[it->second]  = b;
        _final[it->second]  = j;
        _length[it->second] = _wordlen + 2;
        _prefix[it->second] = i;
        _reduced.set(i, j, true);
        _right.set(i, j, it->second);
        if (_wordlen == 0) {
          _suffix[it->second] = _letter_to_pos[j];
        } else {
          _suffix[it->second] = _right.get(s, j);
        }
        _enumerate_order.push_back(it->second);
        old_new[it->second] = true;
      } else {  // it->second >= old->_nr || old_new[it->second]
        // it's old
        _right.set(i, j, it->second);
        _nr_rules++;
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePin - initialisation methods - private
  ////////////////////////////////////////////////////////////////////////

  // Initialise the data member _sorted. We store a list of pairs consisting
  // of an internal_element_type and element_index_type which is sorted on the
  // first entry using the operator< of the Element class. The second
  // component is then inverted (as a permutation) so that we can then find
  // the position of an element in the sorted list of elements.
  VOID FROIDURE_PIN::init_sorted() {
    if (_sorted.size() == size()) {
      return;
    }
    size_t n = size();
    _sorted.reserve(n);
    for (element_index_type i = 0; i < n; i++) {
      _sorted.emplace_back(_elements[i], i);
    }
    std::sort(_sorted.begin(),
              _sorted.end(),
              [](std::pair<internal_element_type, element_index_type> const& x,
                 std::pair<internal_element_type, element_index_type> const& y)
                  -> bool { return less()(x.first, y.first); });

    // Invert the permutation in _sorted[*].second
    std::vector<element_index_type> tmp_inverter;
    tmp_inverter.resize(n);
    for (element_index_type i = 0; i < n; i++) {
      tmp_inverter[_sorted[i].second] = i;
    }
    for (element_index_type i = 0; i < n; i++) {
      _sorted[i].second = tmp_inverter[i];
    }
  }

  // Find the idempotents and store their pointers and positions in a
  // std::pair of type internal_idempotent_pair.
  VOID FROIDURE_PIN::init_idempotents() {
    if (_idempotents_found) {
      return;
    }
    _idempotents_found = true;
    enumerate();
    _is_idempotent.resize(_nr, false);

    internal::Timer timer;

    // Find the threshold beyond which it is quicker to simply product
    // elements rather than follow a path in the Cayley graph. This is the
    // enumerate_index_type i for which length(i) >= 2 * complexity.
    size_t comp             = std::max(complexity()(_tmp_product), size_t(1));
    size_t threshold_length = std::min(_lenindex.size() - 2, comp - 1);
    enumerate_index_type threshold_index = _lenindex[threshold_length];

    size_t total_load = 0;
    for (size_t i = 1; i <= threshold_length; ++i) {
      total_load += i * (_lenindex[i] - _lenindex[i - 1]);
    }

#ifdef LIBSEMIGROUPS_STATS
    REPORT("complexity of multiplication = ", comp);
    REPORT("multiple words longer than ", threshold_length + 1);
    REPORT("number of paths traced in Cayley graph = ", threshold_index);
    REPORT("mean path length = ", total_load / threshold_index);
    REPORT("number of products = ", _nr - threshold_index);
#endif

    total_load
        += comp
           * (_nr
              - (threshold_length == 0 ? 0 : _lenindex[threshold_length - 1]));

    size_t concurrency_threshold = 823543;

    if (_max_threads == 1 || size() < concurrency_threshold) {
      // Use only 1 thread
      idempotents(0, _nr, threshold_index, _idempotents);
    } else {
      // Use > 1 threads
      size_t                            mean_load = total_load / _max_threads;
      size_t                            len       = 1;
      std::vector<enumerate_index_type> first(_max_threads, 0);
      std::vector<enumerate_index_type> last(_max_threads, _nr);
      std::vector<std::vector<internal_idempotent_pair>> tmp(
          _max_threads, std::vector<internal_idempotent_pair>());
      std::vector<std::thread> threads;
      REPORTER.reset_thread_ids();

      for (size_t i = 0; i < _max_threads - 1; i++) {
        size_t thread_load = 0;
        last[i]            = first[i];
        while (thread_load < mean_load && last[i] < threshold_index) {
          if (last[i] >= _lenindex[len]) {
            ++len;
          }
          thread_load += len;
          ++last[i];
        }
        while (thread_load < mean_load) {
          thread_load += comp;
          ++last[i];
        }
        total_load -= thread_load;
        REPORT("thread ", i + 1, " has load ", thread_load);
        first[i + 1] = last[i];

        threads.emplace_back(&FroidurePin::idempotents,
                             this,
                             first[i],
                             last[i],
                             threshold_index,
                             std::ref(tmp[i]));
      }
      // TODO use less threads if the av_load is too low

      REPORT("thread ", _max_threads, " has load ", total_load);
      threads.emplace_back(&FroidurePin::idempotents,
                           this,
                           first[_max_threads - 1],
                           last[_max_threads - 1],
                           threshold_index,
                           std::ref(tmp[_max_threads - 1]));

      size_t nr_idempotents = 0;
      for (size_t i = 0; i < _max_threads; i++) {
        threads[i].join();
        nr_idempotents += tmp[i].size();
      }
      _idempotents.reserve(nr_idempotents);
      for (size_t i = 0; i < _max_threads; i++) {
        std::copy(
            tmp[i].begin(), tmp[i].end(), std::back_inserter(_idempotents));
      }
    }
    REPORT("elapsed time = ", timer);
  }

  // Find the idempotents in the range [first, last) and store
  // the corresponding std::pair of type internal_idempotent_pair in the 4th
  // parameter. The parameter threshold is the point, calculated in
  // init_idempotents, at which it is better to simply product elements
  // rather than trace in the left/right Cayley graph.
  VOID FROIDURE_PIN::idempotents(
      enumerate_index_type const             first,
      enumerate_index_type const             last,
      enumerate_index_type const             threshold,
      std::vector<internal_idempotent_pair>& idempotents) {
    REPORT("first = ", first, ", last = ", last, ", diff = ", last - first);
    internal::Timer timer;

    enumerate_index_type pos = first;

    for (; pos < std::min(threshold, last); pos++) {
      element_index_type k = _enumerate_order[pos];
      if (!_is_idempotent[k]) {
        // The following is product_by_reduction, don't have to consider
        // lengths because they are equal!!
        element_index_type i = k, j = k;
        while (j != UNDEFINED) {
          i = _right.get(i, _first[j]);
          // TODO improve this if R/L-classes are known to stop performing the
          // product if we fall out of the R/L-class of the initial element.
          j = _suffix[j];
        }
        if (i == k) {
          idempotents.emplace_back(_elements[k], k);
          _is_idempotent[k] = true;
        }
      }
    }

    if (pos >= last) {
      REPORT("elapsed time = ", timer);
      return;
    }

    // Cannot use _tmp_product itself since there are multiple threads here!
    internal_element_type tmp_product = this->internal_copy(_tmp_product);
    size_t                tid = REPORTER.thread_id(std::this_thread::get_id());

    for (; pos < last; pos++) {
      element_index_type k = _enumerate_order[pos];
      if (!_is_idempotent[k]) {
        product()(tmp_product, _elements[k], _elements[k], tid);
        if (internal_equal_to()(tmp_product, _elements[k])) {
          idempotents.emplace_back(_elements[k], k);
          _is_idempotent[k] = true;
        }
      }
    }
    this->internal_free(tmp_product);
    REPORT("elapsed time = ", timer);
  }

  ////////////////////////////////////////////////////////////////////////
  // FroidurePin - iterators - private
  ////////////////////////////////////////////////////////////////////////

  TEMPLATE
  struct FROIDURE_PIN::iterator_methods : private TTraits {
    using wrapped_iterator =
        typename std::vector<internal_element_type>::const_iterator;

    const_reference indirection(wrapped_iterator it) const {
      return this->to_external_const(*it);
    }

    const_pointer addressof(wrapped_iterator it) const {
      return &(this->to_external_const(*it));
    }
  };

  TEMPLATE
  struct FROIDURE_PIN::iterator_methods_pair_first : private TTraits {
    using wrapped_iterator = typename std::vector<
        std::pair<internal_element_type, element_index_type>>::const_iterator;

    const_reference indirection(wrapped_iterator it) const {
      return this->to_external_const((*it).first);
    }

    const_pointer addressof(wrapped_iterator it) const {
      return &(this->to_external_const((*it).first));
    }
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_FROIDURE_PIN_IMPL_HPP_