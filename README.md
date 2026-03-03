# keyval
[![CI](https://github.com/bokuyemskyy/keyval/actions/workflows/ci.yml/badge.svg)](https://github.com/bokuyemskyy/keyval/actions/workflows/ci.yml)

![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black) ![Windows](https://custom-icon-badges.demolab.com/badge/Windows-0078D6?logo=windows11&logoColor=white) ![macOS](https://img.shields.io/badge/macOS-000000?logo=apple&logoColor=F0F0F0)

## Overview
An in-memory key-value database built as a simplified clone of Redis / Valkey. Designed primarily as a standalone cache, but also supports message brokering via queue primitives. Networking and I/O polling are handled by a custom cross-platform library developed alongside this project.

## Screenshots
<p float="left">
  <img src="https://github.com/user-attachments/assets/6147bfe7-baa4-465e-959e-162d1b0b6dfa" width="49%" />
  <img src="https://github.com/user-attachments/assets/a2500c32-b597-4ff3-b72e-eee93af5ef6c" width="49%" />
</p>

## Features
Asynchronous server is event-driven, built on non-blocking poll with system-level socket calls for TCP connection handling. The server is RESP-compliant and interoperates with standard Redis/Valkey clients such as `redis-cli` and `valkey-cli` out of the box.

Supported commands
| Category        | Commands                                      |
|-----------------|-----------------------------------------------|
| Server          | PING, INFO, SHUTDOWN, ECHO                   |
| Key Management  | EXISTS, DEL, TYPE, KEYS, EXPIRE, TTL, RENAME, SELECT, FLUSHDB, FLUSHALL |
| String          | SET, GET, INCR, DECR, APPEND                 |
| List            | LPUSH, RPUSH, LPOP, RPOP, LRANGE, LLEN      |
| Set             | SADD, SREM, SMEMBERS, SISMEMBER, SCARD      |

## Technical stack
- C++23
- Catch2
- CMake 3.20

## Usage
**1. Clone the repository**

```
git clone https://github.com/bokuyemskyy/keyval
```

**2. Generate the build files to `build/`**

```
cmake -B build
```
Use `-DBUILD_TESTING ON` to enable tests.

**3. Compile**

```
cmake --build build
````

**4. Run **

Server is available at:
```
./build/server/keyval-server [-h <host>] [-p <port>]
```
Connect via the bundled CLI client:
```
./build/client/keyval-cli [-h <host>] [-p <port>]
```
Or via `valkey-cli`, `socat` or other tool
```bash
socat - TCP:0.0.0.0:6380
```

## References
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Valkey project](https://github.com/valkey-io/valkey)
