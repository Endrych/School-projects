#include "help_function.h"

/**
 * @brief Vytvori packet pro dotaz 
 * 
 * @param dns_header Ukazatel na hlavicku protokolu
 * @param dns_question Ukazatel na dotaz 
 * @return char* Vraci ukazatel na vytvoreny packet ktery se posle na server
 */
char *create_buffer(DNSHeader *dns_header, DNSQuestion *dns_question)
{
    // Alokace pameti
    char *buffer;
    int length;

    buffer = malloc(512 * sizeof(char));

    if (buffer == NULL)
    {
        fprintf(stderr, "Problem with allocating\n");
        exit(1);
    }

    // Ulozeni hlavicky
    memset(buffer, '\0', 512 * sizeof(char));
    memcpy(buffer, &(dns_header->id), sizeof(uint16_t));
    memcpy(&(buffer[2]), &(dns_header->header_settings), sizeof(uint16_t));
    memcpy(&(buffer[4]), &(dns_header->qdcount), sizeof(uint16_t));
    memcpy(&(buffer[6]), &(dns_header->ancount), sizeof(uint16_t));
    memcpy(&(buffer[8]), &(dns_header->nscount), sizeof(uint16_t));
    memcpy(&(buffer[10]), &(dns_header->arcount), sizeof(uint16_t));

    // Ulozeni domenoveho jmena dotazu
    if (dns_question->q_type == htons(QTypePTR))
    {
        char *part = check_and_reverse_ip(dns_question->q_name);
        length = convert_domain_name(part, buffer, 12);
    }
    else
    {
        length = convert_domain_name(dns_question->q_name, buffer, 12);
    }

    // Ulozeni typu a tridy dotazu
    memcpy(&(buffer[13 + length]), &(dns_question->q_type), sizeof(uint16_t));
    memcpy(&(buffer[13 + length + sizeof(uint16_t)]), &(dns_question->q_class), sizeof(uint16_t));

    return buffer;
}

/**
 * @brief Zparsuje odpoved ze serveru do struktury DNSRespond
 * 
 * @param respond Ukazatel na strukturu kde se ukladaji vysledne informace
 * @param type Pozadovany typ odpovedi
 * @param buffer Packet ktery obsahuje vsechny informace
 * @param address Pomocny buffer kam se zapisuje NS adresa pokud se k ni nenajde IP adresa, nebo IP adresa NS adresy
 * @param iterative Informace jestli se jedna o iterativni zpusob
 * @return ParseBufferStatus Navratova hodnota funkce (Nasla se odpoved,  nasel se pouze NS server, nasla se IP NS serveru)
 */
