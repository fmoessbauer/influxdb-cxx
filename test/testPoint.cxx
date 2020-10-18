// MIT License
//
// Copyright (c) 2019 Adam Wegrzynek
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

#define BOOST_TEST_MODULE Test InfluxDB Point
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>

#include "InfluxDBFactory.h"
#include "InfluxDBException.h"
#include <iterator>

namespace influxdb::test {

std::vector<std::string> getVector(const Point& point)
{
  LineSerializer v1serial;
  point.accept(v1serial);
  // TODO: refactor
  std::string_view view = v1serial.finalize_buffer();
  std::istringstream result(std::string(view), std::ios_base::in);

  return std::vector<std::string>{std::istream_iterator<std::string>{result},
                      std::istream_iterator<std::string>{}};
}

BOOST_AUTO_TEST_CASE(test1)
{
  auto point = Point{"test"}
    .addField("value", 10LL);

  auto result = getVector(point);

  BOOST_CHECK_EQUAL(result[0], "test");
  BOOST_CHECK_EQUAL(result[1], "value=10i");
}

BOOST_AUTO_TEST_CASE(test2)
{
  auto point = Point{"test"}
    .addField("value", 10LL)
    .addField("dvalue", 10.1);

  auto result = getVector(point);

  BOOST_CHECK_EQUAL(result[0], "test");
  // TODO: refactor this check after precision is implemented
  BOOST_CHECK_EQUAL(result[1], "dvalue="+std::to_string(10.1)+",value=10i");
}

BOOST_AUTO_TEST_CASE(test3)
{
  auto point = Point{"test"}
    .addField("value", 10LL)
    .addField("dvalue", 10.1)
    .addTag("tag", "tagval");

  auto result = getVector(point);

  BOOST_CHECK_EQUAL(result[0], "test,tag=tagval");
  // TODO: refactor this check after precision is implemented
  BOOST_CHECK_EQUAL(result[1], "dvalue="+std::to_string(10.1)+",value=10i");
}

BOOST_AUTO_TEST_CASE(test4)
{
  auto point = Point{"test"}
    .addField("value", 10)
    .addField("value", 100LL)
    .setTimestamp(std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(1572830914)));

  auto result = getVector(point);
  BOOST_CHECK_EQUAL(result[2], "1572830914000000");
}

BOOST_AUTO_TEST_CASE(fieldsWithEmptyNameAreNotAdded)
{
  auto point = Point{"test"};
  BOOST_REQUIRE_THROW(point.addField("", 10), InvalidData);

  BOOST_CHECK_EQUAL(point.viewFields().size(), 0);
}
#if 0
BOOST_AUTO_TEST_CASE(floatFieldsPrecisionCanBeAdjusted)
{
  //Point::floatsPrecision = 3;
  auto pointPrecision3 = Point{"test"}.addField("float_field", 3.123456789);
  BOOST_CHECK_EQUAL(pointPrecision3.viewFields(), "float_field=3.123");

  //Point::floatsPrecision = 1;
  auto pointPrecision1 = Point{"test"}.addField("float_field", 50.123456789);
  BOOST_CHECK_EQUAL(pointPrecision1.viewFields(), "float_field=50.1");

  pointPrecision1 = Point{"test"}.addField("float_field", 5.99);
  BOOST_CHECK_EQUAL(pointPrecision1.viewFields(), "float_field=6.0");
}

BOOST_AUTO_TEST_CASE(floatFieldsPrecisionWithScientificValues)
{
  //Point::floatsPrecision = 5;
  auto pointPrecision5 = Point{"test"}.addField("float_field", 123456789.0);
  BOOST_CHECK_EQUAL(pointPrecision5.viewFields(), "float_field=123456789.00000");

  pointPrecision5 = Point{"test"}.addField("float_field", 1.23456789E+8);
  BOOST_CHECK_EQUAL(pointPrecision5.viewFields(), "float_field=123456789.00000");

  pointPrecision5 = Point{"test"}.addField("float_field", 1.23456789E-3);
  BOOST_CHECK_EQUAL(pointPrecision5.viewFields(), "float_field=0.00123");

  pointPrecision5 = Point{"test"}.addField("float_field", 1.23456789E-6);
  BOOST_CHECK_EQUAL(pointPrecision5.viewFields(), "float_field=0.00000");
}
#endif

} // namespace influxdb::test
