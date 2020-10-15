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

///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Point.h"
#include "InfluxDBException.h"

#include <iostream>
#include <chrono>
#include <memory>
#include <sstream>
#include <iomanip>

namespace influxdb
{
    Point::Point(std::string_view measurement, std::pmr::memory_resource* pool)
        : Point(measurement, getCurrentTimestamp(), pool) { }

    Point::Point(std::string_view measurement, TimePoint tp, std::pmr::memory_resource * pool)
        : mMeasurement(measurement, pool),
          mTimestamp(tp),
          mTags(pool), mFields(pool)
    {
    }

    Point&& Point::addField(std::string_view name, const Point::FieldValue& value)
    {
        if (name.empty() || value.valueless_by_exception())
        {
            throw InvalidData(__func__, "field name or value invalid");
        }
        mFields.emplace(name, value);
        return std::move(*this);
    }

    Point&& Point::addTag(std::string_view key, std::string_view value)
    {
        if (key.empty())
        {
            throw InvalidData(__func__, "key empty");
        }
        if(!value.empty()){
            mTags.emplace(key, value);
        }
        return std::move(*this);
    }

    Point&& Point::setTimestamp(Point::TimePoint timestamp)
    {
        mTimestamp = timestamp;
        return std::move(*this);
    }

    std::string_view Point::viewName() const
    {
        return mMeasurement;
    }

    Point::TimePoint Point::getTimestamp() const
    {
        return mTimestamp;
    }

    const Point::FieldContainer & Point::viewFields() const
    {
        return mFields;
    }

    const Point::TagContainer & Point::viewTags() const
    {
        return mTags;
    }

    Point::TimePoint Point::getCurrentTimestamp()
    {
        return std::chrono::system_clock::now();
    }

} // namespace influxdb
