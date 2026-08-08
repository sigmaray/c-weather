CC ?= gcc
PKG_CFLAGS := $(shell pkg-config --cflags gtk+-3.0 libcurl)
PKG_LIBS := $(shell pkg-config --libs gtk+-3.0 libcurl)

APPINDICATOR_LIB ?= /usr/lib/x86_64-linux-gnu/libayatana-appindicator3.so.1

BASE_CFLAGS := -Wall -Wextra -std=c11 -Iinclude -Isrc -Ithird_party $(PKG_CFLAGS)
CFLAGS ?= -O2
ALL_CFLAGS = $(CFLAGS) $(BASE_CFLAGS)
LDFLAGS ?=
LIBS := $(PKG_LIBS) $(APPINDICATOR_LIB) -lm

SRCS := \
	src/main.c \
	src/http.c \
	src/history.c \
	src/settings.c \
	src/weather.c \
	src/icon.c \
	src/ui.c \
	third_party/cJSON.c

OBJS := $(SRCS:.c=.o)

.PHONY: all clean run

all: c-weather

c-weather: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(ALL_CFLAGS) -c -o $@ $<

run: c-weather
	./c-weather

clean:
	rm -f $(OBJS) c-weather