ParseBufferStatus parse_buffer(DNSRespond *respond, QType type, char *buffer, char *address, bool iterative)
{
    uint16_t helper = 0;

    memcpy(&helper, &(buffer[0]), sizeof(uint16_t));
    respond->header->id = htons(helper);

    memcpy(&helper, &(buffer[2]), sizeof(uint16_t));
    helper = htons(helper);
    respond->header->header_settings = helper;

    // Kontrola chyby
    helper = helper << 12;
    helper = helper >> 12;

    if (helper != 0)
    {
        return FormatError;
    }

    // Kontrola jestli nedoslo ke zkraceni zpravy
    helper = respond->header->header_settings;
    helper = helper >> 9;

    if (helper % 2 == 1)
    {
        fprintf(stderr, "Truncated message\n");
        exit(1);
    }

    memcpy(&helper, &(buffer[4]), sizeof(uint16_t));
    respond->header->qdcount = htons(helper);

    memcpy(&helper, &(buffer[6]), sizeof(uint16_t));
    respond->header->ancount = htons(helper);

    memcpy(&helper, &(buffer[8]), sizeof(uint16_t));
    respond->header->nscount = htons(helper);

    memcpy(&helper, &(buffer[10]), sizeof(uint16_t));
    respond->header->arcount = htons(helper);

    int index = 12;

    // Preskoceni domenoveho jmena dotazu
    while (buffer[index] != '\0')
    {
        index++;
    }

    // Preskoceni typu a tridy dotazu
    index += 2 * sizeof(uint16_t) + 1;

    // Parsovani odpovedi
    if (respond->header->ancount != 0)
    {
        DNSAnswer answear;

        for (int i = 0; i < respond->header->ancount; i++)
        {
            index = parse_answer(buffer, index, &(answear));
            print_answer(&answear);

            // Ulozeni vysledku pokud se jedna o iterativni zpusob
            if (answear.type == type && iterative)
            {
                memcpy(address, answear.address, strlen(answear.address) + 1);
            }

            if (answear.type == type)
            {
                return Found;
            }
        }
    }

    // Pokud se nejedna o iterativni dotazovani a nebyl nalezen spravny typ odpovedi tak se prochazeni ukonci
    if (iterative == false)
    {
        return NotFound;
    }

    char *helper_name;
    helper_name = malloc(255 * sizeof(char));

    char *helper_address;
    helper_address = malloc(255 * sizeof(char));

    // Parsovani autorativnich name serveru v pripade iterativniho dotazovani
    if (respond->header->nscount != 0)
    {
        DNSAnswer name_server;

        // Ulozeni prvniho nameserveru
        index = parse_answer(buffer, index, &(name_server));
        strcpy(helper_name, name_server.name);
        remove_dots(helper_name, 0);
        strcpy(helper_address, name_server.address);
        memcpy(address, name_server.address, strlen(name_server.address) + 1);

        // Pruchod zbytkem name serveru
        for (int i = 1; i < respond->header->nscount; i++)
        {
            index = parse_answer(buffer, index, &(name_server));
        }
    }

    // Parsovani doplnujicich informaci
    if (respond->header->arcount != 0)
    {
        DNSAnswer answear;

        for (int i = 0; i < respond->header->arcount; i++)
        {
            index = parse_answer(buffer, index, &(answear));

            // Hledani IPv4 name serveru Adresy
            if (answear.type == htons(QTypeA))
            {
                //Prevedeni domenove jmena na teckovou notaci
                remove_dots(answear.name, 0);
                printf("%s IN NS %s\n", helper_name, answear.name);
                print_answer_without_removing_dots(&(answear));
                memcpy(address, answear.address, strlen(answear.address) + 1);
                return NotFound;
            }
        }
    }

    // Pokud se nasel Name server ale ne IP adresa serveru
    if (respond->header->nscount != 0)
    {
        printf("%s IN NS %s\n", helper_name, helper_address);
        return MissingIP;
    }

    return NotFound;
}

/**
 * @brief Funkce parsuje zaznamy v sekcich odpovedi, name servery, doplnujici informace
 * 
 * @param buffer Pole ze ktereho se parsuji hodnoty 
 * @param index Misto kde se zacina parsovat 
 * @param answear Ukazatel na strukturu kam se ukladaji informace
 * @return int Index kde konci zaznam
 */
int parse_answer(char *buffer, int index, DNSAnswer *answear)
{
    memset(answear, '\0', sizeof(answear));
    answear->name = (char *)calloc(255, sizeof(char));

    if (answear->name == NULL)
    {
        fprintf(stderr, "Problem with allocating\n");
        exit(1);
    }

    answear->address = (char *)calloc(255, sizeof(char));

    if (answear->address == NULL)
    {
        fprintf(stderr, "Problem with allocating\n");
        exit(1);
    }

    // Parsovani domenoveho jmena
    index = parse_address_ptr(buffer, index, answear->name);

    memcpy(&(answear->type), &(buffer[index]), sizeof(uint16_t));
    index += 2;

    memcpy(&(answear->an_class), &(buffer[index]), sizeof(uint16_t));
    index += 2;

    memcpy(&(answear->time_to_live), &(buffer[index]), sizeof(int));
    index += 4;

    memcpy(&(answear->data_length), &(buffer[index]), sizeof(uint16_t));
    index += 2;

    // Parsovani datove casti podle typu zaznamu
    if (htons(answear->type) == QTypeA)
    {
        parse_address_a(&(buffer[index]), answear);
        index += 4;
    }
    else if (htons(answear->type) == QTypeAAAA)
    {
        parse_address_aaaa(buffer, index, htons(answear->data_length), answear);
        index += htons(answear->data_length);
    }
    else if (htons(answear->type) == QTypeCNAME)
    {
        index = parse_address_ptr(buffer, index, answear->address);
        remove_dots(answear->address, 0);
    }
    else if (htons(answear->type) == QTypePTR)
    {
        index = parse_address_ptr(buffer, index, answear->address);
        remove_dots(answear->address, 0);
    }
    else if (htons(answear->type) == QTypeNS)
    {
        index = parse_address_ptr(buffer, index, answear->address);
        remove_dots(answear->address, 0);
    }
    else
    {
        fprintf(stderr, "Not supported in answear\n");
        exit(1);
    }

    return index;
}

