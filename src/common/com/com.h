#pragma once

#include <cmath>
#include <functional>
#include <vector>
#include <Wire.h>
#include <SoftwareSerial.h>

#include "common/com/defs.h"
#include "common/com/specialized_encoding.h"
#include "common/utility/utility.h"

/*
 * This service uses I2C / TWI pins to communicate.
 * Uno, Ethernet	A4 (SDA), A5 (SCL)
 * Mega2560	20 (SDA), 21 (SCL)
 * Leonardo	2 (SDA), 3 (SCL)
 * Due	20 (SDA), 21 (SCL), SDA1, SCL1
 */

namespace common::com {


class Com {
public:
  static bool SlaveInit(uint8_t address) {
    [[maybe_unused]] static auto initialized = [=]() -> bool {
      Wire.begin(address);
      return true;
    }();
    return initialized;
  }

  static bool MasterInit() {
    [[maybe_unused]] static auto initialized = []() -> bool {
      Wire.begin();
      return true;
    }();
    return initialized;
  }

private:
  static PROGMEM constexpr const uint8_t kStrMetadataSize{8};

  template <typename ComType>
  inline static void WriteBytesAndFlush(
      ComType &com, const char *data, size_t bytes) {
    // TODO: add timeout
    while (!com.availableForWrite()) {}
    com.write(data, bytes);
    com.flush();
  }

  template <typename ComType>
  static void WriteBytesAndFlush(
      ComType &com, const char *data, size_t bytes, uint16_t chunk_size) {
    if (!chunk_size) {
      WriteBytesAndFlush(com, data, bytes);
      return;
    }
    uint16_t num_chunk = 1 + (bytes - 1) / static_cast<size_t>(chunk_size);

    // Encode metadata
    {
      std::string chunk_size_encoded, num_chunk_encoded, bytes_encoded;
      common::com::Encode(chunk_size, chunk_size_encoded);
      common::com::Encode(num_chunk, num_chunk_encoded);
      common::com::Encode(static_cast<uint32_t>(bytes), bytes_encoded);
      std::string metadata =
          chunk_size_encoded + num_chunk_encoded + bytes_encoded;
      WriteBytesAndFlush(com, metadata.c_str(), metadata.size());
    }
    uint8_t status;
    Com::Read(com, status, 0);

    // Encode chunk
    for (uint16_t i{0}; i < num_chunk; i += (status == 0)) {
      size_t cur_idx = static_cast<size_t>(i * chunk_size);
      size_t cur_chunk_size = std::min(chunk_size, bytes - cur_idx);
      WriteBytesAndFlush(com, data + cur_idx, cur_chunk_size);
      Com::Read(com, status, 0);
    }
  }

  template <typename ComType>
  static void ReadBytes(ComType &com, std::string &data,
                        size_t bytes, uint16_t chunk_size,
                        uint32_t timeout_ms) {
    if (!chunk_size) {
      data.resize(bytes);
      if (CheckAndWaitForMsgToBeAvilable(com, bytes,true,
                                         timeout_ms, kWaitTimeMs)) {
        com.readBytes(&data[0], bytes);
      }
      return;
    }

    size_t available = com.available();
    std::string metadata(kStrMetadataSize, 0);
    if (!CheckAndWaitForMsgToBeAvilable(com, kStrMetadataSize, true,
                                   timeout_ms, kWaitTimeMs)) {
      return;
    }
    com.readBytes(&metadata[0], kStrMetadataSize);
    uint16_t sender_chunk_size, num_chunk;
    uint32_t data_len;
    common::com::Decode(metadata.substr(0, 2), sender_chunk_size);
    common::com::Decode(metadata.substr(2, 2), num_chunk);
    common::com::Decode(metadata.substr(4, 4), data_len);
    if (available != kStrMetadataSize) {
      com.write(uint8_t(1));
      com.flush();
    } else if (sender_chunk_size > chunk_size) {
      com.write(uint8_t(2));
      com.flush();
    } else {
      com.write(uint8_t(0));
      com.flush();
    }

    data.resize(data_len);
    uint8_t status = {0};
    for (uint16_t i{0}; i < num_chunk; i += (status == 0)) {
      size_t idx{i * sender_chunk_size};
      size_t expected_size = (i + 1) == num_chunk ? data_len - idx :
                                                    sender_chunk_size;
      status = 0;
      if (!CheckAndWaitForMsgToBeAvilable(com, expected_size, true,
                                          timeout_ms, kWaitTimeMs)) {
        status = 1;
      }
      com.readBytes(&data[idx], expected_size);
      while (!com.availableForWrite()) {}
      com.write(status);
      com.flush();
    }
  }

protected:
  static PROGMEM constexpr const uint16_t kWaitTimeMs{5};

