#include "hpp/ata.hpp"

#include "hpp/cpuio.hpp"

namespace ata {
  using namespace cpuio;

  DriveStatus::DriveStatus(const uint8_t s) :
   s_(s)
 {}

  bool DriveStatus::err() const {
    return s_ & stdData::statusBitMask::err;
  }
  bool DriveStatus::drq() const {
    return s_ & stdData::statusBitMask::drq;
  }
  bool DriveStatus::srv() const {
    return s_ & stdData::statusBitMask::srv;
  }
  bool DriveStatus::df() const {
    return s_ & stdData::statusBitMask::df;
  }
  bool DriveStatus::rdy() const {
    return s_ & stdData::statusBitMask::rdy;
  }
  bool DriveStatus::bsy() const {
    return s_ & stdData::statusBitMask::bsy;
  }

  uint8_t DriveStatus::raw() const {
    return s_;
  }
  // ---

  Drive::Drive(const port_t port, const DriveType type, const Bus& bus) :
    port_(port),
    type_(type),
    bus_(bus)
  {}

  void Drive::writeData(const uint16_t v) const {
    ensureSelected();
    outw(port_+stdData::portOffset::data, v);
  }
  void Drive::writeSecCount(const uint8_t v) const {
    ensureSelected();
    outb(port_+stdData::portOffset::secCount, v);
  }
  void Drive::writeSec(const uint8_t v) const {
    ensureSelected();
    outb(port_+stdData::portOffset::sec, v);
  }
  void Drive::writeCylLo(const uint8_t v) const {
    ensureSelected();
    outb(port_+stdData::portOffset::cylLo, v);
  }
  void Drive::writeCylHi(const uint8_t v) const {
    ensureSelected();
    outb(port_+stdData::portOffset::cylHi, v);
  }
  void Drive::writeHead(const uint8_t v) const {
    ensureSelected();
    outb(port_+stdData::portOffset::head, v);
  }
  void Drive::writeCommand(const uint8_t v) const {
    ensureSelected();
    outb(port_+stdData::portOffset::command, v);
  }


  uint16_t Drive::readData() const {
    ensureSelected();
    return inw(port_+stdData::portOffset::data);
  }
  uint8_t Drive::readSecCount() const {
    ensureSelected();
    return inb(port_+stdData::portOffset::secCount);
  }
  uint8_t Drive::readSec() const {
    ensureSelected();
    return inb(port_+stdData::portOffset::sec);
  }
  uint8_t Drive::readCylLo() const {
    ensureSelected();
    return inb(port_+stdData::portOffset::cylLo);
  }
  uint8_t Drive::readCylHi() const {
    ensureSelected();
    return inb(port_+stdData::portOffset::cylHi);
  }
  uint8_t Drive::readHead() const {
    ensureSelected();
    return inb(port_+stdData::portOffset::head);
  }
  uint8_t Drive::readStatus() const {
    ensureSelected(); // fixme: need to wait for status to be pushed
    return inb(port_+stdData::portOffset::status);
  }


  void Drive::ensureSelected() const {
    bus_.selectDrive(type_);
  }
  // ---

  MaybeDrive::MaybeDrive(const port_t port, const DriveType type, const Bus& bus) :
    found_(false),
    d_(port, type, bus)
  {}

  Drive* MaybeDrive::lookforDrive() {
    if (found_) return &d_;
    if (d_.readStatus() == 0xff)
      return nullptr;
    else {
      found_ = true;
      return &d_;
    }
  }
  // ---

  Bus::Bus(const port_t port) :
    port_(port),

    master_(MaybeDrive(port_, DriveType::master, *this)),
    slave_ (MaybeDrive(port_, DriveType::slave , *this)),
    curDrive_(DriveType::master)
  {}

  void Bus::selectMaster() const {
    if (isOnMaster())
      return;

    outb(port_+stdData::portOffset::drive, stdData::driveTypeId::master);
  }
  void Bus::selectSlave() const {
    if (isOnSlave())
      return;

    outb(port_+stdData::portOffset::drive, stdData::driveTypeId::slave);
  }
  void Bus::selectDrive(DriveType t) const {
    if (t == DriveType::master)
      selectMaster();
    else if (t == DriveType::slave)
      selectSlave();
  }

  bool Bus::isOnMaster() const {
    return curDrive_ == DriveType::master;
  }
  bool Bus::isOnSlave() const {
    return curDrive_ == DriveType::slave;
  }
  DriveType Bus::cur() const {
    return curDrive_;
  }

  MaybeDrive Bus::master() const {
    return master_;
  }
  MaybeDrive Bus::slave() const {
    return slave_;
  }
}
