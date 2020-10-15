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
/// \author Adam Wegrzynek
///

#ifndef INFLUXDATA_POINT_H
#define INFLUXDATA_POINT_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <chrono>
#include <variant>

namespace influxdb
{

/// \brief Represents a point
class Point
{
public:
    using FieldValue = std::variant<int, long long int, std::string, double, bool>;
    using FieldItem = std::pair<std::string, FieldValue>;
    using FieldContainer = std::unordered_map<std::string, FieldValue>;

    using TagContainer = std::unordered_map<std::string, std::string>;
    using TimePoint = decltype(std::chrono::system_clock::now());

public:
  /// Constructs point based on measurement name
    explicit Point(std::string_view measurement, TimePoint tp = getCurrentTimestamp())
        : mValue({}),
          mMeasurement(measurement),
          mTimestamp(tp),
          mTags({}), mFields({})
    {
    }

    /// Default destructor
    ~Point() = default;

    /// Adds a tags
    Point&& addTag(std::string_view key, std::string_view value);

    /// Adds field
    Point&& addField(std::string_view name, const FieldValue& value);

    /// Sets custom timestamp
    Point&& setTimestamp(TimePoint timestamp);

    /// Name getter
    std::string getName() const;

    /// View to fields
    FieldContainer getFields() const;

    /// View to tags
    TagContainer getTags() const;

    /// Timestamp getter
    TimePoint getTimestamp() const;

private:
    /// Generetes current timestamp
    static TimePoint getCurrentTimestamp();

protected:
    /// A value
    std::variant<long long int, std::string, double> mValue;

    /// A name
    std::string mMeasurement;

    /// A timestamp
    TimePoint mTimestamp;

    /// Tags
    TagContainer mTags;

    /// Fields
    FieldContainer mFields;
};
  
} // namespace influxdb

#endif // INFLUXDATA_POINT_H
