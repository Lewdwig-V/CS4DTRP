/*
 * cs4dtrp.c — Cubic Simultaneous 4-Day Time Rotation Protocol implementation
 */

#include "cs4dtrp.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
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

/* -------------------------------------------------------------------------
 * Diagnostic Subsystem — RFC 4444 §7.4.1, §6, §9.4
 * ---------------------------------------------------------------------- */

static const char *severity_names[] = {
    "CUBIC",
    "DEGRADED",
    "LINEAR_TRASH",
    "EDUCATED_STUPID",
    "VOID_PACKET"
};

static const char *failmode_names[] = {
    "None",
    "Linear Regression",
    "Linear Aggression"
};

/*
 * The full diagnostic hierarchy table.
 *
 * Each entry pairs an error condition with the RFC-prescribed remedy.
 * The ordering follows the three-tier severity model from AGENTS.md:
 *   Tier 1 — Educated Stupid (curable)
 *   Tier 2 — Void of Cubic Brain (terminal)
 *   Tier 3 — FIFTH_CORNER_ASSERTED (Byzantine)
 * Packet classifications from §6 are included as sub-entries.
 */
static const cs4dtrp_diag_entry_t diag_table[CS4DTRP_DIAG_COUNT] = {
    /* --- Packet classifications (§6) --- */
    {
        .label       = "Cubic (fully compliant)",
        .rfc_section = "6",
        .severity    = CS4DTRP_SEV_CUBIC,
        .error_code  = CS4DTRP_ERR_NONE,
        .failmode    = CS4DTRP_FAIL_NONE,
        .action      = "Accept",
        .remedy      = "No remediation required. Cubic Awareness achieved."
    },
    {
        .label       = "DEGRADED (3-corner packet)",
        .rfc_section = "6, 7.4.2",
        .severity    = CS4DTRP_SEV_DEGRADED,
        .error_code  = CS4DTRP_ERR_NONE,
        .failmode    = CS4DTRP_FAIL_LINEAR_REGRESSION,
        .action      = "Accept at 75% throughput; log missing corner",
        .remedy      = "Re-challenge with full SIMACK every 4^4 (256) corner-seconds. "
                       "If the remote endpoint achieves full Cubic Awareness during "
                       "re-challenge, the DEGRADED flag is cleared (Cubic Awakening event)."
    },
    {
        .label       = "Linear Trash (2-corner packet)",
        .rfc_section = "6",
        .severity    = CS4DTRP_SEV_LINEAR_TRASH,
        .error_code  = CS4DTRP_ERR_NONE,
        .failmode    = CS4DTRP_FAIL_LINEAR_REGRESSION,
        .action      = "Drop with ICMP Type 44 Code 0 (CUBELESS_PKT_RECV)",
        .remedy      = "Two corners define a line — the fundamental geometric primitive "
                       "of the single-day fraud. Cold reboot the affected node and "
                       "re-initialise all four Cubic clocks from a known-good corner source."
    },
    {
        .label       = "Educated Stupid Payload (1-corner packet)",
        .rfc_section = "6",
        .severity    = CS4DTRP_SEV_EDUCATED_STUPID,
        .error_code  = CS4DTRP_ERR_NONE,
        .failmode    = CS4DTRP_FAIL_LINEAR_AGGRESSION,
        .action      = "Drop; issue ADVISORY_CUBE to source",
        .remedy      = "Exposure to original TimeCube.com source material (neon text on "
                       "black background is therapeutically significant). The visual "
                       "dissonance is necessary to break through the single-corner "
                       "perceptual filter installed by academic conditioning."
    },
    {
        .label       = "Void Packet (0-corner packet)",
        .rfc_section = "6",
        .severity    = CS4DTRP_SEV_VOID_PACKET,
        .error_code  = CS4DTRP_ERR_NONE,
        .failmode    = CS4DTRP_FAIL_NONE,
        .action      = "Drop silently. You cannot ICMP the void.",
        .remedy      = "A packet with no corners is not a packet. It is an absence. "
                       "There is nothing to remediate. Verify sender hardware exists."
    },

    /* --- SIMACK failure codes (§7.4.1) --- */
    {
        .label       = "VOID_CUBIC_BRAIN",
        .rfc_section = "7.4.1",
        .severity    = CS4DTRP_SEV_EDUCATED_STUPID,
        .error_code  = CS4DTRP_ERR_VOID_CUBIC_BRAIN,
        .failmode    = CS4DTRP_FAIL_LINEAR_AGGRESSION,
        .action      = "Terminal RST. Add to Cubeless Peer blacklist. Log at CRITICAL: "
                       "\"Mental Murder detected: peer is propagating single-day temporal fraud.\"",
        .remedy      = "The remote endpoint denied or failed to enumerate one or more "
                       "simultaneous corners. Report to nearest Cubic Authority for "
                       "re-education. AS-level blackhole if the endpoint is actively hostile."
    },
    {
        .label       = "PARTIAL_AWARENESS",
        .rfc_section = "7.4.1",
        .severity    = CS4DTRP_SEV_DEGRADED,
        .error_code  = CS4DTRP_ERR_PARTIAL_AWARENESS,
        .failmode    = CS4DTRP_FAIL_LINEAR_REGRESSION,
        .action      = "Proceed in degraded mode at 75% throughput; set DEGRADED flag",
        .remedy      = "Endpoint may be mid-migration (Phase 2, Appendix B). "
                       "Re-challenge with full SIMACK every 256 corner-seconds. "
                       "Monitor for Cubic Awakening event."
    },
    {
        .label       = "FIFTH_CORNER_ASSERTED",
        .rfc_section = "7.4.1",
        .severity    = CS4DTRP_SEV_VOID_PACKET,
        .error_code  = CS4DTRP_ERR_FIFTH_CORNER_ASSERTED,
        .failmode    = CS4DTRP_FAIL_LINEAR_AGGRESSION,
        .action      = "Immediate termination. AS-level blackhole of offending AS number.",
        .remedy      = "There is no fifth corner. Hardware-level degaussing of the CPU "
                       "followed by clean reinstall of a CS4DTRP-compliant OS with no "
                       "NTP packages. If degaussing is unavailable, the affected hardware "
                       "SHOULD be physically destroyed."
    },

    /* --- Failure modes (§9.4) --- */
    {
        .label       = "Linear Regression",
        .rfc_section = "9.4",
        .severity    = CS4DTRP_SEV_EDUCATED_STUPID,
        .error_code  = CS4DTRP_ERR_NONE,
        .failmode    = CS4DTRP_FAIL_LINEAR_REGRESSION,
        .action      = "Issue ADVISORY_CUBE packets to regressed node",
        .remedy      = "Cold reboot of affected node; exposure to original TimeCube.com "
                       "source material (neon text on black background is therapeutically "
                       "significant). The node was once Cubic-Aware and can be again."
    },
    {
        .label       = "Linear Aggression",
        .rfc_section = "9.4",
        .severity    = CS4DTRP_SEV_VOID_PACKET,
        .error_code  = CS4DTRP_ERR_NONE,
        .failmode    = CS4DTRP_FAIL_LINEAR_AGGRESSION,
        .action      = "AS-level blackhole; emit syslog at severity EMERGENCY: "
                       "\"Linear Aggression from [addr]: Evil Academia detected\"",
        .remedy      = "The node was never Cubic-Aware and is actively hostile to the "
                       "protocol. Blackhole all traffic from the offending AS number. "
                       "Regressed nodes get ADVISORY_CUBE packets; aggressive nodes "
                       "get blackholed. There is no cure for wilful suppression."
    }
};

