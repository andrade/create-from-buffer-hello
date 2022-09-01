# Copyright 2022 Daniel Andrade
# SPDX-License-Identifier: BSD-3-Clause

CC := gcc
EDGER := sgx_edger8r
SIGNER := sgx_sign
PROG := app_ex

SGX_SDK ?= $(HOME)/.local/lib/x86_64-linux-gnu/sgxsdk
SGX_MODE ?= SIM
SGX_DEBUG ?= 1

$(info [SGX_SDK  ] "$(SGX_SDK)")
$(info [SGX_MODE ] "$(SGX_MODE)")
$(info [SGX_DEBUG] "$(SGX_DEBUG)")

SGX_SDK ?=
ifndef SGX_SDK
$(error SGX_SDK is not set)
endif

$(info ------------------------------------------------------------------------)

ifeq ($(SGX_MODE),HW)
	SGX_TRTS := sgx_trts
	SGX_TSERVICE := sgx_tservice
	SGX_UAE_SERVICE := sgx_uae_service
	SGX_URTS := sgx_urts
else
	SGX_TRTS := sgx_trts_sim
	SGX_TSERVICE := sgx_tservice_sim
	SGX_UAE_SERVICE := sgx_uae_service_sim
	SGX_URTS := sgx_urts_sim
endif

CPPFLAGS ?=
CPPFLAGS += -I.

CFLAGS ?=
CFLAGS += -m64 -std=c11 -Wall -Wextra

ifeq ($(SGX_DEBUG),1)
	CFLAGS += -O0 -g
	CFLAGS += -DDEBUG
else
	CFLAGS += -O2
	CFLAGS += -UDEBUG
endif

APP_CPPFLAGS := -I$(SGX_SDK)/include $(CPPFLAGS)
#APP_CFLAGS := -fPIC -Wno-attributes $(CFLAGS)
APP_CFLAGS := -rdynamic -fPIC -Wno-attributes $(CFLAGS)
APP_LDFLAGS := -L$(SGX_SDK)/lib64
#APP_LDLIBS := -l$(SGX_URTS) -l$(SGX_UAE_SERVICE)
APP_LDLIBS := -l$(SGX_URTS) -l$(SGX_UAE_SERVICE) -Wl,--unresolved-symbols=ignore-in-object-files

all: $(PROG)
	$(call GOOD,,"[  MAKE  ] ","$(notdir $^)")

################################################################
################################ CONFIGURATION
################################################################

.PHONY: clean

define GOOD
	@echo -n $1
	@tput setaf 6
	@echo -n $2
	@tput sgr0
	@echo $3
endef

define EVIL
	@echo -n $1
	@tput setaf 3
	@echo -n $2
	@tput sgr0
	@echo $3
endef

################################################################
################################ UNTRUSTED CODE
################################################################

main.o: main.c

NORMAL_OBJS := main.o

$(NORMAL_OBJS):
	@ $(CC) $(APP_CPPFLAGS) $(APP_CFLAGS) -c $< -o $@
	$(info [   CC   ] $<  >  $@)

$(PROG): $(NORMAL_OBJS)
	@ $(CC) $(APP_CPPFLAGS) $(APP_CFLAGS) $(APP_LDFLAGS) -o $@ $^ $(APP_LDLIBS)
	$(call GOOD,,"[  LINK  ] ","$^  >  $@")

################################################################
################################ MAINTENANCE
################################################################

TRASH := $(NORMAL_OBJS) $(PROG)

clean:
	@ rm -rf $(TRASH)
	$(call EVIL,,"[   RM   ] ",$(TRASH))