/**
 * @brief Parsovani adresy typu AAAA (IPv6)
 * 
 * @param buffer Buffer ze ktereho se adresa parsuje
 * @param index Zacatek odkud se parsuji data
 * @param data_length Delka dat 
 * @param answear Ukazatel na strukturu zaznamu kam se zapisuje adresa
 */
void parse_address_aaaa(char *buffer, int index, int data_length, DNSAnswer *answear)
{
    uint16_t i = 0;
    int string_index = 0;

    memcpy(&i, &(buffer[index]), sizeof(uint16_t));
    i = htons(i);
    index += 2;
    int zeroes = 0;

    if (i != 0)
    {
        string_index += sprintf(&(answear->address[string_index]), "%x", i);
    }

    for (int j = 1; j < data_length / 2; j++)
    {
        // Kontrola aby se nevytiskli zasebou 3 :
        if (zeroes != 2)
        {
            string_index += sprintf(&(answear->address[string_index]), ":");
        }
        else
        {
            zeroes = 0;
        }

        i = 0;
        memcpy(&i, &(buffer[index]), sizeof(uint16_t));
        i = htons(i);
        index += 2;

        if (i != 0)
        {
            string_index += sprintf(&(answear->address[string_index]), "%x", i);
        }
        else
        {
            zeroes++;
        }
    }
}

/**
 * @brief Parsovani domenoveho jmena 
 * 
 * @param buffer Buffer odkud se bude parsovat
 * @param index  Zacatek odkud se parsuji data 
 * @param output Ukazatel na misto kam se zapisuje domenove jmeno
 * @return int Index kde konci domenove jmeno
 */
int parse_address_ptr(char *buffer, int index, char *output)
{
    bool routed = false;
    int helper_index = index;
    int string_index = 0;
    unsigned char c;

    // Situace kdy by se jednalo o .
    if (buffer[index] == '\0')
    {
        return index + 1;
    }

    while ((c = buffer[index]) != '\0')
    {
        // Odchyceni ukazatele
        if (c >= 192)
        {
            // Kontrola jestli uz nebyl pouzit ukazatel
            if (routed == false)
            {
                helper_index = index + 2;
                routed = true;
            }

            // Vypocet noveho indexu
            unsigned int nextIndex = c - 192;
            nextIndex = nextIndex << 8;
            nextIndex += buffer[index + 1];
            index = nextIndex;
            continue;
        }

        output[string_index] = c;
        string_index++;
        index++;
    }

    if (routed == true)
    {
        return helper_index;
    }
    else
    {
        return index + 1;
    }
}

/**
 * @brief Parsovani Ipv4 adresy
 * 
 * @param buffer Buffer odkud se bude parsovat adresa
 * @param answear Ukazatal na strukturu kde se bude ukladat adresa
 */
void parse_address_a(char *buffer, DNSAnswer *answear)
{
    int index = 0;
    int string_index = 0;
    unsigned char c;

    for (int i = 0, length = htons(answear->data_length) - 1; i < length; i++)
    {
        c = buffer[index];
        string_index += sprintf(&(answear->address[string_index]), "%d.", c);
        index++;
    }

    c = buffer[index];
    string_index += sprintf(&(answear->address[string_index]), "%d", c);
}

/**
 * @brief Prevod domenoveho jmena z teckove notace do bufferu
 * 
 * @param domain_name Domenove jmeno
 * @param buffer Buffer do ktereho se jmeno zapisuje 
 * @param start Odkud se zacina v bufferu
 * @return int Vraci delku zapsanych dat
 */
int convert_domain_name(char *domain_name, char *buffer, int start)
{
    int length_index = start;
    char length = 0;
    int index = 0;
    int c;
    bool root = true;

    while ((c = domain_name[index]) != '\0')
    {
        if (c == '.')
        {
            // Delka subdomeny nesmi byt 64 a vice
            if (length > 63)
            {
                fprintf(stderr, "Maximal length of subdomen is 63 chars\n");
                exit(2);
            }

            buffer[length_index] = (char)length;
            length_index = start + index + 1;
            length = 0;
        }
        else
        {
            buffer[start + index + 1] = domain_name[index];
            root = false;
            length++;
        }

        index++;
    }

    // Delka subdomeny nesmi byt 64 a vice
    if (length > 63)
    {
        fprintf(stderr, "Maximal length of subdomen is 63 chars\n");
        exit(2);
    }

    buffer[length_index] = length;

    if (length)
    {
        return index + 1;
    }
    else if (root == false)
    {
        return index;
    }
    else
    {
        return index - 1;
    }
}

