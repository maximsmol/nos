#pragma once

// http://www.t13.org/Documents/UploadedDocuments/docs2013/d2161r5-ATAATAPI_Command_Set_-_3.pdf

#include "cpuio.hpp"

namespace ata {
  using namespace cpuio;

  enum DriveType {
    master, slave
  };

  namespace stdData {
    // uses weird names because you are not supposed to rely on these except primary
    namespace driveTypeId {
      constexpr uint8_t master = 0xa0;
      constexpr uint8_t slave  = 0xb0;
    }

    namespace statusBitMask {
      constexpr uint8_t err = 1 << 0;
      constexpr uint8_t drq = 1 << 3;
      constexpr uint8_t srv = 1 << 4;
      constexpr uint8_t df  = 1 << 5;
      constexpr uint8_t rdy = 1 << 6;
      constexpr uint8_t bsy = 1 << 7;
    }

    namespace portOffset {
      constexpr port_t data     = 0;
      constexpr port_t features = 1;
      constexpr port_t err      = 1;
      constexpr port_t secCount = 2;
      constexpr port_t sec      = 3;
      constexpr port_t LBAlo    = 3;
      constexpr port_t cylLo    = 4;
      constexpr port_t LBAmid   = 4;
      constexpr port_t cylHi    = 5;
      constexpr port_t LBAhi    = 5;
      constexpr port_t drive    = 6;
      constexpr port_t head     = 6;
      constexpr port_t command  = 7;
      constexpr port_t status   = 7;
    }
    namespace command {
      constexpr uint8_t read = 0x20;
      constexpr uint8_t identify = 0xec;
    }

    namespace bus {
      namespace irq {
        constexpr uint8_t primary   = 14;
        constexpr uint8_t secondary = 15;
      }
      namespace ioport {
        constexpr port_t primary   = 0x1f0;
        constexpr port_t secondary = 0x170;

        constexpr port_t tertiary   = 0x1e8;
        constexpr port_t quaternary = 0x168;

        // device control register / alternative status
        namespace dcr_altstatus {
          constexpr port_t primary   = 0x3f6;
          constexpr port_t secondary = 0x376;

          constexpr port_t tertiary   = 0x3e6;
          constexpr port_t quaternary = 0x366;
        }
      }
    }
  }

  struct DriveStatus {
    public:
      DriveStatus(const uint8_t s);

      bool err() const;
      bool drq() const;
      bool srv() const;
      bool df () const;
      bool rdy() const;
      bool bsy() const;

      uint8_t raw() const;

    private:
      uint8_t s_;
  };

  struct Bus;
  struct Drive {
    public:
      Drive(const port_t port, const DriveType type, const Bus& bus);

      void writeData    (const uint16_t v) const;
      void writeSecCount(const uint8_t  v) const;
      void writeSec     (const uint8_t  v) const;
      void writeCylLo   (const uint8_t  v) const;
      void writeCylHi   (const uint8_t  v) const;
      void writeHead    (const uint8_t  v) const;
      void writeCommand (const uint8_t  v) const;

      uint16_t readData    () const;
      uint8_t  readSecCount() const;
      uint8_t  readSec     () const;
      uint8_t  readCylLo   () const;
      uint8_t  readCylHi   () const;
      uint8_t  readHead    () const;
      uint8_t  readStatus  () const;

      void ensureSelected() const;
    private:
      port_t port_;
      DriveType type_;
      const Bus& bus_;
  };
  struct MaybeDrive {
    public:
      MaybeDrive(const port_t port, const DriveType type, const Bus& bus);

      Drive* lookforDrive();

    private:
      bool found_; // fixme: what if drive is disconnected?
      Drive d_;
  };

  struct Bus {
    public:
      Bus(const port_t port);

      void selectMaster() const;
      void selectSlave () const;
      void selectDrive(DriveType t) const;

      bool isOnMaster() const;
      bool isOnSlave () const;
      DriveType cur() const;

      MaybeDrive master() const;
      MaybeDrive slave() const;

    private:
      port_t port_;

      MaybeDrive master_;
      MaybeDrive slave_;
      DriveType curDrive_;
  };
}
