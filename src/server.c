// Using samplesshd as a reference 
// https://github.com/substack/libssh/blob/master/examples/samplesshd.c 

#include <stdlib.h>
#include <stdio.h>
#include <libssh/libssh.h>
#include <libssh/server.h>
#include <pthread.h>

#include "ssh_methods.h"
#include "config.h"
#include "constants.h"

struct handle{
	ssh_bind sshbind;
	ssh_session session;
	struct node *commands;
};

// Handle creating connection to client, and sending commands.
void* handle_client(void* d);

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
	ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, KEYS_FOLDER "sample_key");	
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
	int auth = handle_auth_server(session);
	if(!auth){
		fprintf(stderr, "auth error: %s\n", ssh_get_error(session));
		ssh_disconnect(session);
		pthread_exit(NULL);
	}
	printf("auth success!\n");

	// Create SSH-channel
	ssh_channel channel = create_channel_server(session);
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

		// Read data from client
	
		while(1){
			i = ssh_channel_read(channel, buffer, BUFFER_SIZE, 0);
			if( i>0 ){
				// Check to see if sentinel is at end
				if( buffer[i-1] == '\xFF' ){
					if(!write(1,buffer,i-2)){
						fprintf(stderr, "Write failed");
						pthread_exit(NULL);
					}
					printf("\n\n");
					break;
				}
				// Write data to stdout
				if(!write(1, buffer, i-1)){
					fprintf(stderr, "Write failed (2)");
				}
			}else{
				// Client sent no data, when we were expecting data
				fprintf(stderr, "Shells dead\n");
				pthread_exit(NULL);
			}
		}
	}

	pthread_exit(NULL);
}
