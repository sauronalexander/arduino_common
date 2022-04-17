#pragma once

#include "common/event/defs.h"

namespace common::mongodb {
namespace traits {

template <typename>
struct IsJsonDocument : std::false_type {};

template <unsigned int Size>
struct IsJsonDocument<StaticJsonDocument<Size>> : std::true_type {};

template <>
struct IsJsonDocument<DynamicJsonDocument> : std::true_type {};

}  // namespace traits

constexpr const size_t kDatabaseMetadataSize = 256;

template <typename T,
          typename = std::enable_if_t<
              traits::IsJsonDocument<std::decay_t<T>>::value>>
std::string SerializeToMongDBPayload(
    const std::string& database,
    const std::string& data_source,
    const std::string& collection,
    T&& document) {
  DynamicJsonDocument payload(document.size() + kDatabaseMetadataSize);
  payload["database"] = database;
  payload["dataSource"] = data_source;
  payload["collection"] = collection;
  JsonObject doc = payload.createNestedObject("document");
  doc.set(document.template as<JsonObject>());

  std::string ret;
  serializeJson(payload, ret);
  return ret;
}

}  // namespace common::mongodb
