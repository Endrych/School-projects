/*
 * Soubor:  proj1.c
 * Datum:   Řijen 2016
 * Autor:   David Endrych
 * Projekt: Prace s textem
 * Popis: Program buď to binární data formátuje do textové podoby nebo textovou podobu dat převádí do binární podoby. V případě převodu binárních	dat	na text bude výstupní formát obsahovat adresy vstupních bajtů, hexadecimální kódování a textovou reprezentaci obsahu. V případě převodu textu do binární podoby je na vstupu očekáváné hexadecimální kódování bajtů. 
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define WITHOUT_MAX_LENGHT -1 // Definice makra hodnoty pokud není zadána maximální délka řetězce 

// Deklerace funkcí
void binary_to_text(int start_index, int max_lenght);
void to_hexa();
int reverse();
void binary_sequence(int min_lenght);
int text_to_number(char string[]);

int main (int argc, char *argv[])
{
	int help = 0; // Proměná, která určuje jestli se má vypsat nápověda
	
	//Detekce argumentů
	if(argc == 1)
	{
		binary_to_text(0,WITHOUT_MAX_LENGHT);
	}
	else if(argc == 2)
	{
		if(argv[1][0] == '-')
		{
			if(argv[1][1] == 'x' && argv[1][2]== '\0')	// Zjištění argumenu -x
				to_hexa();
			
			else if(argv[1][1] == 'r' && argv[1][2]== '\0') // Zjištění argumentu -r
			{
				if(reverse() == -1)
					return -1;
			}
			else 
				help = 1;
		}
		else
			help = 1;
	}
	else if(argc==3)
	{
		if(argv[1][0] == '-' && argv[1][1] == 'S') // Zjištění argumentů -S N
		{
			int min_lenght = text_to_number(argv[2]);
			if(min_lenght > 0 && min_lenght < 200)
				binary_sequence(min_lenght);
			else
			{
				fprintf(stderr,"Minimální délka řetězce je v rozmezí 1-199!\n");
				return -1;
			}			
		}		
		else if(argv[1][0] == '-' && argv[1][1] == 's') // Zjištění argumentu -s M
		{
			int start_index = text_to_number(argv[2]);
			if(start_index >= 0)
				binary_to_text(start_index,WITHOUT_MAX_LENGHT);
			else
				return -1;
		}
		else if(argv[1][0] == '-' && argv[1][1] == 'n') // Zjištění argumentu -n N
		{
			int max_lenght = text_to_number(argv[2]);
			if(max_lenght > 0) 
				binary_to_text(0,max_lenght);
			else
			{
				fprintf(stderr, "Maximální délka musí být alespoň 1 znak!\n");
				return -1;
			}			
		}
		else
			help = 1;
	}
	else if(argc == 5)
	{
		int start_index = 0;
		int max_lenght = 0;
		if(argv[1][0] == '-' && argv[1][1] == 's' && argv[3][0] == '-' && argv[3][1] == 'n') // Zjištění argumentu -s M -n N
		{
			start_index = text_to_number(argv[2]);
			max_lenght = text_to_number(argv[4]);
			if(start_index >= 0)
			{
				if(max_lenght > 0)
				{
					if(start_index < 0)
						return -1;
					else
						binary_to_text(start_index,max_lenght);	
				}
				else
				{
					fprintf(stderr, "Maximální délka musí být alespoň 1 znak!\n");
					return -1;
				}			
			}		
		}
		else if(argv[1][0] == '-' && argv[1][1] == 'n' && argv[3][0] == '-' && argv[3][1] == 's') // Zjištění argumentu -n N -s M
		{
			start_index = text_to_number(argv[4]);
			max_lenght = text_to_number(argv[2]);
			if(start_index >= 0)
			{
				if(max_lenght > 0)
				{
					if(start_index < 0)
						return -1;
					else
						binary_to_text(start_index,max_lenght);	
				}
				else
				{
					fprintf(stderr, "Maximální délka musí být alespoň 1 znak!\n");
					return -1;
				}
				
			}			
		}
		else
			help = 1;
	}
	else
		help = 1;
// Výpis nápovědy 
if(help)
		printf("Program převádí buď to binární data do textové podoby nebo textovou podobu dat převádí do binární podoby. V případě převodu textu do binární podoby program očekává na vstupu hexadecimální kódování bajtů.\nProgram muže být spuštěn buď to: \n\tBez argumentů: Výstupní formát se skládá z posloupnosti řádků, kde každý řádek popisuje jednu sérii 16 bajtů ze vstupního souboru.\n\tS argumnty -s M nebo -n N: Přepínáč -s definuje na které adrese má program začít. Přepínač -n definuje maximální délku vstupních bajtů ke zpracování.\n\tS argumentem -x: Veškerá vstupní data budou převedena do hexadecimální podoby na jeden řádek.\n\tS argumentem -S N: Každý řetězec je vytištěn na jeden řádek. Řetězec je nejdelší posloupnost tisknutelných a prázdných znaků, jejíž délka je větší nebo rovna N znaků. N udává celé číslo v intervalu 0 < N < 200.\n\tS argumentem -r: Očekává se na vstupu sekvence hexadecimálních číslic a ty se převádí do binárního formátu.\n");
	return 0;
}

 /** 
	*Veškerá vstupní data budou převedena do hexadecimální podoby na jeden řádek.
	*/
