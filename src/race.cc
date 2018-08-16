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

// This file contains the implementation of the Race class template for
// competitively running different functions/methods in different threads, and
// obtaining the winner.
//

#include "internal/race.h"

namespace libsemigroups {

  Race::Race()
      : _max_threads(std::thread::hardware_concurrency()),
        _mtx(),
        _winner(nullptr) {}

  void Race::set_max_threads(size_t val) {
    if (val == 0) {
      throw LIBSEMIGROUPS_EXCEPTION("the minimum number of threads is 1");
    }
    _max_threads = val;
  }

  Runner* Race::winner() {
    run();
    return _winner;
  }

  void Race::add_runner(Runner* r) {
    if (_winner != nullptr) {
      throw LIBSEMIGROUPS_EXCEPTION("the race is over, cannot add runners");
    }
    _runners.push_back(r);
  }

  typename std::vector<Runner*>::const_iterator Race::begin() const {
    return _runners.cbegin();
  }

  typename std::vector<Runner*>::const_iterator Race::end() const {
    return _runners.cend();
  }

  bool Race::empty() const {
    return _runners.empty();
  }

  void Race::run() {
    run_func(std::mem_fn(&Runner::run));
  }

  void Race::run_for(std::chrono::nanoseconds x) {
    run_func([&x](Runner* rnnr) -> void { rnnr->run_for(x); });
  }
}  // namespace libsemigroups
