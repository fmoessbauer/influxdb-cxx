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

#ifndef INFLUXDATA_TRANSPORTS_HTTP_H
#define INFLUXDATA_TRANSPORTS_HTTP_H

#include "Transport.h"
#include <curl/curl.h>
#include <memory>
#include <string>

namespace influxdb::transports
{

/// \brief HTTP transport
class HTTP : public Transport
{
public:
  /// Constructor
  explicit HTTP(std::string_view url);

  /// Default destructor
  ~HTTP() override;

  /// Sends point via HTTP POST
  ///  \throw InfluxDBException	when CURL fails on POSTing or response code != 200
  void send(std::string_view lineprotocol) override;

  /// Queries database
  /// \throw InfluxDBException	when CURL GET fails
  std::string query(std::string_view query) override;

  /// Creates database used at url if it does not exists
  /// \throw InfluxDBException	when CURL POST fails
  void createDatabase() override;

  /// Enable Basic Auth
  /// \param auth <username>:<password>
  void enableBasicAuth(std::string_view auth);

  /// Get the database name managed by this transport
  [[nodiscard]] std::string databaseName() const;

  /// Get the influx service url which transport connects to
  [[nodiscard]] std::string influxDbServiceUrl() const;

private:

  /// Obtain InfluxDB service url from the url passed
  void obtainInfluxServiceUrl(std::string_view url);

  /// Obtain database name from the url passed
  void obtainDatabaseName(std::string_view url);

  /// Initilizes CURL for writting and common options
  /// \throw InfluxDBException	if database (?db=) not specified
  void initCurl(std::string_view url);

  /// Initializes CURL for reading
  void initCurlRead(std::string_view url);

  /// treats responses of CURL requests
  void treatCurlResponse(const CURLcode &response, long responseCode) const;

  /// CURL pointer configured for writting points
  CURL *writeHandle;

  /// CURL pointer configured for querying
  CURL *readHandle;

  /// basic authentification password
  std::string mAuthPasswd;
  /// InfluxDB read URL
  std::string mReadUrl;

  /// InfluxDB service URL
  std::string mInfluxDbServiceUrl;

  /// Database name used
  std::string mDatabaseName;
};

} // namespace influxdb

#endif // INFLUXDATA_TRANSPORTS_HTTP_H
