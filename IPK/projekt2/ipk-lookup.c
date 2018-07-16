#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "help_function.h"
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int c,
        timeout_sec = 5,
        sockfd;
    struct hostent *he;
    QType qtype = QTypeA;
    struct sockaddr_in server_address;
    bool serverAdressEntered = false,
         is_iterative = false;
    char *iterative_name = NULL;
    char *endPtr;
    struct timeval timeout;

    // Parsovani argumentu
    while ((c = getopt(argc, argv, "hs:T:t:i")) != -1)
    {
        switch (c)
        {
        case 'h':
            printf("Usage ./ipk-lookup [-h] [-s SERVER] [-T timeout] [-t type] [-i] name  "
                   "\n\t-h\tshow this help message and exit\n\t-s\tDNS server\n\t-T\ttimeout in seconds\n"
                   "\t-t\trecord type\n\t-i\titerative resolution\n\tname\tdomain name or adress(IPv4, IPv6)"
                   " if selected type is PTR\n");
            exit(0);
        case 's':
            // Nacteni zadane adresy
            if (serverAdressEntered == false)
            {
                server_address.sin_addr.s_addr = inet_addr(optarg);
            }
            else
            {
                fprintf(stderr, "Server can be entered only one time\n");
                exit(2);
            }
            serverAdressEntered = true;
            break;
        case 'T':
            // Nacteni delky timeoutu
            timeout_sec = (int)strtol(optarg, &endPtr, 10);
            if (*endPtr != '\0')
            {
                fprintf(stderr, "Wrong timeout value\n");
                exit(2);
            }
            break;
        case 't':
            // Nacteni typu
            qtype = get_qtype(optarg);
            if (qtype == 0)
            {
                fprintf(stderr, "Wrong type\n");
                exit(2);
            }
            break;
        case 'i':
            // Nastaveni iterativniho dotazovani
            is_iterative = true;
            break;
        default:
            fprintf(stderr, "Wrong argument\n");
            exit(2);
        }
    }

    DNSRequest request;
    request.header = (DNSHeader *)calloc(1, sizeof(DNSHeader));

    if (request.header == NULL)
    {
        fprintf(stderr, "Problem with allocating\n");
        exit(1);
    }

    request.header->id = htons(1);

    // nastaveni ridicich bajtu
    if (is_iterative)
    {
        request.header->header_settings = 0;
    }
    else
    {
        request.header->header_settings = htons(256);
    }

    request.header->qdcount = htons(1);
    request.header->ancount = 0;
    request.header->nscount = 0;
    request.header->arcount = 0;

    request.question = (DNSQuestion *)calloc(1, sizeof(DNSQuestion));

    if (request.question == NULL)
    {
        fprintf(stderr, "Problem with allocating\n");
        exit(1);
    }

    // Nastaveni dotazu podle toho jestli se jedna o iterativni dotazovani nebo ne
    if (is_iterative == false)
    {
        request.question->q_name = argv[argc - 1];
        request.question->q_type = htons(qtype);
    }
    else
    {
        request.question->q_name = ".";
        request.question->q_type = htons(QTypeNS);
    }

    request.question->q_class = htons(1);

    // Vytvoreni packetu
    char *buffer = create_buffer(request.header, request.question);

    // Vytvoreni socketu
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Can't create socket");
        exit(1);
    }

    // Nastaveni timeoutu
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
        fprintf(stderr, "Socket timeout\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Nastaveni serverove adresy kam se bude posilat pozadavek
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(53);

    // Poslani tokenu
    int rc = sendto(sockfd, buffer, 511, 0, (struct sockaddr *)&server_address, sizeof(server_address));
    memset(buffer, '\0', 512);

    // Prijeti odpovedi
    int addr_length = sizeof(server_address);
    rc = recvfrom(sockfd, buffer, 512, 0, (struct sockaddr *)&server_address, (socklen_t *)&addr_length);

    if (rc <= 0)
    {
        fprintf(stderr, "Timeout\n");
        exit(1);
    }

    DNSRespond respond;
    respond.header = malloc(sizeof(DNSHeader));

    if (respond.header == NULL)
    {
        fprintf(stderr, "Problem with allocating\n");
        exit(1);
    }

    char *address = malloc(256 * sizeof(char));

    if (address == NULL)
    {
        fprintf(stderr, "Problem with allocating\n");
        exit(1);
    }

    // Zpracovani odpovedi
    if (is_iterative == false)
    {
        bool found = parse_buffer(&respond, request.question->q_type, buffer, NULL, false);

        if (found == false)
        {
            free(buffer);
            free(address);
            free(respond.header);
            free(request.header);
            free(request.question);
            exit(1);
        }
    }
    else
    {
        bool found = false;
        Stack stack;

        // Zpracovani odpovedi na root
        if (parse_buffer(&respond, request.question->q_type, buffer, address, true))
        {
            // Priprava packetu pro dotaz na IP adresu name serveru ziskaneho z odpovedi na root
            request.question->q_name = address;
            request.question->q_type = htons(QTypeA);
            free(buffer);
            buffer = create_buffer(request.header, request.question);

            // Poslani dotazu
            int rc = sendto(sockfd, buffer, 511, 0, (struct sockaddr *)&server_address, sizeof(server_address));
            memset(buffer, '\0', 512);
            int addr_length = sizeof(server_address);

            // Ziskani odpovedi
            rc = recvfrom(sockfd, buffer, 512, 0, (struct sockaddr *)&server_address, (socklen_t *)&addr_length);

            if (rc <= 0)
            {
                fprintf(stderr, "Timeout\n");
                exit(1);
            }

            // Zpracovani odpovedi
            if (parse_buffer(&respond, htons(QTypeA), buffer, address, true))
            {
                // Iterativni dotazovani na ziskani pozadove odpovedi
                while (found == false)
                {
                    // Priprava dotazu
                    request.question->q_name = argv[argc - 1];
                    request.question->q_type = htons(qtype);
                    buffer = create_buffer(request.header, request.question);
                    server_address.sin_addr.s_addr = inet_addr(address);

                    // Ulozeni addresy pro hledani ip adres name serveru
                    stack_push(address, &stack);

                    // Poslani dotazu
                    int rc = sendto(sockfd, buffer, 511, 0, (struct sockaddr *)&server_address, sizeof(server_address));
                    memset(buffer, '\0', 512);
                    int addr_length = sizeof(server_address);

                    // Ziskani odpovedi
                    rc = recvfrom(sockfd, buffer, 512, 0, (struct sockaddr *)&server_address, (socklen_t *)&addr_length);

                    if (rc <= 0)
                    {
                        fprintf(stderr, "Timeout\n");
                        exit(1);
                    }

                    // Zpracovani odpovedi
                    ParseBufferStatus status = parse_buffer(&respond, htons(qtype), buffer, address, true);

                    if (status == Found)
                    {
                        free(buffer);
                        free(address);
                        free(respond.header);
                        free(request.header);
                        free(request.question);
                        exit(0);
                    }
                    else if (status == FormatError)
                    {
                        fprintf(stderr, "Format or Permission Error\n");
                        exit(1);
                    }
                    else if (status == MissingIP)
                    {
                        // Pokud se nenajde IP adresa autorativniho name serveru
                        stack_pop(&stack);
                        bool foundIP = false;
                        char *helper_buffer;
                        helper_buffer = malloc(sizeof(char) * strlen(request.question->q_name));

                        if (helper_buffer == NULL)
                        {
                            fprintf(stderr, "Problem with allocating\n");
                            exit(1);
                        }

                        strcpy(helper_buffer, request.question->q_name);
                        strcpy(request.question->q_name, address);
                        request.question->q_type = htons(QTypeA);
                        int pushed = 0;

                        // Hledani IPv4 adresy name serveru
                        while (foundIP == false)
                        {
                            // Pouzije se adresa z vrcholu zasobniku
                            if (stack_top(&stack) != NULL)
                            {
                                server_address.sin_addr.s_addr = inet_addr(stack_top(&stack));
                                free(buffer);
                                buffer = create_buffer(request.header, request.question);

                                // Poslani dotazu
                                int rc = sendto(sockfd, buffer, 511, 0, (struct sockaddr *)&server_address, sizeof(server_address));
                                memset(buffer, '\0', 512);
                                int addr_length = sizeof(server_address);

                                // Ziskani odpovedi
                                rc = recvfrom(sockfd, buffer, 512, 0, (struct sockaddr *)&server_address, (socklen_t *)&addr_length);

                                if (rc <= 0)
                                {
                                    fprintf(stderr, "Timeout\n");
                                    exit(1);
                                }

                                // Zpracovani odpovedi
                                ParseBufferStatus status = parse_buffer(&respond, htons(QTypeA), buffer, address, true);

                                if (status == Found)
                                {
                                    foundIP = true;
                                    strcpy(request.question->q_name, helper_buffer);

                                    // Odstraneni docasnych adress pro hledani IP adresy name serveru
                                    for (int i = 0; i < pushed; i++)
                                    {
                                        stack_pop(&stack);
                                    }
                                    free(helper_buffer);
                                    break;
                                }
                                else if (status == NotFound)
                                {
                                    // Ulozeni docasne adressy ktera muze pomoci pri hledani adresy
                                    stack_push(address, &stack);
                                    pushed++;
                                }
                                else if (status == FormatError)
                                {
                                    // Pokud nastal problem s odpovedi pokracuje na predchozich adressach v zasobniku
                                    stack_pop(&stack);
                                }
                            }
                            else
                            {
                                exit(1);
                            }
                        }
                    }
                }
            }
            else
            {
                fprintf(stderr, "Not found address on name server received from root\n");
                exit(1);
            }
        }
        else
        {
            fprintf(stderr, "Not found IP address of root\n");
            exit(1);
        }
    }

    free(buffer);
    free(address);
    free(respond.header);
    free(request.header);
    free(request.question);

    return 0;
}