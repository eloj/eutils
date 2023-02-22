ARCH:=x86-64-v3
OPT=-O3 -fomit-frame-pointer -fstrict-aliasing -march=$(ARCH) -mtune=native -msse4.2 -mavx2 -fno-math-errno
WARNFLAGS=-Wall -Wextra -Wshadow -Wstrict-aliasing -Wcast-qual -Wcast-align -Wpointer-arith -Wredundant-decls -Wfloat-equal -Wdouble-promotion -Wswitch-enum
CWARNFLAGS=-Wstrict-prototypes -Wmissing-prototypes
MISCFLAGS=-fstack-protector -fvisibility=hidden
DEVFLAGS=-ggdb -Wno-unused -D_FORTIFY_SOURCE=3

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

ifdef MEMCHECK
	TEST_PREFIX:=valgrind --tool=memcheck --leak-check=full --track-origins=yes
endif

ifdef PERF
	TEST_PREFIX:=perf stat
endif

ifdef ANALYZER
	MISCFLAGS+=-fanalyzer
endif

ifdef OPTIMIZED
	MISCFLAGS+=-DNDEBUG -Werror
else
	MISCFLAGS+=$(DEVFLAGS)
endif

CFLAGS=-std=c11 $(OPT) $(CWARNFLAGS) $(WARNFLAGS) $(MISCFLAGS)

LIBVER := 1.0.0
PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include
PKGCONFIGDIR ?= $(LIBDIR)/pkgconfig

.PHONY: clean test install backup cppcheck

all: tests

tests: test_macros test_strings test_arrays

test: tests test-macros test-strings test-arrays

test-%:
	@echo -e $(YELLOW)Running test suite '$*'$(NC)
	$(TEST_PREFIX) ./test_$*

test_macros: test_macros.c internal/tests.h emacros.h
	$(CC) $(CFLAGS) $< -o $@ $(filter %.o, $^)

test_strings: test_strings.c estrings.h internal/tests.h
	$(CC) $(CFLAGS) $< -o $@ $(filter %.o, $^)

test_arrays: test_arrays.c earrays.h internal/tests.h
	$(CC) $(CFLAGS) $< -o $@ $(filter %.o, $^)

install: eutils.pc
	@echo Installing headers \& pkgconfig
	install -m 644 -D -t $(INCLUDEDIR)/eutils emacros.h estrings.h earrays.h glhelpers.h
	install -m 644 eutils.pc $(PKGCONFIGDIR)

eutils.ps: $(eval GIT_HASH=$(shell git show-ref --head --hash=8 | head -n 1))
eutils.pc: eutils.pc.in
	@echo "Creating pkgconfig for version $(LIBVER)-g$(GIT_HASH)"
	@sed -E -e 's|@PREFIX@|$(PREFIX)|' \
		-e 's|@INCLUDEDIR@|$(INCLUDEDIR)/eutils|' \
		-e 's|@VERSION@|$(LIBVER)|' \
		-e 's|@GIT_HASH@|$(GIT_HASH)|' \
		-e 's|=${PREFIX}/|=$${prefix}/|' \
	$< >$@

cppcheck:
	@cppcheck --verbose --error-exitcode=1 --enable=warning,performance,portability .

backup:
	@echo -e $(YELLOW)Making backup$(NC)
	tar -cJf ../$(notdir $(CURDIR))-`date +"%Y-%m"`.tar.xz ../$(notdir $(CURDIR))

clean:
	@echo -e $(YELLOW)Cleaning$(NC)
	rm -f test_macros test_strings test_arrays *.o core core.* eutils.pc
