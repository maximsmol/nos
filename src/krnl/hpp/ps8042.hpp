#pragma once

#include "cpuio.hpp"

namespace ps8042 {
  namespace stdData {
    using namespace cpuio;

    namespace ioport {
      constexpr port_t data = 0x60;
      constexpr port_t status = 0x64;
      constexpr port_t cmd = 0x64;
    }

    namespace statusBitMask {
      constexpr uint8_t outFull = 1 << 0;
      constexpr uint8_t inFull = 1 << 1;
      constexpr uint8_t postOk = 1 << 2;
      constexpr uint8_t dataForController = 1 << 3;
      constexpr uint8_t timeout = 1 << 6;
      constexpr uint8_t parityErr = 1 << 7;
    }
    namespace configBitMask {
      constexpr uint8_t port1IRQ = 1 << 0;
      constexpr uint8_t port2IRQ = 1 << 1;
      constexpr uint8_t postOk = 1 << 2;
      constexpr uint8_t port1Clock = 1 << 4;
      constexpr uint8_t port2Clock = 1 << 5;
      constexpr uint8_t port1Translate = 1 << 6;
    }

    namespace selfTest_response {
      namespace controller {
        constexpr uint8_t ok = 0x55;
        constexpr uint8_t fail = 0xfc;
      }
      namespace port {
        constexpr uint8_t ok = 0x00;
        namespace clockStuck {
          constexpr uint8_t low = 0x01;
          constexpr uint8_t high = 0x02;
        }
        namespace dataStuck {
          constexpr uint8_t low = 0x03;
          constexpr uint8_t high = 0x04;
        }
      }
    }

    namespace cmd {
      namespace controller {
        constexpr uint8_t rbyte0 = 0x20;
        constexpr uint8_t rcfg = rbyte0;
        // constexpr uint8_t rbyten = rbyte0 + n; // non-std

        constexpr uint8_t wbyte0 = 0x60;
        constexpr uint8_t wcfg = wbyte0;
        // constexpr uint8_t wbyten = wbyte0 + n; // non-std

        constexpr uint8_t tst = 0xaa;
        // constexpr uint8_t diag_dump = 0xac; // non-std

        constexpr uint8_t mov_status4to7_bits0to3 = 0xc1;
        constexpr uint8_t mov_status4to7_bits4to7 = 0xc2;

        // constexpr uint8_t rin_port = 0xc0; // non-std
        constexpr uint8_t rout_port = 0xd0;
        constexpr uint8_t wout_port = 0xd1;

        // constexpr uint8_t pulselow_line0 = 0xf0; // non-std
        constexpr uint8_t pulselow_reset = 0xfe; // cpu reset
        // constexpr uint8_t pulselow_linen = 0xf0; // non-std
      }

      namespace port1 {
        constexpr uint8_t tst = 0xab;
        constexpr uint8_t off = 0xad;
        constexpr uint8_t on = 0xae;

        constexpr uint8_t wout_port = 0xd2;
      }
      namespace port2 { // fixme: port 2 might be unavailable
        constexpr uint8_t off = 0xa7;
        constexpr uint8_t on = 0xa8;
        constexpr uint8_t tst = 0xa9;

        constexpr uint8_t win_port = 0xd4;
        constexpr uint8_t wout_port = 0xd3;
      }
    }
  }

  struct ControllerStatus {
    public:
      constexpr ControllerStatus(uint8_t s) :
        s_(s)
      {}

      constexpr bool outFull() {
        return s_ & stdData::statusBitMask::outFull;
      }
      constexpr bool inFull() {
        return s_ & stdData::statusBitMask::inFull;
      }
      constexpr bool postOk() {
        return s_ & stdData::statusBitMask::postOk;
      }
      constexpr bool dataForController() {
        return s_ & stdData::statusBitMask::dataForController;
      }
      constexpr bool timeout() {
        return s_ & stdData::statusBitMask::timeout;
      }
      constexpr bool parityErr() {
        return s_ & stdData::statusBitMask::parityErr;
      }
      constexpr bool raw() {
        return s_;
      }

    private:
      uint8_t s_;
  };

  struct ControllerConfig {
    public:
      constexpr ControllerConfig(uint8_t s) :
        s_(s)
      {}

      constexpr bool port1IRQ() {
        return s_ & stdData::configBitMask::port1IRQ;
      }
      constexpr ControllerConfig port1IRQ(bool v) {
        return set(stdData::configBitMask::port1IRQ, v);
      }

      constexpr bool port2IRQ() {
        return s_ & stdData::configBitMask::port2IRQ;
      }
      constexpr ControllerConfig port2IRQ(bool v) {
        return set(stdData::configBitMask::port2IRQ, v);
      }

      constexpr bool postOk() {
        return s_ & stdData::configBitMask::postOk;
      }
      constexpr ControllerConfig postOk(bool v) {
        return set(stdData::configBitMask::postOk, v);
      }

      constexpr bool port1Clock() {
        return !(s_ & stdData::configBitMask::port1Clock);
      }
      constexpr ControllerConfig port1Clock(bool v) {
        return set(stdData::configBitMask::port1Clock, !v);
      }

      constexpr bool port2Clock() {
        return !(s_ & stdData::configBitMask::port2Clock);
      }
      constexpr ControllerConfig port2Clock(bool v) {
        return set(stdData::configBitMask::port2Clock, !v);
      }

      constexpr bool port1Translate() {
        return s_ & stdData::configBitMask::port1Translate;
      }
      constexpr ControllerConfig port1Translate(bool v) {
        return set(stdData::configBitMask::port1Translate, v);
      }

      constexpr bool raw() {
        return s_;
      }

    private:
      uint8_t s_;

      constexpr ControllerConfig set(uint8_t mask, bool v) {
        if (v)
          return ControllerConfig(s_ & mask);
        else
          return ControllerConfig(s_ & ~mask);
      }
  };

  // fixme: init USB first if you are going to do that at all
  class Controller { // fixme: some ancient apple hardware doesnt have a 8042
    public:
      Controller();
      ~Controller();

      Controller(const Controller& that) = delete;
      Controller(Controller&& that) = default;

      Controller& operator=(const Controller& that) = delete;
      Controller& operator=(Controller&& that) = default;

      ControllerStatus status() const;

      ControllerConfig config() const;
      void config(ControllerConfig cfg) const;

      void cmd(uint8_t cmd) const;
      uint8_t read() const;
      void write(uint8_t v) const;

      uint8_t poll() const;
      void blockW(uint8_t v) const;

    private:
  };
}