void to_hexa()
{
	int c = ' ';
	//Postupné načítání znaků
	while((c = getchar())!= EOF)
	{
		printf("%.2x",c); // Výpis znaků hexadecimálně
	}
	putchar('\n');
}

 /**
	*Funkce tiskne posloupnost řádků, kde každý řádek popisuje jednu sérii 16 bajtů ze vstupu
	*@param start_index definuje, na které adrese má výpis začínat
	*@param max_lenght definuje maximální délku řetězce
	*/
void binary_to_text(int start_index, int max_lenght)
{
	int i = start_index; // Vytvoří pomocnou proměnou i, která se deklaruje na začínající adresu
	int end = 0; // Vytvoří proměnou, která určuje jestli se už načetli všechny znaky na vstupu
	/* Cyklus, který překročí znaky podle parametru start_index  */
	for(int x = 0; x < start_index; x++)
	{
		if(getchar() == EOF)
		{
			end = 1;
			break;
		}
	}
	
	while(!end)
	{
		int hx[16]; // Vytvoření pole pro načtení série 16 bajtů	
		int current_lenght = 1; // Určuje pořadí v aktuální sérii bajtů
		int instant_end = 0; // Proměná, které určuje jestli určuje jestli je první znak série EOF 
		// Cyklus, který načítá sérii 16 bajtů do pole hx
		for(int j = 0;j<16;j++)
		{
			if(!end) 
			{
				int c = getchar();
				if(c == EOF || ((start_index + max_lenght < current_lenght + i) && max_lenght != WITHOUT_MAX_LENGHT)) // Je znak EOF nebo je délka bude větší než max. délka řetězce
				{
					end = 1; 
					if(j==0) // Pokud se jedná o první iteraci cyklu dojde k nastavení proměné instant_end na hodnotu 1
						instant_end = 1;
				}	
				hx[j] = c;
				current_lenght++;
			}
			else
				hx[j] = ' ';		
		}
		if(instant_end) // Pokud se hodnota proměné instant_end rovná 1, dojde k přerušení cyklu
			break;
		printf("%.8x  ",i);
		int status = 1; // Proměná určuje jestli se už narazilo na EOF nebo byla dosažena maximální délka řetězce
		// Cyklus pro výpis bajtů hexadecimálně
		for(int k = 0;k<16;k++)
		{
			if(hx[k] != EOF && status && ((start_index + max_lenght > k + i) || max_lenght == WITHOUT_MAX_LENGHT)) // Zjištění jestli se nedosáhlo EOF nebo maximální délky 
				printf("%.2x ",hx[k]);
			else 
			{
				status = 0;
				printf("   ");		
			}				
			if(k == 7)
				putchar(' ');
		}
		printf(" |");
		for(int l = 0;l<16;l++)
		{
			if(isprint(hx[l]) && ((start_index + max_lenght > l + i) || max_lenght == WITHOUT_MAX_LENGHT)) // Testování tisknutelných znaků
				putchar(hx[l]);
			else if(hx[l]!= EOF && ((start_index + max_lenght > l + i) || max_lenght == WITHOUT_MAX_LENGHT)) // Pokud netisknutelný znak není EOF nebo se nedosáhlo maximální délky
				putchar('.');
			else
				putchar(' ');
		}
		printf("|\n");
		i+=16;
	}
}

 /** 
	*Očekává se na vstupu sekvence hexadecimálních číslic a ty se převádí do binárního formátu. Bílé znaky program ignoruje.
	*/
