#include "ssh_run.h"
#include <stdio.h>
#include <stdlib.h>

// Function comments are in .h file
// (Sorry!)

void session_init(ssh_session session, char* host, int port, int verbosity, long timeout){
	if (session == NULL)
		exit(-1);
	// Set session options
	ssh_options_set(session, SSH_OPTIONS_HOST, host);
	ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
	ssh_options_set(session, SSH_OPTIONS_PORT, &port);
	ssh_options_set(session, SSH_OPTIONS_TIMEOUT, &timeout);
}

int connect_session(ssh_session session, char* host){
	// Create channel
	int rc = ssh_connect(session);
	if( rc != SSH_OK) {
		fprintf(stderr, "Error connecting to %s: %s\n",
			host,
			ssh_get_error(session)
		);
	}
	return rc;
}

void create_channel(ssh_session session, ssh_channel* channel){
	*channel = ssh_channel_new(session);
	int rc = ssh_channel_open_session(*channel);
	if( rc != SSH_OK){
		fprintf(stderr, "Error creating channel\n");
		ssh_channel_free(*channel);	
		exit(-1);
	}
}

void cleanup(ssh_session session){
	ssh_disconnect(session);
	ssh_free(session);
}
