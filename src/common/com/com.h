#pragma once

#include <functional>
#include <vector>
#include <Wire.h>
#include <SoftwareSerial.h>

#include "common/com/defs.h"
#include "common/com/specialized_encoding.h"
#include "common/stl/string.h"
#include "common/stl/utility.h"


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

protected:
  template <typename ComType>
  static void Drain(ComType& com) {
    while (com.available()) {
      com.read();
    }
  }

  using StringLengthSize = unsigned int;

  template<typename MsgType>
  static auto HasEncodingMethodImpl(...) -> std::false_type;

  template<typename MsgType,
      typename = decltype(&MsgType::Encode),
      typename = decltype(&MsgType::Decode)>
  static auto HasEncodingMethodImpl(int) -> std::true_type;

  // Specify namespace to prevent ADL failure
  template<typename MsgType,
      typename =
      decltype(common::com::Encode(std::declval<const MsgType &>(),
                                   std::declval<char *>())),
      typename =
      decltype(common::com::Decode(std::declval<const char *const>(),
                                   std::declval<MsgType &>())),
      typename = std::nullptr_t>
  static auto HasEncodingMethodImpl(int) -> std::true_type;

  template<typename ComType>
  static void Write(ComType &com, const char *msg) {
    com.write(msg);
    com.flush();
  }

  template<typename ComType>
  static void Write(ComType &com, const std::string &msg) {
    com.write(msg.c_str(), msg.length());
    com.flush();
  }

  template<typename ComType, typename MsgType,
      typename = std::enable_if_t<std::is_arithmetic<MsgType>::value>,
      typename = std::enable_if_t<
          decltype(HasEncodingMethodImpl<MsgType>(0))::value>>
  static void Write(ComType &com, const MsgType &msg) {
    char buffer[sizeof(MsgType)];
    common::com::Encode(msg, buffer);
    com.write(buffer, sizeof(MsgType));
    com.flush();
  }

  template<typename ComType, typename MsgType,
      typename = std::enable_if_t<
          decltype(HasEncodingMethodImpl<MsgType>(0))::value>,
      typename =
      std::enable_if_t<!std::is_arithmetic<MsgType>::value>,
      typename = std::nullptr_t>
  static void Write(ComType &com, const MsgType &msg) {
    std::string encoded_msg;
    msg.Encode(encoded_msg);
    com.write(encoded_msg.c_str(), encoded_msg.size());
    com.flush();
  }

  template<typename ComType>
  static void Read(ComType &com, std::string &msg, int bytes) {
    msg.resize(bytes);
    com.readBytes(&msg[0], bytes);
    Drain(com);
  }

  template<typename MsgType, typename ComType,
      typename = std::enable_if_t<std::is_arithmetic<MsgType>::value>,
      typename =
      std::enable_if_t<
          decltype(HasEncodingMethodImpl<MsgType>(0))::value>>
  static void Read(ComType &com, MsgType &msg, int bytes) {
    char buffer[sizeof(MsgType)];
    com.readBytes(buffer, bytes);
    common::com::Decode(buffer, msg);
    Drain(com);
  }

  template<typename MsgType, typename ComType,
      typename = std::enable_if_t<
          decltype(HasEncodingMethodImpl<MsgType>(0))::value>,
      typename =
      std::enable_if_t<!std::is_arithmetic<MsgType>::value>,
      typename = std::nullptr_t>
  static void Read(ComType &com, MsgType &msg, int bytes) {
    std::string msg_str;
    Read(com, msg_str, bytes);
    msg.Decode(msg_str);
    Drain(com);
  }

  template<typename ComType>
  static bool CheckAndWaitForMsgToBeAvilable(
      ComType &com, bool blocking,
      uint32_t timeout_ms, uint32_t delay_time_ms) {
    uint32_t tick{0};
    while (!com.available()) {
      if (!blocking || (timeout_ms && ++tick * delay_time_ms >= timeout_ms)) {
        return false;
      }
      delay(delay_time_ms);
    }
    return true;
  }
};

namespace I2C {
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
      Write(Wire, msg);
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
      Write(Wire, msg);
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
    Read(Wire, msg, sizeof(MsgType));
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
    Read(Wire, msg, 0);
  }

  template<typename MsgType,
      typename = std::enable_if_t<
          decltype(HasEncodingMethodImpl<MsgType>(0))::value>,
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
        Read(Wire, msg, bytes);
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
        Read(Wire, msg, bytes);
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
    Write(Wire, msg);
    return (Wire.endTransmission());
  }
};

}  // namespace I2C

namespace UART {

constexpr uint16_t kWaitTimeMs = 20;

namespace internal {

template <typename SerialType>
class UARTComBase : public Com {
protected:
  template <typename MsgType>
  static void Write(SerialType& com,
                    const MsgType& msg, uint32_t timeout_ms) {
    com.setTimeout(timeout_ms);
    Com::Write(com, msg);
  }

  template <typename MsgType,
      typename = std::enable_if_t<std::is_arithmetic<MsgType>::value>>
  static bool Read(SerialType& com, MsgType& msg,
                   bool blocking, uint32_t timeout_ms) {
    if (CheckAndWaitForMsgToBeAvilable(
        com, blocking, timeout_ms, kWaitTimeMs)) {
      com.setTimeout(timeout_ms);
      Com::Read(com, msg, sizeof(MsgType));
      return true;
    }
    return false;
  }

  static bool Read(SerialType& com, std::string& msg,
                   bool blocking, uint32_t timeout_ms) {
    msg.clear();
    if (CheckAndWaitForMsgToBeAvilable(
        com, blocking, timeout_ms, kWaitTimeMs)) {
      com.setTimeout(timeout_ms);
      while (com.available()) {
        char c = com.read();
        msg.push_back(c);
      }
      return true;
    }
    return false;
  }

  template <typename MsgType,
      typename = std::enable_if_t<!std::is_arithmetic<MsgType>::value>,
      typename = std::enable_if_t<
          decltype(HasEncodingMethodImpl<MsgType>(0))::value>>
  static bool Read(SerialType& com, MsgType& msg,
                   bool blocking, uint32_t timeout_ms) {
    if (CheckAndWaitForMsgToBeAvilable(
        com, blocking, timeout_ms, kWaitTimeMs)) {
      std::string encoded;
      com.setTimeout(timeout_ms);
      while (com.available()) {
        char c = com.read();
        encoded.push_back(c);
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
  static bool Read(MsgType& msg,
                   bool blocking = false, uint32_t timeout_ms = 0) {
  return internal::UARTComBase<SoftwareSerial>::Read(
      GetSerial(), msg, blocking, timeout_ms);
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
        case 1: {
          GetHardwareSerialPtr() = &Serial1;
          break;
        }
        case 2: {
          GetHardwareSerialPtr() = &Serial2;
          break;
        }
        case 3: {
          GetHardwareSerialPtr() = &Serial3;
          break;
        }
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
  static bool Read(MsgType& msg, bool blocking = false,
                   uint32_t timeout_ms = 0) {
    return internal::UARTComBase<HardwareSerial>::Read(
        *GetHardwareSerialPtr(), msg, blocking, timeout_ms);
  }

private:
  static HardwareSerial*& GetHardwareSerialPtr() {
    static HardwareSerial* ptr;
    return ptr;
  }
};

}  // namespace UART

}  // namespace common::com
