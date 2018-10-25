#include "hpp/ps8042.hpp"

#include "hpp/term.hpp"

namespace ps8042 {
  using namespace cpuio;

  Controller::Controller() {
    cmd(stdData::cmd::port1::off);
    cmd(stdData::cmd::port2::off);

    // term::putsln("ports off");
    while (status().outFull())
      read();

    // term::putsln("out emptied");
    ControllerConfig bootCfg = config();
    bool dualChannel = true;
    if (bootCfg.port2Clock())
      dualChannel = false;

    // term::printf("dual channel: %d", dualChannel);

    config(
      bootCfg.port1IRQ(false)
             .port2IRQ(false)
             .port1Translate(false)
    );
    // term::putsln("config redone");

    cmd(stdData::cmd::controller::tst);
    if (poll() != stdData::selfTest_response::controller::ok) {
      term::putsln("8042 self-test fail");
      return;
    }
    // term::putsln("8042 self-test ok");

    if (dualChannel) {
      cmd(stdData::cmd::port2::on);
      if (!config().port2Clock())
        dualChannel = false;
      else
        cmd(stdData::cmd::port2::off);
    }
    // term::printf("dual channel: %d\n", dualChannel);

    cmd(stdData::cmd::port1::tst);
    if (poll() != stdData::selfTest_response::port::ok) {
      term::putsln("8042 port 1 self-test fail");
      return;
    }
    // term::putsln("8042 port 1 self-test ok");

    if (dualChannel) {
      cmd(stdData::cmd::port2::tst);
      if (poll() != stdData::selfTest_response::port::ok) {
        term::putsln("8042 port 2 self-test fail");
        return;
      }
      // term::putsln("8042 port 2 self-test ok");
    }

    cmd(stdData::cmd::port1::on);
    // term::putsln("8042 port 1 on");
    // if (dualChannel) cmd(stdData::cmd::port2::on);
  }
  Controller::~Controller() {

  }

  ControllerStatus Controller::status() const {
    return ControllerStatus(inb(stdData::ioport::status));
  }

  ControllerConfig Controller::config() const {
    cmd(stdData::cmd::controller::rcfg);
    return ControllerConfig(poll());
  }
  void Controller::config(ControllerConfig cfg) const {
    cmd(stdData::cmd::controller::wcfg);
    blockW(cfg.raw());
  }

  void Controller::cmd(uint8_t cmd) const {
    outb(stdData::ioport::cmd, cmd);
  }
  uint8_t Controller::read() const {
    return inb(stdData::ioport::data);
  }
  void Controller::write(uint8_t v) const {
    return outb(stdData::ioport::data, v);
  }

  uint8_t Controller::poll() const {
    while (!status().outFull());
    return read();
  }
  void Controller::blockW(uint8_t v) const {
    while (status().inFull());
    return write(v);
  }
}
