CC = gcc

CFLAGS += -Wall -O2 -Iinclude
LDFLAGS += -lssh

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
	@$(CC) $(SOBJS) $(LDFLAGS) -o $(SERVER)
	@echo Linking $(SERVER)

$(CLIENT): $(COBJS)
	@$(CC) $(COBJS) $(LDFLAGS) -o $(CLIENT)
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
