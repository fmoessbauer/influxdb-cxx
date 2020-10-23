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