  template<typename ComType>
  inline static void Write(ComType &com,
                           const std::string &msg, uint16_t chunk_size) {
    WriteBytesAndFlush(com, msg.c_str(), msg.size(), chunk_size);
  }

  template<typename ComType, typename MsgType,
      typename = std::enable_if_t<std::is_arithmetic<MsgType>::value>,
      typename = std::enable_if_t<IsEncodible<MsgType>::value>>
  inline static void Write(ComType &com, const MsgType &msg, uint16_t = 0) {
    std::string buffer;
    common::com::Encode(msg, buffer);
    WriteBytesAndFlush(com, buffer.c_str(), buffer.size(), 0);
  }

  template<typename ComType, typename MsgType,
      typename = std::enable_if_t<IsEncodible<MsgType>::value>,
      typename = std::enable_if_t<!std::is_arithmetic<MsgType>::value>,
      typename = std::nullptr_t>
  inline static void Write(
      ComType &com, const MsgType &msg, uint16_t chunk_size) {
    std::string encoded_msg;
    common::com::Encode(msg, encoded_msg);
    WriteBytesAndFlush(com, encoded_msg.c_str(),
                       encoded_msg.size(), chunk_size);
  }

  template<typename ComType>
  inline static void Read(ComType &com, std::string &msg, uint32_t timeout_ms,
                          int bytes, uint16_t chunk_size) {
    ReadBytes(com, msg, bytes, chunk_size, timeout_ms);
  }

  template<typename MsgType, typename ComType,
      typename = std::enable_if_t<std::is_arithmetic<MsgType>::value>,
      typename = std::enable_if_t<IsEncodible<MsgType>::value>>
  inline static void Read(ComType &com, MsgType &msg, uint32_t timeout_ms,
                          int = 0, uint16_t = 0) {
    std::string buffer;
    ReadBytes(com, buffer, sizeof(MsgType), 0, 0);
    common::com::Decode(buffer, msg);
  }

  template<typename MsgType, typename ComType,
      typename = std::enable_if_t<IsEncodible<MsgType>::value>,
      typename = std::enable_if_t<!std::is_arithmetic<MsgType>::value>,
      typename = std::nullptr_t>
  inline static void Read(ComType &com, MsgType &msg, uint32_t timeout_ms,
                          int bytes, uint16_t chunk_size) {
    std::string msg_str;
    Read(com, msg_str, bytes, chunk_size, timeout_ms);
    common::com::Decode(msg_str, msg);
  }

  template<typename ComType>
  static bool CheckAndWaitForMsgToBeAvilable(
      ComType &com, size_t expected_bytes, bool blocking,
      uint32_t timeout_ms, uint32_t delay_time_ms) {
    uint32_t tick{0};
    while (static_cast<size_t>(com.available()) < expected_bytes) {
      if (!blocking || (timeout_ms && ++tick * delay_time_ms >= timeout_ms)) {
        return false;
      }
      delay(delay_time_ms);
    }
    return true;
  }

  template <typename ComType>
  inline static void Drain(ComType& com) {
    while (com.available()) {
      com.read();
    }
  }
};

namespace I2C {

constexpr const uint16_t kChunkSize = 0;

class Reply final : public Com {
public:
  Reply() = delete;

  template<typename MsgType>
  static void RegisterCallback(void (*callback)(MsgType &)) {
    CallbackFunctionStore<MsgType>() = callback;
    auto _callback = []() {
      MsgType msg;
      auto callback = Reply::CallbackFunctionStore<MsgType>();
      (*callback)(msg);
      Write(Wire, msg, kChunkSize);
    };
    static decltype(_callback) callback_static = _callback;
    Wire.onRequest(callback_static);
  }

  template<class ClassType, typename MsgType>
  static void RegisterCallback(
      ClassType *class_ptr, void (ClassType::*callback)(MsgType &)) {
    ClassCallbackMethodStore<ClassType, MsgType>() = callback;
    ClassPtrStore<ClassType>() = class_ptr;
    auto _callback = []() {
      MsgType msg;
      auto callback =
          Reply::ClassCallbackMethodStore<ClassType, MsgType>();
      ((Reply::ClassPtrStore<ClassType>())->*callback)(msg);
      Write(Wire, msg, kChunkSize);
    };
    static decltype(_callback) callback_static = _callback;
    Wire.onRequest(callback_static);
  }

private:
  template<typename MsgType>
  struct FunctionTypeImpl {
    typedef void (*FunctionPtrType)(MsgType &);
  };

