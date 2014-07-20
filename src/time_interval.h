/*
 * scrm is an implementation of the Sequential-Coalescent-with-Recombination Model.
 * 
 * Copyright (C) 2013, 2014 Paul R. Staab, Sha (Joe) Zhu and Gerton Lunter
 * 
 * This file is part of scrm.
 * 
 * scrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.  * * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef scrm_src_time_interval
#define scrm_src_time_interval

#include <unordered_set>
#include "forest.h"

class Forest;
class TimeIntervalIterator;

class TimeInterval {
 public:
  friend class TimeIntervalIterator;

#ifdef UNITTEST
  friend class TestTimeInterval;
  friend class Forest;
#endif

  TimeInterval();
  TimeInterval(TimeIntervalIterator const* tii, double start_height, double end_height);
  ~TimeInterval() { };

  double start_height() const { return this->start_height_; };
  double end_height()   const { return this->end_height_; };
  double length()       const { return (end_height() - start_height()); };
  const Forest &forest() const;

  size_t numberOfContemporaries(const size_t pop = 0) const;

  Node* getRandomContemporary(const size_t pop = 0) const;

  const std::unordered_set<Node*> &contemporaries(const size_t pop = 0) const;

 private:
  double start_height_;
  double end_height_; 
  TimeIntervalIterator const* tii_;
};


/* WARNING: DON'T USE MULTIPLE OF THESE AT THE SAME TIME */
class TimeIntervalIterator {
 public:
  TimeIntervalIterator();
  TimeIntervalIterator(Forest *forest, Node *start_node);
  ~TimeIntervalIterator() {};

  void next();
  bool good() const { return this->good_; }

  TimeInterval operator*() const { return current_interval_; }
  TimeInterval operator++() { next(); return current_interval_; }
  TimeInterval operator++(int) { TimeInterval tmp = current_interval_;
                          next();
                          return tmp; }

  // Splits the current interval in two parts by adding a node inside the interval;
  // Only affects the event after the next "next()" which than represents the
  // second part of the interval.
  void splitCurrentInterval(Node* splitting_node, Node* del_node = NULL) {
    this->inside_node_ = splitting_node;
    if (del_node != NULL) removeFromContemporaries(del_node);
  };

  void recalculateInterval();

  void removeFromContemporaries(Node* node) {
    contemporaries_.at(node->population()).erase(node);
  }

  void addToContemporaries(Node* node) { 
    // Assert that the node is not already in the contemporaries
    assert( contemporaries_.at(node->population()).find(node) == 
            contemporaries_.at(node->population()).end() );
    contemporaries_.at(node->population()).insert(node);
  };

  size_t numberOfContemporaries(const size_t pop = 0) const { 
    return contemporaries_.at(pop).size(); 
  };

  void clearContemporaries() {
    for (auto it = contemporaries_.begin(); it != contemporaries_.end(); ++it) {
      (*it).clear();
    }
  }

  void searchContemporariesOfNode(Node *node);
  void searchContemporariesOfNodeTopDown(Node *node, Node *current_node = NULL);
  void updateContemporaries(Node *current_node); 

  const std::unordered_set<Node*> &contemporaries(const size_t pop = 0) const { 
    return contemporaries_.at(pop); 
  };
  const Forest &forest() const { return *forest_; };

#ifdef UNITTEST
  friend class TestTimeInterval;
#endif

 private:
  TimeIntervalIterator( TimeIntervalIterator const &other );
  TimeIntervalIterator& operator= ( TimeIntervalIterator const &other ); 

  Forest* forest_;
  TimeInterval current_interval_;
  double current_time_;
  std::vector<std::unordered_set<Node*> > contemporaries_;
  NodeIterator node_iterator_;

  bool good_;
  bool model_changed_;

  Node* inside_node_;
};

inline const Forest &TimeInterval::forest() const { return tii_->forest(); }

inline size_t TimeInterval::numberOfContemporaries(const size_t pop) const {
  return tii_->numberOfContemporaries(pop); 
}

inline const std::unordered_set<Node*> &TimeInterval::contemporaries(const size_t pop) const {
  return tii_->contemporaries(pop); 
}
#endif