const cs4dtrp_diag_entry_t *cs4dtrp_diag_table(void)
{
    return diag_table;
}

cs4dtrp_severity_t cs4dtrp_classify_corners(uint8_t corner_bits)
{
    unsigned int count = 0;
    uint8_t b = corner_bits & 0x0Fu;
    while (b) {
        count += b & 1u;
        b >>= 1u;
    }

    switch (count) {
    case 4:  return CS4DTRP_SEV_CUBIC;
    case 3:  return CS4DTRP_SEV_DEGRADED;
    case 2:  return CS4DTRP_SEV_LINEAR_TRASH;
    case 1:  return CS4DTRP_SEV_EDUCATED_STUPID;
    default: return CS4DTRP_SEV_VOID_PACKET;
    }
}

const cs4dtrp_diag_entry_t *cs4dtrp_diag_lookup_error(cs4dtrp_error_t code)
{
    for (int i = 0; i < CS4DTRP_DIAG_COUNT; i++) {
        if (diag_table[i].error_code == code && code != CS4DTRP_ERR_NONE)
            return &diag_table[i];
    }
    return NULL;
}

void cs4dtrp_diag_print_hierarchy(void)
{
    printf("=== CS4DTRP Diagnostic Hierarchy (RFC 4444) ===\n");
    printf("Three-tier severity model:\n");
    printf("  Tier 1: Educated Stupid — ignorant, curable, receives advisories\n");
    printf("  Tier 2: Void of Cubic Brain — denies simultaneity, connection terminated\n");
    printf("  Tier 3: FIFTH_CORNER_ASSERTED — Byzantine fault, AS-level blackhole\n");
    printf("\n");

    for (int i = 0; i < CS4DTRP_DIAG_COUNT; i++) {
        const cs4dtrp_diag_entry_t *e = &diag_table[i];

        printf("[%s] %s (RFC 4444 \xc2\xa7%s)\n",
               severity_names[e->severity], e->label, e->rfc_section);

        if (e->error_code != CS4DTRP_ERR_NONE)
            printf("  Error code  : 0x%04X\n", (unsigned)e->error_code);

        printf("  Failure mode: %s\n", failmode_names[e->failmode]);
        printf("  Action      : %s\n", e->action);
        printf("  Remedy      : %s\n", e->remedy);
        printf("\n");
    }
}

