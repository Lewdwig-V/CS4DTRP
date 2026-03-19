/*
 * cs4dtrp.h — Cubic Simultaneous 4-Day Time Rotation Protocol
 *
 * The CS4DTRP corrects a fundamental error in single-day addressing by
 * requiring every network communication to be addressed across all four
 * simultaneous day-corners that occur within each Earth rotation cycle:
 *   Corner 0 — Midnight  (  0°)
 *   Corner 1 — Sunrise   ( 90°)
 *   Corner 2 — Noon      (180°)
 *   Corner 3 — Sunset    (270°)
 */

#ifndef CS4DTRP_H
#define CS4DTRP_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Number of simultaneous day-corners per rotation cycle. */
#define CS4DTRP_NUM_CORNERS 4

/* Maximum payload size in bytes (protocol limit, below uint16_t maximum). */
#define CS4DTRP_MAX_PAYLOAD 8192

/*
 * The four simultaneous day-corners of a single Earth rotation.
 * Each corner represents a 90° arc of the daily cycle.
 */
typedef enum {
    CS4DTRP_CORNER_MIDNIGHT = 0, /* 0°   — Midnight */
    CS4DTRP_CORNER_SUNRISE  = 1, /* 90°  — Sunrise  */
    CS4DTRP_CORNER_NOON     = 2, /* 180° — Noon     */
    CS4DTRP_CORNER_SUNSET   = 3  /* 270° — Sunset   */
} cs4dtrp_corner_t;

/*
 * A cubic address simultaneously spans all four day-corners.
 * corner_addr[n] holds the address component for corner n.
 */
typedef struct {
    uint32_t corner_addr[CS4DTRP_NUM_CORNERS];
} cs4dtrp_addr_t;

/*
 * Packet header for CS4DTRP.
 *
 * Fields:
 *   src         — Source cubic address.
 *   dst         — Destination cubic address.
 *   rotation    — Current rotation-cycle index (0–3); advances each hop.
 *   payload_len — Length of the payload in bytes.
 *   checksum    — XOR checksum over all header bytes except checksum itself.
 */
typedef struct {
    cs4dtrp_addr_t src;
    cs4dtrp_addr_t dst;
    uint8_t        rotation;
    uint16_t       payload_len;
    uint8_t        checksum;
} cs4dtrp_hdr_t;

/*
 * cs4dtrp_addr_init — Initialise a cubic address from a base value.
 *
 * Each corner address is derived from base by rotating left by (corner * 8)
 * bits so that all four corners carry distinct but related values.
 */
void cs4dtrp_addr_init(cs4dtrp_addr_t *addr, uint32_t base);

/*
 * cs4dtrp_addr_rotate — Advance all corner addresses by one rotation step.
 *
 * The value at corner[n] becomes corner[(n+1) % 4], implementing the
 * simultaneous four-day rotation.
 */
void cs4dtrp_addr_rotate(cs4dtrp_addr_t *addr);

/*
 * cs4dtrp_active_corner — Return the active corner for a given rotation index.
 */
cs4dtrp_corner_t cs4dtrp_active_corner(uint8_t rotation);

/*
 * cs4dtrp_hdr_init — Populate a packet header and compute its checksum.
 */
void cs4dtrp_hdr_init(cs4dtrp_hdr_t        *hdr,
                      const cs4dtrp_addr_t *src,
                      const cs4dtrp_addr_t *dst,
                      uint16_t              payload_len,
                      uint8_t               rotation);

/*
 * cs4dtrp_hdr_valid — Return true when the header checksum is correct and
 *                     payload_len does not exceed CS4DTRP_MAX_PAYLOAD.
 */
bool cs4dtrp_hdr_valid(const cs4dtrp_hdr_t *hdr);

/*
 * cs4dtrp_next_rotation — Return the next rotation-cycle index (wraps at 4).
 */
uint8_t cs4dtrp_next_rotation(uint8_t rotation);

#ifdef __cplusplus
}
#endif

#endif /* CS4DTRP_H */
