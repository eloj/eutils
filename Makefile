ARCH:=x86-64-v3
OPT=-O3 -fomit-frame-pointer -fstrict-aliasing -march=$(ARCH) -mtune=native -msse4.2 -mavx2 -fno-math-errno
WARNFLAGS=-Wall -Wextra -Wshadow -Wstrict-aliasing -Wcast-qual -Wcast-align -Wpointer-arith -Wredundant-decls -Wfloat-equal -Wdouble-promotion -Wswitch-enum
CWARNFLAGS=-Wstrict-prototypes -Wmissing-prototypes
MISCFLAGS=-fstack-protector -fvisibility=hidden -D_FILE_OFFSET_BITS=64
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

all: test_macros

test: test_macros
	$(TEST_PREFIX) ./test_macros

test_macros: test_macros.c macros-eddy.h
	$(CC) $(CFLAGS) $< -o $@ $(filter %.o, $^)

.PHONY: clean backup

install: eutils.pc
	@echo Installing header \& pkgconfig
	install -m 644 macros-eddy.h $(INCLUDEDIR)
	install -m 644 eutils.pc $(PKGCONFIGDIR)

eutils.pc: eutils.pc.in
	@echo Creating pkgconfig
	@sed -E -e 's|@PREFIX@|$(PREFIX)|' \
		-e 's|@INCLUDEDIR@|$(INCLUDEDIR)|' \
		-e 's|@VERSION@|$(LIBVER)|' \
		-e 's|=${PREFIX}/|=$${prefix}/|' \
	$< >$@

backup:
	@echo -e $(YELLOW)Making backup$(NC)
	tar -cJf ../$(notdir $(CURDIR))-`date +"%Y-%m"`.tar.xz ../$(notdir $(CURDIR))

clean:
	@echo -e $(YELLOW)Cleaning$(NC)
	rm -f test_macros *.o core core.* eutils.pc
