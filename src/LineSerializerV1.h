#pragma once
#include "Point.h"
#include <string>
#include <memory_resource>

namespace influxdb
{
    // TODO: inheritance
    class LineSerializerV1
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<char>;

    public:
        explicit LineSerializerV1(const allocator_type& alloc = {})
            : monoBuf{alloc.resource()},
            lineBuffer{&monoBuf}
        {
        }

        LineSerializerV1(const LineSerializerV1&) = delete;
        LineSerializerV1(LineSerializerV1&& other, allocator_type& alloc)
            : lineBuffer{other.lineBuffer, alloc}
        {
        }

        LineSerializerV1& operator=(const LineSerializerV1&) = delete;
        LineSerializerV1& operator=(LineSerializerV1&&) = default;

        void visit(const Point& point);
        std::string_view finalize_buffer();
        void reset();

    private:
        void append(const Point::TagContainer& tags);
        void append(const Point::FieldContainer& fields);

    private:
        std::pmr::monotonic_buffer_resource monoBuf;
        std::pmr::string lineBuffer;
    };
}