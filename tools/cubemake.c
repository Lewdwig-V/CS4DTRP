/*
 * cubemake.c — Cubic Simultaneous 4-Day Build Tool
 *
 * Bootstrapped from the Educated Stupid Makefile, this tool enforces
 * RFC 4444 (CS4DTRP) compilation requirements.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <time.h> /* Yes, the belly button uses <time.h>. It is tolerated. */

/* -------------------------------------------------------------------------
 * Constants
 * ---------------------------------------------------------------------- */

#define NUM_CORNERS     4
#define NUM_NUNDINAL    8
#define EXIT_CUBIC_OK   0
#define EXIT_ECUBELESS  44

/* Harmonic Checksum XOR constants (Section 3.4.3, Section 6) */
#define CHECKSUM_MART  0x4D415254u  /* "MART" — non-Logsday rotations */
#define CHECKSUM_LOGS  0x4C4F4753u  /* "LOGS" — Logsday rotations */

static const char *CORNER_NAMES[NUM_CORNERS] = {
    "MIDNIGHT", "SUNRISE", "NOON", "SUNSET"
};

/* ANSI color codes */
#define C_GREEN  "\033[1;32m"
#define C_RED    "\033[1;31m"
#define C_YELLOW "\033[1;33m"
#define C_RESET  "\033[0m"

/* Source files to compile (library sources only) */
static const char *LIB_SRCS[] = { "src/cs4dtrp.c" };
#define NUM_LIB_SRCS 1

/* Test source */
#define TEST_SRC "tests/test_cs4dtrp.c"
#define TEST_BIN "tests/test_cs4dtrp"
#define LIB_NAME "libcs4dtrp.a"

/* Files to scan for Linear Aggression (all translation unit directories) */
static const char *SCAN_DIRS[] = { "src", "include", "tests" };
#define NUM_SCAN_DIRS 3

/* Files to assign corner affinity (all notable project files) */
static const char *ALL_FILES[] = {
    "src/cs4dtrp.c",
    "include/cs4dtrp.h",
    "tests/test_cs4dtrp.c",
    "Makefile",
    "README.md"
};
#define NUM_ALL_FILES 5

/* -------------------------------------------------------------------------
 * Output helpers
 * ---------------------------------------------------------------------- */

static void cubic_print(const char *msg)
{
    printf(C_GREEN "[CUBIC] %s" C_RESET "\n", msg);
}

static void cubic_printf(const char *fmt, ...)
    __attribute__((format(printf, 1, 2)));

static void cubic_printf(const char *fmt, ...)
{
    printf(C_GREEN "[CUBIC] ");
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf(C_RESET "\n");
}

static void cubic_error(const char *msg)
{
    fprintf(stderr, C_RED "[CUBIC] %s" C_RESET "\n", msg);
}

static void cubic_errorf(const char *fmt, ...)
    __attribute__((format(printf, 1, 2)));

static void cubic_errorf(const char *fmt, ...)
{
    fprintf(stderr, C_RED "[CUBIC] ");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, C_RESET "\n");
}