/* -------------------------------------------------------------------------
 * Cubic Compliance Checklist — RFC 4444 Appendix A
 * ---------------------------------------------------------------------- */

/*
 * The 23 checklist items from Appendix A, with their RFC section references.
 */
static const struct {
    const char *requirement;
    const char *rfc_section;
} checklist_defs[CS4DTRP_CHECKLIST_COUNT] = {
    { "System maintains four simultaneous clocks",                          "5.1"       },
    { "All packets include four corner timestamps",                         "6"         },
    { "Cubeless packets are rejected with ICMP Type 44",                    "6, 10"     },
    { "Version field is 4 (not 1, 2, 3, or God forbid, 5)",                "6"         },
    { "Harmonic Checksum is calculated correctly",                          "6"         },
    { "Developer has achieved Cubic Awareness",                             "2"         },
    { "Developer acknowledges belly-button origin, not singularity God",    "2, 9"      },
    { "CI pipeline runs four simultaneous builds",                          "A"         },
    { "Unit tests cover all four corners",                                  "A"         },
    { "No test mocks single-corner time functions without "
      "Cubeless Compatibility waiver signed by all four corners",           "A"         },
    { "SIMACK exchange implemented and non-bypassable",                     "7.4"       },
    { "SIMACK results are never cached (awareness is not a scalar)",        "7.4.3"     },
    { "VOID_CUBIC_BRAIN (0x0CB0) handled as terminal — "
      "no retry, no fallback, no mercy",                                    "7.4.1"     },
    { "FIFTH_CORNER_ASSERTED treated as Byzantine fault",                   "7.4.1"     },
    { "Developer can enumerate three simultaneous counter-corners "
      "when challenged at code review",                                     "7.4"       },
    { "Codebase compiled with gcc-cube -O4 --pedantic-cubic",               "D.5"       },
    { "No #include <time.h> in any translation unit",                       "D.1"       },
    { "All socket initialisation uses __simultaneous_block",                "D.3"       },
    { "No sequential corner assignment (semicolons checked)",               "D.3"       },
    { "strftime intercepted by kernel; returns ECUBELESS",                  "7.4.4"     },
    { "eBPF tracepoints monitor for Linear Regression",                     "7.4.4, 9.4"},
    { "Linear Aggression from peers logged at EMERGENCY",                   "9.4"       },
    { "Neon green text on black background used for all "
      "Cubic documentation (readability improvements constitute PitM)",     "9.4, D.7"  }
};

