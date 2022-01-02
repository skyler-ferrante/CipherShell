# Reverse-SSH-Shell
SSH Server sends commands for client to execute

`Usage: ./ssh-server commands_filename`

`Usage: ./ssh-server #If commands_filename in config/`

Lines starting with C in the commands file will be treated as shell commands (e.g. Cecho hi)

Lines starting with R will cause the client to send the given filename to the server (e.g. R/etc/passwd)
