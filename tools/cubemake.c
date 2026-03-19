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

/* -------------------------------------------------------------------------
 * Constants
 * ---------------------------------------------------------------------- */

#define NUM_CORNERS     4
#define EXIT_CUBIC_OK   0
#define EXIT_ECUBELESS  44

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

/* Files to scan for Linear Aggression */
static const char *SCAN_DIRS[] = { "src", "include" };
#define NUM_SCAN_DIRS 2

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
    if (!f)
        return false;

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
 * Main (placeholder — phases added in subsequent tasks)
 * ---------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "clean") == 0) {
        print_banner();
        do_clean();
        return EXIT_CUBIC_OK;
    }

    print_banner();

    /* Phase 1: Scan for Linear Aggression */
    if (phase1_scan(argc, argv) < 0)
        return EXIT_ECUBELESS;

    /* Phase 8: Final Output */
    cubic_print("Build complete. Cubic Awareness achieved.");
    return EXIT_CUBIC_OK;
}
