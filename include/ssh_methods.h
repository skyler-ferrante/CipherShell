/**
 * SSH Methods for SSH server and client
 *
 * Methods are implemented in ssh_methods.c
**/
#ifndef SSH_RUN
#define SSH_RUN

#define LIBSSH_STATIC 1
#include<libssh/libssh.h>
#include<libssh/server.h>

/**
 * Max size of result output
 * Makes sure we don't run commands forever
**/
#define BUFFER_SIZE 2048

// Client only methods

/**
 * Check if session is valid.
 * Set HOST, LOG_VERBOSITY, and PORT
 * Exit on fail
**/
void session_init(ssh_session session, char* host, int port, int verbosity, long timeout);

/**
 * Setup channel
**/
int connect_session_client(ssh_session session, char* host);

/**
 * Create channel
**/
void create_channel_client(ssh_session session, ssh_channel* channel);

// Server only methods

/**
 * Handle authentication from server standpoint
**/
int handle_auth_server(ssh_session session);

/**
 * Handle creating channel from server standpoint
**/
ssh_channel create_channel_server(ssh_session session);

// Both server and client

/**
 * Cleanup ssh_session
**/
void cleanup(ssh_session session);

#endif
