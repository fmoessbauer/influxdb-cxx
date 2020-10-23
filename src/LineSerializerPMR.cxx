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

///
/// \author Felix Moessbauer
///

#include "LineSerializerPMR.h"
#include "Point.h"
#include <chrono>

// clang-format off
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
// clang-format on

namespace influxdb
{
    void LineSerializerPMR::visit(const Point& point)
    {
        const auto ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(point.getTimestamp().time_since_epoch());
        lineBuffer += point.viewName();
        append(point.viewTags());
        append(point.viewFields());
        lineBuffer += " " + std::to_string(ns_since_epoch.count());
        lineBuffer += '\n';
    }
    std::string_view LineSerializerPMR::finalize_buffer()
    {
        return lineBuffer;
    }

    void LineSerializerPMR::append(const Point::TagContainer& tags)
    {
        for (const auto& tag : tags)
        {
            lineBuffer += ',';
            lineBuffer += tag.first;
            lineBuffer += '=';
            lineBuffer += tag.second;
        }
    }

    void LineSerializerPMR::append(const Point::FieldContainer& fields)
    {
        if (!fields.empty())
            lineBuffer += ' ';

        int i = 0;
        for (const auto& field : fields)
        {
            if (++i != 1)
                lineBuffer += ",";

            lineBuffer += field.first + "=";
            std::visit(overloaded{
                           [&](int v) { lineBuffer += std::to_string(v) + 'i'; },
                           [&](long long int v) { lineBuffer += std::to_string(v) + 'i'; },
                           [&](double v) { lineBuffer += std::to_string(v); },
                           [&](const std::string& v) { lineBuffer += '"' + v + '"'; },
                           [&](bool v) { lineBuffer += std::to_string(v); }},
                       field.second);
        }
    }

    void LineSerializerPMR::reset() {
        lineBuffer.clear();
    }
}