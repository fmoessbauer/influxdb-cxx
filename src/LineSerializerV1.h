#include "Point.h"
#include <string>

namespace influxdb
{
    // TODO: inheritance
    class LineSerializerV1
    {
    public:
        void append(const Point& point);
        std::string&& finalize_buffer();

    private:
        void append(const Point::TagContainer& tags);
        void append(const Point::FieldContainer& fields);

    private:
        std::string lineBuffer;
    };
}