  template<class ClassType, typename MsgType>
  struct ClassMethodImpl {
    typedef void (ClassType::*ClassMethodPtrType)(MsgType &);
  };

  template<typename MsgType>
  static auto CallbackFunctionStore()
  -> typename FunctionTypeImpl<MsgType>::FunctionPtrType & {
    static void (*local_callback)(MsgType &);
    return local_callback;
  }

  template<class ClassType, typename MsgType>
  static auto ClassCallbackMethodStore()
  -> typename ClassMethodImpl<ClassType, MsgType>::ClassMethodPtrType & {
    static void (ClassType::*local_callback)(MsgType &);
    return local_callback;
  }

  template<class ClassType>
  static ClassType *&ClassPtrStore() {
    static ClassType *local_ptr{nullptr};
    return local_ptr;
  }
};

class Request final : public Com {
public:
  Request() = delete;

  template<typename MsgType,
      typename = std::enable_if_t<std::is_arithmetic<MsgType>::value>>
  static void RequestFrom(uint8_t address,
                          MsgType &msg,
                          uint32_t timeout_ms = 0) {
    if (!MasterInit()) {
      return;
    }
    Wire.setWireTimeout(timeout_ms);
    Wire.requestFrom(address,
                     static_cast<uint8_t>(sizeof(MsgType) + 1),
                     static_cast<uint8_t>(true));
    Read(Wire, msg, timeout_ms);
  }

  static void RequestFrom(uint8_t address,
                          uint8_t quantity,
                          std::string &msg,
                          uint32_t timeout_ms = 0) {
    if (!MasterInit()) {
      return;
    }
    Wire.setWireTimeout(timeout_ms);
    Wire.requestFrom(address, quantity, static_cast<uint8_t>(true));
    Read(Wire, msg, quantity, kChunkSize, timeout_ms);
  }

  template<typename MsgType,
      typename = std::enable_if_t<IsEncodible<MsgType>::value>,
      typename = std::enable_if_t<!std::is_arithmetic<MsgType>::value>>
  static void RequestFrom(uint8_t address,
                          uint8_t quantity,
                          MsgType &msg,
                          uint32_t timeout_ms = 0) {
    std::string encoded_msg;
    RequestFrom(address, quantity, msg, timeout_ms);
    msg.Decode(encoded_msg);
  }

};

class Subscriber final : public Com {
public:
  Subscriber() = delete;

  template<typename MsgType>
  static void RegisterCallback(void (*callback)(const MsgType &, int)) {
    CallbackFunctionStore<MsgType>() = callback;
    auto _callback = [](int bytes) {
      if (Wire.available()) {
        MsgType msg;
        Read(Wire, msg, bytes, kChunkSize, 0);
        auto callback = Subscriber::CallbackFunctionStore<MsgType>();
        (*callback)(common::move(msg), bytes);
      }
    };
    static decltype(_callback) callback_static = _callback;
    Wire.onReceive(callback_static);
  }

  template<class ClassType, typename MsgType>
  static void RegisterCallback(
      ClassType *class_ptr, void (ClassType::*callback)(const MsgType &, int)) {
    ClassCallbackMethodStore<ClassType, MsgType>() = callback;
    ClassPtrStore<ClassType>() = class_ptr;
    auto _callback = [](int bytes) {
      if (Wire.available()) {
        MsgType msg;
        Read(Wire, msg, bytes, kChunkSize, 0);
        auto callback =
            Subscriber::ClassCallbackMethodStore<ClassType, MsgType>();
        ((Subscriber::ClassPtrStore<ClassType>())->*callback)(
            common::move(msg), bytes);
      }
    };
    static decltype(_callback) callback_static = _callback;
    Wire.onReceive(callback_static);
  }

private:
  template<typename MsgType>
  struct FunctionTypeImpl {
    typedef void (*FunctionPtrType)(const MsgType &, int);
  };

  template<class ClassType, typename MsgType>
  struct ClassMethodImpl {
    typedef void (ClassType::*ClassMethodPtrType)(const MsgType &, int);
  };

  template<typename MsgType>
  static auto CallbackFunctionStore()
  -> typename FunctionTypeImpl<MsgType>::FunctionPtrType & {
    static void (*local_callback)(const MsgType &, int);
    return local_callback;
  }

  template<class ClassType, typename MsgType>
  static auto ClassCallbackMethodStore()
  -> typename ClassMethodImpl<ClassType, MsgType>::ClassMethodPtrType & {
    static void (ClassType::*local_callback)(const MsgType &, int);
    return local_callback;
  }

