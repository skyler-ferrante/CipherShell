#ifndef CONSTANTS_H
#define CONSTANTS_H

// Documentation says to do this
// Not sure why it's neccessary
// Seems to work fine without it
#define LIBSSH_STATIC 1

/**
 * Constants.h
 *
 * Contains constants that should be embedded into the binary
 * That way, we don't need a config file to be sent with the client
**/

//For client
#define CLIENT_TIMEOUT 5

//For server
#define KEYS_FOLDER "keys/"
#define CONFIG_DIR "config/"
#define LOG_LEVEL SSH_LOG_NOLOG
#define SERVER_TIMEOUT 2

//For both
#define BUFFER_SIZE 2048
#define PORT 22
#define HOST "127.0.0.1"
#define USERNAME "bob"

#endif
