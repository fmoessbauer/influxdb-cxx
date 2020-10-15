#define BOOST_TEST_MODULE Test InfluxDB PmrPoint
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <chrono>
#include <memory_resource>

#include "Point.h"

#define PRINT_SIZES
#ifdef PRINT_SIZES
#include <iostream>
#endif

namespace influxdb::test
{

    BOOST_AUTO_TEST_CASE(ValidAllocation)
    {
        std::vector<unsigned char> buffer(1000);
        std::pmr::monotonic_buffer_resource pool{buffer.data(), buffer.size(), std::pmr::null_memory_resource()};

        auto point = Point{"test", &pool}
                         .addTag("type", "voltage")
                         .addTag("class", "major")
                         .addField("value", 10LL);
    }

    BOOST_AUTO_TEST_CASE(BufferTooSmall)
    {
        std::vector<unsigned char> buffer(200);
        std::pmr::monotonic_buffer_resource pool{buffer.data(), buffer.size(), std::pmr::null_memory_resource()};

#ifdef PRINT_SIZES
        std::cout << "sizeof(Point)            " << sizeof(Point) << std::endl;
        std::cout << "sizeof(std::string)      " << sizeof(std::string) << std::endl;
        std::cout << "sizeof(std::pmr::string) " << sizeof(std::pmr::string) << std::endl;
        std::cout << "SSO size:                " << std::pmr::string{}.capacity() << std::endl;
#endif

        auto point = Point{"test", &pool};        
        BOOST_REQUIRE_THROW(
            point.addTag("type", "voltage")
                .addTag("class", "major")
                .addField("value", 10LL),
            std::bad_alloc);
    }
} // namespace influxdb::test
