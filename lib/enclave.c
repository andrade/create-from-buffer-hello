// Copyright 2022 Daniel Andrade
// SPDX-License-Identifier: BSD-3-Clause

#include <string.h>

#include <sgx_trts.h>

#include "enclave_t.h"

void ecall_empty()
{
	// empty, does nothing
}

int ecall_two(int n)
{
	return n * 2 + 1;
}

int ecall_rand()
{
	int result = 0;

	if (sgx_read_rand(&result, sizeof(result))) {
		return -1;
	}

	ocall_print_1(result);

	return 0;
}
