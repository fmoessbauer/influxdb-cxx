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

#define BOOST_TEST_MODULE Test InfluxDB
#define BOOST_TEST_DYN_LINK

#include <boost/test/included/unit_test.hpp>
#include <memory_resource>
#include <vector>

#include "InfluxDBFactory.h"
#include "InfluxDBException.h"

namespace influxdb::test
{
    namespace
    {
        /// helper function to output the buffer
        template <typename Container>
        void printBuffer(const Container& container)
        {
            std::cout << "Buffer:" << std::endl;
            std::for_each(container.begin(), container.end(), [](auto c) {
                if (c == 0)
                    std::cout << ".";
                else if (c > '0' && c < 'z')
                    std::cout << static_cast<char>(c);
                else
                    std::cout << '-';
            });
            std::cout << std::endl;
        }

        void printPoolOpts(const std::pmr::pool_options& opts){
            std::cout << "max_blocks_per_chunk:        " << opts.max_blocks_per_chunk << std::endl;
            std::cout << "largest_required_pool_block: " << opts.largest_required_pool_block << std::endl;
        }
    }

    BOOST_AUTO_TEST_CASE(NoAllocOnHotPath)
    {
        std::pmr::unsynchronized_pool_resource pool{};
        printPoolOpts(pool.options());

        auto influxdb = influxdb::InfluxDBFactory::Get("http://localhost:8086/?db=test", &pool);
        BOOST_CHECK_NO_THROW(influxdb->createDatabaseIfNotExists());

        std::vector<char> pointBuffer(2048);
        for (int i = 0; i < 10; ++i)
        {
            std::pmr::monotonic_buffer_resource mono{pointBuffer.data(), pointBuffer.size()};

            influxdb->batchOf(3);
            BOOST_CHECK_NO_THROW(
                influxdb->write(Point{"test", &mono}.addTag("str_value", "P0").addTag("tag", "PMR").addField("value", 42)));
            BOOST_CHECK_NO_THROW(
                influxdb->write(Point{"test", &mono}.addTag("str_value", "P1").addTag("tag", "PMR").addField("value", 43)));
            BOOST_CHECK_NO_THROW(
                influxdb->write(Point{"test", &mono}.addTag("str_value", "P2").addTag("tag", "VeryLongTagWithoutSSO").addField("value", 43)));
            BOOST_CHECK_NO_THROW(influxdb->flushBatch());

            printBuffer(pointBuffer);
        }

        BOOST_CHECK_NO_THROW(
            influxdb->write(Point{"test"}.addField("str_value", "P3").addTag("tag", "PMR").addField("value", 43)));
        BOOST_CHECK_NO_THROW(influxdb->flushBatch());
    }
}
