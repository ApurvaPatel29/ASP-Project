//Section 3- 110096127 - Sagar Vivek Pandya
//Section 3 - 110095571 - Apurvakumar Patel


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#define PORT 8888  // Defining the port number to be used by main server.
#define SAP_fILE_SIGNAL_Prjc "SENDING_FILE"  // this Signal is  string to indicate the beginning of the file transfer.

// here i am decalring Global variable to count the number of connections.
int *SAP_connection_countL_Prjc;

// Function used to send a file to a client.
void SAP_send_file_to_client_Prjc(int SAP_client_sock_Prjc, char *SAP_filename_Prjc) {
    char SAP_buffer_Prjc[2048];  // this Buffer is to hold chunks of file data.
    
    // Opening the specified file in read mode.
    FILE *SAP_fp_Prjc = fopen(SAP_filename_Prjc, "r");
    if (SAP_fp_Prjc == NULL) {
        perror("SAP:Cannot open file to send");  // If there's no file found,then show error and return.
        return;
    }

    // Getting the size of the file.
    fseek(SAP_fp_Prjc, 0, SEEK_END);
    int SAP_file_size_Prjc = ftell(SAP_fp_Prjc);
    fseek(SAP_fp_Prjc, 0, SEEK_SET);

    // Sending the "SENDING_FILE" signal to client to indicate that the start of file transfer.
    if(send(SAP_client_sock_Prjc, SAP_fILE_SIGNAL_Prjc, strlen(SAP_fILE_SIGNAL_Prjc), 0) < 0) {
        perror("SAP:Send signal failed");  // If the  sending signal failed, then show error.
        fclose(SAP_fp_Prjc);
        return;
    }

    // Sending the size of the file to client.
    if(send(SAP_client_sock_Prjc, &SAP_file_size_Prjc, sizeof(SAP_file_size_Prjc), 0) < 0) {
        perror("SAP:Send file size failed");  // If sending file size failed, show error.
        fclose(SAP_fp_Prjc);
        return;
    }

    // program remain in Loop until the end of file and send chunks of data to the client.
    while (!feof(SAP_fp_Prjc)) {
        size_t SAP_read_size_Prjc = fread(SAP_buffer_Prjc, 1, sizeof(SAP_buffer_Prjc), SAP_fp_Prjc);
        if (SAP_read_size_Prjc > 0) {
            if(send(SAP_client_sock_Prjc, SAP_buffer_Prjc, SAP_read_size_Prjc, 0) < 0) {
                perror("SAP:Send failed");  // If sending a chunk of data is  failed, show error.
                break;
            }
        }
    }

    fclose(SAP_fp_Prjc);  // Closing the file using fclose().
}


// this Function is to compress specified files and send the compressed archive to the client.
void SAP_handle_fgets_command_Prjc(int SAP_client_sock_Prjc, char *SAP_Files_Prjc[]) {
    char SAP_command_Prjc[512] = "tar czf temp.tar.gz ";  // Initializing the command to create a compressed tar archive.
    int SAP_found_Prjc = 0;  // this Flag is to check if any valid files are found or not.

    //  program remain in Loop through the provided files.
    for (int i = 0; i < 4 && SAP_Files_Prjc[i] != NULL; i++) {
        // Check if the file exists.
        if (access(SAP_Files_Prjc[i], F_OK) != -1) {
            strcat(SAP_command_Prjc, SAP_Files_Prjc[i]);  // Appending the  file name to the tar command.
            strcat(SAP_command_Prjc, " ");  //then  Adding  space for the next file.
            SAP_found_Prjc = 1;  // atlast setting the found flag to true.
        }
    }

    // If at least one valid file was found then if condition will be true
    if (SAP_found_Prjc) {
        system(SAP_command_Prjc);  // Executing the tar command to create the compressed archive.
        SAP_send_file_to_client_Prjc(SAP_client_sock_Prjc, "temp.tar.gz");  // Sending the created archive to the client.
        remove("temp.tar.gz");  // then deleting the temporary archive from the server side.
    } else {
        //however ff no valid files are found, then this will send a message to the client.
        send(SAP_client_sock_Prjc, " SAP: No file SAP_found_Prjc", 15, 0);
    }
}


