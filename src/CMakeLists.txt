add_library(InfluxDB
  InfluxDB.cxx
  Point.cxx
  LineSerializerPMR.cxx
  InfluxDBFactory.cxx
  $<$<BOOL:${Boost_FOUND}>:UDP.cxx>
  $<$<BOOL:${Boost_FOUND}>:UnixSocket.cxx>
  HTTP.cxx
)
generate_export_header(InfluxDB)

target_include_directories(InfluxDB
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    # for export header
    ${PROJECT_BINARY_DIR}/src
)

# Link targets
target_link_libraries(InfluxDB
  PRIVATE
    $<$<BOOL:${Boost_FOUND}>:Boost::system>
    CURL::libcurl
    Threads::Threads
)


# Use C++17
target_compile_features(InfluxDB PUBLIC cxx_std_${CMAKE_CXX_STANDARD})

# Set compile definition if Boost found
target_compile_definitions(InfluxDB
  PRIVATE
    $<$<BOOL:${Boost_FOUND}>:INFLUXDB_WITH_BOOST>
)

# for packaging, avoid uppercase library names
# we currently do not track ABI compability,
# hence always assume an ABI break on a
# new (not patchonly version)
set_target_properties(InfluxDB PROPERTIES
  OUTPUT_NAME influxdb-cxx
  VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")