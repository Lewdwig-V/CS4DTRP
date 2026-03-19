/*
 * test_cs4dtrp.c — Unit tests for the CS4DTRP implementation
 */

#include "../include/cs4dtrp.h"

#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * Minimal test harness
 * ---------------------------------------------------------------------- */

static int g_failures = 0;
static int g_tests    = 0;

#define CHECK(expr)                                                   \
    do {                                                              \
        g_tests++;                                                    \
        if (!(expr)) {                                                \
            fprintf(stderr, "FAIL %s:%d — %s\n",                    \
                    __FILE__, __LINE__, #expr);                       \
            g_failures++;                                             \
        }                                                             \
    } while (0)

/* -------------------------------------------------------------------------
 * Tests
 * ---------------------------------------------------------------------- */

static void test_addr_init(void)
{
    cs4dtrp_addr_t addr;
    cs4dtrp_addr_init(&addr, 0x01020304u);

    /* Corner 0 — no rotation */
    CHECK(addr.corner_addr[0] == 0x01020304u);
    /* Corner 1 — rotl32 by 8 */
    CHECK(addr.corner_addr[1] == 0x02030401u);
    /* Corner 2 — rotl32 by 16 */
    CHECK(addr.corner_addr[2] == 0x03040102u);
    /* Corner 3 — rotl32 by 24 */
    CHECK(addr.corner_addr[3] == 0x04010203u);
}

static void test_addr_init_zero(void)
{
    cs4dtrp_addr_t addr;
    cs4dtrp_addr_init(&addr, 0u);

    for (int c = 0; c < CS4DTRP_NUM_CORNERS; c++)
        CHECK(addr.corner_addr[c] == 0u);
}

static void test_addr_rotate(void)
{
    cs4dtrp_addr_t addr;
    cs4dtrp_addr_init(&addr, 0x01020304u);

    uint32_t orig[CS4DTRP_NUM_CORNERS];
    for (int c = 0; c < CS4DTRP_NUM_CORNERS; c++)
        orig[c] = addr.corner_addr[c];

    cs4dtrp_addr_rotate(&addr);

    /* After one rotation: corner[n] = orig[n+1], corner[3] = orig[0] */
    CHECK(addr.corner_addr[0] == orig[1]);
    CHECK(addr.corner_addr[1] == orig[2]);
    CHECK(addr.corner_addr[2] == orig[3]);
    CHECK(addr.corner_addr[3] == orig[0]);
}

static void test_addr_rotate_full_cycle(void)
{
    cs4dtrp_addr_t addr;
    cs4dtrp_addr_init(&addr, 0xDEADBEEFu);

    uint32_t orig[CS4DTRP_NUM_CORNERS];
    for (int c = 0; c < CS4DTRP_NUM_CORNERS; c++)
        orig[c] = addr.corner_addr[c];

    /* Four rotations must restore the original state */
    for (int i = 0; i < CS4DTRP_NUM_CORNERS; i++)
        cs4dtrp_addr_rotate(&addr);

    for (int c = 0; c < CS4DTRP_NUM_CORNERS; c++)
        CHECK(addr.corner_addr[c] == orig[c]);
}

static void test_active_corner(void)
{
    CHECK(cs4dtrp_active_corner(0) == CS4DTRP_CORNER_MIDNIGHT);
    CHECK(cs4dtrp_active_corner(1) == CS4DTRP_CORNER_SUNRISE);
    CHECK(cs4dtrp_active_corner(2) == CS4DTRP_CORNER_NOON);
    CHECK(cs4dtrp_active_corner(3) == CS4DTRP_CORNER_SUNSET);
    /* Wraps at 4 */
    CHECK(cs4dtrp_active_corner(4) == CS4DTRP_CORNER_MIDNIGHT);
    CHECK(cs4dtrp_active_corner(7) == CS4DTRP_CORNER_SUNSET);
}

static void test_next_rotation(void)
{
    CHECK(cs4dtrp_next_rotation(0) == 1);
    CHECK(cs4dtrp_next_rotation(1) == 2);
    CHECK(cs4dtrp_next_rotation(2) == 3);
    CHECK(cs4dtrp_next_rotation(3) == 0); /* Wraps */
}

static void test_hdr_init_and_valid(void)
{
    cs4dtrp_addr_t src, dst;
    cs4dtrp_addr_init(&src, 0xAAAAAAAAu);
    cs4dtrp_addr_init(&dst, 0xBBBBBBBBu);

    cs4dtrp_hdr_t hdr;
    cs4dtrp_hdr_init(&hdr, &src, &dst, 128, 0);

    CHECK(cs4dtrp_hdr_valid(&hdr));
    CHECK(hdr.payload_len == 128);
    CHECK(hdr.rotation == 0);
}

static void test_hdr_rotation_clamped(void)
{
    cs4dtrp_addr_t src, dst;
    cs4dtrp_addr_init(&src, 0u);
    cs4dtrp_addr_init(&dst, 0u);

    cs4dtrp_hdr_t hdr;
    /* rotation=7 should be clamped to 7 % 4 == 3 */
    cs4dtrp_hdr_init(&hdr, &src, &dst, 0, 7);

    CHECK(hdr.rotation == 3);
    CHECK(cs4dtrp_hdr_valid(&hdr));
}

static void test_hdr_tampered_invalid(void)
{
    cs4dtrp_addr_t src, dst;
    cs4dtrp_addr_init(&src, 0x12345678u);
    cs4dtrp_addr_init(&dst, 0x87654321u);

    cs4dtrp_hdr_t hdr;
    cs4dtrp_hdr_init(&hdr, &src, &dst, 64, 1);

    CHECK(cs4dtrp_hdr_valid(&hdr));

    /* Tamper with the destination corner address using a value whose bytes
     * do not cancel in the XOR checksum (0xFFFFFFFF would cancel). */
    hdr.dst.corner_addr[2] ^= 0x00000001u;
    CHECK(!cs4dtrp_hdr_valid(&hdr));
}

static void test_hdr_payload_overflow_invalid(void)
{
    cs4dtrp_addr_t src, dst;
    cs4dtrp_addr_init(&src, 0u);
    cs4dtrp_addr_init(&dst, 0u);

    cs4dtrp_hdr_t hdr;
    cs4dtrp_hdr_init(&hdr, &src, &dst, CS4DTRP_MAX_PAYLOAD, 0);
    CHECK(cs4dtrp_hdr_valid(&hdr));

    /* Exceed the protocol limit; cs4dtrp_hdr_valid checks length first. */
    hdr.payload_len = (uint16_t)(CS4DTRP_MAX_PAYLOAD + 1u);
    CHECK(!cs4dtrp_hdr_valid(&hdr));
}

/* -------------------------------------------------------------------------
 * Entry point
 * ---------------------------------------------------------------------- */

int main(void)
{
    test_addr_init();
    test_addr_init_zero();
    test_addr_rotate();
    test_addr_rotate_full_cycle();
    test_active_corner();
    test_next_rotation();
    test_hdr_init_and_valid();
    test_hdr_rotation_clamped();
    test_hdr_tampered_invalid();
    test_hdr_payload_overflow_invalid();

    if (g_failures == 0)
        printf("OK — %d/%d tests passed\n", g_tests, g_tests);
    else
        printf("FAILED — %d/%d tests failed\n", g_failures, g_tests);

    return g_failures ? 1 : 0;
}