// This function is use for searching the  files within a certain size range in the home directory and 
//then compresses them into a tar.gz archive. It then sends this archive to a client.

// Function to search and compress files based on size range and send the compressed archive to the client.
void SAP_handle_tarfgetz_command_Prjc(int SAP_client_sock_Prjc, int SAP_size1_Prjc, int SAP_size2_Prjc, int SAP_should_unzip_Prjc) {
    char SAP_command_Prjc[512];
    
    // Removing any existing archive to ensure a clean operation.
    remove("temp.tar.gz");

    // Constructing the find command with size constraints.
    sprintf(SAP_command_Prjc, "find ~ -type f -size +%dc -size -%dc -print0 | xargs -0 tar czf temp.tar.gz", SAP_size1_Prjc, SAP_size2_Prjc);
    int SAP_status_Prjc = system(SAP_command_Prjc);  //this will  executing the find and compress command.

    // here i am checking if the command was successful.
    if (SAP_status_Prjc != 0) {
        send(SAP_client_sock_Prjc, "SAP:Command execution failed", 25, 0);
        return;
    }
    // here i am checking if the tar archive was created.
    if (access("temp.tar.gz", F_OK) != -1) {
        SAP_send_file_to_client_Prjc(SAP_client_sock_Prjc, "temp.tar.gz");  // Send the archive to the client.
        remove("temp.tar.gz");  // Delete the temporary archive from the server side.
    } else {
        // however if the archive was not created,then this will send a message to the client.
        send(SAP_client_sock_Prjc, "SAP:No file SAP_found_Prjc", 15, 0);
    }
}


// This function is used for  searching a specified file by name in the home directory, retrieves its properties,
// and sends these properties to a client.
void SAP_handle_filesrch_command_Prjc(int SAP_client_sock_Prjc, char *SAP_filename_Prjc) {
    char SAP_command_Prjc[512];
    // Constructing the find command based on the file name.
    sprintf(SAP_command_Prjc, "find ~ -type f -name \"%s\"", SAP_filename_Prjc);

    FILE *SAP_pipe_Prjc = popen(SAP_command_Prjc, "r");  // Executing the find command and get the output.
    if (!SAP_pipe_Prjc) return;  // Returning if the command fails.

    char SAP_result_Prjc[1024];
    // Reading the output of the find command.
    if (fgets(SAP_result_Prjc, sizeof(SAP_result_Prjc), SAP_pipe_Prjc) != NULL) {
        struct stat file_stat;
        stat(SAP_result_Prjc, &file_stat);  // Getting the properties of the found file.
        char SAP_response_Prjc[1024];
        // Constructing the response containing file name, size, and creation date.
        sprintf(SAP_response_Prjc, "Name: %s Size: %ld bytes Date: %s",
                SAP_result_Prjc, file_stat.st_size, ctime(&file_stat.st_ctime));
        send(SAP_client_sock_Prjc, SAP_response_Prjc, strlen(SAP_response_Prjc), 0);  // Sending the response to the client.
    } else {
        // If the file is not found, send a message to the client.
        send(SAP_client_sock_Prjc, "SAP:File not SAP_found_Prjc", 15, 0);
    }
    pclose(SAP_pipe_Prjc);  // Closing the pipe.
}

