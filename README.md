## Environment

OS: Ubuntu 18.04

Compiler: gcc

## Installation

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

```bash
./main
```
If the process is successful, that output file will be named 'restored_file'

Check the sha256 checksum
```bash
sha256sum restored_file
```