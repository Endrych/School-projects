/*
 * Soubor:  proj2.c
 * Datum:   Listopad 2016
 * Autor:   David Endrych
 * Projekt: Iterační výpočty
 * Popis: Program slouží pro výpočet logaritmů a exponencionální funkce pomocí taylorova polynomu a zřetězených zlomků.
 */
 
// Vložení knihoven 
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

// Deklerace funkcí
double taylor_log(double x, unsigned int n);
double cfrac_log(double x, unsigned int n);
double taylor_pow(double x, double y, unsigned int n);
double taylorcf_pow(double x, double y, unsigned int n);

// Hlavní funkce
int main (int argc, char *argv[])
{
	char *end_of_number; // Pro kontrolu jestli jsou v argumentu jen čísla
	double x = 0;
	double y = 0;
	long int n_first = 0; // Počet iteraci se nejprve uloží jako long int, provede se kontrola a poté se převeda na unsigned int
	unsigned int n = 0;
	int ispow = 0; // Proměná, která určuje jestli byl program spuštěn s argumentem pow hodnota 1 nebo log hodnota 0
	
	if(argc > 2) 
	{
		x = strtod(argv[2],&end_of_number);	// Uložení argumentu do proměné x, pokud dojde k přerušení z důvodu nespravného znaku(nejedná li se o číslo) uloží se to na adresu end_of_number	
		if(*end_of_number != '\0') // Kontrola jestli se v argumentu objevovala pouze čísla
		{
			fprintf(stderr,"Argument není číslo!\n");
			return -1;
		}
		if(argc == 4 && !strcmp("--log",argv[1])) // Pokud je počet argumenů správný a je požadován výpočet logaritmů 
		{
			n_first =strtoul(argv[3],&end_of_number, 0); // Uložení argumentu do proměné n, pokud dojde k přerušení z důvodu nespravného znaku(nejedná li se o číslo) uloží se to na adresu end_of_number	
			if(*end_of_number != '\0') // Kontrola jestli se v argumentu objevovala pouze čísla
			{
				fprintf(stderr,"Argument není číslo!\n");
				return -1;
			}
			if(n_first < 0) // Kontrola jestli nedošlo k zadání záporných iterací
			{
				fprintf(stderr,"Počet iterací musí být větší než 0!\n");
				return -1;
			}
			else if(n_first > UINT_MAX) // Kontrola jestli číslo nepřeteklo, respektivě jestli nepřeteče po přetypování na unsigned integer
			{
				fprintf(stderr,"Počet iterací je příliš velký\n");
				return -1;
			}
		}		
		else if(argc == 5 && !strcmp("--pow",argv[1])) // Pokud je počet argumenů správný a je požadován výpočet exponencionální funkce
		{
			y = strtod(argv[3],&end_of_number); // Uložení argumentu do proměné y, pokud dojde k přerušení z důvodu nespravného znaku(nejedná li se o číslo) uloží se to na adresu end_of_number	
			if(*end_of_number != '\0') // Kontrola jestli se v argumentu objevovala pouze čísla
			{
				fprintf(stderr,"Argument není číslo!\n");
				return -1;
			}
			n_first =strtoul(argv[4],&end_of_number, 0); // Uložení argumentu do proměné n, pokud dojde k přerušení z důvodu nespravného znaku(nejedná li se o číslo) uloží se to na adresu end_of_number	
			if(n_first < 0) // Kontrola jestli nedošlo k zadání záporných iterací
			{
				fprintf(stderr,"Počet iterací musí být větší než 0!\n");
				return -1;
			}
			else if(n_first > UINT_MAX) // Kontrola jestli číslo nepřeteklo, respektivě jestli nepřeteče po přetypování na unsigned integer
			{
				fprintf(stderr,"Počet iterací je příliš velký\n");
				return -1;
			}
			if(*end_of_number != '\0') // Kontrola jestli se v argumentu objevovala pouze čísla
			{
				fprintf(stderr,"Argument není číslo!\n");
				return -1;
			}
			ispow = 1; // Nastavení proměné, která určuje co se má vypsat
		}
		else
			return 0;		
		n = (unsigned) n_first; // Přetypování na unsigned integer a uložení do proměné n				
		if(ispow) // Výpis výsledků podle toho jestli byl zadán výpočet logarimu nebo exponencionální funkce
			printf("         pow(%g,%g) = %.12g\n  taylor_pow(%g,%g) = %.12g\ntaylorcf_pow(%g,%g) = %.12g\n",x,y,pow(x,y),x,y,taylor_pow(x,y,n),x,y,taylorcf_pow(x,y,n));
		else
			printf("       log(%g) = %.12g\n cfrac_log(%g) = %.12g\ntaylor_log(%g) = %.12g\n",x,log(x),x, cfrac_log(x,n),x,taylor_log(x,n)); 
	}
	return 0;
}
/**
 *Funkce pro výpočet logaritmu pomocí taylorova polynomu
 *@param x udavá číslo, které chceme z logaritmovat
 *@param n udává počet iteraci
 *@return result vrací výsledné číslo
 *@return NAN Pokud je x < 0
 *@return -INFINITY Pokud x == 0
 *@return INFINITY Pokud x == INFINITY
 */
