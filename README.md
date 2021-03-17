## Environment

OS: Ubuntu 18.04

Compiler: gcc

## Installation

Clone this repo and enter the directory
```bash
git submodule init
git submodule update --recursive
sudo apt install libcurl4-openssl-dev
```

## Build

```bash
make
```

## Usage

Run
```bash
./main
```

If the process is successful, that output file will be named 'restored_file'

# Validation

Check the sha256 checksum
```bash
sha256sum restored_file
```

Check memory leak

Install valgrind
```bash
sudo apt install valgrind
```

Check if there is any memory leak
```bash
valgrind  --leak-check=full ./main
```
