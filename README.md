# Reverse-SSH-Shell
ssh-server sends commands for ssh-client to execute

ssh-server can handle multiple connections at once

## Build Libssh statically:
  ```
  git clone https://git.libssh.org/projects/libssh.git libssh
  mkdir build
  cd build/
  cmake ../ -DWITH_EXAMPLES=OFF -DBUILD_SHARED_LIBS=OFF -DWITH_STATIC_LIB=ON
  make
  make install
  ```

## Build Reverse-SSH-Shell:
  ```
  make
  ```

## Start server:
  ```
  Usage: ./ssh-server commands_filename
  Usage: ./ssh-server #If commands_filename in config/
  ```

## Run client:
  ```
  Usage: ./ssh-client
  ```
  
## Configuration
  Lines starting with C in the commands file will be treated as shell commands (e.g. Cecho hi)

  Lines starting with R will cause the client to send the given filename to the server (e.g. R/etc/passwd)

  To change port number for server and client, edit include/constants.h
