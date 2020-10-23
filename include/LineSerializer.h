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