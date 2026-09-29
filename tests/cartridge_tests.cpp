#include "cartridge.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("parseHeader rejects data shorter than 16 bytes") {
  std::vector<uint8_t> data = {'N', 'E', 'S', 0x01};
  REQUIRE_THROWS_AS(parseHeader(data), std::invalid_argument);
}

TEST_CASE("parseHeader missing magic bytes") {
  std::vector<uint8_t> data = {'N', 'B', 'S', 0x01};
  REQUIRE_THROWS_AS(parseHeader(data), std::invalid_argument);
}