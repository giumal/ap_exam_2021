#include "catch.hpp"

#include "list_pool.hpp"
#include <algorithm> // max_element, min_element

SCENARIO("getting confident with the addresses"){
  list_pool<int, std::size_t> pool{16};
  auto l = pool.new_list();

  REQUIRE( l == 0);
  l = pool.push_front(42,l);
  REQUIRE( l == 1);

}

SCENARIO("basic operations on lists") {
  GIVEN("a generic pool") {
    list_pool<int, std::size_t> pool{16};

    GIVEN("a new list") {
      auto l = pool.new_list();
      
      THEN("the list is empty")
	REQUIRE(pool.is_empty(l));

      WHEN("we add one value") {
        l = pool.push_back(1, l);
        REQUIRE(pool.value(l) == 1);
      }

      WHEN("we delete the only node") {
        l = pool.free(l);
	THEN("the list is empty again")
	  REQUIRE(pool.is_empty(l));

        //empty lists point to pool.end()
        REQUIRE(l == pool.end());
      }

      WHEN("we add two values") {
        l = pool.push_front(1, l);
        REQUIRE(pool.value(l) == 1);
        l = pool.push_back(2, l);

        auto tmp = pool.next(l);
        REQUIRE(pool.value(tmp) == 2);

        l = pool.free_list(l);
        REQUIRE(pool.is_empty(l) == true);
      }
    }
  }
}

SCENARIO("handling multiple lists") {
  GIVEN("a simple pool") {
    list_pool<int, uint16_t> pool{};
    WHEN("we create two lists") {
      auto l1 = pool.new_list();
      l1 = pool.push_front(3, l1);
      l1 = pool.push_front(2, l1);
      l1 = pool.push_front(1, l1);

      auto l2 = pool.new_list();

      l2 = pool.push_back(4, l2);
      l2 = pool.push_back(5, l2);

      // scan l1
      auto tmp = l1;
      REQUIRE(pool.value(tmp) == 1);
      tmp = pool.next(tmp);
      REQUIRE(pool.value(tmp) == 2);
      tmp = pool.next(tmp);
      REQUIRE(pool.value(tmp) == 3);
      tmp = pool.next(tmp);
      REQUIRE(tmp == pool.end());

      // scan l2
      tmp = l2;
      REQUIRE(pool.value(tmp) == 4);
      tmp = pool.next(tmp);
      REQUIRE(pool.value(tmp) == 5);

      tmp = pool.next(tmp);
      REQUIRE(tmp == pool.end());
    }
  }
}

SCENARIO("using nodes of a deleted list") {
  GIVEN("a list") {
    list_pool<int, uint16_t> pool{};
    auto l1 = pool.new_list();
    l1 = pool.push_front(3, l1);
    l1 = pool.push_front(2, l1);
    l1 = pool.push_front(1, l1);

    auto capacity = pool.capacity();

    WHEN("we delete the list") {
      l1 = pool.free_list(l1);

      THEN("l1 is now empty") { REQUIRE(pool.is_empty(l1)); }

      WHEN("we create a new list we use the old nodes") {
        auto l2 = pool.new_list();

        l2 = pool.push_back(4, l2);
        l2 = pool.push_back(5, l2);

        auto tmp = l2;
        REQUIRE(pool.value(tmp) == 4);
        tmp = pool.next(tmp);
        REQUIRE(pool.value(tmp) == 5);

        tmp = pool.next(tmp);
        REQUIRE(tmp == pool.end());

        THEN("since we added just two nodes, the capacity didn't change")
        REQUIRE(pool.capacity() == capacity);
      }
    }
  }
}

SCENARIO("using iterators"){
  GIVEN("two lists"){
    list_pool<int, uint16_t> pool{};
    auto l1 = pool.new_list();
    // credits: pi as random number generator :)
    l1 = pool.push_front(3, l1);
    l1 = pool.push_front(1, l1);
    l1 = pool.push_front(4, l1);
    l1 = pool.push_front(1, l1);
    l1 = pool.push_front(5, l1);
    l1 = pool.push_front(9, l1);
    l1 = pool.push_front(2, l1);
    l1 = pool.push_front(6, l1);
    l1 = pool.push_front(5, l1);
    l1 = pool.push_front(3, l1);
    l1 = pool.push_front(5, l1);
    
    auto l2 = pool.new_list();
    l2 = pool.push_front(8, l2);
    l2 = pool.push_front(9, l2);
    l2 = pool.push_front(7, l2);
    l2 = pool.push_front(9, l2);
    l2 = pool.push_front(3, l2);
    l2 = pool.push_front(1, l2);
    l2 = pool.push_front(1, l2);
    l2 = pool.push_front(5, l2);
    l2 = pool.push_front(9, l2);
    l2 = pool.push_front(9, l2);
    l2 = pool.push_front(7, l2);

    THEN("find the max on l1"){
      auto m = std::max_element(pool.begin(l1), pool.end(l1));
      REQUIRE(*m == 9);
    }

    THEN("find the min on l2"){
      auto m = std::min_element(pool.begin(l2), pool.end(l2));
      REQUIRE(*m == 1);
    }
    
    
  }

}