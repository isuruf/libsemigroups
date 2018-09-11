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

#include <stddef.h>  // for size_t

#include <vector>  // for vector

#include "bmat8.hpp"                 // for BMat8
#include "froidure-pin.hpp"          // for FroidurePin, FroidurePi...
#include "libsemigroups-config.hpp"  // for LIBSEMIGROUPS_SIZEOF_VO...
#include "libsemigroups.tests.hpp"   // for LIBSEMIGROUPS_TEST_CASE
#include "types.hpp"                 // for word_type

namespace libsemigroups {
  struct LibsemigroupsException;

  bool constexpr REPORT = false;

#if (!(defined(LIBSEMIGROUPS_DENSEHASHMAP)) && LIBSEMIGROUPS_SIZEOF_VOID_P == 8)

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "095",
                          "(BMat8) regular boolean mat monoid 4",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8> S(gens);
    REPORTER.set_report(REPORT);

    REQUIRE(S.current_max_word_length() == 1);
    REQUIRE(!S.finished());
    REQUIRE(!S.is_begun());
    REQUIRE(S.current_position(S.generator(0) * S.generator(3)) == UNDEFINED);
    REQUIRE(S.current_position(BMat8({{1, 0, 0, 1, 1},
                                      {0, 1, 0, 0, 1},
                                      {1, 0, 1, 0, 1},
                                      {0, 0, 1, 0, 1},
                                      {0, 0, 0, 0, 0}}))
            == UNDEFINED);
    REQUIRE(S.current_size() == 4);
    REQUIRE(S.current_nr_rules() == 0);
    REQUIRE(S.length_const(0) == 1);
    REQUIRE(S.length_non_const(5) == 2);

