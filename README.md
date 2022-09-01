# Example loading an enclave from a buffer

1. The library in `lib` creates an `enclave.signed.so` and an `untrusted.so`; and
2. The application in `app` creates an enclave from the trusted DSO using `sgx_create_enclave_from_buffer_ex` and loads the untrusted DSO using `dlopen`

The application then invokes multiple ecalls.


## Compilation

First compile library:

```
.../lib$ make genkey
.../lib$ make SGX_MODE=HW
```

Then compile application and execute:

```
.../app$ make SGX_MODE=HW
.../app$ ./app_ex
```

Clean with:

```
.../lib$ make clean
.../lib$ make delkey
.../app$ make clean
```

NOTE: The private key life cycle is independent of the rest of the code. Generate the private key with `$ make genkey` and remove it with `$ make delkey`. These targets uses OpenSSL.


# License

BSD-3-Clause

Copyright 2022 Daniel Andrade.
