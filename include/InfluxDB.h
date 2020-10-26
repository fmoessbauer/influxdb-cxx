// MIT License
//
// Copyright (c) 2019 Adam Wegrzynek
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
/// \author Adam Wegrzynek
/// \author Felix Moessbauer
///

#ifndef INFLUXDATA_INFLUXDB_H
#define INFLUXDATA_INFLUXDB_H

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <deque>

#include "Transport.h"
#include "Point.h"
#include "LineSerializer.h"
#include "influxdb_export.h"

namespace influxdb
{

class INFLUXDB_EXPORT InfluxDB
{
  public:
    /// Disable copy constructor
    InfluxDB & operator=(const InfluxDB&) = delete;

    /// Disable copy constructor
    InfluxDB(const InfluxDB&) = delete;

    /// Constructor required valid transport
    explicit InfluxDB(std::unique_ptr<Transport> transport, std::unique_ptr<LineSerializer> serializer);

    /// Flushes buffer
    ~InfluxDB() noexcept(false);

    /**
     * \brief Writes a point (and takes ownership)
     * \param point Point to consume
     *
     * \note In case of an exception, the function can be called again to send the same data.
     *       If that is not intended, the user must call \ref clearBatch().
     */
    void write(Point&& point);

    /**
     * \brief Writes a point (without taking ownership)
     */
    void write(const Point& point);

    void write(std::vector<Point> && points) {
      write_batch(points);
    }

    /**
     * \brief Writes a vector of point
     * \param point Container with points
     *
     * \note sematics of this function are equal to calling \ref write with each point.
     */
    template<typename Container>
    void write_batch(Container &&points) {
      for (auto &&point : points) { 
        point.accept(*serial);
      }
      mBatchCurSize += points.size();
      flushBatch();
    }

    /**
     * \brief clears the current send buffer
     *
     * In case of unsuccessful transmissions, we do not clear the buffer
     * to give the user the option to inspect the error in the exception
     * and try to re-submit the batch using \ref flushBatch().
     *
     * In case of successful transmissions, the buffer is cleared automatically.
     */
    void clearBatch() noexcept {
      serial->reset();
      mBatchCurSize = 0;
    }

    /// Queries InfluxDB database
    std::vector<Point> query(std::string_view query);

    /// Create InfluxDB database if does not exists
    void createDatabaseIfNotExists();

    /**
     * \brief Flushes points batched (this can also happens when buffer is full)
     *
     * \note In case of an exception, the function can be called again to send the same data.
     *       If that is not intended, the user must call \ref clearBatch().
     */
    void flushBatch();

    /// Enables points batching
    /// \param size
    void batchOf(const std::size_t size = 32) { mBatchMaxSize = size; }

private:
    /// Transmits string over transport
    void transmit(std::string_view serializedBatch);

  private:
    /// Maximum number of points to batch
    std::size_t mBatchMaxSize;

    /// Number of points in the current batch
    std::size_t mBatchCurSize;

    /// Underlying transport UDP/HTTP/Unix socket
    std::unique_ptr<Transport> mTransport;

    /// serializer instance
    std::unique_ptr<LineSerializer> serial;
};

} // namespace influxdb

#endif // INFLUXDATA_INFLUXDB_H
