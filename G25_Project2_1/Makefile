# ============================================================
# Makefile for Custom UNIX-like Utilities
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2 -D_POSIX_C_SOURCE=200809L

# All binaries to build
TARGETS = custom_shell \
          custom_ls    \
          custom_cat   \
          custom_grep  \
          custom_wc    \
          custom_cp    \
          custom_mv    \
          custom_rm

.PHONY: all clean install help

# ── Default target ──────────────────────────────────────────
all: $(TARGETS)
	@echo ""
	@echo "  ✅  Build successful!"
	@echo "  Run './custom_shell' to start the shell."
	@echo ""

# ── Individual rules ────────────────────────────────────────
custom_shell: custom_shell.c
	$(CC) $(CFLAGS) -o $@ $<

custom_ls: custom_ls.c
	$(CC) $(CFLAGS) -o $@ $<

custom_cat: custom_cat.c
	$(CC) $(CFLAGS) -o $@ $<

custom_grep: custom_grep.c
	$(CC) $(CFLAGS) -o $@ $<

custom_wc: custom_wc.c
	$(CC) $(CFLAGS) -o $@ $<

custom_cp: custom_cp.c
	$(CC) $(CFLAGS) -o $@ $<

custom_mv: custom_mv.c
	$(CC) $(CFLAGS) -o $@ $<

custom_rm: custom_rm.c
	$(CC) $(CFLAGS) -o $@ $<

# ── Clean ───────────────────────────────────────────────────
clean:
	rm -f $(TARGETS)
	@echo "Cleaned up binaries."

# ── Quick smoke-test ─────────────────────────────────────────
test: all
	@echo "--- custom_ls -l . ---"
	./custom_ls -l .
	@echo ""
	@echo "--- custom_cat Makefile ---"
	./custom_cat -n Makefile
	@echo ""
	@echo "--- custom_wc Makefile ---"
	./custom_wc Makefile
	@echo ""
	@echo "--- custom_grep CC Makefile ---"
	./custom_grep -n CC Makefile

# ── Help ────────────────────────────────────────────────────
help:
	@echo "Targets:"
	@echo "  make        - Build all utilities"
	@echo "  make clean  - Remove binaries"
	@echo "  make test   - Run smoke tests"

