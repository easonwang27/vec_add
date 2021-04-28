RISCV_ARCH := rv64imaf
RISCV_ABI := lp64f
RISCV_MCMODEL := medany


TARGET = fw_payload


CFLAGS += -Wall -Os -fomit-frame-pointer
#CFLAGS += -O2
#ASM_SRCS +=
#LDFLAGS +=
#INCLUDES += -I.

C_SRCS := \
	main.c \
	
COMMON_DIR = ..
TOOLCHAIN_DIR = ../../..
include  ./common.mk
