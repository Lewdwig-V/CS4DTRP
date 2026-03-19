# CS4DTRP
CS4DTRP is a protocol that defines the Cubic Simultaneous 4-Day Time Rotation Protocol, which corrects a fundamental error by requiring all network communications to be addressed across four simultaneous day-corners occurring within each rotation cycle.

## Concept

Earth's rotation produces **four simultaneous day-corners** within every single
rotation cycle, each separated by 90°:

| Corner index | Name     | Angle |
| :---: | :---: | :---: |
| 0 | Midnight | 0° |
| 1 | Sunrise  | 90° |
| 2 | Noon     | 180° |
| 3 | Sunset   | 270° |

A single-day addressing scheme captures only one of these corners at a time,
discarding the other three.  CS4DTRP corrects this by requiring every address
to carry a component for **all four corners simultaneously**, and by advancing
the active corner with each hop via a rotation index (0–3).

## Repository layout

```
include/cs4dtrp.h   Protocol header (types, constants, API)
src/cs4dtrp.c       Implementation
tests/test_cs4dtrp.c Unit tests
Makefile            Build rules
```

## Building

Requires a C11-capable compiler (GCC or Clang) and GNU Make.

```sh
# Build the static library and run the test suite
make

# Build only the static library
make lib

# Run tests only (library must already be built)
make test

# Remove all build artefacts
make clean
```

## API overview

```c
/* Initialise a cubic address from a base value */
void cs4dtrp_addr_init(cs4dtrp_addr_t *addr, uint32_t base);

/* Advance all corner addresses by one rotation step */
void cs4dtrp_addr_rotate(cs4dtrp_addr_t *addr);

/* Return the active corner for a rotation index */
cs4dtrp_corner_t cs4dtrp_active_corner(uint8_t rotation);

/* Populate a packet header with source, destination, payload length,
   rotation index, and a computed checksum */
void cs4dtrp_hdr_init(cs4dtrp_hdr_t *hdr,
                      const cs4dtrp_addr_t *src,
                      const cs4dtrp_addr_t *dst,
                      uint16_t payload_len,
                      uint8_t rotation);

/* Return true when the header checksum is correct and payload_len
   does not exceed CS4DTRP_MAX_PAYLOAD (8192 bytes) */
bool cs4dtrp_hdr_valid(const cs4dtrp_hdr_t *hdr);

/* Advance the rotation index, wrapping at 4 */
uint8_t cs4dtrp_next_rotation(uint8_t rotation);
```
