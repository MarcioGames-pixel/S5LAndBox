#include "s5l8930.h"

#include <string.h>

bool s5l8930_init(s5l8930_t *soc, uint32_t dram_base, uint32_t dram_size)
{
    if (!soc || dram_size == 0u ||
        dram_base > UINT32_MAX - (dram_size - 1u))
        return false;

    const uint32_t dram_last = dram_base + dram_size - 1u;
    const uint32_t rom_last = S5L8930_BOOTROM_BASE + S5L8930_BOOTROM_SIZE - 1u;
    if (dram_base <= rom_last && dram_last >= S5L8930_BOOTROM_BASE)
        return false;

    memset(soc, 0, sizeof(*soc));
    soc->dram_base = dram_base;
    soc->dram_size = dram_size;
    soc->initialized = true;
    return true;
}

void s5l8930_reset(s5l8930_t *soc)
{
    if (!soc) return;
    memset(soc, 0, sizeof(*soc));
}

s5l8930_region_t s5l8930_region_at(const s5l8930_t *soc, uint32_t address)
{
    if (!soc || !soc->initialized) return S5L8930_REGION_UNMAPPED;
    if (address >= S5L8930_BOOTROM_BASE &&
        address < S5L8930_BOOTROM_BASE + S5L8930_BOOTROM_SIZE)
        return S5L8930_REGION_BOOTROM;
    if (address >= soc->dram_base &&
        (uint64_t)address < (uint64_t)soc->dram_base + soc->dram_size)
        return S5L8930_REGION_DRAM;
    return S5L8930_REGION_UNMAPPED;
}

static int ranges_overlap(uint32_t a, uint32_t as, uint32_t b, uint32_t bs)
{
    return (uint64_t)a < (uint64_t)b + bs &&
           (uint64_t)b < (uint64_t)a + as;
}

s5l8930_access_status_t
s5l8930_mmio_register(s5l8930_t *soc, uint32_t base, uint32_t size,
                      void *context, s5l8930_mmio_read_fn read,
                      s5l8930_mmio_write_fn write)
{
    if (!soc || !soc->initialized || size == 0u ||
        base > UINT32_MAX - (size - 1u) || (!read && !write))
        return S5L8930_ACCESS_INVALID_ARGUMENT;

    if (ranges_overlap(base, size, S5L8930_BOOTROM_BASE, S5L8930_BOOTROM_SIZE) ||
        ranges_overlap(base, size, soc->dram_base, soc->dram_size))
        return S5L8930_ACCESS_INVALID_ARGUMENT;

    s5l8930_mmio_region_t *free_region = NULL;
    for (size_t i = 0; i < S5L8930_MMIO_MAX_REGIONS; ++i) {
        s5l8930_mmio_region_t *region = &soc->mmio[i];
        if (!region->used) {
            if (!free_region) free_region = region;
            continue;
        }
        if (ranges_overlap(base, size, region->base, region->size))
            return S5L8930_ACCESS_INVALID_ARGUMENT;
    }
    if (!free_region) return S5L8930_ACCESS_INVALID_ARGUMENT;
    free_region->base = base;
    free_region->size = size;
    free_region->context = context;
    free_region->read = read;
    free_region->write = write;
    free_region->used = true;
    return S5L8930_ACCESS_OK;
}

static s5l8930_access_status_t
mmio_access(const s5l8930_t *soc, uint32_t address, unsigned width,
            bool is_write, uint32_t *value)
{
    if (!soc || !soc->initialized || !value ||
        (width != 1u && width != 2u && width != 4u))
        return S5L8930_ACCESS_INVALID_ARGUMENT;
    if ((uint64_t)address + width - 1u > UINT32_MAX)
        return S5L8930_ACCESS_UNMAPPED;
    for (size_t i = 0; i < S5L8930_MMIO_MAX_REGIONS; ++i) {
        const s5l8930_mmio_region_t *r = &soc->mmio[i];
        if (!r->used || address < r->base) continue;
        const uint64_t end = (uint64_t)address + width;
        if (end > (uint64_t)r->base + r->size) continue;
        const uint32_t offset = address - r->base;
        if (is_write) {
            if (!r->write) return S5L8930_ACCESS_READ_ONLY;
            return r->write(r->context, offset, width, *value);
        }
        if (!r->read) return S5L8930_ACCESS_NO_BACKING;
        return r->read(r->context, offset, width, value);
    }
    return S5L8930_ACCESS_UNMAPPED;
}

