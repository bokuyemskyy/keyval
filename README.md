
# keyval
An in-memory key-value database. Designed as a direct clone of Redis / Valkey. Mainly used as a distributed cache, but can work as a message broker as well.

<p float="left">
  <img src="https://github.com/user-attachments/assets/b09b07f2-91a3-4541-b51a-80bc7cbcb056" width="49%" />
  <img src="https://github.com/user-attachments/assets/7f8f84d0-f4fe-4411-8ca3-2191a0c89bd5" width="49%" />
</p>

## Overview
Utilizes Linux-specific libraries and calls. Asynchronous server is built on `epoll`, using system `socket` calls to establish TCP connections with clients effectively. Fully Redis RESP-protocol compliant, interoperates seamlessly with standard clients such as `valkey-cli`.

## Features
The table of supported commands
| Category        | Commands                                      |
|-----------------|-----------------------------------------------|
| Server          | PING, INFO, SHUTDOWN, ECHO                   |
| Key Management  | EXISTS, DEL, TYPE, KEYS, EXPIRE, TTL, RENAME, SELECT, FLUSHDB, FLUSHALL |
| String          | SET, GET, INCR, DECR, APPEND                 |
| List            | LPUSH, RPUSH, LPOP, RPOP, LRANGE, LLEN      |
| Set             | SADD, SREM, SMEMBERS, SISMEMBER, SCARD      |

## Tech Stack
- Linux API
- Catch2
- CMake

## Setup

```bash
# clone repository recursively (including third-party libraries)
git clone --recurse-submodules https://github.com/bokuyemskyy/keyval
cd keyval

# generate the build files to build/
cmake -B build

# build the project
cmake --build build
````

## Usage
```
# WIP; the client is not ready yet.
# However, you can run ./build/server/server [configuration arguments]

# Then, connect to the server via valkey client
valkey-cli -p 6380

# Or, via socat or other tool
socat - TCP:0.0.0.0:6380

# You can use the database as usual
```

## Credits

* [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
* [Valkey project](https://github.com/valkey-io/valkey)
