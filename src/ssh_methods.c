#include <stdlib.h>
#include <stdio.h>
#include "ssh_methods.h"

// Function comments are in .h file

void session_init(ssh_session session, char* host, int port, int verbosity, long timeout){
	if (session == NULL)
		exit(-1);
	// Set session options
	ssh_options_set(session, SSH_OPTIONS_HOST, host);
	ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
	ssh_options_set(session, SSH_OPTIONS_PORT, &port);
	ssh_options_set(session, SSH_OPTIONS_TIMEOUT, &timeout);
}

int connect_session_client(ssh_session session, char* host){
	int rc = ssh_connect(session);
	if( rc != SSH_OK) {
		fprintf(stderr, "Error connecting to %s: %s\n",
			host,
			ssh_get_error(session)
		);
	}
	return rc;
}

void create_channel_client(ssh_session session, ssh_channel* channel){
	*channel = ssh_channel_new(session);
	int rc = ssh_channel_open_session(*channel);
	if( rc != SSH_OK){
		fprintf(stderr, "Error creating channel\n");
		ssh_channel_free(*channel);	
		exit(-1);
	}
}

int handle_auth_server(ssh_session session){
	ssh_message message;
	int auth = 0;

	// Do while not auth (break on null message)
	do{
		message = ssh_message_get(session);
		if(!message)
			break;
		
		switch( ssh_message_type(message) ){
			case SSH_REQUEST_AUTH:
				switch( ssh_message_subtype(message) ){
					// Not used right now
					// Could be used to force a password
					case SSH_AUTH_METHOD_PASSWORD:
						printf("User %s wants to auth with password %s\n",
							ssh_message_auth_user(message),
							ssh_message_auth_password(message));

						auth = 1;
						ssh_message_auth_reply_success(message, 0);
						break;
					
					case SSH_AUTH_METHOD_NONE:
						printf("No auth\n");
						ssh_message_auth_reply_success(message, 0);
						auth = 1;
						//ssh_message_auth_set_methods(message, SSH_AUTH_METHOD_PASSWORD);
						//ssh_message_reply_default(message);
						break;
				}
				break;
			
			default:
				ssh_message_reply_default(message);
				break;
		}
		
		ssh_message_free(message);
	}
	while(!auth);
	return auth;
}

ssh_channel create_channel_server(ssh_session session){
	ssh_channel channel = 0;
	ssh_message message;

	do{
		message = ssh_message_get(session);
		
		if(message){
			switch( ssh_message_type(message) ){	
				case SSH_REQUEST_CHANNEL_OPEN:
					if( ssh_message_subtype(message) == SSH_CHANNEL_SESSION ){
						channel=ssh_message_channel_request_open_reply_accept(message);
						break;
					}
				
				default:
					ssh_message_reply_default(message);

			}
			ssh_message_free(message);
		}
	}
	while( message && !channel );
	return channel;
}

void cleanup(ssh_session session){
	ssh_disconnect(session);
	ssh_free(session);
}
