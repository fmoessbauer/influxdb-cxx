#include "LineSerializerV1.h"
#include "Point.h"
#include <chrono>

// clang-format off
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
// clang-format on

namespace influxdb
{
    void LineSerializerV1::append(const Point& point)
    {
        const auto ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(point.getTimestamp().time_since_epoch());
        lineBuffer += point.getName();
        append(point.getTags());
        append(point.getFields());
        lineBuffer += " " + std::to_string(ns_since_epoch.count());
        lineBuffer += '\n';
    }
    std::string&& LineSerializerV1::finalize_buffer()
    {
        return std::move(lineBuffer);
    }

    void LineSerializerV1::append(const Point::TagContainer& tags)
    {
        for (const auto& tag : tags)
        {
            lineBuffer += ',';
            lineBuffer += tag.first;
            lineBuffer += '=';
            lineBuffer += tag.second;
        }
    }

    void LineSerializerV1::append(const Point::FieldContainer& fields)
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
}