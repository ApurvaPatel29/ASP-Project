//Section 3- 110096127 - Sagar Vivek Pandya
//Section 3 - 110095571 - Apurvakumar Patel


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SAP_fILE_SIGNAL_Prjc "SENDING_FILE"
#define SAP_BUFFER_SIZE_Prjc 1024// here we  are defining preprocessor macros for declaring a signal string for indicating file transfer and a buffer size which will be used to read and write data
int SAP_is_leap_year_Prjc(int year) {//here in this  function we check if a given year is a leap year by following the leap year rules. It returns 1 if the year is a leap year, and 0 otherwise.
    if (year % 400 == 0) return 1;//here it takes a year as input and determines if it's a leap year according to the leap year rules. If the year is divisible by 400 or divisible by 4 but not divisible by 100, it's a leap year, and the function returns 1. Otherwise, it returns 0.
    if (year % 100 == 0) return 0;
    if (year % 4 == 0) return 1;
    return 0;
}

int SAP_is_valid_date_Prjc(const char *date) {//here this  function, SAP_is_valid_date_Prjc, checks that if  a given date is valid or not. It receives a date string in the format "year-month-day"
    int y, m, d;// It receives a date string in the format "year-month-day". It uses sscanf to parse the date components into y (year), m (month), and d (day)
    if (sscanf(date, "%d-%d-%d", &y, &m, &d) != 3) return 0;//we then furthte check for  conditions to validate the year, month, and day components. Additionally, if year entered is a leap year
    if (y <= 1900 || y >= 3000) return 0;
    if (m < 1 || m > 12) return 0;

    int SAP_days_in_month_Prjc[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (SAP_is_leap_year_Prjc(y)) SAP_days_in_month_Prjc[2] = 29;
    if (d < 1 || d > SAP_days_in_month_Prjc[m]) return 0;

    return 1;
}
int SAP_extract_tar_gz_Prjc(const char *SAP_filename_Prjc) {//here the  function, SAP_extract_tar_gz_Prjc, takes a filename as input and will construct a command to extract the contents of a .tar.gz file using the tar command-line command and then uses the system function to execute the  command
    char SAP_extract_command_Prjc[200];
    snprintf(SAP_extract_command_Prjc, sizeof(SAP_extract_command_Prjc), "tar -xzvf %s", SAP_filename_Prjc);
    return system(SAP_extract_command_Prjc);
}

void SAP_receive_and_extract_file_Prjc(int sock, const char *SAP_filename_Prjc) {//here this function is responsible for receiving a file over a network socket, writing it to a local file, and then extracting the contents of the received .tar.gz file.
    char SAP_buffer_Prjc[SAP_BUFFER_SIZE_Prjc];//sock this parameter represents the socket over which the file is being received.
//SAP_filename_Prjc this parameter specifies the name of the local file in which the received data will be saved.
//The function starts by declaring following   variables:

//SAP_buffer_Prjc which is a buffer used for temporarily storing received data.
//SAP_bytes_received_Prjc its  the number of bytes received in the current iteration of the loop.
//SAP_total_received_Prjc  the total number of bytes received so far.
//SAP_file_size_Prjc it represents the  size of the file being received.
//The function then proceeds to recieve the size of the file using the recv function.

//The function then opens a local file with the provided filename (SAP_filename_Prjc) in binary write mode ("wb").
    int SAP_bytes_received_Prjc = 0;
    int SAP_total_received_Prjc = 0;

//The SAP_total_received_Prjc counter is updated to keep track of the total bytes received.
    int SAP_file_size_Prjc;
    if (recv(sock, &SAP_file_size_Prjc, sizeof(SAP_file_size_Prjc), 0) <= 0) {
        perror("SAP :ERROR receiving file SAP_size_Prjc");
        return;
    }

    FILE *SAP_fp_Prjc = fopen(SAP_filename_Prjc, "wb");
    if (SAP_fp_Prjc == NULL) {
        perror("SAP :ERROR opening file");
        return;
    }

    while (SAP_total_received_Prjc < SAP_file_size_Prjc) {//here this  while loop iterates until the total received bytes match the expected file size.
      //for  each iteration:




        SAP_bytes_received_Prjc = recv(sock, SAP_buffer_Prjc, SAP_BUFFER_SIZE_Prjc, 0);
        if (SAP_bytes_received_Prjc <= 0) {//If the bytes received is less than or equal to 0, an error message is printed, the file will be  closed, and the function returns.
            perror("SAP :ERROR in recv");
            fclose(SAP_fp_Prjc);
            return;
        }

        fwrite(SAP_buffer_Prjc, 1, SAP_bytes_received_Prjc, SAP_fp_Prjc);//The received data in SAP_buffer_Prjc is written to the local file using the fwrite function. This step saves the received data to the file.
        SAP_total_received_Prjc += SAP_bytes_received_Prjc;
    }

    fclose(SAP_fp_Prjc);//After the loop, the local file is closed using fclose(SAP_fp_Prjc).

    // Extract the received tar.gz file
    int SAP_extraction_result_Prjc = SAP_extract_tar_gz_Prjc(SAP_filename_Prjc);//// The result of the extraction is stored in SAP_extraction_result_Prjc.

//further the extracted tar.gz file is extracted by calling the SAP_extract_tar_gz_Prjc function with the provided filename (SAP_filename_Prjc).
    if (SAP_extraction_result_Prjc == 0) { //If the extraction result is 0, a success message is printed. Otherwise, a failure message is printed
        printf("SAP:File extracted successfully.\n");
    } else {
        printf("SAP :File extraction failed.\n");
    }
}

int main(int argc, char *argv[]) {
    int sock;//here we declare variables  to manage the network socket (sock), server address structure (server), user input (SAP_command_Prjc), server reply (server_reply), and IP address (ip_address).
    struct sockaddr_in server;
    char SAP_command_Prjc[2000], server_reply[2000], ip_address[50];

    printf("SAP:Enter server IP address (e.g., 192.168.1.10): ");//we then  prompt the user to enter the IP address of the server they want to connect to. It reads the input using fgets and removes the trailing newline character.
    fgets(ip_address, sizeof(ip_address), stdin);
    ip_address[strlen(ip_address) - 1] = '\0';

    sock = socket(AF_INET, SOCK_STREAM, 0);//we create a socket  using the socket function. we are  using the AF_INET family for IPv4 addresses and SOCK_STREAM for a reliable, stream-oriented connection.
    if (sock == -1) {
        perror("Could not create socket");//If the socket creation fails, an error message is printed, and the program returns with an error code.
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(ip_address);////establish a connection to the server using the connect function.
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);
    //

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {//If the connection fails, an error message is printed, and the program returns with an error code.
        perror("Connect failed");
        return 1;
    }

    while (1) {//while loop will maitain showcase main   portion of the program responsible for taking various commands. It repeatedly prompts the user to enter a command (SAP_command_Prjc), reads the command using fgets, and removes the trailing newline character from the input
        printf("SAP :Enter SAP_command_Prjc: ");
        fgets(SAP_command_Prjc, sizeof(SAP_command_Prjc), stdin);
        SAP_command_Prjc[strlen(SAP_command_Prjc) - 1] = '\0';

        char SAP_cmd_Prjc[10], args[8][200];
        int SAP_scanned_Prjc = sscanf(SAP_command_Prjc, "%s %s %s %s %s %s %s %s %s", SAP_cmd_Prjc, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);//the command string is split into components: SAP_cmd_Prjc is the main command, and args is an array holding potential arguments (up to 8). The sscanf function is used to parse the command and its arguments from the input line. The variable SAP_scanned_Prjc stores the number of items scanned and assigned, which can be used to determine how many arguments were provided

        if (strcmp(SAP_cmd_Prjc, "targzf") == 0) {// here this if-else statements check the extracted command (SAP_cmd_Prjc) to determine which command was entered by the user.
          //Depending on the command, different validation checks and actions are taken.
          // Check if the user command is "targzf"
          // Validation checks for the "targzf" command:


            if (SAP_scanned_Prjc < 2) {// Check if the user entered at least 2 arguments
                printf("SAP :ERROR: No file extension provided.\n");
                continue;
            }
            if (SAP_scanned_Prjc == 2 && strcmp(args[0], "-u") == 0) {//// Check if the user entered exactly 2 arguments and the first argument is "-u"
                printf("SAP :ERROR: No file extension provided.\n");
                continue;
            }
            if (SAP_scanned_Prjc > 7 || (SAP_scanned_Prjc == 7 && strcmp(args[5], "-u") != 0)) {// // Check if the user entered more than 7 arguments or exactly 7 arguments and the sixth argument is not "-u"
                printf("SAP :ERROR: Maximum of 6 file SAP_extensions_Prjc allowed.\n");
                continue;
            }
        } else if (strcmp(SAP_cmd_Prjc, "getdirf") == 0) {// // Check if the user command is "getdirf"
        // Validation checks for the "getdirf" command:


            if (!SAP_is_valid_date_Prjc(args[0]) || !SAP_is_valid_date_Prjc(args[1])) {  // Check if the entered dates are not valid
                printf("SAP :ERROR: Date is not set correctly.\n");
                continue;
            }
        } else if (strcmp(SAP_cmd_Prjc, "tarfgetz") == 0) {
          //  // Check if the user command is "tarfgetz"
    // Validation checks for the "tarfgetz" command:


            int SAP_size1_Prjc = atoi(args[0]);// Convert the arguments to integers
            int SAP_size2_Prjc = atoi(args[1]);
            if (!(SAP_size1_Prjc >= 0 && SAP_size2_Prjc >= 0 && SAP_size1_Prjc <= SAP_size2_Prjc)) {// Check if the size range is not valid
                printf("SAP :ERROR: Invalid SAP_size_Prjc range.\n");
                continue;
            }
        } else if (strcmp(SAP_cmd_Prjc, "filesrch") == 0) {//// Check if the user command is "filesrch"
    // Validation checks for the "filesrch" command:


            if (SAP_scanned_Prjc < 2) {// Check if the user did not provide a filename
                printf("SAP :ERROR: Filename not provided.\n");
                continue;
            }
        } else if (strcmp(SAP_cmd_Prjc, "fgets") == 0) {//// Check if the user command is "fgets"
              // Validation checks for the "fgets" command:


            if (SAP_scanned_Prjc < 2) {// Check if the user did not provide at least one filename
                printf("SAP :ERROR: At least one file name is required.\n");
                continue;
            }
        } else if (strcmp(SAP_cmd_Prjc, "quit") == 0) {// // Check if the user command is "quit"
            // Send the "quit" command to the server and break the loop
            send(sock, SAP_command_Prjc, strlen(SAP_command_Prjc), 0);
            break;
        } else {
            printf("SAP :ERROR: Invalid SAP_command_Prjc.\n");//If none of the recognized commands match, an error message is printed, and the loop continues to prompt for a new command
            continue;
        }

        if (send(sock, SAP_command_Prjc, strlen(SAP_command_Prjc), 0) < 0) {//when the command  validatinon is completed, the program will  send the command to the server using the send function.
            puts("Send failed");// If the send operation fails, an error message is printed, and the program returns with an error code.
            return 1;
        }

        // Receive the server's SAP_response_Prjc
        ssize_t SAP_reply_size_Prjc = recv(sock, server_reply, sizeof(server_reply) - 1, 0);//then  program will wait to receive a response from the server using the recv function. The size of the received data is stored in SAP_reply_size_Prjc
        if (SAP_reply_size_Prjc < 0) {// If the receive operation fails, an error message is printed, and the loop is broken.
            puts("recv failed");
            break;
        }
        server_reply[SAP_reply_size_Prjc] = '\0'; //The received data is stored in the server_reply buffer.

        if (strcmp(server_reply, SAP_fILE_SIGNAL_Prjc) == 0) {//The received server reply is then compared to the predefined SAP_fILE_SIGNAL_Prjc. If the comparison matches, it means that the server is sending a file.
            // Handle file reception and extraction
            SAP_receive_and_extract_file_Prjc(sock, "tem.tar.gz");//it then calls the SAP_receive_and_extract_file_Prjc function to handle the reception and extraction of the file.
            printf("SAP :File received and extracted as tem.tar.gz\n");
        } else {
            // Print server SAP_response_Prjc
            puts(server_reply);//If the reply doesn't match the signal, the program simply prints the server reply to the console
        }

        if (strcmp(SAP_command_Prjc, "quit") == 0) {//here we are checing  if the command entered was "quit". If it was, the loop is exited, effectively terminating the program.
            break;
        }
    }

    close(sock);
    return 0;//the network socket is closed using the close function, and the main function returns 0 to indicate successful execution
}