  template<class ClassType>
  static ClassType *&ClassPtrStore() {
    static ClassType *local_ptr{nullptr};
    return local_ptr;
  }
};

class Publisher final : public Com {
public:
  Publisher() = delete;

  template<typename MsgType>
  static uint8_t Publish(uint8_t address,
                         const MsgType &msg,
                         uint32_t timeout_ms = 0) {
    if (!MasterInit()) {
      return -1;
    }
    Wire.setWireTimeout(timeout_ms);
    Wire.beginTransmission(address);
    Write(Wire, msg, kChunkSize);
    return (Wire.endTransmission());
  }
};

}  // namespace I2C

namespace UART {

constexpr uint16_t kChunkSize = 32;

namespace internal {

template <typename SerialType>
class UARTComBase : public Com {
protected:
  template <typename MsgType>
  inline static void Write(SerialType& com,
                    const MsgType& msg, uint32_t timeout_ms) {
    com.setTimeout(timeout_ms);
    Com::Write(com, msg, kChunkSize);
  }

  template <typename MsgType>
  inline static bool Read(SerialType& com, MsgType& msg,
                          bool blocking, bool drain, uint32_t timeout_ms) {
    if (CheckAndWaitForMsgToBeAvilable(
        com, 1, blocking, timeout_ms, kWaitTimeMs)) {
      com.setTimeout(timeout_ms);
      Com::Read(com, msg, 0, kChunkSize, timeout_ms);
      if (drain) {
        Drain(com);
      }
      return true;
    }
    return false;
  }
};

}  // namespace internal

template <uint8_t RX_, uint8_t TX_>
class SoftwareCom final : public internal::UARTComBase<SoftwareSerial> {
public:
  static constexpr uint8_t RX = RX_;
  static constexpr uint8_t TX = TX_;
  SoftwareCom() = delete;

  static void Init(long baud_rate) {
    [[maybe_unused]] static bool _ = [=]() -> bool {
      GetSerial().begin(baud_rate);
      while (!GetSerial()) {}
      return true;
    }();
  }

  template <typename MsgType>
  static void Write(const MsgType& msg, uint32_t timeout_ms = 0) {
    internal::UARTComBase<SoftwareSerial>::Write(GetSerial(), msg, timeout_ms);
  }

  template <typename MsgType>
  static bool Read(MsgType& msg, bool blocking = false, bool drain = false,
                   uint32_t timeout_ms = 0) {
  return internal::UARTComBase<SoftwareSerial>::Read(
      GetSerial(), msg, blocking, drain, timeout_ms);
  }

private:
  static SoftwareSerial& GetSerial() {
    static SoftwareSerial serial(RX, TX);
    return serial;
  }
};

template <uint8_t SerialPort = 0>
class HardwareCom final : public internal::UARTComBase<HardwareSerial> {
public:
  static_assert(SerialPort <= 3, "Serial Port does not exist");
  HardwareCom() = delete;

  static bool Init(long baud_rate) {
    [[maybe_unused]] static bool status = [&]() -> bool {
      switch (SerialPort) {
        case 0: {
          GetHardwareSerialPtr() = &Serial;
          break;
        }
#if defined(UBRR1H)
        case 1: {
          GetHardwareSerialPtr() = &Serial1;
          break;
        }
#endif
#if defined(UBRR2H)
        case 2: {
          GetHardwareSerialPtr() = &Serial2;
          break;
        }
#endif
#if defined(UBRR2H)
        case 3: {
          GetHardwareSerialPtr() = &Serial3;
          break;
        }
#endif
        default: {
          return false;
        }
      }
      GetHardwareSerialPtr()->begin(baud_rate);
      while (!(*GetHardwareSerialPtr())) {}

      return true;
    }();
    return status;
  }

  template <typename MsgType>
  static void Write(const MsgType& msg, uint32_t timeout_ms = 0) {
    internal::UARTComBase<HardwareSerial>::Write(
        *GetHardwareSerialPtr(), msg, timeout_ms);
  }

  template <typename MsgType,
      typename = std::enable_if_t<std::is_arithmetic<MsgType>::value>>
  static bool Read(MsgType& msg, bool blocking = false, bool drain = false,
                   uint32_t timeout_ms = 0) {
    return internal::UARTComBase<HardwareSerial>::Read(
        *GetHardwareSerialPtr(), msg, blocking, drain, timeout_ms);
  }

private:
  static HardwareSerial*& GetHardwareSerialPtr() {
    static HardwareSerial* ptr;
    return ptr;
  }
};

}  // namespace UART

}  // namespace common::com