/**
 * @brief Ziskani typu z retezce
 * 
 * @param optarg retezec obsahujici typ 
 * @return QType Vysledny typ
 */
QType get_qtype(char *optarg)
{
    if (strcmp(optarg, "A") == 0)
    {
        return QTypeA;
    }
    else if (strcmp(optarg, "AAAA") == 0)
    {
        return QTypeAAAA;
    }
    else if (strcmp(optarg, "CNAME") == 0)
    {
        return QTypeCNAME;
    }
    else if (strcmp(optarg, "NS") == 0)
    {
        return QTypeNS;
    }
    else if (strcmp(optarg, "PTR") == 0)
    {
        return QTypePTR;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief Zkontroluje jestli se jedna o IPv4 nebo IPv6 adresu a podle toho nasledne adrese prevede do bufferu
 * 
 * @param ip Vstupni retezec ktery by mel obsahovat IP adresu
 * @return char* Prevedena adresa
 */
char *check_and_reverse_ip(char *ip)
{
    bool is_ipv4 = true;
    bool is_ipv6 = true;
    char buffer[16];

    if (inet_pton(AF_INET, ip, buffer))
    {
        return reverse_ipv4(ip);
    }

    else if (inet_pton(AF_INET6, ip, buffer))
    {
        return reverse_ipv6(ip);
    }
    else
    {
        fprintf(stderr, "Wrong address %s\n", ip);
        exit(2);
    }
}

/**
 * @brief Prevede IPv4 adresu do bufferu
 * 
 * @param ipv4 Vstupni adresa, predpoklada se prosla kontrolou pomoci funkce check_and_reverse_ip
 * @return char* Prevedena adresa
 */
char *reverse_ipv4(char *ipv4)
{
    char *buffer;
    int adress[4];

    buffer = (char *)malloc(29 * sizeof(char));

    if (buffer == NULL)
    {
        fprintf(stderr, "Problem with allocating\n");
        exit(1);
    }

    sscanf(ipv4, "%d.%d.%d.%d", &(adress[0]), &(adress[1]), &(adress[2]), &(adress[3]));
    sprintf(buffer, "%d.%d.%d.%d.in-addr.arpa", adress[3], adress[2], adress[1], adress[0]);
    return buffer;
}

/**
 * @brief Prevede IPv6 adresu do bufferu
 * 
 * @param ipv6 Vstupni adresa, predpoklada se prosla kontrolou pomoci funkce check_and_reverse_ip
 * @return char* Prevedena adresa
 */
char *reverse_ipv6(char *ipv6)
{
    char *buffer;

    buffer = (char *)malloc(72 * sizeof(char));

    if (buffer == NULL)
    {
        fprintf(stderr, "Problem with allocating\n");
        exit(1);
    }

    int length = strlen(ipv6);
    int part_length = 0;
    int part_count = 0;
    int index = 0;

    for (int i = length - 1; i >= 0; i--)
    {
        if (ipv6[i] != ':')
        {
            part_length++;
            buffer[index] = ipv6[i];
            buffer[index + 1] = '.';
            index += 2;
        }
        else
        {
            if (part_length == 0)
            {

                for (int j = 0; j < i; j++)
                {

                    if (ipv6[j] == ':')
                    {
                        part_count++;
                    }
                }

                for (int k = 0; k < 7 - part_count; k++)
                {

                    for (int l = 0; l < 4; l++, index += 2)
                    {
                        buffer[index] = '0';
                        buffer[index + 1] = '.';
                    }
                }
            }
            else
            {

                for (part_length; part_length < 4; part_length++)
                {
                    buffer[index] = '0';
                    buffer[index + 1] = '.';
                    index += 2;
                }

                part_length = 0;
                part_count++;
            }
        }
    }

    sprintf(&(buffer[index]), "ip6.arpa");
    return buffer;
}

/**
 * @brief Vypise tridu na vystup
 * 
 * @param an_class Trida ktera se ma vypsat vystup 
 */
void print_class(uint16_t an_class)
{
    if (an_class == ClassIN)
    {
        printf("IN");
    }
    else if (an_class == ClassCH)
    {
        printf("CH");
    }
    else if (an_class == ClassHS)
    {
        printf("HS");
    }
    else if (an_class == ClassALL)
    {
        printf("ALL");
    }
}

/**
 * @brief Vypise typ na vystup
 * 
 * @param an_class Typ ktera se ma vypsat vystup 
 */
void print_type(uint16_t type)
{
    if (type == QTypeA)
    {
        printf("A");
    }
    else if (type == QTypeAAAA)
    {
        printf("AAAA");
    }
    else if (type == QTypeCNAME)
    {
        printf("CNAME");
    }
    else if (type == QTypeNS)
    {
        printf("NS");
    }
    else if (type == QTypePTR)
    {
        printf("PTR");
    }
    else if (type == QTypeSOA)
    {
        printf("SOA");
    }
}

/**
 * @brief Prevede domenove jmeno do teckove notace
 * 
 * @param buffer Buffer ve kterem se ma prevest domenove jmeno
 * @param index Index odkud se ma v bufferu zacit
 */
void remove_dots(char *buffer, int index)
{
    int length = buffer[index];

    // Pripad rootu
    if (length == 0)
    {
        buffer[index] = '.';
    }

    index++;

    while (length != 0)
    {
        for (int i = length; i > 0; i--)
        {
            buffer[index - 1] = buffer[index];
            index++;
        }

        length = buffer[index];

        if (length != 0)
        {
            buffer[index - 1] = '.';
        }
        else
        {
            buffer[index - 1] = '\0';
        }

        index++;
    }
}

/**
 * @brief Funkce vypise zaznam na terminal, provadi predtim jeste funkci ktera prevadi domenove jmeno na teckovou notaci
 * 
 * @param answear Zaznam ktery se ma vypsat
 */
void print_answer(DNSAnswer *answear)
{
    remove_dots(answear->name, 0);
    printf("%s ", answear->name);
    print_class(htons(answear->an_class));
    putchar(' ');
    print_type(htons(answear->type));
    printf(" %s\n", answear->address);
}

/**
 * @brief Funkce vypise zaznam na terminal
 * 
 * @param answear Zaznam ktery se ma vypsat
 */
void print_answer_without_removing_dots(DNSAnswer *answear)
{
    printf("%s ", answear->name);
    print_class(htons(answear->an_class));
    putchar(' ');
    print_type(htons(answear->type));
    printf(" %s\n", answear->address);
}

/**
 * @brief Funkce vraci adresu z vrcholu zasobniku
 * 
 * @param stack Zasobnik
 * @return char* Adresa ulozena v polozce na vrcholu zasobniku
 */
char *stack_top(Stack *stack)
{
    if (stack != NULL)
    {
        if (stack->Top != NULL)
        {
            return stack->Top->address;
        }
        else
        {
            return NULL;
        }
    }
}

/**
 * @brief Funkce prida na vrchol zasobniku adresu
 *  
 * @param address Adresa ktera se prida na vrchol zasobniku
 * @param stack Zasobnik
 */
void stack_push(char *address, Stack *stack)
{
    if (address != NULL && stack != NULL)
    {
        struct StackItem *item;
        item = malloc(sizeof(struct StackItem));

        if (item == NULL)
        {
            fprintf(stderr, "Problem with allocating\n");
            exit(1);
        }

        item->address = malloc(256 * sizeof(char));

        if (item->address == NULL)
        {
            fprintf(stderr, "Problem with allocating\n");
            exit(1);
        }

        strcpy(item->address, address);

        if (stack->Top == NULL)
        {
            stack->Top = item;
        }
        else
        {
            item->next = stack->Top;
            stack->Top = item;
        }
    }
}

/**
 * @brief Funkce zrusi polozku na vrcholu zasobniku
 * 
 * @param stack Zasobnik
 */
void stack_pop(Stack *stack)
{
    if (stack != NULL)
    {
        if (stack->Top != NULL)
        {
            struct StackItem *item;
            item = stack->Top;
            stack->Top = item->next;

            free(item->address);
            free(item);
        }
    }
}