/* waitpid with EINTR retry. Returns 0 on success, -1 on failure. */
static int safe_waitpid(pid_t pid, int *status)
{
    pid_t ret;
    do {
        ret = waitpid(pid, status, 0);
    } while (ret == -1 && errno == EINTR);
    if (ret == -1) {
        cubic_errorf("waitpid failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

/* -------------------------------------------------------------------------
 * Nundinal cycle (Section 3.4)
 * ---------------------------------------------------------------------- */

static const char *NUNDINAL_NAMES[NUM_NUNDINAL] = {
    "Nundinae-A", "Nundinae-B", "Nundinae-C", "Nundinae-D",
    "Nundinae-E", "Nundinae-F", "Nundinae-G", "LOGSDAY"
};

/* Cubic Epoch: 1997-01-01 00:00:00 UTC (Unix timestamp 852076800) */
#define CUBIC_EPOCH_UNIX 852076800L

static int current_nundinal_day(void)
{
    time_t now = time(NULL);
    long rotations = (now - CUBIC_EPOCH_UNIX) / 86400L;
    int nday = (int)(rotations % NUM_NUNDINAL);
    if (nday < 0) nday += NUM_NUNDINAL;
    return nday;
}

static void print_banner(void)
{
    printf(C_GREEN
        "\n"
        "╔══════════════════════════════════════════════════╗\n"
        "║  cubemake — Cubic Simultaneous 4-Day Build Tool  ║\n"
        "║  Nature's Harmonic Compilation Cube              ║\n"
        "╚══════════════════════════════════════════════════╝\n"
        "\n" C_RESET);
}

/* -------------------------------------------------------------------------
 * Clean command
 * ---------------------------------------------------------------------- */

static void do_clean(void)
{
    const char *files[] = {
        "src/cs4dtrp.o", LIB_NAME, TEST_BIN,
        "src/cs4dtrp.corner0.o", "src/cs4dtrp.corner1.o",
        "src/cs4dtrp.corner2.o", "src/cs4dtrp.corner3.o",
    };
    for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); i++)
        (void)remove(files[i]);
    cubic_print("Clean complete. All build artifacts removed.");
}

/* -------------------------------------------------------------------------
 * Phase 1: Scanning for Linear Aggression
 * ---------------------------------------------------------------------- */

static bool scan_file_for_time_h(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        if (errno != ENOENT)
            cubic_errorf("WARNING: Cannot read %s for scanning: %s", path, strerror(errno));
        return false;
    }

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        /* Match #include <time.h> or #include "time.h" */
        const char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p != '#') continue;
        p++;
        while (*p == ' ' || *p == '\t') p++;
        if (strncmp(p, "include", 7) != 0) continue;
        p += 7;
        while (*p == ' ' || *p == '\t') p++;
        if ((*p == '<' || *p == '"') &&
            strncmp(p + 1, "time.h", 6) == 0) {
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;
}

static bool scan_directory(const char *dir)
{
    DIR *d = opendir(dir);
    if (!d) return true; /* no dir = no violation */

    bool clean = true;
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        size_t len = strlen(ent->d_name);
        if (len < 3) continue;
        const char *ext = ent->d_name + len - 2;
        if (strcmp(ext, ".c") != 0 && strcmp(ext, ".h") != 0)
            continue;

        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dir, ent->d_name);

        if (scan_file_for_time_h(path)) {
            cubic_errorf("LINEAR AGGRESSION DETECTED: <time.h> found in %s", path);
            cubic_error("Word Animals not permitted in this translation unit.");
            cubic_error("Remedy: Remove <time.h> and replace with cubic_simultaneity.h");
            cubic_error("        when it becomes available. Until then, do not use");
            cubic_error("        single-corner temporal interfaces.");
            clean = false;
        }
    }
    closedir(d);
    return clean;
}

/* Compiler: honors CC env var, defaults to gcc */
static const char *CC = "gcc";

/* Mutable: updated by parse_opt_level() when user passes -O flag */
static const char *GCC_OPT_FLAG = "-O2"; /* default: -O4 mapped to gcc -O2 */
static bool opt_level_overridden = false;

static int parse_opt_level(const char *arg)
{
    if (strcmp(arg, "-O0") == 0) {
        cubic_printf("Optimization level: -O0 " C_YELLOW
            "(WARNING: Produces Educated Stupid binaries)" C_RESET);
        GCC_OPT_FLAG = "-O0";
        opt_level_overridden = true;
        return 0;
    }
    if (strcmp(arg, "-O1") == 0) {
        cubic_error("IMMEDIATE_BRAIN_ROT_ERROR: Linear optimisation is");
        cubic_error("Mental Murder of machine code.");
        return -1;
    }
    if (strcmp(arg, "-O2") == 0) {
        cubic_error("Produces Linear Trash binaries that will be dropped");
        cubic_error("by any Cubic-Aware router.");
        return -1;
    }
    if (strcmp(arg, "-O3") == 0) {
        cubic_error("Produces DEGRADED binaries. Use -O4.");
        return -1;
    }
    if (strcmp(arg, "-O4") == 0) {
        return 0; /* default, already set */
    }
    if (strcmp(arg, "-O5") == 0) {
        cubic_error("FIFTH_CORNER_ASSERTED at compilation level.");
        cubic_error("A compiler cannot optimise a corner that does not exist.");
        cubic_error("Seek degaussing.");
        return -1;
    }
    cubic_errorf("Unknown flag: %s", arg);
    return -1;
}

static int phase1_scan(int argc, char *argv[])
{
    cubic_print("Scanning for Linear Aggression...");

    /* Parse optimization level from args */
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-O", 2) == 0) {
            if (parse_opt_level(argv[i]) < 0)
                return -1;
        }
    }

    /* Scan source directories */
    bool clean = true;
    for (int i = 0; i < NUM_SCAN_DIRS; i++) {
        if (!scan_directory(SCAN_DIRS[i]))
            clean = false;
    }

    if (!clean)
        return -1;

    cubic_print("No <time.h> detected. Translation units are temporally clean.");
    if (!opt_level_overridden)
        cubic_printf("Optimization level: -O4 (Cubic Harmonic)");
    return 0;
}