int reverse()
{
	int c = ' '; 
	int current_index_of_char = 0; // Pořadí načteného znaku
	int hexa_number[2]; // Pole pro načítání hexadecimálních čísel 
	
	// Postupné načítání znaků dokud se nedojde na konec
	while((c = getchar())!= EOF)
	{	
		if(isblank(c) || !isprint(c)) //Pokud je znak bílý nebo je netisknutelný tak cyklus ukončí aktuální iteraci
			continue;
		
		if(('0' <= c) && (c <= '9')) // Určení čísla podle daného znaku, pokud se nejedná o znak v rozmezí 0-9, a-f, nebo A-F dojde k výpisu chyby a následnému ukončení programu
			c = c - '0';
		else if(('a' <= c) && (c <= 'f'))
			c = c - 'a' + 10;
		else if(('A' <= c) && (c <= 'F'))
			c = c - 'A' + 10;		
		else
		{
			fprintf(stderr,"Nejedná se o hexadecimální číslo!\n");
			return -1;
		}	
		if(current_index_of_char%2 == 0) // Pokud je aktuální pořadí dělitelné 2 uloží číslo na nultý index pole
			hexa_number[0] = c;
		else // Jinak číslo uloží na index 1
		{
			hexa_number[1] = c;
			putchar(hexa_number[0]*16+hexa_number[1]); // Vytiskne se daný znak	
		}
	  current_index_of_char++;
	}
	if(current_index_of_char%2==1) //Pokud je na konci pouze jeden znak, tak se převede hodnota z nultého indexu na první index a na nultý index se přiřadí hodnota nula 
	{
		hexa_number[1] = hexa_number[0];
		hexa_number[0] = 0;
		putchar(hexa_number[0]*16+hexa_number[1]);
	}
	return 0;
}

 /**
 	*Funkce vypysuje řetězce, které jsou delší než zadaná minimální délka, řetězce jsou posloupnosti tisknutelných a prázdných znaků
 	*@param min_lenght Udává minimální délku řetězce, který se má vypsát
 	*/
void binary_sequence(int min_lenght)
{
	char buffer[199]; // Vytvoří pole, do kterého se ukládají aktuální řetězce
	int lenght = 0; // Aktuální délka řetězce
	int c = ' ';
	// Cyklus, který postupně načítá znaky
	while((c = getchar())!=EOF) 
	{
		if(isprint(c)) // Zjišťování jestli se jedná o tisknutelný znak
		{
			if(lenght + 1 < min_lenght) // Pokud je délka aktualního řetězce menší než jeho požadovaná minimální délka tak se znak načte do bufferu
				buffer[lenght] = c;
			else if(lenght + 1 == min_lenght) // Pokud se délka aktualního řetězce rovná minimální délce, tak se vypíše řetězec z bufferu
			{
				for(int i = 0; i < min_lenght -1; i++) // Výpis řetězce z bufferu
				{
					putchar(buffer[i]);
				}
				putchar(c);
			}
			else // Pokud už je délka řetězce větší než požadovaná minimální velikost, tak se znaky rovnou tisknou
				putchar(c);
			lenght++;
		}
		else // Vykoná se pokud je znak netisknutelný
		{
			if(lenght + 1 > min_lenght)  
				putchar('\n');
			lenght = 0; // Vyresetování aktuální délky řetězce
		}
	}
}

 /**
	*Funkce převádí řetězec znaků na číslo
	*@param string Řetězec znaků, který se má převést na číslo
	*@return number Převedéné číslo
	*/
int text_to_number(char string[])
{
	int i = 0;
	int unit = 1; // Proměná, která označuje jednotky čísla př. stovky, desítky apod.
	int current_number = 0; 
	int number = 0;
	int c = string[i]; // Proměná pro ukládání vstupních znaků
	// Cyklus, který si projde celý řetězec a zjistí jeho velikost
	while((c = string[i]) != '\0')
	{
		if(!isdigit(c)) // Testování jestli je znak číslo
		{
			fprintf(stderr,"Nejedná se o kladné celé číslo nebo nulu!\n"); // Pokud není dojde k výstupu na standartní chybový výstup a program se ukončí
			return -1;
		}
		i++;
	}
	// Cyklus prochází znaky od konce pomocí proměné i
	for(int j = i -1;j>=0;j--)
	{
		current_number = string[j] - '0'; // Převod číselného znaku na konkrétní číslo
		number += unit * current_number; //Přičte se číslo podle toho jaká je zvolená aktuální jednotka
		if(number < 0) // Kontrola přetečení, zjištění jestli se krokem zpět dojde na stejné číslo od kterého se přišlo
		{
			fprintf(stderr,"Zadané číslo je příliš velké.\n");
			return -1;
		}
		if(unit == 1) // V první iteraci se přičte 9, tím v další iteraci budou jednotky desítky
			unit += 9;
		else // Jinak jednotky vynásobí 10 , př. z jednotek desítek se přejde na stovky
			unit *= 10;  	
	}
	return number;
}
