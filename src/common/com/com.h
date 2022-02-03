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
  using StringLengthSize = unsigned int;

  template <typename MsgType>
  static auto HasEncodingMethodImpl(...) -> std::false_type;

  template <typename MsgType,
            typename = decltype(&MsgType::Encode),
            typename = decltype(&MsgType::Decode)>
  static auto HasEncodingMethodImpl(int) -> std::true_type;

  // Specify namespace to prevent ADL failure
  template <typename MsgType,
            typename =
                decltype(common::com::Encode(std::declval<const MsgType&>(),
                                             std::declval<char*>())),
            typename =
                decltype(common::com::Decode(std::declval<const char* const>(),
                                             std::declval<MsgType&>())),
      typename = std::nullptr_t>
  static auto HasEncodingMethodImpl(int) -> std::true_type;

  template <typename ComType>
  static void Write(ComType& com, const char* msg) {
    com.write(msg);
    com.write('\0');
  }

  template <typename ComType>
  static void Write(ComType& com, const std::string& msg) {
    com.write(msg.c_str(), msg.length());
    com.write('\0');
  }

  template <typename ComType, typename MsgType,
            typename = std::enable_if_t<std::is_arithmetic<MsgType>::value>,
            typename = std::enable_if_t<
                decltype(HasEncodingMethodImpl<MsgType>(0))::value>>
  static void Write(ComType& com, const MsgType& msg) {
    char buffer[sizeof(MsgType)];
    common::com::Encode(msg, buffer);
    com.write(buffer, sizeof(MsgType));
    com.write('\0');
  }

  template <typename ComType, typename MsgType,
      typename = std::enable_if_t<
          decltype(HasEncodingMethodImpl<MsgType>(0))::value>,
      typename =
          std::enable_if_t<!std::is_arithmetic<MsgType>::value>,
      typename = std::nullptr_t>
  static void Write(ComType& com, const MsgType& msg) {
    com.write(msg.Encode().c_str());
    com.write('\0');
  }

  template <typename ComType>
  static void Read(ComType& com, std::string& msg, int bytes) {
    msg.resize(bytes);
    com.readBytes(&msg[0], bytes);
  }

  template <typename MsgType, typename ComType,
            typename = std::enable_if_t<std::is_arithmetic<MsgType>::value>,
            typename =
                std::enable_if_t<
                    decltype(HasEncodingMethodImpl<MsgType>(0))::value>>
  static void Read(ComType& com, MsgType& msg, int bytes) {
    char buffer[sizeof(MsgType)];
    com.readBytes(buffer, bytes);
    common::com::Decode(buffer, msg);
  }

  template <typename MsgType, typename ComType,
            typename = std::enable_if_t<
                decltype(HasEncodingMethodImpl<MsgType>(0))::value>,
            typename =
                std::enable_if_t<!std::is_arithmetic<MsgType>::value>,
            typename = std::nullptr_t>
  static void Read(ComType& com, MsgType& msg, int bytes) {
    std::string msg_str;
    Read(com, msg_str, bytes);
    msg.Decode(msg_str);
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
  static void RegisterCallback(void (*callback)(MsgType &&, int)) {
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
      ClassType *class_ptr, void (ClassType::*callback)(MsgType &&, int)) {
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
    typedef void (*FunctionPtrType)(MsgType &&, int);
  };

  template<class ClassType, typename MsgType>
  struct ClassMethodImpl {
    typedef void (ClassType::*ClassMethodPtrType)(MsgType &&, int);
  };

  template<typename MsgType>
  static auto CallbackFunctionStore()
  -> typename FunctionTypeImpl<MsgType>::FunctionPtrType & {
    static void (*local_callback)(MsgType &&, int);
    return local_callback;
  }

  template<class ClassType, typename MsgType>
  static auto ClassCallbackMethodStore()
  -> typename ClassMethodImpl<ClassType, MsgType>::ClassMethodPtrType & {
    static void (ClassType::*local_callback)(MsgType &&, int);
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

template <uint8_t RX_, uint8_t TX_>
class SoftwareCom final : public Com {
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
  static void Publish(const MsgType& msg, uint32_t timeout_ms) {
    GetSerial().setTimeout(timeout_ms);
    Write(GetSerial(), msg);
  }

  template <typename MsgType>
  static bool Read(MsgType& msg, uint32_t timeout_ms = 0) {
    if (!GetSerial().available()) {
      return false;
    }
    GetSerial().setTimeout(timeout_ms);
    Com::Read(GetSerial(), msg, sizeof(MsgType));
    return true;
  }

  static bool Read(std::string& msg, uint32_t timeout_ms = 0) {
    msg.clear();
    GetSerial().setTimeout(timeout_ms);
    while (GetSerial().available()) {
      char c = GetSerial().read();
      msg.push_back(c);
    }
    return msg.empty();
  }

private:
  static SoftwareSerial& GetSerial() {
    static SoftwareSerial serial(RX, TX);
    return serial;
  }
};

class HardwareCom final : public Com {
public:
  HardwareCom() = delete;

  static void Init(long baud_rate) {
    [[maybe_unused]] static bool _ = [&]() -> bool {
      Serial.begin(baud_rate);
      while (!Serial) {}
      return true;
    }();
  }

  template <typename MsgType>
  static void Publish(const MsgType& msg, uint32_t timeout_ms) {
    Serial.setTimeout(timeout_ms);
    Write(Serial, msg);
  }

  template <typename MsgType>
  static bool Read(MsgType& msg, uint32_t timeout_ms = 0) {
    if (!Serial.available()) {
      return false;
    }
    Serial.setTimeout(timeout_ms);
    Com::Read(Serial, msg, sizeof(MsgType));
    return true;
  }

  static bool Read(std::string& msg, uint32_t timeout_ms = 0) {
    msg.clear();
    Serial.setTimeout(timeout_ms);
    while (Serial.available()) {
      char c = Serial.read();
      msg.push_back(c);
    }
    return msg.empty();
  }
};

}  // namespace UART

}  // namespace common::com
