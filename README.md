# keyval

![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)

## Overview
This project is an in-memory key-value database. Designed as a direct clone of Redis / Valkey. Main purpose is a distributed cache, but can work as a message broker as well.

## Screenshots
<p float="left">
  <img src="https://github.com/user-attachments/assets/b09b07f2-91a3-4541-b51a-80bc7cbcb056" width="49%" />
  <img src="https://github.com/user-attachments/assets/7f8f84d0-f4fe-4411-8ca3-2191a0c89bd5" width="49%" />
</p>

## Features
Utilizes Linux-specific libraries and calls. Asynchronous server is built on `epoll`, using system `socket` calls to establish TCP connections with clients effectively. Fully Redis RESP-protocol compliant, interoperates seamlessly with standard clients such as `valkey-cli`.

The table of supported commands
| Category        | Commands                                      |
|-----------------|-----------------------------------------------|
| Server          | PING, INFO, SHUTDOWN, ECHO                   |
| Key Management  | EXISTS, DEL, TYPE, KEYS, EXPIRE, TTL, RENAME, SELECT, FLUSHDB, FLUSHALL |
| String          | SET, GET, INCR, DECR, APPEND                 |
| List            | LPUSH, RPUSH, LPOP, RPOP, LRANGE, LLEN      |
| Set             | SADD, SREM, SMEMBERS, SISMEMBER, SCARD      |

## Technical stack
- Linux API
- Catch2
- CMake

## Usage
### Clone the repository
Clone including the Catch2 submodule:
```
git clone --recurse-submodules https://github.com/bokuyemskyy/keyval
```
Or clone and update submodules manually:
```
git clone https://github.com/bokuyemskyy/keyval
cd keyval
git submodule update --init --recursive
```

### Compilation
Generate the build files to `build/`

```bash
cmake -B build
```

Build the project 
```bash
cmake --build build
````

### Run
Server is available with:
```
./build/server/server [configuration arguments]
```
WIP: the client is still developed
Connect to the server via valkey client
```
valkey-cli -p 6380
```
Or via `socat` or other tool
```bash
socat - TCP:0.0.0.0:6380
```

## Credits
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Valkey project](https://github.com/valkey-io/valkey)
