CC = gcc

CFLAGS += -Wall -O2 -Iinclude -I/usr/local/include/libssh/ -pthread
LDFLAGS += -lssh \
	   -lrt \
	   -lcrypto \
	   -lz \
	   -lpthread \
	   -ldl \

LIBSSH_FILE = /usr/local/lib/libssh.a

SERVER = ssh-server
CLIENT = ssh-client

SSRCS := server.c \
       config.c \
       ssh_run.c

CSRCS := client.c \
       ssh_run.c

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

$(SOBJS): obj/%.o : src/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo Compiling

$(COBJS): obj/%.o : src/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo Compiling

clean:
	rm -f $(SOBJS)
	rm -f $(COBJS)
	rm -f $(NAME)
	rm -f $(CLIENT) $(SERVER)
