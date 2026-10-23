#pragma once

#include <esp_partition.h>
#include <stdint.h>

#include "ContentModel.h"

// Keeps the latest ValueBox values in flash so they survive a power loss.
//
// The values live in the otherwise unused "spiffs" data partition, written
// as a ring log of 16-byte records. Writing the next record never rewrites
// an earlier one, so the wear spreads over the whole partition instead of a
// single sector. Each sector starts with a header carrying an increasing
// sequence number; when a sector fills up, the next one is erased and first
// receives a copy of every stored value, so erasing the oldest sector later
// never loses a value that has not changed since.
class ValueBoxStore {
public:
  struct Entry {
    bool valid = false;
    // Hash of the box's source settings; a value is restored only into a
    // box that still has the same source.
    uint32_t identity = 0;
    // int32 value or float bits.
    uint32_t bits = 0;
  };

  bool begin();
  bool ready() const;
  const Entry &entry(uint8_t index) const;
  bool save(uint8_t index, uint32_t identity, uint32_t bits);
  // Drops a box that no longer exists; it disappears from flash with the
  // next sector change.
  void forget(uint8_t index);
  // Forgets every value. Two empty sectors are started, so that neither the
  // newest sector nor the one before it, read at boot, holds a value.
  bool clear();

private:
  struct Record {
    uint8_t kind;
    uint8_t index;
    uint16_t reserved;
    uint32_t identity;
    uint32_t value;
    uint32_t crc;
  };

  static Record makeRecord(
    uint8_t kind,
    uint8_t index,
    uint32_t identity,
    uint32_t value);
  static bool validRecord(const Record &record);
  static bool erasedRecord(const Record &record);

  bool readHeader(uint32_t sector, uint32_t &sequence) const;
  bool loadSector(uint32_t sector, Record *buffer, uint16_t *nextSlot);
  bool startNextSector();
  bool writeRecord(const Record &record);
  void fail(const char *operation, esp_err_t error);

  const esp_partition_t *partition_ = nullptr;
  uint32_t sectorCount_ = 0;
  uint32_t currentSector_ = 0;
  uint32_t sequence_ = 0;
  uint16_t nextSlot_ = 0;
  bool ready_ = false;
  Entry entries_[maxContentItems] = {};
};
