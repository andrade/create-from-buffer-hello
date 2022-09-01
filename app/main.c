// Copyright 2022 Daniel Andrade
// SPDX-License-Identifier: BSD-3-Clause

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sgx_urts.h>

static int read_from_file(void *dest, size_t cap, size_t *len, const char *path)
{
	FILE *fp = fopen(path, "rb");
	if (!fp)
		return 1;

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	if (size < 0 || (size_t) size > cap) {
		fclose(fp);
		return 1;
	}
	rewind(fp);

	fread(dest, size, 1, fp);
	fclose(fp);

	*len = size;

	return 0;
}

// output: pointer to mapped area and total size
// input: path to enclave.signed.so
static int read_into_mmap(void **base, size_t *len, const char *path)
{
	FILE *fp = fopen(path, "rb");
	if (!fp)
		return 1;

	int fd = fileno(fp);
	struct stat st = {0};
	if (fstat(fd, &st)) {
		fprintf(stderr, "Error: fstat\n");
		fclose(fp);
		return 1;
	}

	*base = mmap(NULL, (size_t) st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (*base == MAP_FAILED) {
		fprintf(stderr, "Error: mmap");
		fclose(fp);
		return 1;
	}

	fclose(fp);

	*len = (size_t) st.st_size;

	return 0;
}

int main(void)
{
	printf("Loads remote enclave using sgx_create_enclave_from_buffer_ex\n");

	// uint8_t enclave[3 * 1024 * 1024] = {0};
	void *base = NULL;
	size_t size = 0;

	// if (read_from_file(enclave, sizeof(enclave), &size, "../lib/enclave.signed.so")) {
	// 	fprintf(stderr, "Error: reading enclave\n");
	// 	return EXIT_FAILURE;
	// }
	if (read_into_mmap(&base, &size, "../lib/enclave.signed.so")) {
		fprintf(stderr, "Error: reading enclave\n");
		return EXIT_FAILURE;
	}

	sgx_enclave_id_t eid;
	sgx_status_t ss = SGX_SUCCESS;

	// ss = sgx_create_enclave_from_buffer_ex(enclave, size, SGX_DEBUG_FLAG, &eid, NULL, 0, NULL);
	ss = sgx_create_enclave_from_buffer_ex(base, size, SGX_DEBUG_FLAG, &eid, NULL, 0, NULL);
	if (SGX_SUCCESS != ss) {
		fprintf(stderr, "sgx_create_enclave_from_buffer_ex: failure (%#x)\n", ss);
		return EXIT_FAILURE;
	}
	fprintf(stdout, "sgx_create_enclave: success\n");


	// ss = ecall_empty(eid);
	// if (SGX_SUCCESS != ss) {
	// 	fprintf(stderr, "ecall_empty: failure (%#x)\n", ss);
	// 	abort();
	// }
	// fprintf(stdout, "ecall_empty: success\n");

	int result = 0;
	ss = ecall_two(eid, &result, 7);
	if (SGX_SUCCESS != ss) {
		fprintf(stderr, "ecall_two: failure (%#x)\n", ss);
		abort();
	}
	fprintf(stdout, "ecall_two: success (%d)\n", result);


	ss = sgx_destroy_enclave(eid);
	if (SGX_SUCCESS != ss) {
		fprintf(stderr, "sgx_destroy_enclave: failure (%#x)\n", ss);
		return EXIT_FAILURE;
	}
	fprintf(stdout, "sgx_destroy_enclave: success\n");
	munmap(base, size);

	return EXIT_SUCCESS;
}
