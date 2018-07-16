#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include "status_code.h"
#include <sys/select.h>
#include "help_function.h"
#include <signal.h>
#include <sys/shm.h>
#include <netinet/in.h>

static int *glob_var;
int welcome_socket;
int shmid;
#define BUFFER_LENGTH 4096
#define SHARED_MEMORY_COUNT 200

/**
 * @brief Reakce na SIGINT
 * 
 * @param sig id signalu
 */
void handler(int sig)
{
    close(welcome_socket);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}

int main(int argc, char *argv[])
{
    // Ziskani sdilene pameti
    if ((shmid = shmget(IPC_PRIVATE, sizeof(unsigned long) * SHARED_MEMORY_COUNT, IPC_CREAT | IPC_EXCL | 0666)) == 1)
    {
        fprintf(stderr, "Problem with getting share memory");
        exit(EXIT_FAILURE);
    }

    unsigned long *shared = (unsigned long *)shmat(shmid, 0, 0);
    memset(shared, 0, SHARED_MEMORY_COUNT * sizeof(int));

    // Registrace funkce handler pro signal SIGINT
    signal(SIGINT, handler);

    char *filename, *data;
    FILE *file_descriptor;
    int c, port, valread, welcome_socket, new_socket, send_length, recv_length, actual, filename_size;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    bool pflag = false;
    unsigned char buffer[BUFFER_LENGTH] = {0};
    fd_set read_socks, write_socks;
    long int filename_hash;
    struct timeval tv, timeout;

    // Zpracovani argumentu
    while ((c = getopt(argc, argv, "p:")) != -1)
    {
        switch (c)
        {
        case 'p':
            if (pflag == true)
            {
                fprintf(stderr, "-p is already defined\n");
                exit(EXIT_FAILURE);
            }
            port = atoi(optarg);
            pflag = true;
            break;
        default:
            fprintf(stderr, "Wrong parametr\n");
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (pflag == false)
    {
        fprintf(stderr, "Wrong format ./ipk-server -p port\n");
        exit(EXIT_FAILURE);
    }

    // Vytvoreni welcome socketu
    if ((welcome_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Nastaveni struktury adresy
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(welcome_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Poslouchani socketu
    if (listen(welcome_socket, 1) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        // PRijmenut klienta
        if ((new_socket = accept(welcome_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            fprintf(stderr, "Error with accept");
            exit(EXIT_FAILURE);
        }

        // Vytvori se novy proces aby bylo mozne obslouzit dalsi klienty
        if (!fork())
        {
            // Nastaveni timeoutu
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
            if (setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
            {
                fprintf(stderr, "Socket timeout\n");
                close(new_socket);
                exit(EXIT_FAILURE);
            }

            // Zpracovani jednoho klienta
            bool connection_with_Client = true;
            while (connection_with_Client)
            {
                FD_ZERO(&read_socks);
                FD_SET(new_socket, &read_socks);

                // Timeout pro select
                tv.tv_sec = 60;
                tv.tv_usec = 0;

                int i;
                if ((i = select(FD_SETSIZE, &read_socks, NULL, NULL, &tv)) == 0)
                {
                    fprintf(stderr, "Select timeout\n");
                    close(new_socket);
                    exit(EXIT_FAILURE);
                }
                else if (i == -1)
                {
                    fprintf(stderr, "Error in select\n");
                    close(new_socket);
                    exit(EXIT_FAILURE);
                }

                if (FD_ISSET(new_socket, &read_socks))
                {
                    memset(&buffer, '0', sizeof(buffer));
                    
                    // Zpracovani recevu
                    send_length = 0;
                    while (send_length != BUFFER_LENGTH)
                    {
                        recv_length = recv(new_socket, &buffer[send_length], BUFFER_LENGTH - send_length, 0);
                        if (recv_length <= 0)
                        {
                            fprintf(stderr, "Error or timeout on recv\n");
                            close(new_socket);
                            shared[actual] = 0;
                            exit(EXIT_FAILURE);
                        }
                        send_length += recv_length;
                    }

                    unsigned char status_code = 0;
                    int status;
                    int data_length = 0;
                    convert_header_from_binary(&status_code, &data_length, buffer);

                    // Zpracovani nazvu souboru,
                    if (status_code == STATUS_BEGIN_READ || status_code == STATUS_BEGIN_WRITE)
                    {
                        filename = get_filename_from_binary(buffer);
                        filename_size = strlen(filename) + 1;

                        // Zaheshovani nazvu pro ulozeni do sdilene pameti
                        filename_hash = hash(filename);
                        fprintf(stderr, "FILENAME %s\nHASH:%ld", filename, filename_hash);
                        // Kontrola jestli se ze souborem uz nepracuje
                        actual = -1;
                        for (int i = 0; i < SHARED_MEMORY_COUNT; i++)
                        {
                            if (shared[i] == filename_hash)
                            {
                                fprintf(stderr, "File is already use\n");
                                memset(buffer, '0', BUFFER_LENGTH);
                                convert_header_to_binary(STATUS_FILE_ALREADY_USED, 0, buffer);
                         
                                // Odeslani zpravy klientovi, ze soubor je uz pouzivany
                                send_length = 0;
                                while (send_length != BUFFER_LENGTH)
                                {
                                    send_length = send(new_socket, buffer, BUFFER_LENGTH, 0);
                                }
                                close(new_socket);
                                exit(EXIT_FAILURE);
                            }
                            if (actual == -1 && shared[i] == 0)
                            {
                                actual = i;
                            }
                        }

                        // Pokud neni zadne misto ve sdilene pameti volne
                        if (actual == -1)
                        {
                            fprintf(stderr, "So many connection\n");
                            memset(buffer, '0', BUFFER_LENGTH);
                            convert_header_to_binary(STATUS_MANY_CONNECTION, 0, buffer);
                            send_length = 0;
                            while (send_length != BUFFER_LENGTH)
                            {
                                send_length = send(new_socket, buffer, BUFFER_LENGTH, 0);
                            }
                            close(new_socket);
                            exit(EXIT_FAILURE);
                        }
                        shared[actual] = filename_hash;
                    }
                    switch (status_code)
                    {
                    case STATUS_BEGIN_READ:
                        // Otevreni souboru pro cteni
                        file_descriptor = fopen(filename, "rb");
                        if (file_descriptor == NULL)
                        {
                            perror("Error with opening file\n");
                            shared[actual] = 0;
                            return EXIT_FAILURE;
                        }
                        memset(buffer, '0', BUFFER_LENGTH);
                        
                        // Zapis dat do bufferu
                        data_length = write_from_file_to_binary(file_descriptor, buffer, 2, BUFFER_LENGTH -2);
                        status = STATUS_DATA_SEND;

                        // Kontrola jestli nejsou zapsane veskera data souboru
                        if (feof(file_descriptor))
                            status = STATUS_ALL_DATA_SEND;
                        convert_header_to_binary(status, data_length, buffer);

                        // Kontrola sendu
                        send_length = 0;
                        while (send_length != BUFFER_LENGTH)
                        {
                            send_length = send(new_socket, buffer, BUFFER_LENGTH, 0);
                        }
                        break;
                    case STATUS_BEGIN_WRITE:
                        // Otevreni pro zapis
                        file_descriptor = fopen(filename, "wb+");
                        if (file_descriptor == NULL)
                        {
                            perror("Error with opening file\n");
                            shared[actual] = 0;
                            return EXIT_FAILURE;
                        }

                        // Zapis dat z bufferu do souboru
                        int filename_size = strlen(filename);
                        fwrite(&(buffer[3 + filename_size]), sizeof(unsigned char), data_length, file_descriptor);
                        fflush(file_descriptor);

                        memset(buffer, '0', BUFFER_LENGTH);
                        convert_header_to_binary(STATUS_DATA_RECEIVED, 0, buffer);
                        send_length = 0;
                        while (send_length != BUFFER_LENGTH)
                        {
                            send_length = send(new_socket, buffer, BUFFER_LENGTH, 0);
                        }
                        break;
                    case STATUS_DATA_SEND:
                        fwrite(&(buffer[2]), sizeof(unsigned char), data_length, file_descriptor);
                        fflush(file_descriptor);
                        memset(buffer, '0', BUFFER_LENGTH);
                        convert_header_to_binary(STATUS_DATA_RECEIVED, 0, buffer);
                        send_length = 0;
                        while (send_length != BUFFER_LENGTH)
                        {
                            send_length = send(new_socket, buffer, BUFFER_LENGTH, 0);
                        }
                        break;
                    case STATUS_DATA_RECEIVED:
                        memset(buffer, '0', BUFFER_LENGTH);

                        // Zapis dat ze souboru do bufferu
                        data_length = write_from_file_to_binary(file_descriptor, buffer, 2, BUFFER_LENGTH-2);
                        status = STATUS_DATA_SEND;
                        
                        // Kontrola jestli nejsou prectena vsechny data ze souboru
                        if (feof(file_descriptor))
                            status = STATUS_ALL_DATA_SEND;
                        convert_header_to_binary(status, data_length, buffer);

                        send_length = 0;
                        while (send_length != BUFFER_LENGTH)
                        {
                            send_length = send(new_socket, buffer, BUFFER_LENGTH, 0);
                        }
                        break;
                    case STATUS_ALL_DATA_SEND:
                        // Zapis prijatych dat do souboru
                        fwrite(&(buffer[2]), sizeof(unsigned char), data_length, file_descriptor);
                        fflush(file_descriptor);

                        memset(buffer, '0', BUFFER_LENGTH);
                        convert_header_to_binary(STATUS_ALL_DATA_RECEIVED, 0, buffer);
                        
                        send_length = 0;
                        while (send_length != BUFFER_LENGTH)
                        {
                            send_length = send(new_socket, buffer, BUFFER_LENGTH, 0);
                        }

                        connection_with_Client = false;
                        fclose(file_descriptor);
                        file_descriptor = NULL;
                        break;
                    case STATUS_ALL_DATA_RECEIVED:
                        fprintf(stderr,"All data received\n");
                        connection_with_Client = false;
                        fclose(file_descriptor);
                        file_descriptor = NULL;
                        break;
                    }
                }
            }
            close(new_socket);
            // Odstraneni zaznamu ve sdilene pameti
            shared[actual] = 0;
            exit(EXIT_SUCCESS);
        }
    }
    close(welcome_socket);
    return 0;
}