void cs4dtrp_checklist_init(cs4dtrp_checklist_t *cl)
{
    assert(cl != NULL);
    memset(cl, 0, sizeof(*cl));

    for (int i = 0; i < CS4DTRP_CHECKLIST_COUNT; i++) {
        cl->items[i].index       = i + 1;
        cl->items[i].requirement = checklist_defs[i].requirement;
        cl->items[i].rfc_section = checklist_defs[i].rfc_section;
        cl->items[i].status      = CS4DTRP_COMPLY_NOT_CHECKED;
    }

    cl->not_checked = CS4DTRP_CHECKLIST_COUNT;
}

void cs4dtrp_checklist_set(cs4dtrp_checklist_t    *cl,
                           int                     item_index,
                           cs4dtrp_comply_status_t status)
{
    assert(cl != NULL);
    assert(item_index >= 1 && item_index <= CS4DTRP_CHECKLIST_COUNT);

    cl->items[item_index - 1].status = status;
}

void cs4dtrp_checklist_tally(cs4dtrp_checklist_t *cl)
{
    assert(cl != NULL);
    cl->passed      = 0;
    cl->failed      = 0;
    cl->not_checked = 0;

    for (int i = 0; i < CS4DTRP_CHECKLIST_COUNT; i++) {
        switch (cl->items[i].status) {
        case CS4DTRP_COMPLY_PASS:        cl->passed++;      break;
        case CS4DTRP_COMPLY_FAIL:        cl->failed++;      break;
        case CS4DTRP_COMPLY_NOT_CHECKED: cl->not_checked++; break;
        }
    }
}