//this function is used for  searching files with specified extensions in the home directory,
// compresses them into a tar.gz archive, and sends this archive to a client over a socket.
void SAP_handle_targzf_command_Prjc(int SAP_client_sock_Prjc, char *SAP_extensions_Prjc[], int ext_count, int SAP_should_unzip_Prjc) {
    // below is the Command buffers for find and tar commands
    char SAP_find_command_Prjc[1024] = "find ~ -type f ";  // Initializing command to search for files in the home directory.
    char SAP_tar_command_Prjc[512];  

    // Constructing the find command to search for files with specified extensions.
    for (int i = 0; i < ext_count; i++) {
        char SAP_buffer_Prjc[64];
        // For the first extension, this will use -name; for subsequent extensions, use -o -name for OR operation.
        if (i == 0) {
            snprintf(SAP_buffer_Prjc, sizeof(SAP_buffer_Prjc), "-name \"*.%s\"", SAP_extensions_Prjc[i]);
        } else {
            snprintf(SAP_buffer_Prjc, sizeof(SAP_buffer_Prjc), " -o -name \"*.%s\"", SAP_extensions_Prjc[i]);
        }
        strcat(SAP_find_command_Prjc, SAP_buffer_Prjc);
    }
    
    // Appendding to the find command the action to create the tarball with the results.
    strcat(SAP_find_command_Prjc, " | tar czf temp.tar.gz -T -");
    printf("Executing command: %s\n", SAP_find_command_Prjc);  // Printing the complete command for debugging.

    // then i am executing the find and compress command.
    int SAP_status_Prjc = system(SAP_find_command_Prjc);

    // here i am checking if the command execution was successful.
    if (SAP_status_Prjc != 0) {
        perror("Error executing command");
        send(SAP_client_sock_Prjc, "Internal server error", 22, 0);
        return;
    }

    // just Checking if the tarball was created and its size.
    FILE *SAP_fi_Prjc = fopen("temp.tar.gz", "r");
    if (SAP_fi_Prjc) {
        fseek(SAP_fi_Prjc, 0, SEEK_END);  //this will Move to the end of the file to get its size.
        long SAP_size_Prjc = ftell(SAP_fi_Prjc);  //finally getting the file size.
        fclose(SAP_fi_Prjc);

        // If the tarball is empty, then whis will remove it and send an error message.
        if (SAP_size_Prjc == 0) {
            send(SAP_client_sock_Prjc, "SAP:No file found", 15, 0);
            remove("temp.tar.gz");
            return;
        } else {
            // If the tarball has content, then below statement will send it to the client and then remove it.
            SAP_send_file_to_client_Prjc(SAP_client_sock_Prjc, "temp.tar.gz");
            remove("temp.tar.gz");
        }
    } else {
        send(SAP_client_sock_Prjc, "SAP:No file found", 15, 0);
    }
}

//This function will search for files within a date range in the home directory and 
//then compresses them into a tar.gz archive.
    void SAP_handle_getdirf_command_Prjc(int SAP_client_sock_Prjc, char start_date[], char end_date[], int SAP_should_unzip_Prjc) {
    char SAP_command_Prjc[512];
    
    // Removing any existing archive to ensure a clean operation.
    remove("temp.tar.gz");

    // then Constructing the find command to search for files based on date range.
    sprintf(SAP_command_Prjc, "find ~ -type f -newermt \"%s\" ! -newermt \"%s\" -print0 | xargs -0 tar czf temp.tar.gz", start_date, end_date);
    int SAP_status_Prjc = system(SAP_command_Prjc);  // finally executing the find and compress command.

    // again i am checking if the command execution was successful or not .
    if (SAP_status_Prjc != 0) {
        send(SAP_client_sock_Prjc, "SAP:Command execution failed", 25, 0);
        return;
    }

    // Checking if the tar archive was created.
    if (access("temp.tar.gz", F_OK) != -1) {
        SAP_send_file_to_client_Prjc(SAP_client_sock_Prjc, "temp.tar.gz");  // Sending the archive to the client.
        remove("temp.tar.gz");  //and atlast this will remove the temporary archive.
    } else {
        send(SAP_client_sock_Prjc, "SAP:No file found", 15, 0);
    }
}

// this function is used for client to quit from current session.
    void SAP_handle_quit_command_Prjc(int SAP_client_sock_Prjc) {
        close(SAP_client_sock_Prjc);
    }