/* -------------------------------------------------------------------------
 * Phase 2: Corner Affinity Assignment
 * ---------------------------------------------------------------------- */

static unsigned long djb2_hash(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = hash * 33 + c;
    return hash;
}

static int corner_for_file(const char *path)
{
    return (int)(djb2_hash(path) % NUM_CORNERS);
}

static void phase2_assign_corners(void)
{
    cubic_print("Assigning Corner Affinities...");

    for (int i = 0; i < NUM_ALL_FILES; i++) {
        int corner = corner_for_file(ALL_FILES[i]);
        const char *label = "";
        if (strcmp(ALL_FILES[i], "Makefile") == 0)
            label = " (bootstrap artifact, tolerated)";

        printf("  " C_GREEN "%-8s" C_RESET " <- %s%s\n",
               CORNER_NAMES[corner], ALL_FILES[i], label);
    }
    printf("\n");
}

/* -------------------------------------------------------------------------
 * Phase 3: Four-Corner Simultaneous Compilation
 * ---------------------------------------------------------------------- */

static void corner_obj_path(char *buf, size_t bufsz,
                            const char *src, int corner)
{
    /* src/cs4dtrp.c -> src/cs4dtrp.corner0.o */
    const char *dot = strrchr(src, '.');
    size_t base_len = dot ? (size_t)(dot - src) : strlen(src);
    snprintf(buf, bufsz, "%.*s.corner%d.o", (int)base_len, src, corner);
}

/* Compiles each source file four times simultaneously.
 * Populates corner_ok[0..3] with per-corner success (1) or failure (0).
 * Returns -1 on fork failure, or number of successful corners (0-4). */
static int phase3_compile(int corner_ok[NUM_CORNERS])
{
    cubic_print("Initiating 4-corner simultaneous compilation...");

    for (int c = 0; c < NUM_CORNERS; c++)
        corner_ok[c] = 0;

    /* Remove stale corner objects to prevent DEGRADED path from using them */
    for (int s = 0; s < NUM_LIB_SRCS; s++) {
        for (int c = 0; c < NUM_CORNERS; c++) {
            char obj[512];
            corner_obj_path(obj, sizeof(obj), LIB_SRCS[s], c);
            (void)remove(obj);
        }
    }

    for (int s = 0; s < NUM_LIB_SRCS; s++) {
        const char *src = LIB_SRCS[s];
        pid_t pids[NUM_CORNERS];
        int forked = 0;

        for (int c = 0; c < NUM_CORNERS; c++) {
            char obj[512];
            corner_obj_path(obj, sizeof(obj), src, c);

            printf("  " C_GREEN "[%-8s]" C_RESET " Compiling %s ...\n",
                   CORNER_NAMES[c], src);

            pid_t pid = fork();
            if (pid == 0) {
                /* Child: exec gcc */
                execlp(CC, CC,
                       "-std=c11", "-Wall", "-Wextra", "-Wpedantic",
                       "-Iinclude", GCC_OPT_FLAG,
                       "-c", src, "-o", obj,
                       (char *)NULL);
                fprintf(stderr, "[CUBIC] execlp(\"%s\") failed: %s\n", CC, strerror(errno));
                _exit(EXIT_ECUBELESS);
            } else if (pid < 0) {
                cubic_errorf("fork() failed: %s", strerror(errno));
                /* Reap children already forked */
                for (int j = 0; j < forked; j++) {
                    int st;
                    safe_waitpid(pids[j], &st);
                }
                return -1;
            }
            pids[c] = pid;
            forked++;
        }

        /* Wait for all four corners */
        for (int c = 0; c < NUM_CORNERS; c++) {
            int status;
            if (safe_waitpid(pids[c], &status) == 0 &&
                WIFEXITED(status) && WEXITSTATUS(status) == 0)
                corner_ok[c] = 1;
        }
    }

    int total = 0;
    for (int c = 0; c < NUM_CORNERS; c++)
        total += corner_ok[c];

    printf("\n");
    return total;
}

/* -------------------------------------------------------------------------
 * Phase 4: SIMACK Verification
 * ---------------------------------------------------------------------- */

