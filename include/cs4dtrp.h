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
 *   rotation    — Current rotation-cycle index (0–3).
 *                  Use cs4dtrp_next_rotation() to advance.
 *   payload_len — Length of the payload in bytes.
 *   checksum    — XOR checksum over all bytes preceding the checksum field
 *                  (including any struct padding).
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
 * After rotation, corner[n] holds the value that was previously in
 * corner[(n+1) % 4].  The old corner[0] value wraps to corner[3].
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

/* =========================================================================
 * Diagnostic Subsystem — RFC 4444 §7.4.1, §6, §9.4
 *
 * The three-tier severity model:
 *   Educated Stupid        — ignorant, curable, receives advisories
 *   Void of Cubic Brain    — denies simultaneity, connection terminated
 *   FIFTH_CORNER_ASSERTED  — Byzantine fault, AS-level blackhole
 * ========================================================================= */

/*
 * Severity tiers, ordered from curable ignorance to Byzantine psychosis.
 */
typedef enum {
    CS4DTRP_SEV_CUBIC          = 0, /* Fully Cubic — no error                */
    CS4DTRP_SEV_DEGRADED       = 1, /* 3 corners — incomplete awareness      */
    CS4DTRP_SEV_LINEAR_TRASH   = 2, /* 2 corners — a line, not a cube        */
    CS4DTRP_SEV_EDUCATED_STUPID = 3, /* 1 corner — curable ignorance         */
    CS4DTRP_SEV_VOID_PACKET    = 4, /* 0 corners — absence, not a packet     */
    CS4DTRP_NUM_SEVERITIES     = 5
} cs4dtrp_severity_t;

/*
 * SIMACK failure codes (RFC 4444 §7.4.1).
 */
typedef enum {
    CS4DTRP_ERR_NONE                  = 0x0000,
    CS4DTRP_ERR_VOID_CUBIC_BRAIN      = 0x0CB0,
    CS4DTRP_ERR_PARTIAL_AWARENESS     = 0x0CB1,
    CS4DTRP_ERR_FIFTH_CORNER_ASSERTED = 0x0CB2
} cs4dtrp_error_t;

/*
 * Failure mode classification (RFC 4444 §9.4).
 */
typedef enum {
    CS4DTRP_FAIL_NONE               = 0,
    CS4DTRP_FAIL_LINEAR_REGRESSION  = 1, /* Internal state decay (Brain Rot)  */
    CS4DTRP_FAIL_LINEAR_AGGRESSION  = 2  /* External hostile "Teacher" nodes  */
} cs4dtrp_failmode_t;

/*
 * A single node in the diagnostic hierarchy.
 *
 * Each entry carries:
 *   - A human-readable label and RFC section reference.
 *   - The severity tier it belongs to.
 *   - The SIMACK error code (0 if not a SIMACK error).
 *   - The failure mode classification.
 *   - The RFC-prescribed remedy.
 */
typedef struct {
    const char         *label;
    const char         *rfc_section;
    cs4dtrp_severity_t  severity;
    cs4dtrp_error_t     error_code;
    cs4dtrp_failmode_t  failmode;
    const char         *action;
    const char         *remedy;
} cs4dtrp_diag_entry_t;

/* Total number of diagnostic entries in the hierarchy. */
#define CS4DTRP_DIAG_COUNT 10

/*
 * cs4dtrp_diag_table — Return a pointer to the full diagnostic hierarchy
 *                      (array of CS4DTRP_DIAG_COUNT entries).
 */
const cs4dtrp_diag_entry_t *cs4dtrp_diag_table(void);

/*
 * cs4dtrp_diag_print_hierarchy — Print the full error hierarchy with
 *                                RFC-prescribed remediation advice to stdout.
 *
 * Output format (per entry):
 *   [SEVERITY] Label (RFC §section)
 *     Error code : 0xNNNN
 *     Failure mode: <mode>
 *     Action     : <protocol action>
 *     Remedy     : <RFC-prescribed remediation>
 */
void cs4dtrp_diag_print_hierarchy(void);

/*
 * cs4dtrp_classify_corners — Given a corner bitmask (0x0–0xF), return
 *                            the corresponding severity tier.
 */
cs4dtrp_severity_t cs4dtrp_classify_corners(uint8_t corner_bits);