    REQUIRE(S.size() == 63904);
    REQUIRE(S.nr_idempotents() == 2360);
    REQUIRE(S.word_to_pos({0, 1, 2, 0, 1, 2}) == 378);
    REQUIRE(S.word_to_element({0, 1, 2, 0, 1, 2})
            == BMat8({{1, 0, 0, 1}, {0, 1, 0, 0}, {1, 0, 1, 0}, {0, 0, 1, 0}}));
    REQUIRE(S.current_max_word_length() == 21);
    REQUIRE(S.degree() == 8);
    REQUIRE(S.nr_generators() == 4);
    REQUIRE(S.generator(0)
            == BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    REQUIRE(S.generator(1)
            == BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    REQUIRE(S.generator(2)
            == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
    REQUIRE(S.generator(3)
            == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
    REQUIRE(S.finished());
    REQUIRE(S.is_begun());
    REQUIRE(S.current_position(S.generator(0) * S.generator(3)) == 7);
    REQUIRE(S.current_position(BMat8({{1, 0, 0, 1, 1},
                                      {0, 1, 0, 0, 1},
                                      {1, 0, 1, 0, 1},
                                      {0, 0, 1, 0, 1},
                                      {0, 0, 0, 0, 0}}))
            == UNDEFINED);
    REQUIRE(S.current_nr_rules() == 13716);
    REQUIRE(S.prefix(0) == UNDEFINED);
    REQUIRE(S.suffix(0) == UNDEFINED);
    REQUIRE(S.first_letter(0) == 0);
    REQUIRE(S.final_letter(0) == 0);
    REQUIRE(S.batch_size() == 8192);
    REQUIRE(S.length_const(0) == 1);
    REQUIRE(S.length_const(7) == 2);
    REQUIRE(S.length_const(63903) == 21);
    REQUIRE(S.length_non_const(7) == 2);
    REQUIRE(S.length_non_const(63903) == 21);
    REQUIRE(S.product_by_reduction(0, 3) == 7);
    REQUIRE(S.fast_product(0, 3) == 7);
    REQUIRE(S.letter_to_pos(0) == 0);
    REQUIRE(S.letter_to_pos(1) == 1);
    REQUIRE(S.letter_to_pos(2) == 2);
    REQUIRE(S.letter_to_pos(3) == 3);
    REQUIRE(!S.is_idempotent(0));
    REQUIRE(S.is_idempotent(3));
    REQUIRE(!S.is_idempotent(7));
    REQUIRE(S.nr_rules() == 13716);
    REQUIRE(S.contains(S.generator(1)));
    REQUIRE(!S.contains(BMat8({{1, 0, 0, 1, 1},
                               {0, 1, 0, 0, 1},
                               {1, 0, 1, 0, 1},
                               {0, 0, 1, 0, 1},
                               {0, 0, 0, 0, 0}})));
    REQUIRE(S.position(S.generator(1)) == 1);
    REQUIRE(S.position(S.generator(0) * S.generator(3)) == 7);
    REQUIRE(S.position(BMat8({{1, 0, 0, 1, 1},
                              {0, 1, 0, 0, 1},
                              {1, 0, 1, 0, 1},
                              {0, 0, 1, 0, 1},
                              {0, 0, 0, 0, 0}}))
            == UNDEFINED);

    REQUIRE(S.sorted_position(BMat8({{1, 0, 0, 1, 1},
                                     {0, 1, 0, 0, 1},
                                     {1, 0, 1, 0, 1},
                                     {0, 0, 1, 0, 1},
                                     {0, 0, 0, 0, 0}}))
            == UNDEFINED);
    REQUIRE(S.sorted_position(S.generator(0)) == 18185);
    REQUIRE(S.sorted_position(S.generator(3)) == 33066);
    REQUIRE(S.sorted_position(S.generator(0) * S.generator(3)) == 18184);
    REQUIRE(S.position_to_sorted_position(0) == 18185);
    REQUIRE(S.position_to_sorted_position(3) == 33066);
    REQUIRE(S.position_to_sorted_position(7) == 18184);

    REQUIRE(S.at(7) == S.generator(0) * S.generator(3));
    REQUIRE(S[7] == S[0] * S[3]);

    REQUIRE(S.sorted_at(18185) == S.at(0));
    REQUIRE(S.sorted_at(33066) == S.at(3));
    REQUIRE(S.sorted_at(18184) == S.generator(0) * S.generator(3));

    REQUIRE(S.right(0, 3) == 7);
    REQUIRE(S.right(0, 3) == 7);

    REQUIRE(S.left(0, 3) == 7);
    REQUIRE(S.left(0, 3) == 7);

    word_type w;
    S.minimal_factorisation(w, 378);
    REQUIRE(w == word_type({0, 1, 2, 0, 1, 2}));
    REQUIRE(S.length_const(378) == 6);

    REQUIRE(S.minimal_factorisation(S.at(378))
            == word_type({0, 1, 2, 0, 1, 2}));

    REQUIRE_THROWS_AS(S.minimal_factorisation(BMat8({{1, 0, 0, 1, 1},
                                                     {0, 1, 0, 0, 1},
                                                     {1, 0, 1, 0, 1},
                                                     {0, 0, 1, 0, 1},
                                                     {0, 0, 0, 0, 0}})),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(S.minimal_factorisation(1000000), LibsemigroupsException);

    w.clear();
    S.factorisation(w, 378);
    REQUIRE(w == word_type({0, 1, 2, 0, 1, 2}));
    REQUIRE(S.length_const(378) == 6);

    REQUIRE(S.factorisation(S.at(378)) == word_type({0, 1, 2, 0, 1, 2}));

    REQUIRE_THROWS_AS(S.factorisation(BMat8({{1, 0, 0, 1, 1},
                                             {0, 1, 0, 0, 1},
                                             {1, 0, 1, 0, 1},
                                             {0, 0, 1, 0, 1},
                                             {0, 0, 0, 0, 0}})),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(S.factorisation(1000000), LibsemigroupsException);

    S.next_relation(w);
    REQUIRE(w == std::vector<size_t>({2, 2, 2}));
    S.next_relation(w);
    REQUIRE(w == std::vector<size_t>({3, 0, 7}));
    S.next_relation(w);
    REQUIRE(w == std::vector<size_t>({3, 2, 3}));

    size_t pos = 0;
    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      REQUIRE(S.position(*it) == pos);
      pos++;
    }
    REQUIRE(pos == S.size());

    // Copy - after enumerate
    FroidurePin<BMat8> T(S);
    REQUIRE(T.size() == 63904);
    REQUIRE(T.nr_idempotents() == 2360);
    REQUIRE(T.word_to_pos({0, 1, 2, 0, 1, 2}) == 378);
    REQUIRE(T.word_to_element({0, 1, 2, 0, 1, 2})
            == BMat8({{1, 0, 0, 1}, {0, 1, 0, 0}, {1, 0, 1, 0}, {0, 0, 1, 0}}));
    REQUIRE(T.current_max_word_length() == 21);
    REQUIRE(T.degree() == 8);
    REQUIRE(T.nr_generators() == 4);
    REQUIRE(T.generator(0)
            == BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    REQUIRE(T.generator(1)
            == BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    REQUIRE(T.generator(2)
            == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
    REQUIRE(T.generator(3)
            == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
    REQUIRE(T.finished());
    REQUIRE(T.is_begun());
  }
#endif

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "096",
                          "(BMat8) exception zero generators given",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens;

    REQUIRE_THROWS_AS(FroidurePin<BMat8>(gens), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "097",
                          "(BMat8) exception word_to_element",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    FroidurePin<BMat8> S(gens);

    REQUIRE_THROWS_AS(S.word_to_element({}), LibsemigroupsException);
    REQUIRE_NOTHROW(S.word_to_element({0}));
    REQUIRE_NOTHROW(S.word_to_element({0, 3, 0, 3, 1}));
    REQUIRE_THROWS_AS(S.word_to_element({0, 1, 0, 4}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "098",
                          "(BMat8) exception prefix",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);

    for (size_t i = 0; i < S.size(); ++i) {
      REQUIRE_NOTHROW(S.prefix(i));
      REQUIRE_THROWS_AS(S.prefix(i + S.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "099",
                          "(BMat8) exception suffix",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "100",
                          "(BMat8) exception first_letter",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);

    for (size_t i = 0; i < S.size(); ++i) {
      REQUIRE_NOTHROW(S.first_letter(i));
      REQUIRE_THROWS_AS(S.first_letter(i + S.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "101",
                          "(BMat8) exception final_letter",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "102",
                          "(BMat8) exception length_const",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);

    for (size_t i = 0; i < S.size(); ++i) {
      REQUIRE_NOTHROW(S.length_const(i));
      REQUIRE_THROWS_AS(S.length_const(i + S.size()), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "103",
                          "(BMat8) exception product_by_reduction",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);

    for (size_t i = 1; i < S.size(); ++i) {
      for (size_t j = 1; j < S.size(); ++j) {
        REQUIRE_NOTHROW(S.product_by_reduction(i, j));
        REQUIRE_THROWS_AS(S.product_by_reduction(i + S.size(), j),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(S.product_by_reduction(i, j + S.size()),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(S.product_by_reduction(i + S.size(), j + S.size()),
                          LibsemigroupsException);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "104",
                          "(BMat8) exception fast_product",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
    FroidurePin<BMat8> S(gens);

    for (size_t i = 1; i < S.size(); ++i) {
      for (size_t j = 1; j < S.size(); ++j) {
        REQUIRE_NOTHROW(S.fast_product(i, j));
        REQUIRE_THROWS_AS(S.fast_product(i + S.size(), j),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(S.fast_product(i, j + S.size()),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(S.fast_product(i + S.size(), j + S.size()),
                          LibsemigroupsException);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "105",
                          "(BMat8) exception is_idempotent",
                          "[quick][froidure-pin][bmat8]") {
    std::vector<BMat8> gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    FroidurePin<BMat8> S(gens);

    // S has size 63904
    for (size_t i = 0; i < 63904; ++i) {
      REQUIRE_NOTHROW(S.is_idempotent(i));
    }
    for (size_t i = 0; i < 20; ++i) {
      REQUIRE_THROWS_AS(S.is_idempotent(63904 + i), LibsemigroupsException);
    }
  }
}  // namespace libsemigroups