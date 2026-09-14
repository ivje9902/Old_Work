# The refmem library
**refmem** is a library that offers a reference counting memory allocation and de-allocation. Supported and tested operating systems are Linux only.

## Usage

### Dependencies
- GNU Make
- GNU C compiler
- GNU Gcov
- Valgrind
- CUnit

### Makefile

> [!NOTE]
> The makefile contains the aliases `memtest -> test-valgrind` and `demo -> demo-webstore` to comply with the assignment instructions.

- The build directory for object files is [`build/`](build).
- The build directory for demo object files is [`build/demo/`](build/demo).
- The build directory for executable binaries is [`build/bin/`](build/bin).

#### To build the library object files
```shell
make
```

#### To build and run all unit tests
```shell
make test
```

#### To build and run all unit tests with Valgrind
```shell
make test-valgrind
```

#### To run unit tests coverage check with Gcov
```shell
make test-coverage
```

#### To build and run webstore demo
```shell
make demo-webstore
```

#### To indent code according to Apache C Code style guidelines
```shell
make indent
```

#### To remove directories created by makefile
```shell
make clean
```


#### To build and run typemem unit tests
```shell
make test-typemem
```

#### To build and run default destructors demo
```shell
make demo-default-destructors
```

#### To build and run webstore demo internal unit tests
```shell
make test-demo-webstore
```


#### To build and run typemem unit tests with Valgrind
```shell
make test-typemem-valgrind
```

#### To build and run webstore demo with Valgrind
```shell
make demo-webstore-valgrind
```

#### To build and run default destructors demo with Valgrind
```shell
make demo-default-destructors-valgrind
```

#### To build and run default webstore demo unit tests with Valgrind
```shell
make test-demo-webstore-valgrind
```
