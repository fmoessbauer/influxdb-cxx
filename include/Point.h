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
/// \author Felix Moessbauer
///

#ifndef INFLUXDATA_POINT_H
#define INFLUXDATA_POINT_H

#include <string>
#include <string_view>
#include <map>
#include <chrono>
#include <variant>
#include <memory_resource>

namespace influxdb
{

/// \brief Represents a point
class Point
{
public:
    using FieldValue = std::variant<int, long long int, std::string, double, bool>;
    using FieldContainer = std::pmr::map<std::pmr::string, FieldValue>;

    using TagContainer = std::pmr::map<std::pmr::string, std::pmr::string>;
    using TimePoint = decltype(std::chrono::system_clock::now());

    using allocator_type = std::pmr::polymorphic_allocator<char>;

public:
    
    /// Constructs point based on measurement name and pool
    explicit Point(std::string_view measurement, const allocator_type & alloc);

    /// Constructs point based on measurement name
    explicit Point(std::string_view measurement,
        TimePoint tp = getCurrentTimestamp(),
        const allocator_type & alloc = {});

    /// Copy constructor with allocator support
    Point(const Point& other, const allocator_type& alloc) 
    : mMeasurement{other.mMeasurement, alloc},
      mTimestamp{other.mTimestamp},
      mTags{other.mTags, alloc},
      mFields{other.mFields, alloc} { }

    /// Move constructor with allocator support
    Point(const Point&& other, const allocator_type& alloc) 
    : mMeasurement{std::move(other.mMeasurement), alloc},
      mTimestamp{other.mTimestamp},
      mTags{std::move(other.mTags), alloc},
      mFields{std::move(other.mFields), alloc} { }

    /// Default destructor
    ~Point() = default;

    /// Adds a tags
    Point&& addTag(std::string_view key, std::string_view value);

    /// Adds field
    Point&& addField(std::string_view name, const FieldValue& value);

    /// Sets custom timestamp
    Point&& setTimestamp(TimePoint timestamp);

    /// View to measurement name
    std::string_view viewName() const;

    /// View to fields
    const FieldContainer & viewFields() const;

    /// View to tags
    const TagContainer & viewTags() const;

    /// Timestamp getter
    TimePoint getTimestamp() const;

    /// visitor entry
    template<typename Visitor>
    void accept(Visitor & visitor) const {
        visitor.visit(*this);
    }

private:
    /// Generetes current timestamp
    static TimePoint getCurrentTimestamp();

protected:
    /// A name
    std::pmr::string mMeasurement;

    /// A timestamp
    TimePoint mTimestamp;

    /// Tags
    TagContainer mTags;

    /// Fields
    FieldContainer mFields;
};
  
} // namespace influxdb

#endif // INFLUXDATA_POINT_H
