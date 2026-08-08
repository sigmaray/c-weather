CC ?= gcc

UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
# MSYS2/MinGW report MINGW64_NT-*, MSYS_NT-*, etc.
IS_WINDOWS := $(shell echo "$(UNAME_S)" | grep -qiE 'mingw|msys|cygwin' && echo 1 || echo 0)

PKG_MODULES := gtk+-3.0 libcurl
APPINDICATOR_PKG := ayatana-appindicator3-0.1

USE_APPINDICATOR_STUB := 0
ifeq ($(IS_WINDOWS),1)
  USE_APPINDICATOR_STUB := 1
endif
ifeq ($(UNAME_S),Darwin)
  USE_APPINDICATOR_STUB := 1
endif

PKG_CFLAGS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --cflags $(PKG_MODULES) 2>/dev/null)
PKG_LIBS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --libs $(PKG_MODULES) 2>/dev/null)

APPINDICATOR_CFLAGS :=
APPINDICATOR_LIBS :=
SRCS_EXTRA :=

ifeq ($(USE_APPINDICATOR_STUB),1)
  SRCS_EXTRA += src/appindicator_stub.c
else
  ifeq ($(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --exists $(APPINDICATOR_PKG) && echo yes),yes)
    APPINDICATOR_CFLAGS := $(shell pkg-config --cflags $(APPINDICATOR_PKG))
    APPINDICATOR_LIBS := $(shell pkg-config --libs $(APPINDICATOR_PKG))
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

SRCS := \
	src/main.c \
	src/http.c \
	src/history.c \
	src/settings.c \
	src/weather.c \
	src/icon.c \
	src/ui.c \
	third_party/cJSON.c \
	$(SRCS_EXTRA)

OBJS := $(SRCS:.c=.o)

.PHONY: all clean run lint

all: c-weather

c-weather: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(ALL_CFLAGS) -c -o $@ $<

run: c-weather
	./c-weather

lint:
	cppcheck --error-exitcode=1 --enable=warning,style,performance,portability \
		--suppressions-list=cppcheck-suppressions.txt \
		-I include -I src -I third_party src/

clean:
	rm -f $(OBJS) src/appindicator_stub.o c-weather c-weather.exe