s5l8930_access_status_t
s5l8930_attach_memory(s5l8930_t *soc, uint8_t *dram, size_t dram_size,
                      const uint8_t *bootrom, size_t bootrom_size)
{
    if (!soc || !soc->initialized || !dram)
        return S5L8930_ACCESS_INVALID_ARGUMENT;
    if ((uint64_t)dram_size < soc->dram_size ||
        (bootrom_size != 0u && !bootrom) ||
        bootrom_size > S5L8930_BOOTROM_SIZE)
        return S5L8930_ACCESS_INVALID_ARGUMENT;

    soc->dram = dram;
    soc->dram_backing_size = dram_size;
    soc->bootrom = bootrom;
    soc->bootrom_backing_size = bootrom_size;
    return S5L8930_ACCESS_OK;
}

const char *s5l8930_access_status_name(s5l8930_access_status_t status)
{
    switch (status) {
    case S5L8930_ACCESS_OK: return "ok";
    case S5L8930_ACCESS_INVALID_ARGUMENT: return "invalid-argument";
    case S5L8930_ACCESS_UNMAPPED: return "unmapped";
    case S5L8930_ACCESS_READ_ONLY: return "read-only";
    case S5L8930_ACCESS_NO_BACKING: return "no-backing";
    }
    return "invalid-status";
}

static s5l8930_access_status_t
resolve_access(const s5l8930_t *soc, uint32_t address, size_t width,
               bool write, uint8_t **writable, const uint8_t **readable,
               size_t *offset)
{
    if (writable) *writable = NULL;
    if (readable) *readable = NULL;
    if (offset) *offset = 0u;
    if (!soc || !soc->initialized || width == 0u ||
        !writable || !readable || !offset)
        return S5L8930_ACCESS_INVALID_ARGUMENT;

    const s5l8930_region_t region = s5l8930_region_at(soc, address);
    if ((uint64_t)address + width - 1u > UINT32_MAX ||
        region == S5L8930_REGION_UNMAPPED ||
        s5l8930_region_at(soc, address + (uint32_t)(width - 1u)) != region)
        return S5L8930_ACCESS_UNMAPPED;

    if (region == S5L8930_REGION_BOOTROM) {
        if (write) return S5L8930_ACCESS_READ_ONLY;
        *readable = soc->bootrom;
        *offset = (size_t)(address - S5L8930_BOOTROM_BASE);
        if (!soc->bootrom || *offset > soc->bootrom_backing_size ||
            width > soc->bootrom_backing_size - *offset)
            return S5L8930_ACCESS_NO_BACKING;
        return S5L8930_ACCESS_OK;
    }

    *offset = (size_t)(address - soc->dram_base);
    if (!soc->dram || *offset > soc->dram_backing_size ||
        width > soc->dram_backing_size - *offset)
        return S5L8930_ACCESS_NO_BACKING;
    *writable = soc->dram;
    *readable = soc->dram;
    return S5L8930_ACCESS_OK;
}

s5l8930_access_status_t
s5l8930_read8(const s5l8930_t *soc, uint32_t address, uint8_t *value)
{
    uint8_t *w = NULL;
    const uint8_t *r = NULL;
    size_t offset = 0u;
    if (!value) return S5L8930_ACCESS_INVALID_ARGUMENT;
    s5l8930_access_status_t st =
        resolve_access(soc, address, 1u, false, &w, &r, &offset);
    if (st == S5L8930_ACCESS_OK) *value = r[offset];
    else if (st == S5L8930_ACCESS_UNMAPPED) {
        uint32_t v = 0u;
        st = mmio_access(soc, address, 1u, false, &v);
        if (st == S5L8930_ACCESS_OK) *value = (uint8_t)v;
    }
    return st;
}

