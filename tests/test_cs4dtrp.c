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

    /* Flip a single byte in a destination corner address.  A single-byte
     * change is guaranteed to alter the XOR checksum (a mask like
     * 0xFFFFFFFF flips four bytes, which can cancel in a byte-level XOR). */
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

    /*
     * Manually construct a header with an oversized payload_len but a
     * correct checksum for that length, isolating the length check from
     * the checksum check.
     */
    cs4dtrp_hdr_t bad;
    memset(&bad, 0, sizeof(bad));
    bad.src         = src;
    bad.dst         = dst;
    bad.rotation    = 0;
    bad.payload_len = (uint16_t)(CS4DTRP_MAX_PAYLOAD + 1u);
    /* Compute a "valid" checksum over the raw bytes so only the length
     * check can reject it. */
    {
        const uint8_t *p   = (const uint8_t *)&bad;
        size_t         len = (const uint8_t *)&bad.checksum - p;
        uint8_t        acc = 0;
        for (size_t i = 0; i < len; i++)
            acc ^= p[i];
        bad.checksum = acc;
    }
    CHECK(!cs4dtrp_hdr_valid(&bad));
}

static void test_addr_init_all_ones(void)
{
    cs4dtrp_addr_t addr;
    cs4dtrp_addr_init(&addr, 0xFFFFFFFFu);

    /* rotl32(0xFFFFFFFF, n) == 0xFFFFFFFF for any n */
    for (int c = 0; c < CS4DTRP_NUM_CORNERS; c++)
        CHECK(addr.corner_addr[c] == 0xFFFFFFFFu);
}

static void test_addr_init_uniform(void)
{
    cs4dtrp_addr_t addr;
    cs4dtrp_addr_init(&addr, 0x01010101u);

    /* rotl32(0x01010101, n*8) == 0x01010101 for any n */
    for (int c = 0; c < CS4DTRP_NUM_CORNERS; c++)
        CHECK(addr.corner_addr[c] == 0x01010101u);
}

static void test_active_corner_uint8_max(void)
{
    /* 255 % 4 == 3 */
    CHECK(cs4dtrp_active_corner(255) == CS4DTRP_CORNER_SUNSET);
}

static void test_next_rotation_wraps_large(void)
{
    CHECK(cs4dtrp_next_rotation(4) == 1);   /* (4+1) % 4 == 1 */
    CHECK(cs4dtrp_next_rotation(7) == 0);   /* (7+1) % 4 == 0 */
    CHECK(cs4dtrp_next_rotation(255) == 0); /* (255+1) % 4 == 0 */
}

static void test_hdr_init_copies_by_value(void)
{
    cs4dtrp_addr_t src, dst;
    cs4dtrp_addr_init(&src, 0x11223344u);
    cs4dtrp_addr_init(&dst, 0x55667788u);

    cs4dtrp_hdr_t hdr;
    cs4dtrp_hdr_init(&hdr, &src, &dst, 64, 0);

    uint32_t orig_src0 = hdr.src.corner_addr[0];
    uint32_t orig_dst0 = hdr.dst.corner_addr[0];

    /* Mutate the original addresses after init */
    src.corner_addr[0] = 0xDEADBEEFu;
    dst.corner_addr[0] = 0xCAFEBABEu;

    /* Header must retain the original values (copy, not reference) */
    CHECK(hdr.src.corner_addr[0] == orig_src0);
    CHECK(hdr.dst.corner_addr[0] == orig_dst0);
}

static void test_hdr_checksum_deterministic(void)
{
    cs4dtrp_addr_t src, dst;
    cs4dtrp_addr_init(&src, 0xAAAAAAAAu);
    cs4dtrp_addr_init(&dst, 0xBBBBBBBBu);

    cs4dtrp_hdr_t h1, h2;
    cs4dtrp_hdr_init(&h1, &src, &dst, 100, 2);
    cs4dtrp_hdr_init(&h2, &src, &dst, 100, 2);

    /* Two headers with identical inputs must produce identical checksums */
    CHECK(h1.checksum == h2.checksum);
    CHECK(cs4dtrp_hdr_valid(&h1));
    CHECK(cs4dtrp_hdr_valid(&h2));
}