double taylor_log(double x, unsigned int n)
{
	if(x < 0)
		return NAN;
	else if(x == 0)
		return -INFINITY;
	else if(x == INFINITY)
		return INFINITY;
	double result = 0;
	int element = 1; // Pořadí polynomu 
	double actual = 1; // Proměná ve, které je uložena hodnota aktualního členu taylorova polynomu
	double numerator = 1; // Proměná ve, které je uložena hodnota čitatele
	if(x > 0 && x <= 1) // Algoritmus pokud je 0 < x <= 1
	{
		double y = 1 - x; // Výpočet proměné se kterou se bude počítat v algoritmu
		while(n > 0)
		{
			numerator *= y;
			actual = numerator/element;
			result -= actual;
			element++;
			n--;
		}
	}
	else if(x > 1/2) // Algoritmus pokud x nevyhovuje předchozí podmínce a je větší než 1/2
	{
		for(unsigned k = 1; k <= n ;k++)
		{
			numerator *= ((x-1)/x);
			result += numerator/k;
		}
	}
	return result; 
}
/**
 *Funkce pro výpočet logaritmu pomocí zřetězených zlomků
 *@param x udavá číslo, které chceme z logaritmovat
 *@param n udává počet iteraci
 *@return result vrací výsledné číslo
 *@return NAN Pokud je x < 0
 *@return -INFINITY Pokud x == 0
 *@return INFINITY Pokud x == INFINITY
 */
double cfrac_log(double x, unsigned int n)
{
	if(x < 0)
		return NAN;
	else if(x == 0)
		return -INFINITY;
	else if(x == INFINITY)
		return INFINITY;
	double result = 1;
	double z = (x - 1)/(1+x); // Pomocný výpočet, který se uloží do proměné z
	while(n > 0)
	{
		result = -(n*n*z*z/result);
		result += n*2-1;
		n--;
	}
	result = 2*z / result;
	return result;
}
/**
 *Funkce pro výpočet exponencionální funkce s obecným základem pomocí výpočtu logaritmu přes funkci taylor_log
 *@param x obecný základ
 *@param y číslo ze kterého chcemě dělat exponencionální funkci
 *@param n udává počet iteraci
 *@return result vrací výsledné číslo
 *@return -INFINITY Pokud x == 0 nebo y == -INFINITY
 *@return 1 Pokud y == 0 
 *@return INFINITY Pokud y = INFINITY nebo x == INFINITY
 *@return NAN Pokud je x < 0
 */
double taylor_pow(double x, double y, unsigned int n)
{
	if(y == INFINITY)
		return INFINITY;
	else if(y == -INFINITY)
		return 0;
	else if(y == 0)
		return 1;
	else if(x < 0)
		return NAN;
	else if(x == 0)
		return -INFINITY;
	else if(x == INFINITY)
		return INFINITY;
	double result = 1;
	double actual = 1; // Proměná do které se ukládá aktualní člen
	double log_x = taylor_log(x,n); // Proměná do které je uložena hodnota logaritmu pro následující výpočty
	for(unsigned k = 1;k<=n;k++)
	{
		actual *= y*log_x/k; 
		result += actual;
	}
	return result;
}
/**
 *Funkce pro výpočet exponencionální funkce s obecným základem pomocí výpočtu logaritmu přes funkci cfrac_log
 *@param x obecný základ
 *@param y číslo ze kterého chcemě dělat exponencionální funkci
 *@param n udává počet iteraci
 *@return result vrací výsledné číslo
 *@return -INFINITY Pokud x == 0 nebo y == -INFINITY
 *@return 1 Pokud y == 0 
 *@return INFINITY Pokud y = INFINITY nebo x == INFINITY
 *@return NAN Pokud je x < 0
 */
double taylorcf_pow(double x, double y, unsigned int n)
{
	if(y == INFINITY)
		return INFINITY;
	else if(y == -INFINITY)
		return 0;
	else if(y == 0)
		return 1;
	else if(x < 0)
		return NAN;
	else if(x == 0)
		return -INFINITY;
	else if(x == INFINITY)
		return INFINITY;
	double log_x = cfrac_log(x,n); // Proměná do které je uložena hodnota logaritmu pro následující výpočty
	double result = 1;
	double actual = 1;
	for(unsigned k = 1;k<=n;k++)
	{
		actual *= y*log_x/k; 
		result += actual;
	}
	return result;
}
