CC = gcc

CFLAGS += -Wall -Os -Iinclude -I/usr/local/include/libssh/ -pthread
LDFLAGS += -lz -lcrypto -lpthread

LIBSSH_FILE = /usr/local/lib/libssh.a
CONSTANTS_FILE = include/constants.h

SERVER = ssh-server
CLIENT = ssh-client

SSRCS := server.c \
       config.c

CSRCS := client.c \
       ssh_run.c

SOBJS := $(SSRCS:%.c=obj/%.o)
COBJS := $(CSRCS:%.c=obj/%.o)

all: dirs $(SERVER) $(CLIENT) 

dirs:
	@mkdir -p obj

$(SERVER): $(SOBJS)
	@$(CC) $(SOBJS) $(LIBSSH_FILE) $(LDFLAGS) -o $(SERVER)
	@strip $(SERVER)
	@echo Linking $(SERVER)

$(CLIENT): $(COBJS)
	@$(CC) $(COBJS) $(LIBSSH_FILE) $(LDFLAGS) -o $(CLIENT)
	@strip $(CLIENT)
	@echo Linking $(CLIENT)

$(SOBJS): obj/%.o : src/%.c $(CONSTANTS_FILE)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo Compiling

$(COBJS): obj/%.o : src/%.c $(CONSTANTS_FILE)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo Compiling

force: clean all;

constants.h: ;

clean:
	rm -f $(SOBJS)
	rm -f $(COBJS)
	rm -f $(NAME)
	rm -f $(CLIENT) $(SERVER)