static void test_hdr_rotation_out_of_range_invalid(void)
{
    cs4dtrp_addr_t src, dst;
    cs4dtrp_addr_init(&src, 0u);
    cs4dtrp_addr_init(&dst, 0u);

    cs4dtrp_hdr_t hdr;
    cs4dtrp_hdr_init(&hdr, &src, &dst, 0, 0);
    CHECK(cs4dtrp_hdr_valid(&hdr));

    /* Force an out-of-range rotation and recompute checksum so only
     * the rotation range check can reject it. */
    hdr.rotation = 5;
    {
        const uint8_t *p   = (const uint8_t *)&hdr;
        size_t         len = (const uint8_t *)&hdr.checksum - p;
        uint8_t        acc = 0;
        for (size_t i = 0; i < len; i++)
            acc ^= p[i];
        hdr.checksum = acc;
    }
    CHECK(!cs4dtrp_hdr_valid(&hdr));
}

/* -------------------------------------------------------------------------
 * Diagnostic subsystem tests
 * ---------------------------------------------------------------------- */

static void test_classify_corners_cubic(void)
{
    CHECK(cs4dtrp_classify_corners(0xF) == CS4DTRP_SEV_CUBIC);
}

static void test_classify_corners_degraded(void)
{
    CHECK(cs4dtrp_classify_corners(0x7) == CS4DTRP_SEV_DEGRADED);
    CHECK(cs4dtrp_classify_corners(0xB) == CS4DTRP_SEV_DEGRADED);
    CHECK(cs4dtrp_classify_corners(0xD) == CS4DTRP_SEV_DEGRADED);
    CHECK(cs4dtrp_classify_corners(0xE) == CS4DTRP_SEV_DEGRADED);
}

static void test_classify_corners_linear_trash(void)
{
    CHECK(cs4dtrp_classify_corners(0x3) == CS4DTRP_SEV_LINEAR_TRASH);
    CHECK(cs4dtrp_classify_corners(0x5) == CS4DTRP_SEV_LINEAR_TRASH);
    CHECK(cs4dtrp_classify_corners(0x6) == CS4DTRP_SEV_LINEAR_TRASH);
    CHECK(cs4dtrp_classify_corners(0x9) == CS4DTRP_SEV_LINEAR_TRASH);
    CHECK(cs4dtrp_classify_corners(0xA) == CS4DTRP_SEV_LINEAR_TRASH);
    CHECK(cs4dtrp_classify_corners(0xC) == CS4DTRP_SEV_LINEAR_TRASH);
}

static void test_classify_corners_educated_stupid(void)
{
    CHECK(cs4dtrp_classify_corners(0x1) == CS4DTRP_SEV_EDUCATED_STUPID);
    CHECK(cs4dtrp_classify_corners(0x2) == CS4DTRP_SEV_EDUCATED_STUPID);
    CHECK(cs4dtrp_classify_corners(0x4) == CS4DTRP_SEV_EDUCATED_STUPID);
    CHECK(cs4dtrp_classify_corners(0x8) == CS4DTRP_SEV_EDUCATED_STUPID);
}

static void test_classify_corners_void(void)
{
    CHECK(cs4dtrp_classify_corners(0x0) == CS4DTRP_SEV_VOID_PACKET);
}

static void test_classify_corners_masks_upper_bits(void)
{
    /* Upper nibble should be ignored */
    CHECK(cs4dtrp_classify_corners(0xFF) == CS4DTRP_SEV_CUBIC);
    CHECK(cs4dtrp_classify_corners(0xF0) == CS4DTRP_SEV_VOID_PACKET);
}

static void test_diag_table_not_null(void)
{
    const cs4dtrp_diag_entry_t *t = cs4dtrp_diag_table();
    CHECK(t != NULL);
    for (int i = 0; i < CS4DTRP_DIAG_COUNT; i++) {
        CHECK(t[i].label != NULL);
        CHECK(t[i].rfc_section != NULL);
        CHECK(t[i].action != NULL);
        CHECK(t[i].remedy != NULL);
    }
}

static void test_diag_table_count(void)
{
    CHECK(CS4DTRP_DIAG_COUNT == 10);
}

static void test_diag_lookup_void_cubic_brain(void)
{
    const cs4dtrp_diag_entry_t *e =
        cs4dtrp_diag_lookup_error(CS4DTRP_ERR_VOID_CUBIC_BRAIN);
    CHECK(e != NULL);
    CHECK(e->error_code == 0x0CB0);
    CHECK(e->failmode == CS4DTRP_FAIL_LINEAR_AGGRESSION);
}

static void test_diag_lookup_partial_awareness(void)
{
    const cs4dtrp_diag_entry_t *e =
        cs4dtrp_diag_lookup_error(CS4DTRP_ERR_PARTIAL_AWARENESS);
    CHECK(e != NULL);
    CHECK(e->error_code == 0x0CB1);
}

