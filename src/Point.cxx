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

#include <iostream>
#include <chrono>
#include <memory>
#include <sstream>
#include <iomanip>

namespace influxdb
{

//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

Point&& Point::addField(std::string_view name, const Point::FieldValue & value)
{
  if (name.empty())
  {
    // TODO: why? better throw exeption
    return std::move(*this);
  }

  mFields.emplace_back(name, value);
  #if 0
  /** TODO: refactor */
  std::stringstream convert;
  convert << std::setprecision(floatsPrecision);
  if (!mFields.empty()) convert << ",";

  convert << name << "=";
  std::visit(overloaded {
    [&convert](int v) { convert << v << 'i'; },
    [&convert](long long int v) { convert << v << 'i'; },
    [&convert](double v) { convert  << std::fixed << v; },
    [&convert](const std::string& v) { convert << '"' << v << '"'; },
    }, value);
  mFields += convert.str();
  #endif
  return std::move(*this);
}

Point&& Point::addTag(std::string_view key, std::string_view value)
{
  if (value.empty())
  {
    // TODO: why? better throw exeption
    return std::move(*this);
  }
  mTags.emplace_back(key, value);
  #if 0
  mTags += ",";
  mTags += key;
  mTags += "=";
  mTags += value;
  #endif
  return std::move(*this);
}

#if 0
std::string Point::toLineProtocol() const
{
  return mMeasurement + mTags + " " + mFields + " " + std::to_string(
    std::chrono::duration_cast <std::chrono::nanoseconds>(mTimestamp.time_since_epoch()).count()
  );
}
#endif

std::string Point::getName() const
{
  return mMeasurement;
}

Point::TimePoint Point::getTimestamp() const
{
  return mTimestamp;
}

Point::FieldsView Point::getFieldsView() const
{
  return {mFields.cbegin(), mFields.cend()};
}

Point::TagsView Point::getTagsView() const
{
  return {mTags.cbegin(), mTags.cend()};
}

Point::TimePoint Point::getCurrentTimestamp(){
  return std::chrono::system_clock::now();
}

} // namespace influxdb
