/*
 * cs4dtrp.c — Cubic Simultaneous 4-Day Time Rotation Protocol implementation
 */

#include "cs4dtrp.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

_Static_assert(
    offsetof(cs4dtrp_hdr_t, checksum) + sizeof(((cs4dtrp_hdr_t *)0)->checksum)
        <= sizeof(cs4dtrp_hdr_t),
    "checksum must be the last field in cs4dtrp_hdr_t");

/* -------------------------------------------------------------------------
 * Internal helpers
 * ---------------------------------------------------------------------- */

/*
 * rotl32 — Rotate a 32-bit value left by n bits.
 */
static uint32_t rotl32(uint32_t v, unsigned int n)
{
    n &= 31u;
    if (n == 0)
        return v;
    return (v << n) | (v >> (32u - n));
}

/*
 * hdr_checksum — XOR every byte of the header up to the checksum field.
 *
 * This range may include compiler-inserted padding bytes; callers must
 * ensure padding is zeroed (e.g. via memset) for deterministic checksums.
 */
static uint8_t hdr_checksum(const cs4dtrp_hdr_t *hdr)
{
    const uint8_t *p   = (const uint8_t *)hdr;
    size_t         len = offsetof(cs4dtrp_hdr_t, checksum);
    uint8_t        acc = 0;

    for (size_t i = 0; i < len; i++)
        acc ^= p[i];

    return acc;
}

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

void cs4dtrp_addr_init(cs4dtrp_addr_t *addr, uint32_t base)
{
    assert(addr != NULL);
    for (unsigned int c = 0; c < CS4DTRP_NUM_CORNERS; c++)
        addr->corner_addr[c] = rotl32(base, c * 8u);
}

void cs4dtrp_addr_rotate(cs4dtrp_addr_t *addr)
{
    assert(addr != NULL);
    uint32_t tmp = addr->corner_addr[0];
    for (unsigned int c = 0; c < CS4DTRP_NUM_CORNERS - 1u; c++)
        addr->corner_addr[c] = addr->corner_addr[c + 1u];
    addr->corner_addr[CS4DTRP_NUM_CORNERS - 1u] = tmp;
}

cs4dtrp_corner_t cs4dtrp_active_corner(uint8_t rotation)
{
    return (cs4dtrp_corner_t)(rotation % CS4DTRP_NUM_CORNERS);
}

void cs4dtrp_hdr_init(cs4dtrp_hdr_t        *hdr,
                      const cs4dtrp_addr_t *src,
                      const cs4dtrp_addr_t *dst,
                      uint16_t              payload_len,
                      uint8_t               rotation)
{
    assert(hdr != NULL);
    assert(src != NULL);
    assert(dst != NULL);
    assert(payload_len <= CS4DTRP_MAX_PAYLOAD);

    memset(hdr, 0, sizeof(*hdr));
    hdr->src         = *src;
    hdr->dst         = *dst;
    hdr->rotation    = rotation % (uint8_t)CS4DTRP_NUM_CORNERS;
    hdr->payload_len = payload_len;
    hdr->checksum    = hdr_checksum(hdr);
}

bool cs4dtrp_hdr_valid(const cs4dtrp_hdr_t *hdr)
{
    assert(hdr != NULL);
    if (hdr->rotation >= CS4DTRP_NUM_CORNERS)
        return false;
    if (hdr->payload_len > CS4DTRP_MAX_PAYLOAD)
        return false;
    return hdr->checksum == hdr_checksum(hdr);
}

uint8_t cs4dtrp_next_rotation(uint8_t rotation)
{
    return (uint8_t)((rotation + 1u) % (uint8_t)CS4DTRP_NUM_CORNERS);
}
