#pragma once
#include "Point.h"
#include <string>
#include <memory_resource>

namespace influxdb
{
    class LineSerializer
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<char>;

    public:
        explicit LineSerializer(const allocator_type& alloc = {})
            : monoBuf{alloc.resource()},
              lineBuffer{&monoBuf}
        {
        }

        LineSerializer(const LineSerializer&) = delete;
        LineSerializer(LineSerializer&& other, allocator_type& alloc)
            : lineBuffer{other.lineBuffer, alloc}
        {
        }

        LineSerializer& operator=(const LineSerializer&) = delete;
        LineSerializer& operator=(LineSerializer&&) = default;

        /// serialize the point and append to buffer
        void visit(
            /// point to serialize
            const Point& point);

        /**
         * \brief finalize the buffer and return a view
         * \return view to the final buffer
         */
        std::string_view finalize_buffer();

        /// clear the state of the serializer
        void reset();

    private:
        void append(const Point::TagContainer& tags);
        void append(const Point::FieldContainer& fields);

    private:
        std::pmr::monotonic_buffer_resource monoBuf;
        std::pmr::string lineBuffer;
    };
}