// This function will continually reads from a client's socket, then interpreting and executing commands which are sent by the client.
    void SAP_processclient_Prjc(int SAP_client_sock_Prjc, int is_main_server) 
    {

    char SAP_client_message_Prjc[2000];  // this is the buffer to hold client messages.
    int SAP_read_size_Prjc;  // whis will store number of bytes received from client.

    // Continuously reading from the client socket.
    while (1) {
        memset(SAP_client_message_Prjc, 0, sizeof(SAP_client_message_Prjc));  // Resetting the client message buffer.
        SAP_read_size_Prjc = recv(SAP_client_sock_Prjc, SAP_client_message_Prjc, sizeof(SAP_client_message_Prjc), 0);  // Receive client message.

        // If some data was received.
        if (SAP_read_size_Prjc > 0) {
            char SAP_cmd_Prjc[10];  // To hold the command type.
            sscanf(SAP_client_message_Prjc, "%s", SAP_cmd_Prjc);  // Extract command type.

            // Check and process based on the received command.

            // this will handle fgets command: Get certain files.
            if (strcmp(SAP_cmd_Prjc, "fgets") == 0) {
                char *SAP_Files_Prjc[4] = {NULL, NULL, NULL, NULL};  // File array to hold file names.
                // Parse the message for file names.
                sscanf(SAP_client_message_Prjc, "%s %ms %ms %ms %ms", SAP_cmd_Prjc, &SAP_Files_Prjc[0], &SAP_Files_Prjc[1], &SAP_Files_Prjc[2], &SAP_Files_Prjc[3]);
                SAP_handle_fgets_command_Prjc(SAP_client_sock_Prjc, SAP_Files_Prjc);  // this will Handle the command.
                for(int i = 0; i < 4 && SAP_Files_Prjc[i] != NULL; i++) {
                    free(SAP_Files_Prjc[i]);  // free dynamically allocated memory for file names.
                }

            // this will handle tarfgetz command: Get compressed file of specified size.
            } else if (strcmp(SAP_cmd_Prjc, "tarfgetz") == 0) {
                int SAP_sizes_Prjc[2];  // To hold sizes.
                int SAP_should_unzip_Prjc = strstr(SAP_client_message_Prjc, "-u") != NULL;  // Check if there's an unzip flag.
                sscanf(SAP_client_message_Prjc, "%s %d %d", SAP_cmd_Prjc, &SAP_sizes_Prjc[0], &SAP_sizes_Prjc[1]);
                SAP_handle_tarfgetz_command_Prjc(SAP_client_sock_Prjc, SAP_sizes_Prjc[0], SAP_sizes_Prjc[1], SAP_should_unzip_Prjc);  // Handle the command.

            // this will handle filesrch command: Search for a specific file.
            } else if (strcmp(SAP_cmd_Prjc, "filesrch") == 0) {
                char SAP_filename_Prjc[200];  // To hold file name.
                sscanf(SAP_client_message_Prjc, "%s %s", SAP_cmd_Prjc, SAP_filename_Prjc);
                SAP_handle_filesrch_command_Prjc(SAP_client_sock_Prjc, SAP_filename_Prjc);  // Handle the command.

            //this will handle targzf command: Compress files with specific extensions.
            } else if (strcmp(SAP_cmd_Prjc, "targzf") == 0) {
                char *SAP_extensions_Prjc[6];  // Array to hold extensions.
                int ext_count = sscanf(SAP_client_message_Prjc, "%s %s %s %s %s %s %s", SAP_cmd_Prjc, SAP_extensions_Prjc[0], SAP_extensions_Prjc[1], SAP_extensions_Prjc[2], SAP_extensions_Prjc[3], SAP_extensions_Prjc[4], SAP_extensions_Prjc[5]);
                int SAP_should_unzip_Prjc = strstr(SAP_client_message_Prjc, "-u") != NULL;  // Checking if there's an unzip flag.
                SAP_handle_targzf_command_Prjc(SAP_client_sock_Prjc, SAP_extensions_Prjc, ext_count, SAP_should_unzip_Prjc);  // Handle the command.

            // this will handle the getdirf command: Get files within a certain date range.
            } else if (strcmp(SAP_cmd_Prjc, "getdirf") == 0) {
                char SAP_dates_Prjc[2][11];  //this array will hold start and end dates.
                sscanf(SAP_client_message_Prjc, "%s %s %s", SAP_cmd_Prjc, SAP_dates_Prjc[0], SAP_dates_Prjc[1]);
                int SAP_should_unzip_Prjc = strstr(SAP_client_message_Prjc, "-u") != NULL;  // Check if there's an unzip flag.
                SAP_handle_getdirf_command_Prjc(SAP_client_sock_Prjc, SAP_dates_Prjc[0], SAP_dates_Prjc[1], SAP_should_unzip_Prjc);  // Handle the command.

            // quit command: End the client session.
            } else if (strcmp(SAP_cmd_Prjc, "quit") == 0){}
        }
    }
    }

