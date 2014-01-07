<!--
  ** File Name:	README.md
  ** Author:	Aditya Ramesh
  ** Date:	07/08/2013
  ** Contact:	_@adityaramesh.com
-->

# Introduction

The `neo` library provides a simple, uniform interface for high-performance,
asynchronous file IO in C++.

# Urgent TODO

- Implement the `buffer` class.
- Move `source` and `sink` from NTL to this library.
- Move the buffer and offset management code into the `source` and `sink`
  classes.
- Move `source_iterator` and `sink_iterator` from NTL to this library.

# Future TOOD

- Create Rakefile.
- Support for scatter reads and writes (`IOCB_CMD_PREADV`, `IOCB_CMD_PWRITEV`).
  Refer to `notes/eventfd-aio-test.c` for details. Scatter write may be useful
  if we want to write from many user buffers to a file.
