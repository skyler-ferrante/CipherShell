CC = gcc

CFLAGS += -Wall -Os -Iinclude -I/usr/local/include/libssh/ -pthread
LDFLAGS += -lz -lcrypto -lpthread

LIBSSH_FILE = /usr/local/lib/libssh.a
CONSTANTS_FILE = include/constants.h

SERVER = ssh-server
CLIENT = ssh-client

SSRCS := server.c \
	ssh_methods.c \
	config.c \

CSRCS := client.c \
       ssh_methods.c

SOBJS := $(SSRCS:%.c=obj/%.o)
COBJS := $(CSRCS:%.c=obj/%.o)

all: dirs $(SERVER) $(CLIENT) 

dirs:
	@mkdir -p obj

$(SERVER): $(SOBJS)
	@$(CC) $(SOBJS) $(LIBSSH_FILE) $(LDFLAGS) -o $(SERVER)
	@echo Linking $(SERVER)

$(CLIENT): $(COBJS)
	@$(CC) $(COBJS) $(LIBSSH_FILE) $(LDFLAGS) -o $(CLIENT)
	@echo Linking $(CLIENT)

$(SOBJS): obj/%.o : src/%.c $(CONSTANTS_FILE)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo Compiling

$(COBJS): obj/%.o : src/%.c $(CONSTANTS_FILE)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo Compiling

small: clean $(SERVER) $(CLIENT)
	@strip $(CLIENT) $(SERVER)
	@upx $(CLIENT) $(SERVER)
	@perl -pi -e 's/UPX!/\x0\x0\x0\x0/g' $(CLIENT) $(SERVER)

force: clean all;

constants.h: ;

clean:
	rm -f $(SOBJS)
	rm -f $(COBJS)
	rm -f $(NAME)
	rm -f $(CLIENT) $(SERVER)