//This function is used to sends a welcome message to a connected client and then immediately closes the client's connection.
  void SAP_server_logic_Prjc(int SAP_client_sock_Prjc) {
    char message[2000]; // Buffer to hold a message.
    strcpy(message, "SAP: Connected to the server."); // Copy the welcome message to the buffer.
    send(SAP_client_sock_Prjc, message, strlen(message), 0); // Sending the welcome message to the client.
    close(SAP_client_sock_Prjc); // Finally closing the client's socket connection.
}

//This function is used to handles the SIGCHLD signal, which is sent to a process when its child process terminates. 
//By calling waitpid in a loop, it cleans up the terminated child processes to prevent them from becoming zombies.

  void SAP_handle_sigchldc_Prjc(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0); // Reap all terminated child processes.
}


//This function is a just proxy that relays data between a connected client and a mirror server. 
void SAP_proxy_to_mirror_Prjc(int SAP_client_sock_Prjc)

{
    int SAP_mirror_fd_Prjc; // this is a File descriptor for the mirror server's socket.
    struct sockaddr_in SAP_mirror_addr_Prjc; // Address structure for the mirror server.

    // here i am creating a new socket for the mirror server connection.
    if ((SAP_mirror_fd_Prjc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("SAP:Cannot create socket for mirror connection");
        return;
    }

    // Setting the address details for the mirror server.
    SAP_mirror_addr_Prjc.sin_family = AF_INET;
    SAP_mirror_addr_Prjc.sin_port = htons(8887); // setting port number
    if (inet_pton(AF_INET, "127.0.0.1", &SAP_mirror_addr_Prjc.sin_addr) <= 0)
    {
        perror("SAP:Invalid mirror address or Address not supported");
        return;
    }

    // Connecting to the mirror server.
    if (connect(SAP_mirror_fd_Prjc, (struct sockaddr *)&SAP_mirror_addr_Prjc, sizeof(SAP_mirror_addr_Prjc)) < 0)
    {
        perror("SAP:Connection to mirror failed");
        return;
    }

    // Relaying data between client and mirror server.
    fd_set set;
    char SAP_buffer_Prjc[2048];
    while (1)
    {
        FD_ZERO(&set); // Clearing the set.
        FD_SET(SAP_client_sock_Prjc, &set); // Adding the client socket to the set.
        FD_SET(SAP_mirror_fd_Prjc, &set); // Adding the mirror server socket to the set.

        // Finding the highest-numbered file descriptor in the set.
        int SAP_max_fdr_Prjc = (SAP_client_sock_Prjc > SAP_mirror_fd_Prjc) ? SAP_client_sock_Prjc : SAP_mirror_fd_Prjc;

        // Waiting for one of the sockets to become ready.
        if (select(SAP_max_fdr_Prjc + 1, &set, NULL, NULL, NULL) < 0)
        {
            perror("SAP:Select failed");
            break;
        }

        // If the client socket is ready, then read from it and send to the mirror server.
        if (FD_ISSET(SAP_client_sock_Prjc, &set))
        {
            int SAP_bytes_read_Prjc = recv(SAP_client_sock_Prjc, SAP_buffer_Prjc, 2048, 0);
            if (SAP_bytes_read_Prjc <= 0)
                break;
            send(SAP_mirror_fd_Prjc, SAP_buffer_Prjc, SAP_bytes_read_Prjc, 0);
        }

        // If the mirror server socket is ready, then  read from it and send to the client.
        if (FD_ISSET(SAP_mirror_fd_Prjc, &set))
        {
            int SAP_bytes_read_Prjc = recv(SAP_mirror_fd_Prjc, SAP_buffer_Prjc, 2048, 0);
            if (SAP_bytes_read_Prjc <= 0)
                break;
            send(SAP_client_sock_Prjc, SAP_buffer_Prjc, SAP_bytes_read_Prjc, 0);
        }
    }

    close(SAP_mirror_fd_Prjc); // Closing the mirror server socket.
}

int main(int argc, char *argv[]) {
    // Declaring server socket and client socket.
    int server_sock, SAP_client_sock_Prjc, c;
    // Declaring socket address structures for the server and client.
    struct sockaddr_in server, client;

    // Settingup signal handling for SIGCHLD signal (signal sent to parent when child terminates).
    struct sigaction sa;
    sa.sa_handler = &SAP_handle_sigchldc_Prjc;  // Setting the signal handler function.
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;    // these are Flags to restart certain syscalls and not to receive signal when children stop.
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror("sigaction failed");   // Printing the error message if signal setup fails.
        exit(EXIT_FAILURE);           // Exiting the program with a failure status.
    }

    // Settingup shared memory for SAP_connection_countL_Prjc
    int SAP_shmid_Prjc = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);  // then i am requesting shared memory segment.
    if (SAP_shmid_Prjc < 0) {
        perror("shmget"); // Printing error message if shared memory request fails.
        exit(1);         // then will Exit from the program.
    }
    // Attaching to the shared memory segment.
    SAP_connection_countL_Prjc = shmat(SAP_shmid_Prjc, NULL, 0);
    if ((long)SAP_connection_countL_Prjc == -1) {
        perror("shmat"); // Printing the error message if shared memory attachment fails.
        exit(1);         //  finally Exit the program.
    }
    *SAP_connection_countL_Prjc = 0;   // Initializing connection count in shared memory to 0.

    // Creating server socket.
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("SAP:Could not create socket"); // Printing error message if socket creation fails.
        return 1;                          // Returning with error code.
    }

    // Setup server address structure.
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;   // Binding to any available interface.
    server.sin_port = htons(PORT);         // Binding to specified PORT.

    // Binding the socket to the server address.
    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("SAP:Bind failed");  // Printing the error message if binding fails.
        return 1;               // Returning  with error code.
    }

    //this will set the server socket to listen mode with a backlog of 5 connections.
    listen(server_sock, 5);

    printf("SAP:Server is  listening on port %d...\n", PORT);
    c = sizeof(struct sockaddr_in);

    // Declaring mirror server address structure and set its properties.
                 // Setting mirror port.

    // this is the main server loop to accept and handle client connections.
    while (1) {
        c = sizeof(struct sockaddr_in);
        // Acceptting client connection.
        SAP_client_sock_Prjc = accept(server_sock, (struct sockaddr *)&client, (socklen_t *)&c);
        if (SAP_client_sock_Prjc < 0) {
            perror("SAP:Accept failed");  // Printing the error message if accept will fails.
            continue;                 // skip the current loop.
        }

        printf("SAP:Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        //this is the logic to decide if the main server or the mirror should handle the client.
        if (*SAP_connection_countL_Prjc <= 6 || (*SAP_connection_countL_Prjc > 12 && *SAP_connection_countL_Prjc % 2 == 1)) {
            printf("SAP:Main server handling client connection...\n");
            SAP_processclient_Prjc(SAP_client_sock_Prjc, 1); // Handle the client with the main server logic.
            close(SAP_client_sock_Prjc);                      // Close client socket.
        } else if ((*SAP_connection_countL_Prjc <= 12 && *SAP_connection_countL_Prjc > 6) || (*SAP_connection_countL_Prjc > 12 && *SAP_connection_countL_Prjc % 2 == 0)) {
            SAP_proxy_to_mirror_Prjc(SAP_client_sock_Prjc);   // Proxy the client to the mirror server.
            close(SAP_client_sock_Prjc);                      // Close client socket.
        }

        // Forking a child process to handle the client.
        if (fork() == 0) {
            (*SAP_connection_countL_Prjc)++;   // Incrementing connection count.
            printf("SAP:Current connection count: %d\n", *SAP_connection_countL_Prjc);
            close(server_sock);  // Closing the main server socket in the child process.

            shmdt(SAP_connection_countL_Prjc); // Detaching from shared memory in the child process.
            exit(0);                           // Exiting the child process.
        }
        close(SAP_client_sock_Prjc); // finally closing the client socket in the parent process.
    }

    // Detaching the program from shared memory and remove it.
    shmdt(SAP_connection_countL_Prjc);
    shmctl(SAP_shmid_Prjc, IPC_RMID, NULL);
    return 0; // Atlast the main program return with success status.
}


