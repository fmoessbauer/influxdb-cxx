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
#include "Point.h"
#include <string>

namespace influxdb
{
    /**
     * \brief Interface of the LineSerializer
     */
    class LineSerializer {
        public:
        virtual ~LineSerializer() {}

        /// serialize the point and append to buffer
        virtual void visit(
            /// point to serialize
            const Point& point) = 0;

        /**
         * \brief finalize the buffer and return a view
         * \return view to the final buffer
         */
        virtual std::string_view finalize_buffer() = 0;

        /// clear the state of the serializer
        virtual void reset() = 0;
    };
}