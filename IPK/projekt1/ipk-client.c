#include<stdio.h>
#include<stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include "status_code.h"
#include <string.h>
#include "help_function.h"
#include <netinet/in.h>
#include <string.h>
#include <libgen.h>
#include <sys/time.h>

#define BUFFER_LENGTH 4096

int main(int argc, char*argv[])
{
    FILE * file_descriptor;
    struct sockaddr_in address, serv_addr;
    int c, port, valread, sock, current_char, data_length, recv_length;
    char *host_name;
    unsigned char *file;
    bool hflag = false, fflag = false,  pflag = false, write_flag = false;
    struct hostent *he;
    unsigned char buffer[BUFFER_LENGTH] = {0};
    struct timeval timeout;      

    // Zpracovani argumentu
    while((c = getopt(argc, argv, "hp:r:w:")) != -1){
        switch(c)
        {
            case 'h':
                if(hflag == true)
                {
                    fprintf(stderr, "-h is already defined");
                    exit(EXIT_FAILURE);
                }
                host_name = optarg;
                hflag = true;
                break;
            case 'p':
                if(pflag == true)
                {
                    fprintf(stderr,"-p is already defined");
                    exit(EXIT_FAILURE);
                }
                port = atoi(optarg);
                pflag = true;
                break;
            case 'r':
                if(fflag == true)
                {
                    fprintf(stderr,"-r or -w is already defined");
                    exit(EXIT_FAILURE);
                }
                file = optarg;
                fflag = true;
                write_flag = false;
                break;
            case 'w':
                if(fflag == true)
                {
                    fprintf(stderr,"-r or -w is already defined");
                    exit(EXIT_FAILURE);
                }
                file = optarg;
                fflag = true;
                write_flag = true;
                break;
            default:
                fprintf(stderr,"Wrong parametr\n");
                exit(EXIT_FAILURE);
                break;
        }
    } 

    if(pflag == false || hflag == false || fflag == false)
    {
        fprintf(stderr,"Wrong format ./ipk-client -h host -p port [-r|-w] file\n");
        exit(EXIT_FAILURE);
    }

    // Vytvoreni socketu
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr,"Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    
    // Nastaveni adresy
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if((he = gethostbyname(host_name))==NULL)
	{
		fprintf(stderr,"cant get hostname");
		exit(-1);
	}

    // ulozeni adresy
	memcpy(&serv_addr.sin_addr,he->h_addr_list[0],he->h_length);

    // Pripojeni k serveru
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr,"Connection Failed \n");
        return -1;
    }

    // Nastaveni 10 sekundoveho 
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    
    if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) < 0){
            fprintf(stderr,"Socket timeout\n");
            close(sock);
            exit(EXIT_FAILURE);
    }

    // Pokud se jedna o zapis na server
    if(write_flag)
    {
        // Otevreni souboru
        file_descriptor = fopen(file, "rb");
        if(file_descriptor == NULL)
        {
            perror("Problem with opening file");
            exit(EXIT_FAILURE);
        }

        // Priprava bufferu
        int buffer_length = prepare_first_buffer(STATUS_BEGIN_WRITE, basename(file),buffer,BUFFER_LENGTH, file_descriptor);
        bool send_file = false;
        unsigned char server_status = STATUS_BEGIN_WRITE;

        // Postupne poslani celeho souboru
        while(send_file == false)
        {   
            // Kontrola send funckce
            int send_length = 0;
            while(send_length != BUFFER_LENGTH){
                send_length = send(sock, buffer, BUFFER_LENGTH, 0);
            }

            // Kontrola recv
            send_length = 0;
            while(send_length != BUFFER_LENGTH){
                recv_length = recv( sock , &buffer[send_length], BUFFER_LENGTH - send_length,0);
                if(recv_length <= 0){
                    fprintf(stderr,"Error or timeout on recv\n");
                    close(sock);
                    exit(EXIT_FAILURE);
                }
                send_length += recv_length;
            }
                        
            // Ziskani status kodu a delky dat
            convert_header_from_binary(&server_status, &data_length,buffer);

            // Kontrola souboru/ Pocet klientu
            if(server_status == STATUS_MANY_CONNECTION){
                fprintf(stderr,"Connection failed: Many connection\n");
                close(sock);
                exit(EXIT_FAILURE);
            }else if(server_status == STATUS_FILE_ALREADY_USED){
                fprintf(stderr,"Connection failed: File is already use\n");
                close(sock);
                exit(EXIT_FAILURE);
            }

            // Smazani dat z bufferu
            memset(buffer, '0', BUFFER_LENGTH);

            if(server_status == STATUS_ALL_DATA_RECEIVED)
            {
                fprintf(stderr,"All Data send\n");
                send_file = true;
            }
            else
            {
                int status = STATUS_DATA_SEND;
                data_length = write_from_file_to_binary(file_descriptor, buffer, 2, BUFFER_LENGTH-2);
                
                // Kontrola jestli se neposilaji posledni data
                if(feof(file_descriptor))
                {
                    status = STATUS_ALL_DATA_SEND;
                }
                convert_header_to_binary(status, data_length,buffer);
            }
        }   
        fclose(file_descriptor);  
    }
    else{
        // Kontrola + vygenerovani cesty k souboru
        generate_path_to_file(file);

        // Otevreni souboru
        file_descriptor = fopen(file, "wb");
        if(file_descriptor == NULL)
        {
            perror("Problem with opening file");
            exit(EXIT_FAILURE);
        }

        // Priprava prvniho bufferu
        int buffer_length = prepare_first_buffer(STATUS_BEGIN_READ, basename(file),buffer,BUFFER_LENGTH, NULL);
        unsigned char server_status = STATUS_BEGIN_READ;

        // Postupne ziskani souboru ze serveru
        bool receive_file = false;
        while(receive_file == false)
        {
            // Osetreni sendu
            int send_length = 0;
            while(send_length != BUFFER_LENGTH){
                send_length = send(sock, buffer, BUFFER_LENGTH,0);
            }

            // Osetreni funkce recv
            send_length = 0;
            while(send_length != BUFFER_LENGTH){
                recv_length = recv( sock , &buffer[send_length], BUFFER_LENGTH - send_length,0);
                if(recv_length <= 0){
                    fprintf(stderr,"Error or timeout on recv\n");
                    close(sock);
                    exit(EXIT_FAILURE);
                }
                send_length += recv_length;
            }
            
            // Ziskani status kodu, delky dat
            convert_header_from_binary(&server_status, &data_length,buffer);

            // Kontrola souboru/ Pocet klientu
            if(server_status == STATUS_MANY_CONNECTION){
                fprintf(stderr,"Connection failed: Many connection\n");
                close(sock);
                exit(EXIT_FAILURE);
            }else if(server_status == STATUS_FILE_ALREADY_USED){
                fprintf(stderr,"Connection failed: File is already use\n");
                close(sock);
                exit(EXIT_FAILURE);
            }
            
            // Zapis dat do souboru
            fwrite(&(buffer[2]),sizeof(unsigned char), data_length, file_descriptor);
            fflush(file_descriptor);

            // Smazani dat z bufferu
            memset(buffer, '0', BUFFER_LENGTH);

            // Kontrola jestli se ziskali vsechny soubory
            if(server_status == STATUS_ALL_DATA_SEND)
            {
                fprintf(stderr,"All Data received\n");
                receive_file = true;
                convert_header_to_binary(STATUS_ALL_DATA_RECEIVED, 0, buffer);

                // Kontrola sendu
                int send_length = 0;
                while(send_length != BUFFER_LENGTH){
                    send_length = send(sock, buffer, BUFFER_LENGTH,0);
                }
            }
            convert_header_to_binary(STATUS_DATA_RECEIVED, 0, buffer);
            buffer_length = 2;
        }
        fclose(file_descriptor);
    }
    close(sock);
    return 0;
}