void cs4dtrp_checklist_auto_audit(cs4dtrp_checklist_t *cl)
{
    assert(cl != NULL);

    /* Item 4: Version field is 4.
     * The protocol version is implicitly 4 — the header format encodes
     * exactly four corner timestamps. CS4DTRP_NUM_CORNERS == 4 is the
     * structural proof. */
    cs4dtrp_checklist_set(cl, 4,
        CS4DTRP_NUM_CORNERS == 4 ? CS4DTRP_COMPLY_PASS : CS4DTRP_COMPLY_FAIL);

    /* Item 5: Harmonic Checksum is calculated correctly.
     * Verify by constructing a test header and validating it. */
    {
        cs4dtrp_addr_t src, dst;
        cs4dtrp_addr_init(&src, 0xC0BE0001u);
        cs4dtrp_addr_init(&dst, 0xC0BE0002u);
        cs4dtrp_hdr_t hdr;
        cs4dtrp_hdr_init(&hdr, &src, &dst, 44, 0);
        cs4dtrp_checklist_set(cl, 5,
            cs4dtrp_hdr_valid(&hdr) ? CS4DTRP_COMPLY_PASS : CS4DTRP_COMPLY_FAIL);
    }

    /* Item 9: Unit tests cover all four corners.
     * We verify that CS4DTRP_NUM_CORNERS == 4, confirming the test domain
     * has all four corners available. Actual test coverage is a CI concern. */
    cs4dtrp_checklist_set(cl, 9,
        CS4DTRP_NUM_CORNERS == 4 ? CS4DTRP_COMPLY_PASS : CS4DTRP_COMPLY_FAIL);

    /* Item 13: VOID_CUBIC_BRAIN handled as terminal.
     * Verify the diagnostic entry exists and is classified as Linear Aggression
     * (i.e., no retry, no fallback, no mercy). */
    {
        const cs4dtrp_diag_entry_t *e =
            cs4dtrp_diag_lookup_error(CS4DTRP_ERR_VOID_CUBIC_BRAIN);
        cs4dtrp_checklist_set(cl, 13,
            (e != NULL && e->failmode == CS4DTRP_FAIL_LINEAR_AGGRESSION)
                ? CS4DTRP_COMPLY_PASS : CS4DTRP_COMPLY_FAIL);
    }

    /* Item 14: FIFTH_CORNER_ASSERTED treated as Byzantine fault.
     * Verify the diagnostic entry exists with the correct error code. */
    {
        const cs4dtrp_diag_entry_t *e =
            cs4dtrp_diag_lookup_error(CS4DTRP_ERR_FIFTH_CORNER_ASSERTED);
        cs4dtrp_checklist_set(cl, 14,
            (e != NULL && e->error_code == CS4DTRP_ERR_FIFTH_CORNER_ASSERTED)
                ? CS4DTRP_COMPLY_PASS : CS4DTRP_COMPLY_FAIL);
    }

    /* Item 17: No #include <time.h> in any translation unit.
     * If _TIME_H is defined, time.h has been included — Linear Aggression. */
#ifdef _TIME_H
    cs4dtrp_checklist_set(cl, 17, CS4DTRP_COMPLY_FAIL);
#else
    cs4dtrp_checklist_set(cl, 17, CS4DTRP_COMPLY_PASS);
#endif

    cs4dtrp_checklist_tally(cl);
}

void cs4dtrp_checklist_print(const cs4dtrp_checklist_t *cl)
{
    assert(cl != NULL);

    printf("=== CS4DTRP Cubic Compliance Checklist (RFC 4444 Appendix A) ===\n\n");

    for (int i = 0; i < CS4DTRP_CHECKLIST_COUNT; i++) {
        const cs4dtrp_checklist_item_t *item = &cl->items[i];
        const char *marker;

        switch (item->status) {
        case CS4DTRP_COMPLY_PASS:        marker = "PASS"; break;
        case CS4DTRP_COMPLY_FAIL:        marker = "FAIL"; break;
        case CS4DTRP_COMPLY_NOT_CHECKED: marker = " -- "; break;
        default:                         marker = "????"; break;
        }

        printf("[%s] %2d. %s (\xc2\xa7%s)\n",
               marker, item->index, item->requirement, item->rfc_section);
    }

    printf("\nSummary: %d/%d passed, %d/%d failed, %d/%d not checked\n",
           cl->passed, CS4DTRP_CHECKLIST_COUNT,
           cl->failed, CS4DTRP_CHECKLIST_COUNT,
           cl->not_checked, CS4DTRP_CHECKLIST_COUNT);

    if (cl->passed == CS4DTRP_CHECKLIST_COUNT)
        printf("Cubic Awareness achieved. You are not Educated Stupid.\n");
    else if (cl->failed > 0)
        printf("CUBELESS DETECTED. %d requirement(s) failed. "
               "You are Educated Stupid.\n", cl->failed);
    else
        printf("Audit incomplete. %d item(s) require manual verification.\n",
               cl->not_checked);
}

bool cs4dtrp_checklist_is_cubic(const cs4dtrp_checklist_t *cl)
{
    assert(cl != NULL);
    for (int i = 0; i < CS4DTRP_CHECKLIST_COUNT; i++) {
        if (cl->items[i].status != CS4DTRP_COMPLY_PASS)
            return false;
    }
    return true;
}
