#ifndef HELP_FUNCTION_H
#define HELP_FUNCTION_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>

/**
 * @brief Struktura reprezentujici hlavicku DNS protokolu
 * id Identifikator 
 * header_settings Ridici informace
 * qdcount Pocet dotazu
 * nscount Pocet zaznamu v sekci autoritativnich name serveru
 * arcount Pocet zaznamu v sekci doplnujicich informaci
 */
struct DNSHeader
{
    uint16_t id;
    uint16_t header_settings;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} typedef DNSHeader;

/**
 * @brief Struktura reprezentujici zaznam v sekci dotazu
 * q_name Domenove jmeno
 * q_type Typ dotazu 
 * q_class Trida dotazu
 */
struct DNSQuestion
{
    char *q_name;
    uint16_t q_type;
    uint16_t q_class;
} typedef DNSQuestion;

/**
 * @brief Struktura reprezentujici zaznam v sekcich odpoved, autorativni name servery, doplnujici informace
 * name Domenove jmeno
 * type Typ vety
 * an_class Trida vety
 * time_to_live Doba platnosti
 * data_length Delka datove casti
 * address Datova cast (domenove jmeno nebo IP adresa)
 */
struct DNSAnswer
{
    char *name;
    uint16_t type;
    uint16_t an_class;
    unsigned int time_to_live;
    uint16_t data_length;
    char *address;
} typedef DNSAnswer;

/**
 * @brief Struktura reprezentujici odpoved ze serveru
 * header Ukazatel na zahlavi DNS protokolu
 * question Ukazatel na strukturu reprezentujici zaznamy v sekci dotaz
 * answears Ukazatel na strukturu reprezentujici zaznamy v sekci odpovedi
 * name_servers Ukazatel na strukturu reprezentujici zaznamy v sekci autorativnich name serveru
 * additional_records Ukazatel na strukturu reprezentujici zaznamy v sekci doplnujicich informaci
 */
struct DNSRespond
{
    DNSHeader *header;
    DNSQuestion *question;
    DNSAnswer *answears;
    DNSAnswer *name_servers;
    DNSAnswer *additional_records;
} typedef DNSRespond;

/**
 * @brief Struktura reprezentujici pozadavek na server
 * header Ukazatel na zahlavi protokolu
 * question Ukazatel na strukturu reprezentujici zaznamy v sekci dotaz
 */
struct DNSRequest
{
    DNSHeader *header;
    DNSQuestion *question;
} typedef DNSRequest;

/**
 * @brief Typ veti/dotazu
 */
enum QType
{
    QTypeA = 1,
    QTypeNS = 2,
    QTypeCNAME = 5,
    QTypeSOA = 6,
    QTypePTR = 12,
    QTypeAAAA = 28
} typedef QType;

/**
 * @brief Trida veti/dotazu
 */
enum QClass
{
    ClassIN = 1,
    ClassCH = 3,
    ClassHS = 4,
    ClassALL = 255
} typedef QClass;

/**
 * @brief Vystup z funkce parse_buffer
 */
enum ParseBufferStatus
{
    NotFound = 0,
    Found = 1,
    MissingIP = 2,
    FormatError = 3
} typedef ParseBufferStatus;

/**
 * @brief Polozka zasobniku
 * address Adresa serveru na ktere se dotazuje
 * next Ukazatel na dalsi polozku zasobniku
 */
struct StackItem
{
    char *address;
    struct StackItem *next;
};

/**
 * @brief Zasobnik adres na ktere se bude dotazovat
 * Top Ukazatel na vrchol zasobniku
 */
struct Stack
{
    struct StackItem *Top;
} typedef Stack;

QType get_qtype(char *optarg);
char *create_buffer(DNSHeader *dns_header, DNSQuestion *dns_question);
int convert_domain_name(char *domain_name, char *buffer, int start);
char *check_and_reverse_ip(char *ip);
char *reverse_ipv4(char *ipv4);
char *reverse_ipv6(char *ipv6);
ParseBufferStatus parse_buffer(DNSRespond *respond, QType type, char *buffer, char *address, bool iterative);
int parse_answer(char *buffer, int index, DNSAnswer *answear);
void print_class(uint16_t an_class);
void print_type(uint16_t type);
void parse_address_a(char *buffer, DNSAnswer *answear);
int parse_address_ptr(char *buffer, int index, char *answear);
void parse_address_aaaa(char *buffer, int index, int data_length, DNSAnswer *answear);
void remove_dots(char *buffer, int index);
void iterative_question(DNSRespond *respond, QType qtype, char *request_name, char *buffer);
void print_answer(DNSAnswer *answear);
void stack_push(char *address, Stack *stack);
void stack_pop(Stack *stack);
char *stack_top(Stack *stack);
void print_answer_without_removing_dots(DNSAnswer *answear);
#endif