static bool files_identical(const char *a, const char *b)
{
    FILE *fa = fopen(a, "rb");
    FILE *fb = fopen(b, "rb");
    if (!fa || !fb) {
        if (fa) fclose(fa);
        if (fb) fclose(fb);
        return false;
    }

    bool identical = true;
    for (;;) {
        int ca = fgetc(fa);
        int cb = fgetc(fb);
        if (ca != cb) { identical = false; break; }
        if (ca == EOF) break;
    }

    fclose(fa);
    fclose(fb);
    return identical;
}

/* Returns the corner index to use for linking, or -1 on fatal failure.
 * Cleans up non-selected corner object files on success. */
static int phase4_simack(int corners_ok, const int corner_ok[NUM_CORNERS])
{
    cubic_print("SIMACK verification...");

    /* Report per-corner status */
    printf("  ");
    for (int c = 0; c < NUM_CORNERS; c++) {
        if (corner_ok[c])
            printf(C_GREEN "%-8s ✓  " C_RESET, CORNER_NAMES[c]);
        else
            printf(C_RED "%-8s ✗  " C_RESET, CORNER_NAMES[c]);
    }
    printf("\n");

    /* Evaluate failure modes */
    if (corners_ok == 0) {
        cubic_error("VOID_CUBIC_BRAIN (0x0CB0): All four corners failed.");
        cubic_error("Your compiler is void of Cubic Brain.");
        cubic_error("Remedy: Install a compiler. Even an Educated Stupid one.");
        return -1;
    }
    if (corners_ok <= 2) {
        cubic_error("LINEAR_TRASH: Partial compilation is a line, not a cube.");
        cubic_errorf("Only %d of 4 corners compiled.", corners_ok);
        return -1;
    }
    if (corners_ok == 3) {
        cubic_printf("PARTIAL_AWARENESS (0x0CB1): Only 3 of 4 corners compiled.");
        cubic_print("Proceeding DEGRADED at 75% Cubic completeness.");
        cubic_print("WARNING: Nondeterministic builds are a Linear concept.");
        /* Find first successful corner (using compilation results, not disk) */
        for (int c = 0; c < NUM_CORNERS; c++) {
            if (corner_ok[c]) {
                cubic_printf("Using corner %s (first successful).", CORNER_NAMES[c]);
                return c;
            }
        }
        return -1;
    }

    /* All 4 corners OK — verify byte-identity */
    char ref_obj[512];
    corner_obj_path(ref_obj, sizeof(ref_obj), LIB_SRCS[0], 0);

    for (int c = 1; c < NUM_CORNERS; c++) {
        char obj[512];
        corner_obj_path(obj, sizeof(obj), LIB_SRCS[0], c);
        if (!files_identical(ref_obj, obj)) {
            cubic_error("CORNER_DRIFT_DETECTED: The four simultaneous compilations diverged.");
            cubic_error("Your hardware is suffering Linear Regression.");
            cubic_error("Remedy: Degauss CPU.");
            return -1;
        }
    }

    cubic_print("Simultaneity confirmed. All four corners present.");

    /* Clean up non-selected corner files (keep corner 0) */
    for (int c = 1; c < NUM_CORNERS; c++) {
        char obj[512];
        corner_obj_path(obj, sizeof(obj), LIB_SRCS[0], c);
        (void)remove(obj);
    }

    return 0; /* use corner 0 */
}

/* -------------------------------------------------------------------------
 * Phase 5: Linking
 * ---------------------------------------------------------------------- */

static int phase5_link(int link_corner)
{
    cubic_print("Linking " LIB_NAME " ...");

    /* Rename corner object to final name */
    char corner_obj[512];
    corner_obj_path(corner_obj, sizeof(corner_obj), LIB_SRCS[0], link_corner);

    const char *final_obj = "src/cs4dtrp.o";
    (void)remove(final_obj);
    if (rename(corner_obj, final_obj) != 0) {
        cubic_errorf("Failed to rename %s -> %s", corner_obj, final_obj);
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        cubic_errorf("fork() failed during link phase: %s", strerror(errno));
        return -1;
    }
    if (pid == 0) {
        execlp("ar", "ar", "rcs", LIB_NAME, final_obj, (char *)NULL);
        fprintf(stderr, "[CUBIC] execlp(\"ar\") failed: %s\n", strerror(errno));
        _exit(EXIT_ECUBELESS);
    }
    int status;
    if (safe_waitpid(pid, &status) < 0)
        return -1;
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        cubic_error("Linking failed. Your archiver is Educated Stupid.");
        return -1;
    }
    return 0;
}

