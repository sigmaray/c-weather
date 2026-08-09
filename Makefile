CC ?= gcc
STATIC ?= 0

UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
# MSYS2/MinGW report MINGW64_NT-*, MSYS_NT-*, etc.
IS_WINDOWS := $(shell echo "$(UNAME_S)" | grep -qiE 'mingw|msys|cygwin' && echo 1 || echo 0)

PKG_CONFIG := $(shell command -v pkg-config 2>/dev/null)
APPINDICATOR_PKG := ayatana-appindicator3-0.1

USE_APPINDICATOR_STUB := 0
ifeq ($(IS_WINDOWS),1)
  USE_APPINDICATOR_STUB := 1
endif
ifeq ($(UNAME_S),Darwin)
  USE_APPINDICATOR_STUB := 1
endif

PKG_CFLAGS := $(shell test -n "$(PKG_CONFIG)" && $(PKG_CONFIG) --cflags gtk+-3.0 libcurl 2>/dev/null)

# Full static GTK is not available from distro packages. STATIC=1 means:
#   - static libgcc on Linux/Windows (not used on macOS/Apple Clang)
#   - on Windows: prefer static libcurl via pkg-config --static
#   - GTK/GLib remain shared and are shipped by release packaging
#     (AppImage / DLL zip / dylib tarball).
ifeq ($(STATIC),1)
  ifeq ($(IS_WINDOWS),1)
    PKG_LIBS := $(shell test -n "$(PKG_CONFIG)" && $(PKG_CONFIG) --libs gtk+-3.0 2>/dev/null) \
	$(shell test -n "$(PKG_CONFIG)" && $(PKG_CONFIG) --static --libs libcurl 2>/dev/null)
  else
    PKG_LIBS := $(shell test -n "$(PKG_CONFIG)" && $(PKG_CONFIG) --libs gtk+-3.0 libcurl 2>/dev/null)
  endif
else
  PKG_LIBS := $(shell test -n "$(PKG_CONFIG)" && $(PKG_CONFIG) --libs gtk+-3.0 libcurl 2>/dev/null)
endif

APPINDICATOR_CFLAGS :=
APPINDICATOR_LIBS :=
SRCS_EXTRA :=

ifeq ($(USE_APPINDICATOR_STUB),1)
  SRCS_EXTRA += src/appindicator_stub.c
else
  ifeq ($(shell test -n "$(PKG_CONFIG)" && $(PKG_CONFIG) --exists $(APPINDICATOR_PKG) && echo yes),yes)
    APPINDICATOR_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(APPINDICATOR_PKG))
    APPINDICATOR_LIBS := $(shell $(PKG_CONFIG) --libs $(APPINDICATOR_PKG))
  else
    APPINDICATOR_LIB ?= /usr/lib/x86_64-linux-gnu/libayatana-appindicator3.so.1
    APPINDICATOR_LIBS := $(APPINDICATOR_LIB)
  endif
endif

BASE_CFLAGS := -Wall -Wextra -std=c11 -Iinclude -Isrc -Ithird_party $(PKG_CFLAGS) $(APPINDICATOR_CFLAGS)
CFLAGS ?= -O2
ALL_CFLAGS = $(CFLAGS) $(BASE_CFLAGS)
LDFLAGS ?=
LIBS := $(PKG_LIBS) $(APPINDICATOR_LIBS) -lm

ifeq ($(STATIC),1)
  ifneq ($(UNAME_S),Darwin)
    # Apple Clang rejects -static-libgcc; system libc is always shared on macOS.
    LDFLAGS += -static-libgcc
  endif
  ifeq ($(IS_WINDOWS),1)
    LDFLAGS += -static-libstdc++
    LIBS += -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic
  endif
endif

SRCS := \
	src/main.c \
	src/http.c \
	src/log.c \
	src/history.c \
	src/settings.c \
	src/weather.c \
	src/icon.c \
	src/ui.c \
	third_party/cJSON.c \
	$(SRCS_EXTRA)

OBJS := $(SRCS:.c=.o)

# Library objects used by unit tests (no main/ui/tray).
TEST_LIB_SRCS := \
	src/http.c \
	src/log.c \
	src/history.c \
	src/settings.c \
	src/weather.c \
	src/icon.c \
	third_party/cJSON.c

TEST_LIB_OBJS := $(TEST_LIB_SRCS:.c=.o)

TEST_SRCS := \
	tests/run_tests.c \
	tests/test_weather.c \
	tests/test_settings.c \
	tests/test_history.c \
	tests/test_http.c \
	tests/test_icon.c

TEST_OBJS := $(TEST_SRCS:.c=.o)

.PHONY: all clean run lint test

all: c-weather

c-weather: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(ALL_CFLAGS) -c -o $@ $<

tests/%.o: tests/%.c
	$(CC) $(ALL_CFLAGS) -Itests -c -o $@ $<

run: c-weather
	./c-weather

test: c-weather-tests
	./c-weather-tests

c-weather-tests: $(TEST_OBJS) $(TEST_LIB_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(TEST_OBJS) $(TEST_LIB_OBJS) $(LIBS)

lint:
	cppcheck --error-exitcode=1 --enable=warning,style,performance,portability \
		--suppressions-list=cppcheck-suppressions.txt \
		-I include -I src -I third_party src/

clean:
	rm -f $(OBJS) $(TEST_OBJS) src/appindicator_stub.o \
		c-weather c-weather.exe c-weather-tests c-weather-tests.exe
