# ASP-Project: Client Server File Transfer with Load Balancer 

This GitHub project is a client-server file transfer system developed as part of a master's program in Advanced System Programming. The system allows clients to request files or sets of files from a server, which searches for the requested files and sends them to the client in a compressed format (tar.gz). 

## Team Members and Contributions:
- Apurvakumar Patel:
  - Implemented server.c
  - Worked on mirror.c
- Sagar Vivek Pandya:
  - Developed client.c

## Language and Operating System:
- Language: C++
- Supported Operating Systems: Any Linux distribution or Windows Subsystem for Linux (WSL)

## Project Overview:
In this client-server system, multiple clients can connect to the server from different machines and request files using specific commands defined for the project. The server, mirror, and client processes must run on separate machines and communicate using sockets.

### Section 1: Server
- The server and its mirror counterpart both need to be running before any clients connect.
- Upon receiving a connection request from a client, the server forks a child process to handle the client's request.
- The child process, called `processclient()`, waits for client commands, processes them, and returns results to the client.
- When a client sends a "quit" command, the `processclient()` function exits.
- For each client request, the server forks a separate process to handle the request and continues listening for other clients.

### Section 2: Client
- The client runs in an infinite loop, waiting for user commands.
- User commands are specific to this project and are not standard Linux commands.
- The client verifies the syntax of entered commands. If the syntax is correct, it sends the command to the server; otherwise, it displays an error message.
- List of Client Commands:
  - `fgets file1 file2 file3 file4`: Requests files from the server.
  - `tarfgetz size1 size2 <-u>`: Requests files based on their size.
  - `filesrch filename`: Searches for a specific file and retrieves information if found.
  - `targzf <extension list> <-u>`: Requests files based on file extensions.
  - `getdirf date1 date2 <-u>`: Requests files created within a specified date range.
  - `quit`: Terminates the client process.

### Section 3: Alternating Between Server and Mirror
- The server and its mirror counterpart run on different machines/terminals.
- The first six client connections are handled by the server, and the next six are handled by the mirror.
- Subsequent client connections are alternated between the server and the mirror.

Please refer to the project's source code  for more details on implementation and usage.
