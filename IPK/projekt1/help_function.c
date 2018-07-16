#include "help_function.h"

/**
 * @brief Funkce prevede status kod a delku dat do bufferu
 * 
 * @param number_code Status kod 
 * @param data_length Delka dat
 * @param binary Buffer
 */
void convert_header_to_binary(unsigned char number_code, int data_length, unsigned char * binary)
{
    // Kontrola rozsahu Status kodu
    if(number_code >=  16)
    {
        perror("Number code must be smaller than 64");
        exit(EXIT_FAILURE);
    }

    // Ulozeni do hornich 4 bitu
    binary[0] = number_code;
    binary[0] = binary[0] << 4;

    // Kontrola maximalni delky dat
    if(data_length >= 4096)
    {
        perror("Data length must be smaller than 1024");
        exit(EXIT_FAILURE);
    }

    unsigned char rest_data_length = 0;
    int upper_limit = 2048;
    int part_data_length = 8;

    //Omezeni na 12 bitu
    for(int i=0;i<4;i++){
        if(data_length > upper_limit){
            rest_data_length += part_data_length;
            data_length -= upper_limit;
        }
        upper_limit/=2;
        part_data_length /= 2;
    }

    // Ulozeni hornich 4 bitu do prvni bajtu bufferu
    binary[0] |= rest_data_length;

    //Ulozeni zbytku delky dat do druheho bajtu
    binary[1] = (unsigned char) data_length;
}

/**
 * @brief Funkce prevede z bufferu status kod a delku dat
 * 
 * @param number_code Ukazatel kam se zapise status kod
 * @param data_length Ukazatel kam se se zapise delka dat
 * @param binary Buffer s potrebnymi daty
 */
void convert_header_from_binary(unsigned char * number_code, int * data_length, unsigned char * binary)
{
    *number_code = 0;
    *data_length = 0;

    // Ziskani hodnoty z poslednich 4 bitu prvniho bajtu
    int rest_data_length = 0;
    int part_data = 256;
    for(int i=0;i<4;i++){
        if(binary[0] % 2 == 1){
            rest_data_length += part_data;
        }   
        part_data *= 2;
        binary[0] = binary[0] >>1;
    }

    // Ulozeni hodnot
    *number_code = binary[0];
    *data_length = (int) binary[1] + rest_data_length;
}

/**
 * @brief Funkce pripravy prvni buffer ktery se ma poslat na server
 * Pri cteni ze serveru se ulozi status kod, nulova delka dat a nazev souboru
 * Pri zapisu na server se navic ulozi delka prvnich nacteni dat a samotna data 
 * @param number_code Status kod
 * @param filename Nazev souboru
 * @param buffer Buffer do ktereho se zapisuje
 * @param buffer_length Delka bufferu
 * @param input file description pro soubor ze ktereho se ctou data
 * @return int Delka zapsanych dat
 */
int prepare_first_buffer(unsigned char number_code, char* filename, char * buffer, int buffer_length, FILE * input)
{
    // Vynulovani bufferu
    memset(buffer,'0', buffer_length);

    if(number_code == STATUS_BEGIN_READ)
    {
        convert_header_to_binary(number_code, 0, buffer);
        int length = set_filename_to_binary(filename, buffer);
        return 2 + length;
    }
    else if(number_code == STATUS_BEGIN_WRITE)
    {
        int length = set_filename_to_binary(filename, buffer);
        int data_length = write_from_file_to_binary(input, buffer, 2 + length,4094 - length);
        convert_header_to_binary(STATUS_BEGIN_WRITE,data_length,buffer);
        return 2 + length + data_length;
    }
    else{
        perror("Wrong status code\n");
        exit(EXIT_FAILURE);
    }
}


/**
 * @brief Ulozi nazev souboru a delku nazvu do bufferu
 * 
 * @param filename Nazev souboru
 * @param buffer Buffer pro zapis
 * @return int Vraci delku ulozenych dat
 */
int set_filename_to_binary(char * filename, char* buffer)
{
    unsigned char filename_size = (unsigned char) strlen(filename);
    buffer[2] = filename_size;
    memcpy(&(buffer[3]),filename,filename_size);
    return 1 + filename_size;
}

/**
 * @brief Nacte se nazev souboru z bufferu
 * 
 * @param buffer Buffer ve kterem jsou ulozena data
 * @return char* Nazev souboru
 */
char * get_filename_from_binary(char * buffer)
{
    unsigned char filename_size = buffer[2];
    unsigned char * filename;
    filename = (unsigned char*) malloc(sizeof(char) * filename_size);
    if(filename == NULL)
    {
        perror("Error with malloc");
        exit(EXIT_FAILURE);
    }
    memcpy(filename, &(buffer[3]),filename_size);
    return filename;
}

/**
 * @brief Zapis dat ze souboru do bufferu
 * 
 * @param file_descriptor File decriptor na soubor do ktereho se zapisuje
 * @param buffer Buffer do ktereho se data zapisuji
 * @param start Index od kteho se zapisuje do bufferu
 * @param max_length Maximalni delka dat k zapsani
 * @return int Delka zapsanych dat
 */
int write_from_file_to_binary(FILE * file_descriptor, char * buffer, int start, int max_length)
{
    int c;
    for(int i=0;i<max_length;i++)
    {
        c = getc(file_descriptor);
        if(c == EOF)
        {
            return i;
        }
        buffer[start + i] = c;
    }
    return max_length;
    
}

/**
 * @brief Funkce zahashuje retezec na unsigned long
 * 
 * @param str Retezec ktery se ma heshovat
 * @return unsigned long Vysledny hash
 */
unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash;
}

/**
 * @brief Funkce vygeneruje cestu k souboru pokud neegistuje
 * 
 * @param path cesta k souboru
 */
void generate_path_to_file(char * path)
{
    // Pomocne buffery
    char buffer[4097] = {0};
    char buffer1[4097] = {0};
    char dir_path[4097] = {0};

    strcpy (buffer, path);
    
    // Ziskani cesty k souboru
    strcpy(dir_path,dirname(buffer));

    // Postupna kontrola existence hierarchie adresaru, jinak se vygeneruji
    char *pch = strtok (dir_path,"/");
    int index = 0;
    while (pch != NULL)
    {
        struct stat st = {0};
        strcpy(&buffer1[index],pch);
        index += strlen(pch);
        buffer1[index]='/';
        index++;
        if(strcmp(pch,".")==0){
            pch = strtok (NULL, "/");
            continue;
        }
        else{
            pch = strtok (NULL, "/");
        }
        if (stat(buffer1, &st) == -1) {
            if(mkdir(buffer1, 0700)!=0){
                exit(EXIT_FAILURE);
            }
        }
    }
}