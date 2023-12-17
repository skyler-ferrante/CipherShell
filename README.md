# CipherShell
ssh-server sends commands for ssh-client to execute

ssh-server can handle multiple connections at once

## Build Libssh statically:
  ```
  sudo apt-get install libssl-dev
  git clone https://git.libssh.org/projects/libssh.git libssh
  cd libssh
  mkdir build
  cd build/
  cmake ../ -DWITH_EXAMPLES=OFF -DBUILD_SHARED_LIBS=OFF -DWITH_STATIC_LIB=ON
  make
  sudo make install
  ```

## Build Reverse-SSH-Shell:
  ```
  make
  make small #Strip and UPX pack 
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
  Lines starting with `#` in the commands file will be treated as shell commands (e.g. `#echo hi`)

  Lines starting with `G` will cause the client to send the given filename to the server (e.g. `G/etc/passwd`)

  To change port number for server and client, edit include/constants.h