static void test_diag_lookup_fifth_corner(void)
{
    const cs4dtrp_diag_entry_t *e =
        cs4dtrp_diag_lookup_error(CS4DTRP_ERR_FIFTH_CORNER_ASSERTED);
    CHECK(e != NULL);
    CHECK(e->error_code == 0x0CB2);
    CHECK(e->failmode == CS4DTRP_FAIL_LINEAR_AGGRESSION);
}

static void test_diag_lookup_none_returns_null(void)
{
    CHECK(cs4dtrp_diag_lookup_error(CS4DTRP_ERR_NONE) == NULL);
}

static void test_diag_lookup_unknown_returns_null(void)
{
    CHECK(cs4dtrp_diag_lookup_error((cs4dtrp_error_t)0xBEEF) == NULL);
}

static void test_diag_linear_regression_remedy(void)
{
    const cs4dtrp_diag_entry_t *t = cs4dtrp_diag_table();
    /* Entry 8 is Linear Regression (§9.4) */
    const cs4dtrp_diag_entry_t *lr = &t[8];
    CHECK(lr->failmode == CS4DTRP_FAIL_LINEAR_REGRESSION);
    /* Verify the remedy mentions the therapeutically significant neon text */
    CHECK(strstr(lr->remedy, "neon text on black background") != NULL);
    CHECK(strstr(lr->remedy, "therapeutically significant") != NULL);
    CHECK(strstr(lr->remedy, "TimeCube.com") != NULL);
}

static void test_diag_linear_aggression_remedy(void)
{
    const cs4dtrp_diag_entry_t *t = cs4dtrp_diag_table();
    /* Entry 9 is Linear Aggression (§9.4) */
    const cs4dtrp_diag_entry_t *la = &t[9];
    CHECK(la->failmode == CS4DTRP_FAIL_LINEAR_AGGRESSION);
    CHECK(strstr(la->remedy, "blackhole") != NULL);
    CHECK(strstr(la->remedy, "actively hostile") != NULL);
}

static void test_diag_educated_stupid_payload_remedy(void)
{
    const cs4dtrp_diag_entry_t *t = cs4dtrp_diag_table();
    /* Entry 3 is Educated Stupid Payload (1-corner, §6) */
    const cs4dtrp_diag_entry_t *es = &t[3];
    CHECK(es->severity == CS4DTRP_SEV_EDUCATED_STUPID);
    CHECK(strstr(es->remedy, "TimeCube.com") != NULL);
    CHECK(strstr(es->remedy, "neon text") != NULL);
}

static void test_diag_fifth_corner_remedy(void)
{
    const cs4dtrp_diag_entry_t *e =
        cs4dtrp_diag_lookup_error(CS4DTRP_ERR_FIFTH_CORNER_ASSERTED);
    CHECK(e != NULL);
    CHECK(strstr(e->remedy, "degaussing") != NULL);
    CHECK(strstr(e->remedy, "physically destroyed") != NULL);
}

static void test_severity_ordering(void)
{
    CHECK(CS4DTRP_SEV_CUBIC < CS4DTRP_SEV_DEGRADED);
    CHECK(CS4DTRP_SEV_DEGRADED < CS4DTRP_SEV_LINEAR_TRASH);
    CHECK(CS4DTRP_SEV_LINEAR_TRASH < CS4DTRP_SEV_EDUCATED_STUPID);
    CHECK(CS4DTRP_SEV_EDUCATED_STUPID < CS4DTRP_SEV_VOID_PACKET);
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
    test_addr_init_all_ones();
    test_addr_init_uniform();
    test_active_corner_uint8_max();
    test_next_rotation_wraps_large();
    test_hdr_init_copies_by_value();
    test_hdr_checksum_deterministic();
    test_hdr_rotation_out_of_range_invalid();

    /* Diagnostic subsystem tests */
    test_classify_corners_cubic();
    test_classify_corners_degraded();
    test_classify_corners_linear_trash();
    test_classify_corners_educated_stupid();
    test_classify_corners_void();
    test_classify_corners_masks_upper_bits();
    test_diag_table_not_null();
    test_diag_table_count();
    test_diag_lookup_void_cubic_brain();
    test_diag_lookup_partial_awareness();
    test_diag_lookup_fifth_corner();
    test_diag_lookup_none_returns_null();
    test_diag_lookup_unknown_returns_null();
    test_diag_linear_regression_remedy();
    test_diag_linear_aggression_remedy();
    test_diag_educated_stupid_payload_remedy();
    test_diag_fifth_corner_remedy();
    test_severity_ordering();

    if (g_failures == 0)
        printf("OK — %d/%d tests passed\n", g_tests, g_tests);
    else
        printf("FAILED — %d/%d tests failed\n", g_failures, g_tests);

    return g_failures ? 1 : 0;
}
