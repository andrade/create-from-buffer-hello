// Copyright 2022 Daniel Andrade
// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>

#include <sgx_urts.h>

#include "enclave_u.h"

void ocall_print_1(int n)
{
	fprintf(stdout, "print_1, n=%d\n", n);
}
