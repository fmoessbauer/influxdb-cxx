// MIT License
//
// Copyright (c) 2020 Felix Moessbauer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#define BOOST_TEST_MODULE Test InfluxDB PmrPoint
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <chrono>
#include <memory_resource>

#include "Point.h"
#include "LineSerializerPMR.h"

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

    BOOST_AUTO_TEST_CASE(SerializeToV1)
    {
        std::vector<unsigned char> buffer(500);
        std::pmr::monotonic_buffer_resource pool{buffer.data(), buffer.size(), std::pmr::null_memory_resource()};
        LineSerializerPMR serial{&pool};
        {
            Point point{"test", std::chrono::system_clock::time_point{}};
            point.addTag("type", "voltage")
                    .addTag("class", "major")
                    .addField("value", 10LL);

            point.accept(serial);
            BOOST_CHECK_EQUAL(
                serial.finalize_buffer(), "test,class=major,type=voltage value=10i 0\n"
            );
        }
        serial.reset();
        BOOST_CHECK_EQUAL(serial.finalize_buffer(), "");
    }
} // namespace influxdb::test
