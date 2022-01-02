#ifndef SSH_RUN
#define SSH_RUN

#define LIBSSH_STATIC 1
#include<libssh/libssh.h>

#define HASH_TYPE SSH_PUBLICKEY_HASH_SHA256

/**
 * Max size of result output
 * Makes sure we don't run commands forever
**/
#define BUFFER_SIZE 2048

/**
 * Check if session is valid.
 * Set HOST, LOG_VERBOSITY, and PORT
 * Exit on fail
**/
void session_init(ssh_session session, char* host, int port, int verbosity, long timeout);

/**
 * Setup channel
**/
int connect_session(ssh_session session, char* host);

/**
 * Create channel
**/
void create_channel(ssh_session session, ssh_channel* channel);

/**
 * Cleanup ssh_session
**/
void cleanup(ssh_session session);

#endif
