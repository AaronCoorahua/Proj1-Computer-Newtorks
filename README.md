# Echo Server Project

## Author:
**Name:** Aaron Coorahua

## Description:
This is a simple **Echo** server implemented in C++, which accepts incoming TCP connections, receives data blocks of up to 1024 bytes, and echoes them back exactly as received. The server is configured to handle special commands such as `QUIT` and `CLOSE` that allow the client to close the connection or shut down the server.

## Included Files:
- `echo_s.cpp`: Source code for the Echo server.
- `echo_s.h`: Header file for the server implementation.
- `logging.h`: Module for log handling and debug levels.
- `Makefile`: Script for compiling, cleaning, and submitting the project.

## Compilation Instructions:

To compile the project, simply run:

```bash
make
```

This will generate an executable named echo_s.

## Running the Server:

You can run the server using the following syntax:

```bash
./echo_s [-d <debug_level>]
```

## Notes

- The server defaults to port 2024, but if this port is unavailable, it will automatically try the next available port.
- Ensure that the data block size does not exceed 1024 bytes, as this is the configured limit.