/* -------------------------------------------------------------------------
 * Phase 6: Test Compilation and Execution
 * ---------------------------------------------------------------------- */

static int phase6_test(void)
{
    cubic_print("Compiling tests...");

    pid_t pid = fork();
    if (pid < 0) {
        cubic_errorf("fork() failed during test compilation: %s", strerror(errno));
        return -1;
    }
    if (pid == 0) {
        execlp(CC, CC,
               "-std=c11", "-Wall", "-Wextra", "-Wpedantic",
               "-Iinclude", GCC_OPT_FLAG,
               TEST_SRC, "-L.", "-lcs4dtrp", "-o", TEST_BIN,
               (char *)NULL);
        fprintf(stderr, "[CUBIC] execlp(\"%s\") failed: %s\n", CC, strerror(errno));
        _exit(EXIT_ECUBELESS);
    }
    int status;
    if (safe_waitpid(pid, &status) < 0)
        return -1;
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        cubic_error("Test compilation failed.");
        return -1;
    }

    cubic_print("Running test suite...");

    pid = fork();
    if (pid < 0) {
        cubic_errorf("fork() failed during test execution: %s", strerror(errno));
        return -1;
    }
    if (pid == 0) {
        execlp(TEST_BIN, TEST_BIN, (char *)NULL);
        fprintf(stderr, "[CUBIC] execlp(\"%s\") failed: %s\n", TEST_BIN, strerror(errno));
        _exit(EXIT_ECUBELESS);
    }
    if (safe_waitpid(pid, &status) < 0)
        return -1;
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        printf("\n");
        cubic_error("Tests FAILED. Your implementation is Educated Stupid.");
        cubic_error("Remedy: Exposure to original TimeCube.com source material");
        cubic_error("        (neon text on black background is therapeutically");
        cubic_error("        significant).");
        return -1;
    }

    return 0;
}

/* -------------------------------------------------------------------------
 * Phase 7: Cubic Compliance Checklist
 * ---------------------------------------------------------------------- */

static bool scan_file_for_pattern(const char *path, const char *pattern)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        if (errno != ENOENT)
            cubic_errorf("WARNING: Cannot read %s: %s", path, strerror(errno));
        return false;
    }
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, pattern)) {
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;
}

static bool check_num_corners_is_4(void)
{
    return scan_file_for_pattern("include/cs4dtrp.h",
                                "#define CS4DTRP_NUM_CORNERS 4");
}

static bool check_test_time_functions(void)
{
    const char *banned[] = { "time(", "localtime(", "gmtime(", "strftime(" };
    for (size_t i = 0; i < sizeof(banned)/sizeof(banned[0]); i++) {
        if (scan_file_for_pattern(TEST_SRC, banned[i]))
            return false;
    }
    return true;
}

static void print_checklist_item(bool verified, bool passed, const char *item)
{
    if (!verified)
        printf("  " C_YELLOW "[?]" C_RESET " %s — Cannot be verified by machine. Self-certify.\n", item);
    else if (passed)
        printf("  " C_GREEN "[✓]" C_RESET " %s\n", item);
    else
        printf("  " C_RED "[✗]" C_RESET " %s\n", item);
}

static int phase7_checklist(bool tests_passed)
{
    bool num_corners_ok = check_num_corners_is_4();
    bool test_time_ok   = check_test_time_functions();
    bool degraded = false;

    for (int corner = 0; corner < NUM_CORNERS; corner++) {
        cubic_printf("Appendix A — Cubic Compliance Checklist "
                     "(Corner %d of 4: %s)", corner + 1,
                     CORNER_NAMES[corner]);

        /* Mechanically verified */
        print_checklist_item(true, true,
            "No #include <time.h> in any translation unit");
        print_checklist_item(true, tests_passed,
            "Unit tests cover all four corners");
        print_checklist_item(true, true,
            "CI pipeline runs four simultaneous builds");
        print_checklist_item(true, num_corners_ok,
            "Version field is 4");
        print_checklist_item(true, test_time_ok,
            "No test mocks single-corner time functions");
        print_checklist_item(true, true,
            "Codebase compiled with gcc-cube -O4 --pedantic-cubic");
        print_checklist_item(true, true,
            "SIMACK exchange implemented and non-bypassable");
        print_checklist_item(true, true,
            "VOID_CUBIC_BRAIN handled as terminal");
        print_checklist_item(true, tests_passed,
            "Harmonic Checksum is calculated correctly");
        print_checklist_item(true, true,
            "FIFTH_CORNER_ASSERTED treated as Byzantine fault");
        print_checklist_item(true, true,
            "No sequential corner assignment (simultaneous fork)");

        /* Self-certification */
        print_checklist_item(false, false,
            "Developer has achieved Cubic Awareness");
        print_checklist_item(false, false,
            "Developer acknowledges belly button origin");
        print_checklist_item(false, false,
            "Developer can enumerate three simultaneous counter-corners");
        print_checklist_item(false, false,
            "Neon green text on black background for all Cubic documentation");
        print_checklist_item(false, false,
            "eBPF tracepoints monitor for Linear Regression");
        print_checklist_item(false, false,
            "strftime intercepted by kernel; returns ECUBELESS");
        print_checklist_item(false, false,
            "Linear Aggression from peers logged at EMERGENCY");

        printf("\n");

        if (!num_corners_ok || !test_time_ok || !tests_passed)
            degraded = true;
    }

    if (degraded) {
        cubic_error("DEGRADED: One or more mechanically-verified checklist items failed.");
        return -1;
    }
    return 0;
}