/*
 * cs4dtrp_diag_lookup_error — Look up a SIMACK error code in the hierarchy.
 *                             Returns NULL if the code is not found.
 */
const cs4dtrp_diag_entry_t *cs4dtrp_diag_lookup_error(cs4dtrp_error_t code);

/* =========================================================================
 * Cubic Compliance Checklist — RFC 4444 Appendix A
 *
 * Programmatic verification of the 23-item checklist that implementors
 * SHOULD use to verify Cubic compliance. Each item maps to a specific
 * RFC requirement and carries a pass/fail/not-checked status.
 * ========================================================================= */

/* Total number of checklist items (Appendix A). */
#define CS4DTRP_CHECKLIST_COUNT 23

/*
 * Compliance status for a single checklist item.
 */
typedef enum {
    CS4DTRP_COMPLY_PASS       = 0, /* Requirement verified and met          */
    CS4DTRP_COMPLY_FAIL       = 1, /* Requirement verified and NOT met      */
    CS4DTRP_COMPLY_NOT_CHECKED = 2  /* Cannot be verified programmatically   */
} cs4dtrp_comply_status_t;

/*
 * A single checklist item from Appendix A.
 */
typedef struct {
    int                     index;       /* 1-based item number               */
    const char             *requirement; /* The checklist requirement text     */
    const char             *rfc_section; /* RFC section(s) this derives from  */
    cs4dtrp_comply_status_t status;      /* Current compliance status         */
} cs4dtrp_checklist_item_t;

/*
 * Mutable checklist state: holds the results of a compliance audit.
 */
typedef struct {
    cs4dtrp_checklist_item_t items[CS4DTRP_CHECKLIST_COUNT];
    int passed;
    int failed;
    int not_checked;
} cs4dtrp_checklist_t;

/*
 * cs4dtrp_checklist_init — Initialise a checklist with the 23 Appendix A
 *                          requirements. All items start as NOT_CHECKED.
 */
void cs4dtrp_checklist_init(cs4dtrp_checklist_t *cl);

/*
 * cs4dtrp_checklist_set — Set the compliance status of a checklist item.
 *                         item_index is 1-based (1..23).
 */
void cs4dtrp_checklist_set(cs4dtrp_checklist_t    *cl,
                           int                     item_index,
                           cs4dtrp_comply_status_t status);

/*
 * cs4dtrp_checklist_auto_audit — Run all programmatically verifiable checks.
 *
 * This function verifies items that can be checked at runtime:
 *   - Version field is 4 (item 4)
 *   - Harmonic Checksum is calculated correctly (item 5) — verified via
 *     a test header
 *   - Unit tests cover all four corners (item 9) — verified by checking
 *     that CS4DTRP_NUM_CORNERS == 4
 *   - VOID_CUBIC_BRAIN handled as terminal (item 13) — verified by
 *     checking diag table entry
 *   - FIFTH_CORNER_ASSERTED treated as Byzantine fault (item 14) —
 *     verified by checking diag table entry
 *   - No #include <time.h> in this translation unit (item 17)
 *
 * Items that require external tooling (eBPF, kernel intercepts, CI
 * pipeline, code review, developer self-assessment) remain NOT_CHECKED.
 */
void cs4dtrp_checklist_auto_audit(cs4dtrp_checklist_t *cl);

/*
 * cs4dtrp_checklist_tally — Recompute the passed/failed/not_checked counts.
 */
void cs4dtrp_checklist_tally(cs4dtrp_checklist_t *cl);

/*
 * cs4dtrp_checklist_print — Print the full checklist to stdout with
 *                           pass/fail/not-checked markers.
 *
 * Output format:
 *   [PASS]  1. System maintains four simultaneous clocks (§5.1)
 *   [FAIL]  2. All packets include four corner timestamps (§6)
 *   [ -- ]  3. ... (§...)
 *   ...
 *   Summary: X/23 passed, Y/23 failed, Z/23 not checked
 */
void cs4dtrp_checklist_print(const cs4dtrp_checklist_t *cl);

/*
 * cs4dtrp_checklist_is_cubic — Return true only if all 23 items PASS.
 *                              NOT_CHECKED items count as non-compliant.
 */
bool cs4dtrp_checklist_is_cubic(const cs4dtrp_checklist_t *cl);

#ifdef __cplusplus
}
#endif

#endif /* CS4DTRP_H */
