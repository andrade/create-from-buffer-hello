// Copyright 2022 Daniel Andrade
// SPDX-License-Identifier: BSD-3-Clause

#include <string.h>

#include <sgx_trts.h>

#include "enclave_t.h"

// void ecall_empty()
// {
// 	// empty, does nothing
// }

int ecall_two(int n)
{
	return n * 2 + 1;
}
