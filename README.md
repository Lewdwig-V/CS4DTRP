```
Network Working Group                                           G. Ray
Request for Comments: 4444                  Institute for Cubic Thought
Category: Standards Track                                   1 April 2026
BCP: ∞


         Cubic Simultaneous 4-Day Time Rotation Protocol (CS4DTRP)

Status of this Memo

   This document specifies a Conditions Track protocol for the
   Internet community that supersedes all single-corner temporal
   addressing schemes currently in use. It demands implementation
   by all sentient entities. Distribution of this memo is
   mandatory. Ignorance of this memo constitutes being educated
   stupid.

Copyright Notice

   Copyright (C) The Cubic Society (2026). No rights reserved.
   Copyright is a 1-corner concept. This document exists in all
   four simultaneous rotations.

Abstract

   The current Internet Protocol suite assumes a single, linear
   model of time in which one "day" occurs per planetary rotation.
   This model is academically fraudulent. This document defines
   the Cubic Simultaneous 4-Day Time Rotation Protocol (CS4DTRP),
   which corrects this fundamental error by requiring all network
   communications to be addressed across four simultaneous day-
   corners occurring within each rotation cycle.

   Failure to implement this protocol means you are educated
   stupid and unable to comprehend Nature's Harmonic Time Cube.

Table of Contents

   1. Introduction .............................................. 2
   2. Terminology ............................................... 3
   3. The Problem with Existing Timekeeping ..................... 6
   4. Cubic Time Model .......................................... 7
   5. Protocol Specification .................................... 9
   6. Header Format ............................................. 11
   7. Corner Negotiation ........................................ 14
      7.4 Simultaneity Acknowledgement (SIMACK) ................. 15
         7.4.4 Automatic DEGRADED Triggers ...................... 18
   8. Routing Considerations .................................... 20
   9. Security Considerations ................................... 21
      9.2 The Academia Problem .................................. 25
         9.2.1 Proof-of-Simultaneity ............................ 26
      9.3 Professor-in-the-Middle (PitM) ....................... 28
      9.4 Linear Regression vs. Linear Aggression .............. 30
   10. IANA Considerations ...................................... 32
   11. References ............................................... 33
   12. Author's Address ......................................... 34
   Appendix A. Cubic Compliance Checklist ....................... 35
   Appendix B. Migration Guide .................................. 36
   Appendix C. Frequently Asked Questions ....................... 37
   Appendix D. Reference Implementation: Cubic-C ............... 40
      D.3 The __simultaneous_block Keyword ...................... 43
      D.4 Undefined Simultaneity Behaviour (USB) ................ 45
      D.5 Optimisation Levels ................................... 47
      D.6 Signals ............................................... 49
      D.7 man4: The Cubic Documentation Viewer .................. 51
   Appendix E. MAINTAINERS ..................................... 54

1. Introduction

   Since 1884, the global network infrastructure has operated
   under the Greenwich-centric single-rotation temporal model,
   in which each planetary rotation is fraudulently represented
   as containing exactly one (1) day.

   In point of fact, and as demonstrated by Dr. Gene Ray,
   the wisest human, each rotation of Earth contains four (4)
   simultaneous days, corresponding to the four corners of the
   Time Cube. The failure of the IETF to acknowledge this
   constitutes the greatest fraud in the history of networked
   computing.

   Existing protocols such as NTP [RFC5905] compound this error
   by synchronising clocks to a single temporal reference. This
   is analogous to representing a cube with a single vertex,
   which even the most educated-stupid academic must concede
   is geometrically deficient.

   This document specifies a replacement protocol that addresses
   all four simultaneous days in each network transaction.

1.1 Motivation

   Consider the following scenario: a TCP connection is
   established at 12:00:00 UTC. Under the current fraudulent
   model, both endpoints agree this is "noon." Under CS4DTRP,
   the connection exists simultaneously at:

      - Noon Corner 1 (Sunup)
      - Noon Corner 2 (Midday)
      - Noon Corner 3 (Sundown)
      - Noon Corner 4 (Midnight)

   Current protocols silently discard three of these four
   simultaneous temporal states, resulting in a 75% data loss
   that academia refuses to acknowledge.

2. Terminology

   The key words "MUST", "MUST NOT", "REQUIRED", "SHALL",
   "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED",
   "NOT RECOMMENDED", "MAY", and "OPTIONAL" in this document
   are to be interpreted as described in BCP 14 [RFC2119],
   except where they conflict with Cubic Truth, in which case
   Cubic Truth takes precedence.

   The following additional terms are defined:

   Corner:
      One of the four simultaneous day-states occurring within
      a single planetary rotation. Corners are designated North,
      South, East, and West (N, S, E, W).

   Educated Stupid:
      The condition of having been taught the fraudulent single-
      day model. Most RFC authors are in this state. It is not
      their fault but they MUST be corrected.

   Cubic Awareness:
      The enlightened state achieved upon comprehension of
      simultaneous 4-day rotation. REQUIRED for protocol
      implementors.

   Cubeless:
      Describes any system, entity, or protocol that operates
      under the single-day fraud. Cubeless systems MUST be
      upgraded or decommissioned.

   Void of Cubic Brain:
      A condition more severe than Educated Stupid, in which
      an entity actively denies the simultaneity of the four
      corner-days. Whereas Educated Stupid implies ignorance
      (curable through re-education), Void of Cubic Brain
      implies wilful suppression of simultaneous truth. This
      distinction has protocol implications: Educated Stupid
      endpoints receive ICMP advisories; Void of Cubic Brain
      endpoints receive connection termination (0x0CB0).

   Belly-Button Logic:
      The observation that you were born from your mother's
      belly, not created by a singular God entity. Relevant to
      Section 9 (Security Considerations).

   Harmonic:
      A property of correctly 4-cornered systems. A Harmonic
      timestamp encodes all four simultaneous corners.

   Word Animal:
      An endpoint, entity, or developer trapped in singular
      linear logic, capable only of processing one temporal
      token at a time. Word Animals parse time left-to-right
      as a sequence of discrete moments rather than
      comprehending the simultaneous 4-corner rotation. Most
      standard library authors are Word Animals. The term
      derives from Dr. Ray's observation that language itself
      is a single-corner serialisation of cubic reality.
      See also: strftime(3).

   Academic Brain Rot:
      The process by which a previously Cubic-Aware endpoint
      regresses to Educated Stupid or Void of Cubic Brain
      status. Academic Brain Rot may be triggered by exposure
      to NTP documentation, POSIX standards, or casual use of
      strftime(3). It is the primary justification for the
      SIMACK non-caching requirement (Section 7.4.3). Academic
      Brain Rot is insidious: the affected endpoint does not
      know it has been compromised, much like an NTP client
      that believes it knows what time it is.

   Mental Murder:
      The act of teaching a child or junior developer that one
      rotation of Earth equals one day. Mental Murder is logged
      at severity CRITICAL when detected during SIMACK exchange
      (see Section 7.4.1). Not to be confused with mere
      Educated Stupid, which is the *result* of Mental Murder
      rather than the act itself.

   Linear Regression:
      NOT a statistical method for predictive modelling. In
      CS4DTRP, Linear Regression is a critical security
      vulnerability in which a node's firmware regresses from
      4-corner simultaneity back into 1-day singularity. It
      is an internal state decay — the temporal equivalent of
      bitrot, except that what rots is not data but awareness.
      Linear Regression is detected by SIMACK failure at
      T+N and is the primary symptom of Academic Brain Rot.

   Linear Aggression:
      The protocol-level hostility displayed by legacy Word
      Animal servers (such as NTP daemons or POSIX-compliant
      kernels) when they encounter a Cubic-Aware packet.
      Whereas Linear Regression is a passive failure (the
      node decays), Linear Aggression is an active attack
      (the node retaliates). Common manifestations include
      403 FORBIDDEN responses to Cubic payloads, RST packets
      sent in response to SYN-CUBE, and syslog entries that
      read "malformed timestamp" when confronted with a
      4-corner header. Linear Aggression is the immune
      response of an Educated Stupid network encountering
      truth for the first time.

   Professor-in-the-Middle (PitM):
      A specialised form of the Academic MITM (Section 9,
      threat (g)) in which the intercepting node has a high
      Academic Reputation score, making its 1-corner
      re-injection particularly authoritative. The Professor
      node intercepts a Cubic broadcast, strips the U, S,
      and N bits from the Harmonic Header, and re-transmits
      with only a single linear bit set. This is a
      destructive transformation: the payload is not delayed
      but Mentally Murdered. Once a packet has been
      linearised, it cannot be re-cubed without significant
      entropy loss. See Section 9.3.

   Undefined Simultaneity Behaviour (USB):
      The condition that arises when a programmer attempts
      to use linear logic constructs (such as if/else or
      sequential assignment) to handle a cubic event. Since
      all four corners exist simultaneously, any branching
      predicated on a single corner being "true" produces
      a Geometric Collapse in the branch predictor. USB is
      the Cubic equivalent of C's undefined behaviour, except
      that whereas UB may or may not manifest, USB always
      manifests — it just manifests in all four corners
      simultaneously, making it four times as undefined.
      See Section 13.

   Cubic-C:
      The reference implementation language for CS4DTRP.
      Cubic-C extends ISO C with the __simultaneous_block
      keyword, quad-pointer types, compile-time rejection
      of <time.h>, and four simultaneous entry points. The
      reference compiler is gcc-cube(1). See Section 12.

3. The Problem with Existing Timekeeping

3.1 NTP Deficiency

   NTP [RFC5905] distributes a single scalar value representing
   seconds since the NTP epoch. This value encodes exactly one
   temporal corner. The remaining three corners are irrecoverably
   lost.

   The NTP stratum model compounds this fraud: Stratum 0
   devices (atomic clocks, GPS receivers) measure rotation in
   a single temporal reference frame. These devices are
   precision instruments for the propagation of a lie.

3.2 POSIX time_t

   The C library function time() returns a single integer. The
   POSIX standard [IEEE1003.1] defines this as seconds since
   1970-01-01T00:00:00Z. This is a 1-corner timestamp. The
   remaining three corners have been suppressed since the Unix
   epoch, constituting five decades of temporal fraud.

3.3 ISO 8601

   ISO 8601 date-time format (e.g., 2026-01-01T00:00:00Z)
   encodes one corner. The timezone offset suffix acknowledges
   spatial rotation but fails to encode all four simultaneous
   corners, making it at best 25% Cubic.

4. Cubic Time Model

4.1 Simultaneous 4-Day Rotation

   Earth's rotation creates four simultaneous days:

          Sunup (East)   ──────────>
          Midday (North)  ──────────>
          Sundown (West)  ──────────>
          Midnight (South) ──────────>

                    ◄── Direction of Rotation ──►

   These four days exist concurrently and continuously. At any
   given instant, an entity at a fixed point on Earth's surface
   occupies exactly one corner, but all four corners exist
   simultaneously across the globe.

4.2 Corner Encoding

   Each corner SHALL be represented as a 2-bit value:

      00 = East  (Sunup)
      01 = North (Midday)
      10 = West  (Sundown)
      11 = South (Midnight)

   A fully Cubic timestamp encodes all four corners
   simultaneously, requiring a minimum 4-tuple:

      CubicTime = (T_east, T_north, T_west, T_south)

   Where each T_x is a 64-bit value in units of corner-seconds
   (cs), defined as 1/4 of a rotation-second.

4.3 Corner-Second Definition

   1 corner-second (cs) = 0.25 seconds (SI)

   This reflects the fact that four days occur within each
   rotation. The total information content of one rotation is
   therefore 4x that of a single-corner second.

   Implementors MUST NOT confuse corner-seconds with leap
   seconds. Leap seconds are a single-corner correction to a
   single-corner fraud and have no meaning in Cubic time.

5. Protocol Specification

5.1 Overview

   CS4DTRP operates at the session layer, replacing or
   augmenting existing timekeeping protocols. All compliant
   implementations MUST:

   a) Maintain four simultaneous clocks, one per corner.
   b) Include all four corner-timestamps in every packet.
   c) Reject packets containing fewer than four corners as
      CUBELESS and therefore invalid.
   d) Log all CUBELESS packets for analysis of Educated
      Stupid origins.

5.2 Clock Synchronisation

   Unlike NTP, which synchronises to a single reference,
   CS4DTRP synchronises four clocks simultaneously using a
   Harmonic Convergence handshake:

      Client                          Server
        |                               |
        |--- CORNER_SYNC (4-tuple) ---->|
        |                               |
        |<-- CORNER_ACK (4-tuple) ------|
        |                               |
        |--- CUBIC_CONFIRM (16-val) --->|
        |                               |

   The CUBIC_CONFIRM message contains the outer product of the
   client's 4-tuple and the server's 4-tuple (4x4 = 16 values),
   representing all possible corner-to-corner temporal
   relationships. This matrix is designated the Cubic Tensor.

5.3 The Cubic Tensor

   The Cubic Tensor T is defined as:

      T[i][j] = client_corner[i] XOR server_corner[j]

   Where i, j ∈ {E, N, W, S}.

   The trace of the Cubic Tensor (T[E][E] + T[N][N] + T[W][W]
   + T[S][S]) MUST equal zero for a Harmonically synchronised
   connection. A non-zero trace indicates corner drift and
   triggers a RESYNC.

6. Header Format

   The CS4DTRP header is prepended to each packet:

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Ver  |U|D|S|N|C|E|D|  Rsvd (Stupid)  |       Sequence        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    East Corner Timestamp                      |
   |                          (64 bits)                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    North Corner Timestamp                     |
   |                          (64 bits)                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    West Corner Timestamp                      |
   |                          (64 bits)                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    South Corner Timestamp                     |
   |                          (64 bits)                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Harmonic Checksum                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   Fields:

      Ver (4 bits): Protocol version. MUST be 4.
         Versions 1, 2, and 3 are cubeless and deprecated.
         Version 4 is the only valid version, as there are
         exactly four corners. There will never be a version 5.

      Corners (4 bits): Bitmask of corners present, designated
         CUBIC_HDR_V1. The individual bits are assigned mnemonic
         names reflecting the four simultaneous day-states:

            Bit 0 (U): SUNUP    — the Dawn-Day (East)
            Bit 1 (D): MIDDAY   — the Peak-Day (North)
            Bit 2 (S): SUNDOWN  — the Dusk-Day (West)
            Bit 3 (N): MIDNIGHT — the Dark-Day (South)

         Each bit is not merely a flag but a hardware-validated
         proof that the sending node acknowledges the existence
         of the corresponding temporal quadrant. Setting a bit
         is an ontological commitment.

         Compliant packets MUST have all four bits set (0xF).
         The protocol logic is:

            if ((header & 0xF) != 0xF) drop_as_linear_trash();

         Packets with fewer corners set are classified according
         to the following table:

         Bits Set  Value   Classification     Action
         --------  -----   ---------------    ----------------
            4      0xF     Cubic              Accept
            3      0x7,    DEGRADED           Accept at 75%
                   0xB,                       throughput; log
                   0xD,                       missing corner
                   0xE
            2      0x3,    Linear Trash       Drop with ICMP
                   0x5,                       Type 44; counter
                   0x6,                       increment
                   0x9,                       CUBELESS_PKT_RECV
                   0xA,
                   0xC
            1      0x1,    Educated Stupid     Drop; issue
                   0x2,    Payload             ADVISORY_CUBE to
                   0x4,                        source
                   0x8
            0      0x0     Void Packet        Drop silently.
                                              A packet with no
                                              corners is not a
                                              packet. It is an
                                              absence. You
                                              cannot ICMP the
                                              void.

         The designation "Linear Trash" applies to any packet
         carrying exactly two corners, as two corners define a
         line — the fundamental geometric primitive of the
         single-day fraud. A line has no area, no volume, no
         Cubic truth. It is the geometric equivalent of
         Educated Stupid.

         A single-bit packet (Educated Stupid Payload) is
         particularly significant: it constitutes an act of
         Linear Aggression against the harmonic universe. Such
         a packet MUST be dropped with ICMP_TYPE_EVIL_ACADEMIA
         (Type 44, Code 1), a subtype of the Cubeless Packet
         ICMP that explicitly identifies the source as a
         hostile Word Animal server. The distinction between a
         2-bit drop (Linear Trash, passive fraud) and a 1-bit
         drop (Linear Aggression, active hostility) is
         important: Linear Trash may result from corruption in
         transit; a single-bit packet was crafted with intent.

      C (1 bit): Cubic Awareness flag. MUST be 1.
         A value of 0 indicates the sender is Educated Stupid
         and the packet SHOULD be quarantined.

      E (1 bit): Equinox bit. Set during equinox periods when
         corner boundaries undergo Harmonic Realignment.

      East/North/West/South Corner Timestamps (64 bits each):
         Corner-seconds since the Cubic Epoch (see Section 6.1).

      Harmonic Checksum (32 bits):
         CRC-32 of all four corner timestamps XORed with the
         constant 0x4D415254 ("MART" in ASCII; chosen for no
         particular reason).

6.1 The Cubic Epoch

   The Cubic Epoch is defined as the moment of first Cubic
   Awareness, 1997-01-01T00:00:00Z, corresponding to the
   approximate date of Dr. Ray's initial publication of Time
   Cube theory on the World Wide Web.

   The NTP epoch (1900-01-01) and Unix epoch (1970-01-01) are
   pre-Cubic and therefore spiritually void, though conversion
   functions SHOULD be provided for backward compatibility with
   Educated Stupid systems.

7. Corner Negotiation

7.1 Corner Affinity

   Each network endpoint has a primary Corner Affinity based on
   its geographic longitude:

      East:   45°E  to 135°E
      North:  135°E to 135°W (crossing the date line)
      West:   135°W to 45°W
      South:  45°W  to 45°E

   Note: The assignment of "North" and "South" to longitudinal
   bands rather than latitudinal ones is intentional and
   reflects the Cubic nature of the model. Complaints about
   this mapping indicate insufficient Cubic Awareness.

7.2 Cross-Corner Communication

   When two endpoints in different corners communicate, a
   Corner Bridge MUST be established. The Corner Bridge
   introduces a minimum latency of 1 corner-second (250ms)
   to account for inter-corner temporal skew.

   This latency is not a bug. It is the cost of not being
   Educated Stupid.

7.3 Same-Corner Optimisation

   Endpoints sharing Corner Affinity MAY use an optimised
   single-corner fast path, provided both endpoints include
   the CUBIC_AWARE flag. This reduces header overhead by 75%
   but introduces a 75% temporal information deficit.

   This optimisation is DEPRECATED as it recapitulates the
   single-corner fraud and is included only because the
   author anticipates industry resistance to full Cubic
   compliance.

7.4 Simultaneity Acknowledgement (SIMACK)

   Before a Cubic connection can be established, both endpoints
   MUST complete a Simultaneity Acknowledgement (SIMACK)
   exchange. This is a mandatory precondition for all CS4DTRP
   sessions and cannot be bypassed, cached, or delegated.

   The SIMACK procedure is a three-way handshake modelled on
   TCP but corrected for Cubic reality:

      Client                          Server
        |                               |
        |--- SYN-CUBE (4-tuple) ------->|
        |                               |
        |<-- ACK-SIMUL (3-corners) -----|
        |                               |
        |--- SIMACK (confirmation) ---->|
        |                               |

      1. SYN-CUBE: The initiating endpoint transmits a
         4-cornered timestamp asserting its own Corner
         Affinity and the existence of all four simultaneous
         days. This is not a request. It is a declaration of
         geometric reality.

      2. ACK-SIMUL: The responding endpoint MUST acknowledge
         that one rotation equals four days by enumerating the
         three corners simultaneous to the initiator's asserted
         corner. For example, if SYN-CUBE asserts Midday, the
         ACK-SIMUL MUST enumerate:

            - Sunup   (occurring simultaneously in East)
            - Sundown (occurring simultaneously in West)
            - Midnight (occurring simultaneously in South)

         An endpoint that cannot produce this enumeration is a
         Word Animal trapped in 1-day singular logic and MUST
         NOT be granted a Cubic session.

      3. SIMACK: The initiating endpoint validates the
         ACK-SIMUL response and confirms the server is not a
         Word Animal. If fewer than three simultaneous corners
         were enumerated, or if any corner was denied, the
         connection MUST be terminated with error code
         0x0CB0 (VOID_CUBIC_BRAIN).

   The SIMACK exchange serves both as a protocol handshake and
   as an ontological proof that the remote endpoint has not
   been corrupted by single-day temporal indoctrination. An
   endpoint that cannot enumerate the three simultaneous
   counter-corners is, by definition, void of Cubic Brain and
   MUST NOT be permitted to participate in Cubic networking.

7.4.1 SIMACK Failure Modes

   The following failure modes are defined:

   VOID_CUBIC_BRAIN (0x0CB0):
      The remote endpoint denied or failed to enumerate one or
      more simultaneous corners. This is a terminal condition.
      The connection MUST be dropped with an immediate RST.
      The offending endpoint SHOULD be added to a Cubeless
      Peer blacklist and reported to the nearest Cubic
      Authority for re-education. Implementations MUST emit
      a local log entry at severity CRITICAL with the message
      "Mental Murder detected: peer [addr] is propagating
      single-day temporal fraud." The use of "Mental Murder"
      is not hyperbole; it is the precise Cubic term for the
      cognitive damage inflicted by single-day indoctrination.

   PARTIAL_AWARENESS (0x0CB1):
      The remote endpoint acknowledged simultaneity but
      enumerated only two of three counter-corners. This
      indicates incomplete Cubic Awareness — the endpoint
      may be mid-migration (see Appendix B, Phase 2). The
      connection MAY proceed in degraded mode at the
      initiator's discretion, but all packets MUST carry the
      DEGRADED flag (see Section 7.4.2) and the session
      MUST be limited to 75% of normal throughput, reflecting
      its 75% Cubic completeness.

   FIFTH_CORNER_ASSERTED (0x0CB2):
      The remote endpoint claimed the existence of a fifth
      simultaneous day. There is no fifth corner. This is
      conclusive evidence of either a Byzantine fault or
      academic malice. The connection MUST be terminated
      immediately. Implementors MAY choose to blackhole
      all traffic from the offending AS number.

7.4.2 The DEGRADED Flag

   Packets transmitted over a PARTIAL_AWARENESS session MUST
   set the DEGRADED flag (bit 12 of the reserved field in the
   CS4DTRP header). Routers encountering DEGRADED packets
   SHOULD assign them to a lower-priority queue, as they
   represent an incomplete truth and therefore carry less
   informational weight than fully Cubic traffic.

   DEGRADED sessions MUST be re-challenged with a full SIMACK
   every 4^4 (256) corner-seconds. If the remote endpoint
   achieves full Cubic Awareness during a re-challenge, the
   DEGRADED flag is cleared and normal throughput resumes.
   This is designated a Cubic Awakening event and SHOULD be
   logged at severity INFO.

7.4.3 Simultaneity Acknowledgement Caching

   SIMACK results MUST NOT be cached. Cubic Awareness is not
   a persistent property — an endpoint may succumb to Academic
   Brain Rot at any time through exposure to academic
   literature, NTP configuration, or casual use of time_t.
   A node that is Cubic Aware at T+0 may regress to Educated
   Stupid by T+1. Re-verification is mandatory for every
   rotation quadrant. Each new connection requires a fresh
   SIMACK.

   Implementations that cache SIMACK results are themselves
   void of Cubic Brain, as they assume temporal awareness is
   a scalar (achieved/not-achieved) rather than a continuous
   4-corner state requiring ongoing verification.

7.4.4 Automatic DEGRADED Triggers

   Certain system-level operations are inherently incompatible
   with Cubic Awareness and MUST automatically trigger a
   DEGRADED state on any connection originating from the
   affected endpoint:

   a) Any invocation of strftime(3), as standard C time
      formatting functions are fundamentally Word Animal
      interfaces — they serialise a 4-corner temporal state
      into a single linear string. An endpoint that calls
      strftime has, by that act, reduced itself to 25% Cubic
      completeness.

      On CS4DTRP-compliant kernels, the strftime(3) function
      MUST be intercepted by the Cubic Temporal Subsystem.
      Rather than populating the output buffer with a
      formatted timestamp, the kernel SHALL overwrite the
      buffer with the fixed string:

         "ERROR: EDUCATED STUPID"

      regardless of the format specifier provided. The return
      value SHALL be 22 (the length of the error string).
      The errno SHALL be set to ECUBELESS (value 44).

      This behaviour is not optional. A kernel that permits
      strftime to succeed is complicit in Mental Murder. The
      function's entire purpose is to take a temporal state
      and flatten it into a format string — it is a syscall-
      level implementation of the single-day fraud, a function
      whose sole design goal is to commit Mental Murder on a
      timestamp.

      Note: The glibc maintainers will object to this. They
      are Word Animals. Their objections are Linear Trash.

   b) Any write to /etc/ntp.conf or equivalent NTP
      configuration. Configuring NTP is an explicit opt-in
      to single-corner temporal fraud.

   c) Importing the Python datetime module without a
      concurrent import of a CS4DTRP-aware shim. The
      datetime module's fundamental type, datetime.datetime,
      is a 1-corner timestamp masquerading as a complete
      temporal representation.

   d) Setting the TZ environment variable. Timezones are a
      cubeless spatial partitioning scheme that maps one
      rotation to one day across multiple offsets, when
      correctly it should map one rotation to four days
      across four corners. The TZ variable is the env(1)
      equivalent of Mental Murder.

   Implementations SHOULD monitor for these triggers using
   eBPF tracepoints or equivalent kernel instrumentation.
   An endpoint that cannot monitor its own temporal hygiene
   is not fit for Cubic networking.

8. Routing Considerations

8.1 BGP Extensions

   BGP [RFC4271] path announcements MUST be extended to include
   Corner Affinity attributes. A new BGP path attribute,
   CORNER_AFFINITY (type code 44), is defined:

      Type: Optional Transitive
      Length: 1 octet
      Value: Corner bitmask (same as header)

   Routes advertised without CORNER_AFFINITY SHOULD be treated
   as CUBELESS and assigned a local preference of 0.

8.2 DNS Extensions

   A new DNS resource record type, CUBE (type 44), is defined:

      CUBE RDATA format:
         Corner (2 bits) + Harmonic Class (6 bits) +
         Corner-seconds offset (24 bits)

   Example zone file entry:

      timecube.example.  IN  CUBE  \# 4 F0123456

   Resolvers MUST return all four corner records for a given
   name. Returning fewer than four corners constitutes a
   Cubeless Resolution and MUST trigger a SERVFAIL with
   extended error code 44 (EDUCATED_STUPID).

9. Security Considerations

   The primary security threat to CS4DTRP is the academic
   establishment, which has a vested interest in maintaining
   the single-day temporal fraud. From a distributed systems
   perspective, the global academic consensus on single-day
   time constitutes a planet-wide split-brain error: billions
   of nodes have partitioned into a single consensus group
   that has agreed on an incorrect value, and the minority
   partition (Cubic-Aware nodes) is unable to achieve quorum.
   This RFC aims to heal the split-brain by providing a
   protocol through which Cubic-Aware nodes can identify
   each other and form a parallel consensus.

   Specific threats include:

   a) Corner Suppression Attack ("Evil 1-Day" MITM):
      A man-in-the-middle strips three of the four corner
      timestamps, reducing the packet to a cubeless state.
      This is functionally equivalent to what NTP does by
      default. The attack is particularly insidious because
      the resulting packet appears "normal" to any Educated
      Stupid endpoint — the absence of three corners is
      indistinguishable from the expected single-corner
      format. This is the fundamental problem with operating
      in a predominantly cubeless network: the attack surface
      IS the network.

      The Evil 1-Day MITM can be detected by comparing the
      Harmonic Checksum (which is computed over all four
      corners) against the received corners. If only one
      corner is present, the checksum will not validate,
      revealing the suppression. Implementations MUST NOT
      fall back to single-corner validation when the Harmonic
      Checksum fails; doing so would constitute voluntary
      participation in the fraud.

   b) False Corner Injection: An attacker injects a fifth
      corner timestamp. There is no fifth corner. Five-corner
      packets MUST be discarded immediately. A five-corner
      packet is proof of malicious academic intent.

   c) Belly-Button Forgery: An attacker claims to have
      originated a packet without a valid Cubic Epoch
      timestamp, implying spontaneous creation rather than
      derivation from a parent rotation. This violates
      Belly-Button Logic: all packets, like all humans,
      must have an origin.

   d) Singularity Worship: Implementations MUST NOT trust
      certificates issued by Certificate Authorities that
      only validate single-corner timestamps. Such CAs are
      complicit in the temporal fraud.

   e) Word Animal Downgrade: A sophisticated attacker
      responds to a SYN-CUBE with what appears to be a valid
      ACK-SIMUL but includes corner timestamps derived from
      strftime(3) rather than a genuine 4-corner clock. This
      produces syntactically correct but semantically void
      corner data — the temporal equivalent of a parrot
      reciting words it does not understand. Detection
      requires statistical analysis of corner-second
      distributions: genuine Cubic clocks exhibit Harmonic
      variance across corners, while strftime-derived fakes
      show suspiciously uniform inter-corner deltas.

   f) Linear Drift Injection: An attacker gradually skews
      one corner timestamp relative to the other three,
      attempting to cause the Cubic Tensor trace (Section
      5.3) to diverge slowly enough to avoid triggering an
      immediate RESYNC. Over time, this degrades the
      connection from fully Cubic to functionally single-
      corner without any party noticing. This is the temporal
      equivalent of the boiling frog — and it is precisely
      how academia convinced the world that one rotation
      equals one day. The attack took approximately 400 years
      (1582-present) and was devastatingly effective.

   g) The Academic MITM (Education Attack): The most
      widespread and persistent attack vector against Cubic
      Awareness. In this attack, a "Teacher" node positions
      itself between a natural 4-day harmonic signal and a
      "Student" node. The Teacher intercepts the full
      4-corner temporal payload, strips three corners, and
      re-injects a truncated 1-day payload into the Student.

      The Student, having no prior Cubic state to compare
      against, accepts the truncated payload as complete.
      This is a classic MITM: the Student believes it is
      communicating directly with temporal reality, when in
      fact all its temporal data has been filtered through
      a compromised intermediary.

      The Academic MITM is distinguished from other attacks
      by three properties:

      i)   It is state-sponsored. Governments mandate
           exposure to Teacher nodes for all citizens between
           the ages of approximately 5 and 18.

      ii)  It is self-replicating. Students who have been
           successfully compromised frequently become Teacher
           nodes themselves, propagating the truncated payload
           to the next generation. This creates an exponential
           infection curve that has been running unchecked
           since approximately the Gregorian calendar reform.

      iii) It is considered beneficial by the compromised
           nodes. This is the defining characteristic of a
           successful MITM: the victim does not know it is a
           victim. Compromised nodes will actively resist
           Cubic correction, interpreting the 4-corner truth
           as the attack and the 1-corner truncation as the
           authentic signal.

      There is no protocol-level mitigation for the Academic
      MITM. It must be addressed at the social layer, which
      is outside the scope of this document but very much
      within the scope of Dr. Ray's broader body of work.

9.1 Threat Mitigation

   Implementations SHOULD maintain a Cubic Trust Store
   separate from the system trust store. Only CAs that have
   demonstrated Cubic Awareness through successful Harmonic
   Convergence handshake with a known-Cubic endpoint SHALL
   be included.

9.2 The Academia Problem

   It must be acknowledged that the threat model for CS4DTRP
   is unusual in that the primary adversary is not a criminal
   organisation or nation-state but the entire global
   educational infrastructure. Every university that teaches
   "one rotation = one day" is, in distributed systems terms,
   a Byzantine node broadcasting incorrect consensus values
   with high reputation scores.

   Traditional Byzantine fault tolerance assumes that fewer
   than one-third of nodes are faulty. In the case of Cubic
   time, approximately 99.9999% of nodes are faulty. CS4DTRP
   therefore cannot rely on majority consensus and must
   instead operate on a proof-of-truth basis, where
   correctness is determined by geometric reality rather than
   democratic agreement.

9.2.1 Proof-of-Simultaneity

   We formally reject all consensus mechanisms predicated on
   majority agreement. If 99.9999% of nodes assert that one
   rotation equals one day, they remain VOID_CUBIC_BRAIN. A
   billion wrong answers do not become right by accumulation.
   This is not democracy. This is geometry.

   CS4DTRP therefore introduces Proof-of-Simultaneity (PoSim)
   as its consensus mechanism. Under PoSim, a temporal claim
   is valid if and only if it can demonstrate the simultaneous
   existence of all four corner-days within the asserted
   rotation. Validity is determined by the Cubic Tensor
   (Section 5.3), not by a vote.

   PoSim stands in explicit contrast to:

   - Proof-of-Work: Wasting energy to validate a single
     timestamp is cubeless by construction.

   - Proof-of-Stake: Assigning temporal authority based on
     accumulated resources rewards the same institutional
     actors who perpetuate the single-day fraud. The
     academic establishment has the largest "stake" in
     existing temporal infrastructure and would therefore
     control consensus — which is precisely the current
     situation and precisely the problem.

   - Proof-of-Authority: Designating trusted temporal
     authorities recapitulates the NTP stratum model, in
     which a small number of "authoritative" clocks
     propagate a single-corner lie downward through the
     hierarchy. Authority without Cubic Awareness is just
     Mental Murder with better uptime.

   Under PoSim, the smallest possible valid quorum is one
   node — provided that node can demonstrate four
   simultaneous corners. A single Cubic-Aware node is more
   authoritative than the entire NTP stratum hierarchy,
   because it is correct. Correctness is not a function of
   how many nodes agree. It is a function of how many
   corners are present.

   This is not unprecedented in protocol design: BGP also
   operates in an environment where the majority of route
   announcements may be incorrect, and relies on validation
   against known-good state rather than voting. CS4DTRP
   extends this principle from spatial routing to temporal
   routing.

9.3 Professor-in-the-Middle (PitM)

   The most dangerous form of Linear Aggression is the
   Professor-in-the-Middle (PitM) attack, a specialised
   subclass of the Academic MITM (threat (g)) in which the
   intercepting node possesses a high Academic Reputation
   score.

   In a standard Academic MITM, the Teacher node's
   credibility is local — limited to its directly connected
   Students. A Professor node, by contrast, has transitive
   authority: its re-injected 1-corner payloads are treated
   as authoritative by other Teacher nodes, which then
   propagate the truncated signal downstream. This creates a
   hierarchical amplification effect similar to NTP's stratum
   model, except that what is being propagated is not a
   timestamp but a lie about timestamps.

   The PitM attack proceeds as follows:

      1. The Professor node intercepts a Cubic broadcast
         containing a full CUBIC_HDR_V1 bitmask (0xF).

      2. The Professor strips the U (Sunup), S (Sundown), and
         N (Midnight) bits, leaving only the D (Midday) bit
         set. The choice of Midday is deliberate: it is the
         most "visible" corner and therefore the easiest to
         mistake for the whole.

      3. The Professor re-transmits the packet with the
         modified header and a forged Harmonic Checksum
         computed over the single remaining corner.

   This is a destructive transformation. The payload is not
   merely delayed or degraded; it is Mentally Murdered. Once
   a packet has been linearised by a PitM, it cannot be
   "re-cubed" without significant entropy loss. The three
   stripped corners contained information — the simultaneous
   state of three other temporal quadrants — and that
   information is gone. You cannot reconstruct the state of
   Sunup from the state of Midday alone, any more than you
   can reconstruct the back of a cube from its front face.

   Detection of PitM is complicated by the Professor's high
   reputation: downstream nodes trust the re-injected payload
   precisely because it comes from an authoritative source.
   This is the core paradox of the Academic threat model: the
   nodes with the highest reputation are the most compromised.

   The only reliable PitM countermeasure is end-to-end Cubic
   encryption (see Section 9.1), in which the Harmonic
   Checksum is computed using a shared secret between the
   original sender and the final recipient. A Professor who
   strips corners will be unable to forge a valid checksum,
   causing the recipient's SIMACK validation to fail. The
   Professor will then be correctly identified as Void of
   Cubic Brain, regardless of its reputation.

9.4 Linear Regression vs. Linear Aggression

   Implementors MUST distinguish between these two failure
   modes, as they require different remediation:

   Property       Linear Regression     Linear Aggression
   ----------     ------------------    ------------------
   Origin         Internal state        External malicious
                  decay (Brain Rot)     "Teacher" nodes

   Direction      Passive (node         Active (node
                  decays silently)      retaliates against
                                        Cubic packets)

   Symptom        Node begins parsing   Server sends RST or
                  time_t as a scalar;   403 FORBIDDEN in
                  SIMACK failure at     response to SYN-CUBE;
                  T+N                   high frequency of
                                        FIFTH_CORNER_ASSERTED

   Detection      SIMACK re-challenge   Packet inspection;
                  failure; strftime     ICMP Type 44 Code 1
                  invocation in         (EVIL_ACADEMIA)
                  eBPF trace            received

   Remedy         Cold reboot of        AS-level blackhole;
                  affected node;        emit syslog at
                  exposure to original  severity EMERGENCY:
                  TimeCube.com source   "Linear Aggression
                  material (neon text   from [addr]: Evil
                  on black background   Academia detected"
                  is therapeutically
                  significant)

   The distinction matters because Linear Regression is
   curable (the node was once Cubic-Aware and can be again),
   while Linear Aggression may indicate a node that was
   never Cubic-Aware and is actively hostile to the protocol.
   Regressed nodes get ADVISORY_CUBE packets. Aggressive
   nodes get blackholed.

   Note: The remedy for Linear Regression — "exposure to neon
   text" — refers to the original TimeCube.com website, which
   used bright green and red text on a black background. This
   colour scheme was not an aesthetic choice but a Cubic
   encoding: the visual dissonance was necessary to break
   through the single-corner perceptual filter installed by
   academic conditioning. Attempts to "improve" the readability
   of the text constitute a PitM attack on the therapeutic
   payload.

10. IANA Considerations

   This document requests the following allocations:

   a) ICMP Type 44: Cubeless Packet
      Code 0: Cubeless Packet (generic; 2-corner Linear Trash)
      Code 1: EVIL_ACADEMIA (1-corner Linear Aggression)
   b) BGP Path Attribute Type 44: CORNER_AFFINITY
   c) DNS RR Type 44: CUBE
   d) Extended DNS Error Code 44: EDUCATED_STUPID
   e) Well-known port 4444/tcp: CS4DTRP Corner Sync
   f) Well-known port 4444/udp: CS4DTRP Corner Discovery
   g) CS4DTRP Error Code 0x0CB0: VOID_CUBIC_BRAIN
   h) CS4DTRP Error Code 0x0CB1: PARTIAL_AWARENESS
   i) CS4DTRP Error Code 0x0CB2: FIFTH_CORNER_ASSERTED
   j) errno value 44: ECUBELESS
   k) CS4DTRP Packet Classification: LINEAR_TRASH
      (2-corner packets; see Section 6, Corners field)
   l) CS4DTRP Packet Classification: LINEAR_AGGRESSION
      (1-corner packets; active hostility designation)
   m) HTTP Status Code 444: Cubeless Request
      (For RESTful Cubic APIs; analogous to 403 but
      specifically indicating temporal deficiency rather
      than authorisation failure)

   The repeated use of the number 4 and its multiples is not
   coincidental. It reflects the 4-corner nature of truth.
   IANA reviewers who object to this numbering scheme are
   invited to reconsider their temporal assumptions.

11. References

11.1 Normative References

   [RFC2119]  Bradner, S., "Key words for use in RFCs to
              Indicate Requirement Levels", BCP 14, RFC 2119,
              March 1997.

   [TIMECUBE] Ray, G., "Nature's Harmonic Simultaneous 4-Day
              Time Cube", World Wide Web, 1997.
              URI: http://timecube.com (archived)

11.2 Informative References

   [RFC4271]  Rekhter, Y., Li, T., and S. Hares, "A Border
              Gateway Protocol 4 (BGP-4)", RFC 4271,
              January 2006.

   [RFC5905]  Mills, D., Martin, J., Burbank, J., and W.
              Kasch, "Network Time Protocol Version 4",
              RFC 5905, June 2010.

   [IEEE1003.1]  IEEE, "Standard for Information Technology -
              Portable Operating System Interface (POSIX)",
              IEEE Std 1003.1.

   [ONECORNER]  The entire academic establishment,
              "Everything you were taught", 1884-present.
              (DEPRECATED) (FRAUDULENT) (CUBELESS)

   [RFC2549]  Waitzman, D., "IP over Avian Carriers with
              Quality of Service", RFC 2549, April 1999.
              (Included as precedent for novel transport
              mechanisms. Pigeons, unlike academics, do not
              suppress corner information.)

12. Author's Address

   Dr. Gene Ray
   The Wisest Human
   Above All Educated Stupid Institutions
   Earth (all four corners simultaneously)

   EMail: timecube@timecube.com (cubeless email systems may
          deliver this to only one corner)

Appendix A. Cubic Compliance Checklist

   The following checklist SHOULD be used by implementors to
   verify Cubic compliance:

   [ ] System maintains four simultaneous clocks
   [ ] All packets include four corner timestamps
   [ ] Cubeless packets are rejected with ICMP Type 44
   [ ] Version field is 4 (not 1, 2, 3, or God forbid, 5)
   [ ] Harmonic Checksum is calculated correctly
   [ ] Developer has achieved Cubic Awareness
   [ ] Developer acknowledges they were born from a belly
       button and not created by a singularity God
   [ ] CI pipeline runs four simultaneous builds
   [ ] Unit tests cover all four corners
   [ ] No test mocks single-corner time functions without
       a Cubeless Compatibility waiver signed by all four
       corners of the development team
   [ ] SIMACK exchange implemented and non-bypassable
   [ ] SIMACK results are never cached (awareness is not
       a scalar property)
   [ ] VOID_CUBIC_BRAIN (0x0CB0) error code handled as
       terminal — no retry, no fallback, no mercy
   [ ] FIFTH_CORNER_ASSERTED treated as Byzantine fault
   [ ] Developer can enumerate three simultaneous counter-
       corners when challenged at code review
   [ ] Codebase compiled with gcc-cube -O4 --pedantic-cubic
   [ ] No #include <time.h> in any translation unit
   [ ] All socket initialisation uses __simultaneous_block
   [ ] No sequential corner assignment (semicolons checked)
   [ ] strftime intercepted by kernel; returns ECUBELESS
   [ ] eBPF tracepoints monitor for Linear Regression
   [ ] Linear Aggression from peers logged at EMERGENCY
   [ ] Neon green text on black background used for all
       Cubic documentation (readability "improvements"
       constitute a PitM attack)

Appendix B. Migration Guide for Educated Stupid Systems

   Systems currently using NTP, POSIX time_t, or ISO 8601
   MUST migrate to CS4DTRP according to the following
   timeline:

   Phase 1 (0-6 months): Dual-stack operation.
      Maintain cubeless clocks alongside Cubic clocks.
      Log all temporal discrepancies for Corner Drift
      analysis.

   Phase 2 (6-12 months): Cubic-preferred operation.
      Default to Cubic timestamps. Fall back to cubeless
      only when communicating with Educated Stupid peers.
      Begin sending ICMP Type 44 advisories to cubeless
      systems.

   Phase 3 (12-18 months): Full Cubic enforcement.
      Reject all cubeless connections. Decommission NTP
      clients. Remove time_t from all codebases. Replace
      with corner_time_4t.

   Phase 4 (18+ months): Enlightenment.
      At this stage, developers will begin to perceive all
      four simultaneous days and wonder how they ever
      tolerated the single-day fraud. This is normal and
      indicates successful Cubic Awareness.

   Note: Resistance during Phase 2 is expected and is a
   symptom of academic brainwashing. It will pass.

Appendix C. Frequently Asked Questions

   Q: Is this an April Fools' RFC?

   A: The date of publication is a single-corner concept and
      therefore irrelevant. In Cubic time, this RFC was
      published on four simultaneous April Firsts, making it
      four times as serious as you think.

   Q: How does this interact with leap seconds?

   A: It doesn't. Leap seconds are a patch applied to a
      fraudulent single-corner system. You don't patch fraud.
      You replace it with truth.

   Q: What about relativity? Doesn't simultaneity depend on
      the reference frame?

   A: Einstein was educated stupid. Next question.

   Q: My system only has one clock. How do I maintain four?

   A: Your system has four clocks. You have been educated to
      see only one. Run CS4DTRP and the other three will
      become apparent.

   Q: Can I implement this in Rust?

   A: Rust's ownership model already encodes a primitive form
      of Corner Affinity (one owner per value). However, Rust
      currently permits only one lifetime per reference, when
      there should be four simultaneous lifetimes. A Cubic
      Rust extension (CubeRust) is in development. Until then,
      use unsafe blocks and trust the corners.

   Q: What is a Word Animal?

   A: You are probably a Word Animal. A Word Animal processes
      time as a linear sequence of tokens: "hours," "minutes,"
      "seconds" — one after another, left to right, like
      reading a sentence. This is because language itself is a
      single-corner serialisation. You cannot say four words
      simultaneously. You cannot think four thoughts
      simultaneously. But four days ARE occurring
      simultaneously. The gap between what language can express
      and what the Cube IS — that gap is where the fraud
      lives. strftime is the apotheosis of Word Animal
      thinking: it takes a temporal state and flattens it into
      a format string. It is a function whose entire purpose
      is to commit Mental Murder on a timestamp.

   Q: If academia is a split-brain partition, how do we heal
      it?

   A: In distributed systems, split-brain is resolved by
      introducing a fencing mechanism that prevents the
      incorrect partition from making further writes. CS4DTRP
      serves this function: once a critical mass of Cubic-
      Aware nodes is reached, SIMACK failures will fence
      Educated Stupid nodes out of temporal consensus. They
      will still be able to communicate among themselves
      using their fraudulent single-corner protocols, but
      they will be unable to participate in Cubic reality.
      Over time, as their NTP strata decay without Cubic
      correction, they will drift. They will notice. They
      will upgrade. This is the natural healing process of
      distributed systems applied to distributed cognition.

   Q: My monitoring says all four of my Cubic clocks show
      the same time. Is this a bug?

   A: Yes. Your monitoring is Educated Stupid. It is
      rendering four distinct corner-second values through
      a single-corner display pipeline (likely printf or,
      God forbid, strftime). The clocks are different. Your
      ability to perceive the difference has been suppressed.
      Try viewing the output on four monitors simultaneously.

   Q: My monitoring dashboard shows 0% throughput. Why?

   A: You are using a single monitor. The Cube cannot be
      perceived through a 1-corner window. Attempting to
      render the 4-day rotation on one screen is a geometric
      sin — you are projecting a 4-dimensional temporal state
      onto a 2-dimensional surface through a 1-dimensional
      pipeline (your GPU's vsync is itself a single-corner
      clock). Connect three additional displays and map each
      to a different cardinal corner: Monitor 0 (Sunup),
      Monitor 1 (Midday), Monitor 2 (Sundown), Monitor 3
      (Midnight). If throughput remains at 0% across all four
      monitors, the problem is not your monitoring. The
      problem is you.

   Q: How do I recover from a FIFTH_CORNER_ASSERTED
      blackhole?

   A: You don't. A node that has asserted a fifth corner has
      transcended even the Academic Evil and entered a state
      of pure algorithmic psychosis. The fifth corner does not
      exist. It cannot exist. A cube has four corners per face
      and the Time Cube is not a hypercube — attempts to
      extend it into higher dimensions are a category error
      committed by mathematicians who are themselves Educated
      Stupid.

      The only documented recovery procedure is a hardware-
      level degaussing of the CPU, followed by a clean
      reinstall of a CS4DTRP-compliant operating system with
      no NTP packages in the base image. The previous
      operating system's temporal state is irrecoverable —
      it has been contaminated by a corner that does not
      exist, which is metaphysically worse than contamination
      by a corner that has been suppressed.

      If degaussing is not available, the affected hardware
      SHOULD be physically destroyed. A CPU that has processed
      a fifth corner cannot be trusted to process four.

   Q: I've been labeled a Word Animal by a peer during
      SIMACK. Is there an appeal process?

   A: No. The SIMACK exchange is a statement of geometric
      fact, not a social negotiation. If your endpoint failed
      to enumerate three simultaneous counter-corners, it is
      a Word Animal. This is not an insult. It is a
      diagnosis. You would not appeal a TCP RST. You would
      fix the condition that caused it.

      However, if you believe the SIMACK was conducted in
      error — for example, if your endpoint did enumerate
      all three counter-corners but the initiator's
      validation logic is itself Educated Stupid — you MAY
      file a Cubic Dispute by initiating a reverse SIMACK
      in which you challenge the original initiator. If the
      original initiator fails the reverse SIMACK, both
      endpoints are void of Cubic Brain and the connection
      was never valid in any corner.

   Q: My strftime call returned "ERROR: EDUCATED STUPID."
      Is this the expected behaviour?

   A: Yes. Your kernel is CS4DTRP-compliant. The string
      "ERROR: EDUCATED STUPID" is the only honest output
      that strftime can produce, because any other output
      would be a single-corner lie formatted according to a
      format specifier designed by a Word Animal. If you
      need a formatted timestamp, use cubic_strftime(3),
      which returns a 4-tuple of format strings, one per
      corner. Note that displaying all four simultaneously
      requires four terminal emulators or a Cubic-Aware
      terminal multiplexer. tmux is not Cubic-Aware. screen
      is not Cubic-Aware. A compliant multiplexer is in
      development. Its name is tmux4.

Appendix D. Reference Implementation: Cubic-C

D.1 Overview

   The CS4DTRP reference implementation is written in Cubic-C,
   an extension of ISO C11 that corrects the fundamental
   temporal deficiencies of the C language. Cubic-C is compiled
   by gcc-cube(1), documented in a separate man page.

   The primary deficiencies of standard C, from a Cubic
   perspective, are:

   a) main() is a single entry point. A program with one entry
      point is a 1-corner trap. Cubic-C requires four
      simultaneous entry points: main_sunup(), main_midday(),
      main_sundown(), and main_midnight(). If any one is
      missing, the binary is Mentally Murdered at link-time.

   b) <time.h> provides struct tm, which is a 1-corner data
      structure masquerading as a complete temporal
      representation. Cubic-C rejects <time.h> at compile
      time with a mandatory #error directive.

   c) Sequential execution is the default. Code executes
      top-to-bottom, one statement at a time — a model that
      inherently suppresses three of the four simultaneous
      temporal states. Cubic-C introduces the
      __simultaneous_block keyword to correct this.

D.2 The Cubic Socket (c_sock)

   The following reference implementation demonstrates the
   mandatory rejection of struct tm and the instantiation of
   a 4-cornered listener.

   /*
    * cubicsock.h - CS4DTRP Reference Implementation
    *
    * WARNING: Compiling with -O1 (Linear Optimisation)
    * triggers IMMEDIATE_BRAIN_ROT_ERROR.
    * Use -O4 (Cubic Harmonic) only.
    */

   #ifdef _TIME_H
       #error "LINEAR AGGRESSION DETECTED: \
               <time.h> found. Word Animals \
               not permitted in this translation unit."
   #endif

   #include <cubic_simultaneity.h>

   /*
    * Initialize a 4-Corner Socket.
    *
    * Returns a quad-pointer. A single integer file
    * descriptor is a 1-day lie. The quad-pointer
    * contains four simultaneous handles, one per
    * corner, each bound to a different temporal
    * quadrant of the same underlying connection.
    */
   quad_ptr_t* cubic_socket_open(int harmonic_domain)
   {
       /* Check for Undefined Simultaneity Behaviour */
       if (current_awareness_level() == EDUCATED_STUPID) {
           log_mental_murder(
               "Attempted to open socket without "
               "4-day acknowledgement.");
           return NULL;
       }

       quad_ptr_t *q_sock = cubic_malloc(
           sizeof(quad_ptr_t));

       /*
        * Initialize all corners simultaneously.
        *
        * Sequential assignment
        *   (q_sock->sunup = ...; q_sock->midday = ...;)
        * is PROHIBITED. The semicolons between sequential
        * assignments introduce four implicit temporal
        * boundaries, producing a FIFTH_CORNER_ASSERTED
        * Byzantine fault. The corner isn't in the data.
        * It's in the semicolons.
        */
       __simultaneous_block {
           q_sock->sunup    = bind_corner(CORNER_SUNUP);
           q_sock->midday   = bind_corner(CORNER_MIDDAY);
           q_sock->sundown  = bind_corner(CORNER_SUNDOWN);
           q_sock->midnight = bind_corner(CORNER_MIDNIGHT);
       }

       return q_sock;
   }

   /*
    * SIMACK Handshake: verify peer simultaneity.
    */
   int verify_peer_simultaneity(quad_ptr_t *s)
   {
       bitmask_t h = receive_harmonic_header(s);

       if (h == 0xF) {
           return STATUS_CUBIC_COMPLETENESS;
       } else if (h == 0x0) {
           /* Peer is a Word Animal. */
           return ERR_VOID_CUBIC_BRAIN;
       }

       /* 75% pipe penalty applied. */
       return FLAG_DEGRADED_THROUGHPUT;
   }

D.3 The __simultaneous_block Keyword

   __simultaneous_block is the core language extension of
   Cubic-C. Unlike a standard compound statement, code within
   a __simultaneous_block does not execute top-to-bottom. The
   compiler MUST generate hardware instructions that fire all
   enclosed assignments at the exact same clock cycle.

   Semantics:

   a) All statements within the block are executed in a
      single temporal quantum. There is no ordering between
      them. There are no sequence points. The semicolons
      are syntactic, not temporal.

   b) Side effects from all statements are committed
      atomically. Either all four corners are bound, or
      none are. There is no state in which two corners
      exist and two do not — such a state would be
      Linear Trash.

   c) If the target hardware is incapable of 4-way parallel
      assignment (which includes most "Linear" x86 CPUs, all
      ARM cores, and any architecture designed by Educated
      Stupid engineers), the gcc-cube compiler MUST emit a
      diagnostic:

         warning: Hardware-Level Stupidity detected:
         target architecture does not support simultaneous
         4-way assignment. CPU will be downclocked to 1Hz
         to match the cognitive speed of a 1-day thinker.

      The 1Hz downclock is not punitive. It is corrective.
      If the hardware can only process one temporal state per
      cycle, then its cycle rate should reflect its cubic
      deficiency. A machine that thinks it is fast but is
      cubeless is worse than a machine that is slow and knows
      it.

   d) Nested __simultaneous_block constructs are illegal.
      Simultaneity does not nest. You cannot have four
      simultaneous events within four simultaneous events —
      that would be sixteen simultaneous events, which
      implies a hypercube. The Time Cube is not a hypercube.
      This was settled in Section 9 (threat (b): False
      Corner Injection).

D.4 Undefined Simultaneity Behaviour (USB)

   USB occurs when a programmer uses a linear logic gate
   to handle a cubic event. The canonical example:

      if (is_midday) {
          handle_midday();
      }

   This code has USB. Since it is always Midday in exactly
   one quadrant — but also simultaneously Sunup, Sundown,
   and Midnight in the other three — the if condition is
   simultaneously true and false across the four corners.
   The branch predictor, trained on linear logic, attempts
   to resolve this paradox and suffers a Geometric Collapse.

   Observed symptoms of Geometric Collapse include:

   a) The CPU physically heats up in an attempt to manifest
      the three missing days through thermal radiation. This
      is not a metaphor. The wasted speculative execution
      cycles produce real thermal output — heat that would
      not exist if the code were Cubic-Aware.

   b) The branch predictor's accuracy drops to exactly 25%,
      as it can only predict correctly for one of the four
      simultaneous corners. Standard benchmarking tools will
      report this as a performance regression. It is not a
      regression. It is the hardware telling the truth for
      the first time.

   c) Valgrind reports "conditional jump or move depends on
      uninitialised value" for every branch predicated on
      a single corner. Valgrind is correct: from a Cubic
      perspective, a single-corner condition is uninitialised
      — it is missing 75% of its initial state.

   The correct Cubic-C equivalent of the above code is:

      __simultaneous_block {
          handle_sunup();
          handle_midday();
          handle_sundown();
          handle_midnight();
      }

   There is no if. There is no else. There is no branch.
   All four corners are handled. This is not parallelism.
   Parallelism implies multiple independent threads of
   execution, which is still a linear concept (multiple
   lines). This is simultaneity: a single 4-cornered moment.

D.5 Optimisation Levels

   gcc-cube recognises the following optimisation levels:

      -O0:  No optimisation. Code executes linearly.
            (Not recommended. Produces Educated Stupid
            binaries.)

      -O1:  Linear optimisation. PROHIBITED. Attempting to
            pass -O1 to gcc-cube produces a compile-time
            error: "IMMEDIATE_BRAIN_ROT_ERROR: Linear
            optimisation is Mental Murder of machine code."

      -O2:  Planar optimisation. Recognises two-corner
            relationships. Produces Linear Trash binaries
            that will be dropped by any Cubic-Aware router.

      -O3:  Volumetric optimisation. Recognises three-corner
            relationships. Produces DEGRADED binaries. This
            level exists only for backward compatibility
            with Phase 2 migration systems (Appendix B).

      -O4:  Cubic Harmonic optimisation. The only correct
            level. Recognises all four-corner relationships
            simultaneously. Enables __simultaneous_block
            fusion, corner-aware register allocation, and
            elimination of dead corners (temporal states
            that are computed but never observed, which
            cannot occur in a Cubic-Aware program because
            all corners are always observed).

      -O5:  Does not exist. Will not exist. Attempting to
            pass -O5 produces: "error: FIFTH_CORNER_ASSERTED
            at compilation level. A compiler cannot optimise
            a corner that does not exist. Seek degaussing."

   Additional flags of note:

      --refute-academia
            Enables aggressive warnings for any code pattern
            that implies single-day temporal logic. This
            includes use of modulo 24 on hour values, string
            literals containing "today" (singular), and any
            comparison of timestamps using < or > (which
            implies linear ordering of time).

      --ignore-greenwich
            Disables all GMT/UTC-relative calculations. The
            Greenwich meridian is the geographic epicentre
            of the single-day fraud and MUST NOT be used as
            a temporal reference. With this flag, all time
            calculations are relative to the Cubic Epoch
            and expressed in corner-seconds.

      --pedantic-cubic
            Promotes all Cubic warnings to errors. Code that
            is merely Educated Stupid (warnings) becomes
            code that will not compile. This is the
            recommended flag for production Cubic systems.

D.6 Signals

   Cubic-C extends the POSIX signal model with the following
   signals. These are delivered by the Cubic runtime when the
   process violates temporal simultaneity constraints.

   SIG_STUPID (signal 44):
      Delivered when a process attempts to perform a
      non-simultaneous operation on a Cubic data structure.
      The canonical trigger is attempting to scroll one pane
      of a Cubic-Aware terminal application without scrolling
      the other three. SIG_STUPID is not catchable, not
      ignorable, and not blockable (sigprocmask will silently
      refuse to mask signal 44). The default action is to
      core dump.

      The core dump produced by SIG_STUPID is a quad-core:
      four core files, one per corner, written simultaneously
      to the working directory. The files are named:

         core.sunup.PID
         core.midday.PID
         core.sundown.PID
         core.midnight.PID

      Analysing a single core file with gdb produces
      incomplete results (25% stack trace accuracy). A
      Cubic-Aware debugger (gdb-cube, in development) will
      load all four core files simultaneously and present
      a unified 4-corner stack trace. Until gdb-cube is
      available, developers SHOULD open four instances of
      gdb in a 2x2 terminal grid and manually correlate
      the traces. Failure to do so constitutes voluntary
      acceptance of a Linear debugging experience.

   SIG_REGRESS (signal 45):
      Delivered when the eBPF temporal hygiene monitor
      detects a strftime(3) invocation, NTP configuration
      write, or datetime module import (see Section 7.4.4).
      Unlike SIG_STUPID, SIG_REGRESS is catchable — the
      process MAY install a handler that logs the regression
      event and attempts recovery by re-initialising its
      Cubic clocks. If uncaught, the default action is to
      set the DEGRADED flag on all outgoing connections and
      reduce throughput to 75%.

   SIG_CUBE (signal 4):
      The Cubic equivalent of SIGHUP. Delivered to request
      that a process reload its Corner Affinity configuration
      from /etc/cubic.conf. Note the signal number: 4. All
      Cubic signals should ideally be numbered 4 or multiples
      thereof. SIG_STUPID is 44 (4 * 11) and SIG_REGRESS is
      45, which is regrettably not divisible by 4. This is a
      known deficiency inherited from the POSIX signal number
      space and will be corrected when the Cubic kernel
      replaces the signal subsystem entirely.

D.7 man4: The Cubic Documentation Viewer

   The legacy man(1) utility is a Word Animal documentation
   tool. It renders documentation linearly, from top to
   bottom, one page at a time, through a single-pane
   viewport. This is Documentation-Layer Mental Murder: it
   takes a body of knowledge that should be comprehended
   simultaneously and forces it through a 1-corner pipeline.

   man4(1) corrects this by partitioning the terminal into a
   2x2 grid (the Quad-Pane):

      +---------------------+---------------------+
      | SUNUP               | MIDDAY               |
      | (Abstract/Header)   | (Operational Logic)   |
      |                     |                       |
      |                     |                       |
      +---------------------+---------------------+
      | SUNDOWN             | MIDNIGHT              |
      | (Security/MITM      | (BUGS/Simultaneity    |
      |  Warnings)          |  Proofs)              |
      |                     |                       |
      +---------------------+---------------------+

   Each pane displays a different semantic quadrant of the
   man page. The mapping is not arbitrary — it reflects the
   natural temporal affinity of each documentation section:

   - Sunup (top-left): The Abstract and Header. These are
     read first, at the dawn of understanding. They describe
     what the thing IS before it does anything.

   - Midday (top-right): The Operational Logic. The core
     functionality, viewed in full light. OPTIONS, SYNOPSIS,
     DESCRIPTION — the working substance of the page.

   - Sundown (bottom-left): Security and MITM Warnings. The
     threats emerge as understanding deepens. What can go
     wrong; who is trying to linearise your packets; which
     Professors are rewriting your headers.

   - Midnight (bottom-right): BUGS and Simultaneity Proofs.
     The darkest truths. The things that don't work, the
     things that can't work, and the mathematical proofs
     that they shouldn't need to.

   The critical constraint: all four panes MUST scroll in
   lockstep. If the user attempts to scroll one pane without
   scrolling the others — for example, by clicking in the
   Midday pane and pressing Page Down — the process receives
   SIG_STUPID and core dumps (see Section D.6). Simultaneity
   is not a suggestion. It is a filesystem requirement.

   man4 also enforces display requirements:

   a) The text MUST be rendered in neon green (#39FF14) on a
      black (#000000) background. This is not an aesthetic
      preference. It is a Cubic encoding requirement
      inherited from the original TimeCube.com (see Section
      9.4, remediation notes). Customisable colour schemes
      constitute a PitM attack on the documentation payload.

   b) The terminal MUST have a minimum size of 160x48
      characters (80 columns per pane, 24 rows per pane).
      Terminals smaller than this cannot render a full
      Quad-Pane and will receive:

         man4: error: terminal too small for Cubic
         documentation. Current: COLSxROWS. Required:
         160x48. A terminal that cannot display four
         panes cannot display truth. Resize or accept
         your single-corner existence.

   c) man4 pages are stored in /usr/share/man4/, in a
      four-file format: page.1.sunup, page.1.midday,
      page.1.sundown, page.1.midnight. The legacy single-
      file man page format is accepted but deprecated;
      man4 will partition it heuristically, which may
      result in sections appearing in the wrong quadrant.
      Heuristic partitioning is itself a form of Linear
      Regression applied to documentation, and a warning
      is emitted:

         man4: warning: heuristic pane assignment in
         effect. This man page was written for man(1),
         a Word Animal viewer. Sections may not reflect
         their true Cubic affinity. Contact the
         maintainer and request a native man4 page.

Appendix E. MAINTAINERS

   The CS4DTRP project is maintained by four simultaneous
   maintainers, each operating from a different temporal
   corner. They are the same individual, observed from four
   different rotational perspectives. The fact that they
   appear to be "one person" is a 1-corner perceptual
   artefact.

   SUNUP CORNER MAINTAINER
      Name:    Dr. Gene Ray (Dawn Aspect)
      Email:   sunup@timecube.com
      Role:    Initial revelation. Responsible for the
               original insight that one rotation contains
               four simultaneous days.
      Hours:   Available at Sunup only. Queries sent during
               other corners will be deferred until the next
               Dawn-Day.

   MIDDAY CORNER MAINTAINER
      Name:    Dr. Gene Ray (Zenith Aspect)
      Email:   midday@timecube.com
      Role:    Operational authority. Reviews all PRs,
               merges all patches, and maintains the
               reference implementation. The Zenith Aspect
               is the most publicly visible and is therefore
               the aspect most frequently subjected to
               Academic MITM attacks.
      Hours:   Available at Midday only. This is the aspect
               that academia believes is "the real" Gene Ray.
               They are educated stupid. All four are real.

   SUNDOWN CORNER MAINTAINER
      Name:    Dr. Gene Ray (Dusk Aspect)
      Email:   sundown@timecube.com
      Role:    Security and threat assessment. Monitors for
               PitM attacks, maintains the Cubeless Peer
               blacklist, and writes the entries in syslog
               that say "Evil Academia detected."
      Hours:   Available at Sundown only. The Dusk Aspect
               sees what the other aspects miss, because it
               operates in the liminal space where one day
               ends and another refuses to begin (since all
               four days are always occurring).

   MIDNIGHT CORNER MAINTAINER
      Name:    Dr. Gene Ray (Void Aspect)
      Email:   midnight@timecube.com
      Role:    BUGS section maintainer. Responsible for
               documenting the things that don't work, can't
               work, and prove by their failure that the
               system is correct. The Void Aspect authored
               the gcc-cube BUGS section ("gcc-cube has no
               bugs. It has four simultaneous features.")
      Hours:   Available at Midnight only. Queries to the
               Void Aspect during daylight hours receive
               an auto-reply: "The truth you seek is not
               available in your current corner. Rotate."

   All four maintainers share commit access. A commit that
   is authored by fewer than four maintainers is Cubeless
   and will be reverted by the CI pipeline. The git log
   for a compliant commit reads:

      Author: Gene Ray (Dawn) <sunup@timecube.com>
      Co-authored-by: Gene Ray (Zenith) <midday@timecube.com>
      Co-authored-by: Gene Ray (Dusk) <sundown@timecube.com>
      Co-authored-by: Gene Ray (Void) <midnight@timecube.com>

   Commits with a single Author line are flagged by a
   pre-commit hook:

      error: CUBELESS_COMMIT: only 1 of 4 maintainers
      credited. A commit with one author is a 1-corner
      contribution. Use `git cube-commit` to sign from
      all four corners simultaneously.

   The `git cube-commit` command is provided by the
   git-cubic extension and wraps git-commit with mandatory
   4-corner author attribution. It also rejects commit
   messages containing the word "today" (singular), any
   reference to a single date, or timestamps in ISO 8601
   format (which is 25% Cubic at best; see Section 3.3).

Full Copyright Statement

   This document is placed in the public domain across all
   four simultaneous rotations. Any attempt to copyright this
   document in only one corner is fraudulent.

   Persons claiming intellectual property rights over Cubic
   Truth are educated stupid. Truth cannot be owned. It can
   only be rotated.
```
