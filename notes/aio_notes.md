# Notes on Asynchronous IO

## Design

### Introduction

For the purposes of this library, we are concerned with nonblocking IO that does
not rely on the use of user space threads. The kernel would therefore need to
provide explicit support for asynchronous IO. The streams in this library will
provide a uniform interface that does not change functionally based on whether
blocking or non-blocking IO is used.

### Reading

If asynchronous IO is available and the input file is "large enough", the input
stream will maintain two buffers and swap between them. Initially, both buffers
are sent to the kernel to be populated with consecutive parts of the file. The
programmer interacts with the data by means of a file pointer that is initially
set to the address of the first buffer. When the end of the first buffer is
reached, the file pointer is set to the beginning of the second buffer. The
first buffer is then sent to the kernel to be populated with the next chunk of
the file. This process continues until the file is read. If the buffer size is
chosen correctly, the data processing comes at no additional cost.

Seeking into the file will, by default, cause both internal buffers to be
populated, as described earlier. However, the interface will also accomodate for
the possibility that the user only wishes to read a portion of the file
beginning at an offset. In this case, only the amount of data necessary will be
read from the file.

### Writing

A strategy similar to the one used for sequential read is also used for
sequential writes. As for reads, the interface will accomodate for the
possibility that the programmer wishes to write only a small buffer of data at
as specific offset into the file. In this case, the double-buffering strategy
will not be used.

Good choices of buffer sizes for sequential reads and writes will be determined
by a benchmark conducted using a script that should be invoked as part of the
installation process. The script will issue read or write commands using
increasing buffer sizes, and will flush the operating system's page cache after
each such operation.

## Implementations

There is support for AIO on Windows. On OS X, an implementation of POSIX AIO
exists, but this is accomplished via the use of kernel threads, and could easily
be made more efficient. On Linux, there are two implementations of AIO
available. The first is the user space POSIX AIO implementation provided by
`glib`, and the second is constitued by a set of 5 system calls which were added
to the kernel in version 2.5.

### Linux

On Linux, the `io_submit` system call will block unless direct IO is used via
`O_DIRECT`. This issue is explained [here][io_submit_blocks]. Starting from
version 2.6, the buffer used for direct IO must be aligned to a 512-byte
boundary. This can be accomplished by `posix_memalign`.

## OS X

The `aio` interface for OS X is similar to that of Linux. As explained
[here][aio_on_osx], each `aio_read` or `aio_write` call must be accompanied by
an `aio_return` call. Otherwise, resources will be leaked.

## Windows

Although Windows has native support for asynchronous IO, support for Windows is
not planned, because I do not use the platform for running my experiments.

[linux_aio_explained]: http://www.fsl.cs.sunysb.edu/~vass/linux-aio.txt "Linux
Asynchronous I/O Explained"
[aio_user_guide]: http://code.google.com/p/kernel/wiki/AIOUserGuide "AIO User
Guide"
[io_submit_blocks]:
http://comments.gmane.org/gmane.linux.kernel.aio.general/3024 "io_submit Blocks"
[linux_aio_example]: http://www.xmailserver.org/eventfd-aio-test.c "Linux AIO
Example"
[aio_on_osx]:
http://stackoverflow.com/questions/4665618/aio-read-from-file-error-on-os-x "AIO
on OS X"
