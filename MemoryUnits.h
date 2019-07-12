#ifndef MEMORYUNITS_H
#define MEMORYUNITS_H
#include <cstdint>

class MemoryUnit {
public:
    explicit MemoryUnit(const uint64_t size) : mValue(size) {}
    virtual ~MemoryUnit() = default;
    virtual uint64_t memoryInBytes() const {
        return value() * multiplier();
    }
    virtual uint64_t value() const {
        return mValue;
    }
protected:
    virtual uint64_t multiplier() const = 0;
private:
    uint64_t mValue;
};

class ByteMemoryUnit : public MemoryUnit {
public:
    explicit ByteMemoryUnit(const uint64_t size) : MemoryUnit(size) {}
protected:
    uint64_t multiplier() const override {
        return 1;
    }
};

class KiloByteMemoryUnit : public MemoryUnit {
public:
    explicit KiloByteMemoryUnit(const uint64_t size) : MemoryUnit(size) {}
protected:
    uint64_t multiplier() const override {
        return 1024;
    }
};

class MegaByteMemoryUnit : public MemoryUnit {
public:
    explicit MegaByteMemoryUnit(const uint64_t size) : MemoryUnit(size) {}
protected:
    uint64_t multiplier() const override {
        return 1024 * 1024;
    }
};

class GigaByteMemoryUnit : public MemoryUnit {
public:
    explicit GigaByteMemoryUnit(const uint64_t size) : MemoryUnit(size) {}
protected:
    uint64_t multiplier() const override {
        return 1024 * 1024 * 1024;
    }
};
#endif // MEMORYUNITS_H