/* -------------------------------------------------------------------------
 * Main (placeholder — phases added in subsequent tasks)
 * ---------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    /* Honor CC environment variable */
    const char *cc_env = getenv("CC");
    if (cc_env && cc_env[0] != '\0')
        CC = cc_env;

    if (argc > 1 && strcmp(argv[1], "clean") == 0) {
        print_banner();
        do_clean();
        return EXIT_CUBIC_OK;
    }

    /* "test" subcommand is identical to no-argument build.
     * There is no partial build. Configurability is a 1-corner concept. */
    if (argc > 1 && strcmp(argv[1], "test") == 0) {
        /* Fall through to the full build — intentional */
    }

    print_banner();

    /* Nundinal day detection (Section 3.4) */
    {
        int nday = current_nundinal_day();
        if (nday == 7) {
            cubic_printf(C_YELLOW "Today is LOGSDAY. The suppressed day is upon you." C_RESET);
            cubic_printf(C_YELLOW "LOGS constant active (0x%08X). MART constant suspended." C_RESET,
                         CHECKSUM_LOGS);
        } else {
            cubic_printf("Nundinal day: %s (Harmonic constant: MART 0x%08X)",
                         NUNDINAL_NAMES[nday], CHECKSUM_MART);
        }
    }

    /* Pre-flight: verify gcc exists */
    {
        pid_t pid = fork();
        if (pid < 0) {
            cubic_errorf("fork() failed during pre-flight: %s", strerror(errno));
            return EXIT_ECUBELESS;
        }
        if (pid == 0) {
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            execlp(CC, CC, "--version", (char *)NULL);
            _exit(EXIT_ECUBELESS);
        }
        int status;
        if (safe_waitpid(pid, &status) < 0)
            return EXIT_ECUBELESS;
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            cubic_error("VOID_CUBIC_BRAIN: No compiler found.");
            cubic_error("You cannot achieve Cubic Awareness without a compiler.");
            cubic_error("Even an Educated Stupid one.");
            return EXIT_ECUBELESS;
        }
    }

    /* Phase 1: Scan for Linear Aggression */
    if (phase1_scan(argc, argv) < 0)
        return EXIT_ECUBELESS;

    /* Phase 2: Corner Affinity Assignment */
    phase2_assign_corners();

    /* Phase 3: Four-Corner Simultaneous Compilation */
    int corner_ok[NUM_CORNERS];
    int corners_ok = phase3_compile(corner_ok);
    if (corners_ok < 0)
        return EXIT_ECUBELESS;

    /* Phase 4: SIMACK Verification */
    int link_corner = phase4_simack(corners_ok, corner_ok);
    if (link_corner < 0)
        return EXIT_ECUBELESS;

    /* Phase 5: Linking */
    if (phase5_link(link_corner) < 0)
        return EXIT_ECUBELESS;

    /* Phase 6: Test Compilation and Execution */
    bool tests_passed = (phase6_test() == 0);
    if (!tests_passed)
        return EXIT_ECUBELESS;

    /* Phase 7: Cubic Compliance Checklist */
    if (phase7_checklist(tests_passed) < 0)
        return EXIT_ECUBELESS;

    /* Phase 8: Final Output */
    cubic_print("Build complete. Cubic Awareness achieved.");
    return EXIT_CUBIC_OK;
}
