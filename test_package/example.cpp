#include <InfluxDBFactory.h>
#include <Point.h>

int main()
{
    // TODO: in this environment, we do not have a running influxdb
    // hence, simulate with a batch and clear it before termination.
    // By that, no http request should be made (this unforunately
    // relies on internal implementation details)
    auto influxdb = influxdb::InfluxDBFactory::Get("http://localhost:8086/?db=test");
    influxdb->batchOf(2);
    influxdb->write(influxdb::Point{"test"});
    influxdb->clearBatch();
    return 0;
}
