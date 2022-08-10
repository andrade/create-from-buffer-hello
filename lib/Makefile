CC := gcc
EDGER := sgx_edger8r
SIGNER := sgx_sign

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

ENC_CPPFLAGS := \
		-I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc \
		$(CPPFLAGS)
ENC_CFLAGS := \
		-fPIC -Wno-implicit-function-declaration \
		-nostdinc -fvisibility=hidden \
		-fpie -fstack-protector -fno-builtin-printf \
		$(CFLAGS)
ENC_LDFLAGS := \
		-nostdlib -nodefaultlibs -nostartfiles \
		-L$(SGX_SDK)/lib64
ENC_LDLIBS := \
		-Wl,--no-undefined \
		-Wl,--whole-archive -l$(SGX_TRTS) -Wl,--no-whole-archive \
		-Wl,--start-group -lsgx_tstdc -lsgx_tcrypto -l$(SGX_TSERVICE) -Wl,--end-group \
		-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined -Wl,-pie,-eenclave_entry -Wl,--export-dynamic \
		-Wl,--defsym,__ImageBase=0 -Wl,--gc-sections \
		-Wl,--version-script=enclave.lds

all: enclave.signed.so
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
################################ CRYPTO
################################################################

KEY_PR := key.pem

genkey:
	@openssl genrsa -3 -out $(KEY_PR) 3072
	$(call GOOD,,"[ GENKEY ] ",$(KEY_PR))
	$(info [ GENKEY ] Enclave private key is left untouched by clean.)
	$(info [ GENKEY ] Explicitly use delkey to delete it.)

delkey:
	@rm -f $(KEY_PR)
	$(call EVIL,,"[ DELKEY ] ",$(KEY_PR))

$(KEY_PR):
	$(call GOOD,"[ KEY_PR ] Create private key with ","make genkey",)
	$(call EVIL,"[ KEY_PR ] Delete private key with ","make delkey",)

################################################################
################################ TRUSTED CODE
################################################################

enclave_t.c enclave_t.h: enclave.edl
	@ $(EDGER) --trusted $< --search-path $(SGX_SDK)/include
	$(info [  GEN   ] $^  >  $@)

enclave_t.o: enclave_t.c enclave_t.h

enclave.o: enclave.c

SECURE_OBJS := enclave_t.o enclave.o

$(SECURE_OBJS):
	@ $(CC) $(ENC_CPPFLAGS) $(ENC_CFLAGS) -c $< -o $@
	$(info [   CC   ] $<  >  $@)

enclave.so: $(SECURE_OBJS)
	@ gcc $(ENC_CPPFLAGS) $(ENC_CFLAGS) $(ENC_LDFLAGS) $^ -o $@ $(ENC_LDLIBS)
	$(call GOOD,,"[  LINK  ] ","$^  >  $@")

enclave.signed.so: enclave.so enclave.config.xml $(KEY_PR)
	@ $(SIGNER) sign \
		-out $@ \
		-enclave $< \
		-config $(word 2,$^) \
		-key $(KEY_PR)
	$(call GOOD,,"[  SIGN  ] ","$<  >  $@")

################################################################
################################ MAINTENANCE
################################################################

TRASH := \
		enclave_t.c enclave_t.h $(SECURE_OBJS) $(SECURE_LIB) \
		enclave.so enclave.signed.so

clean:
	@ rm -rf $(TRASH)
	$(call EVIL,,"[   RM   ] ",$(TRASH))
