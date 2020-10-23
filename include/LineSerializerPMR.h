#pragma once
#include "LineSerializer.h"
#include "Point.h"
#include <string>
#include <memory_resource>

namespace influxdb
{
    class LineSerializerPMR : public LineSerializer
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<char>;

    public:
        explicit LineSerializerPMR(const allocator_type& alloc = {})
            : lineBuffer{alloc}
        {
        }

        LineSerializerPMR(const LineSerializerPMR&) = delete;
        LineSerializerPMR(LineSerializerPMR&& other, allocator_type& alloc)
            : lineBuffer{other.lineBuffer, alloc}
        {
        }

        LineSerializerPMR& operator=(const LineSerializerPMR&) = delete;
        LineSerializerPMR& operator=(LineSerializerPMR&&) = delete;

        /// serialize the point and append to buffer
        void visit(
            /// point to serialize
            const Point& point) override;

        /**
         * \brief finalize the buffer and return a view
         * \return view to the final buffer
         */
        std::string_view finalize_buffer() override;

        /// clear the state of the serializer
        void reset() override;

    private:
        void append(const Point::TagContainer& tags);
        void append(const Point::FieldContainer& fields);

    private:
        std::pmr::string lineBuffer;
    };
}