s5l8930_access_status_t
s5l8930_read16(const s5l8930_t *soc, uint32_t address, uint16_t *value)
{
    uint8_t *w = NULL; const uint8_t *r = NULL; size_t o = 0u;
    if (!value) return S5L8930_ACCESS_INVALID_ARGUMENT;
    s5l8930_access_status_t st =
        resolve_access(soc, address, 2u, false, &w, &r, &o);
    if (st == S5L8930_ACCESS_OK)
        *value = (uint16_t)((uint16_t)r[o] | ((uint16_t)r[o + 1u] << 8));
    else if (st == S5L8930_ACCESS_UNMAPPED) {
        uint32_t v = 0u;
        st = mmio_access(soc, address, 2u, false, &v);
        if (st == S5L8930_ACCESS_OK) *value = (uint16_t)v;
    }
    return st;
}

s5l8930_access_status_t
s5l8930_read32(const s5l8930_t *soc, uint32_t address, uint32_t *value)
{
    uint8_t *w = NULL; const uint8_t *r = NULL; size_t o = 0u;
    if (!value) return S5L8930_ACCESS_INVALID_ARGUMENT;
    s5l8930_access_status_t st =
        resolve_access(soc, address, 4u, false, &w, &r, &o);
    if (st == S5L8930_ACCESS_OK)
        *value = (uint32_t)r[o] | ((uint32_t)r[o + 1u] << 8) |
                 ((uint32_t)r[o + 2u] << 16) | ((uint32_t)r[o + 3u] << 24);
    else if (st == S5L8930_ACCESS_UNMAPPED)
        st = mmio_access(soc, address, 4u, false, value);
    return st;
}

s5l8930_access_status_t
s5l8930_write8(s5l8930_t *soc, uint32_t address, uint8_t value)
{
    uint8_t *w = NULL; const uint8_t *r = NULL; size_t o = 0u;
    s5l8930_access_status_t st =
        resolve_access(soc, address, 1u, true, &w, &r, &o);
    if (st == S5L8930_ACCESS_OK) w[o] = value;
    else if (st == S5L8930_ACCESS_UNMAPPED) {
        uint32_t v = value;
        st = mmio_access(soc, address, 1u, true, &v);
    }
    return st;
}

s5l8930_access_status_t
s5l8930_write16(s5l8930_t *soc, uint32_t address, uint16_t value)
{
    uint8_t *w = NULL; const uint8_t *r = NULL; size_t o = 0u;
    s5l8930_access_status_t st =
        resolve_access(soc, address, 2u, true, &w, &r, &o);
    if (st == S5L8930_ACCESS_OK) {
        w[o] = (uint8_t)value; w[o + 1u] = (uint8_t)(value >> 8);
    } else if (st == S5L8930_ACCESS_UNMAPPED) {
        uint32_t v = value;
        st = mmio_access(soc, address, 2u, true, &v);
    }
    return st;
}

s5l8930_access_status_t
s5l8930_write32(s5l8930_t *soc, uint32_t address, uint32_t value)
{
    uint8_t *w = NULL; const uint8_t *r = NULL; size_t o = 0u;
    s5l8930_access_status_t st =
        resolve_access(soc, address, 4u, true, &w, &r, &o);
    if (st == S5L8930_ACCESS_OK) {
        w[o] = (uint8_t)value; w[o + 1u] = (uint8_t)(value >> 8);
        w[o + 2u] = (uint8_t)(value >> 16); w[o + 3u] = (uint8_t)(value >> 24);
    } else if (st == S5L8930_ACCESS_UNMAPPED)
        st = mmio_access(soc, address, 4u, true, &value);
    return st;
}
