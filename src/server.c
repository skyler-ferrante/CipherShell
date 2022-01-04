// Using samplesshd as a reference 
// https://github.com/substack/libssh/blob/master/examples/samplesshd.c 

#include <stdlib.h>
#include <stdio.h>
#include <libssh/libssh.h>
#include <libssh/server.h>
#include <pthread.h>

#include "config.h"
#include "constants.h"

struct handle{
	ssh_bind sshbind;
	ssh_session session;
	struct node *commands;
};

void* handle_client(void* d);
int handle_auth(ssh_session session);
ssh_channel create_channel(ssh_session session);

int main(int argc, char** argv){
	ssh_session session;
	ssh_bind sshbind;

	int rc;
	char* commands_filename;
	struct node* commands;
	int port = PORT;

	if(argc == 1){
		commands_filename = CONFIG_DIR "commands";
	}else if(argc == 2){
		commands_filename = argv[1];
	}else{
		fprintf(stderr, "Usage: %s commands_filename\n", argv[0]);
		fprintf(stderr, "Usage: %s #If commands_filename in " CONFIG_DIR "\n", argv[0]);
		exit(-1);
	}
	
	sshbind = ssh_bind_new();
	ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, KEYS_FOLDER "ssh_host_rsa_key");	
	ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &port);

	if( ssh_bind_listen(sshbind) < 0 ){
		fprintf(stderr, "Error listening to socket: %s\n", ssh_get_error(sshbind));
		exit(-1);
	}
	printf("Listening on port %d\n", port);

	// Needed for threading (only if static)
	ssh_init();

	//Don't actually need thread id
	pthread_t ptid;

	//Main server loop
	while( 1 ){
		session = ssh_new();

		rc = ssh_bind_accept(sshbind, session);
		if( rc == SSH_ERROR ){
			fprintf(stderr, "Error accepting a connection: %s\n", ssh_get_error(sshbind));
			exit(-1);
		}
		
		// Reread commands in case of file is changed
		commands = get_lines(commands_filename);
		
		// Data needed to handle client
		struct handle *h = malloc(sizeof (struct handle));
		h->sshbind = sshbind;
		h->session = session;
		h->commands = commands;

		printf("Starting new thread\n");
		pthread_create(&ptid, NULL, handle_client, (void*)h);
	}

	ssh_disconnect(session);
	ssh_bind_free(sshbind);

	ssh_finalize();
}

void* handle_client(void* data){
	struct handle *h = data;

	// Get function args
	ssh_bind sshbind = h->sshbind;
	ssh_session session = h->session;
	struct node *commands = h->commands;
	
	// Handle struct no longer needed	
	free(h);
	
	// Key exchange
	if( ssh_handle_key_exchange(session) ){
		fprintf(stderr, "ssh_handle_key_exchange: %s\n", ssh_get_error(sshbind));
		pthread_exit(NULL);
	}

	// Handle auth (none)
	int auth = handle_auth(session);
	if(!auth){
		fprintf(stderr, "auth error: %s\n", ssh_get_error(session));
		ssh_disconnect(session);
		pthread_exit(NULL);
	}
	printf("auth success!\n");

	// Create SSH-channel
	ssh_channel channel = create_channel(session);
	if( !channel ){
		fprintf(stderr, "error: %s\n", ssh_get_error(session));
		pthread_exit(NULL);
	}
	printf("Channel success!\n");

	// C is a node in a linkedlist of commands (c->line is current command)
	struct node* c;
	// i is the amount of bytes read from client
	int i;
	// buffer to hold data recv from client
	char buffer[BUFFER_SIZE];

	for(c = commands; c; c = c->next){
		printf("Sending: %s\n", c->line);	
		ssh_channel_write(channel, c->line, strlen(c->line));
	
		// This won't work if the command takes longer than SERVER_TIMEOUT seconds	
		// TODO: find a cleaner solution
		sleep( SERVER_TIMEOUT );

		// Read data from client
		i = ssh_channel_read(channel, buffer, BUFFER_SIZE, 0);
		if( i>0 ){
			// Write data to stdout
			if( write(1, buffer, i) < 0){
				fprintf(stderr, "Error writing output\n");
				pthread_exit(NULL);
			}
		}else{
			// Client sent no data, when we were expecting data
			fprintf(stderr, "Shells dead\n");
			pthread_exit(NULL);
		}
	}

	pthread_exit(NULL);
}

int handle_auth(ssh_session session){
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

ssh_channel create_channel(ssh_session session){
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
