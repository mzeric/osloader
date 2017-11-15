PGM=bin
#SOURCES :=$(wildcard *.c) $(wildcard *.S)  $(wildcard *.cc)
SOURCES := start.s init.c i386.c lib.c elf.c
OBJS := $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(SOURCES)))
#OBJS = main.o init.o
LDFLAGS += -n
CFLAGS += -g -nostdlib -nostartfiles -fms-extensions
ASFLAGS += -m32

TOP_DIR := $(shell pwd)

C_PIECES=a b
C_FILES=$(C_PIECES:%=%.S)

HEADER_DIR += include
TARGET_ARCH = -m32

T :=a
all-main: prepare df all

all: $(OBJS)
	#$(RM) $@ $^
	# $(TOP_DIR)
	$(info $(T) $(origin T))
	$(LD) $(LDFLAGS) -o $(PGM) $(OBJS) -Tld.script

include rule.mk
