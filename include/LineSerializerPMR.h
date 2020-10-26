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

#pragma once
#include "LineSerializer.h"
#include "Point.h"
#include <string>
#include <memory_resource>
#include "influxdb_export.h"

namespace influxdb
{
    class INFLUXDB_EXPORT LineSerializerPMR : public LineSerializer
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