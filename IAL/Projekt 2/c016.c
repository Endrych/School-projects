
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, říjen 2014
**                              Radek Hranický, listopad 2015
**                              Radek Hranický, říjen 2016
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {
	for(int i = 0;i<HTSIZE;i++){
		(*ptrht)[i] = NULL;
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	if(ptrht == NULL)
		return NULL;
	int index = hashCode(key); // Zjisteni indexu
	tHTItem * current = (*ptrht)[index]; // Prvni prvek pro pruchod
	
	// Hledani klice
	while(current != NULL){ 
		if(!strcmp(key,current->key))
			return current;
		current = current->ptrnext;
	}
	return NULL;
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	if(ptrht == NULL){
		return;
	}
	tHTItem* element = htSearch(ptrht,key); // Zkusime najit polozku podle klice
	if(element != NULL){ // Pokud tabulka polozku obsahuje aktualizujeme data
		element->data = data;
	}
	else{
		int index = hashCode(key);
		element = malloc(sizeof(tHTItem));
		if(element == NULL){ // Kontrola jestli malloc byl uspesni
			return;
		}
		element->key = malloc(strlen(key) + 1);
		if(element->key == NULL){ // Kontrola jestli malloc byl uspesni
			free(element); // Je potreba uvolnit polozku
			return;
		}
		strcpy(element->key,key); 
		element->data = data;
		element->ptrnext = (*ptrht)[index];
		
		(*ptrht)[index] = element;
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {

	tHTItem* element = htSearch(ptrht,key); 
	if(element == NULL){
		return NULL;
	}
	else{
		return &(element->data);
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
	if(ptrht == NULL)
		return;
	int index = hashCode(key);
	tHTItem * prev = NULL; // Predchozi prvek
	tHTItem * current = (*ptrht)[index]; // AKtualni prvek v cykly
	while(current != NULL){
		if(!strcmp(key,current->key)){
			if(prev != NULL){
				prev->ptrnext = current->ptrnext; // nejedna se o prvni prvek a je potreba propojit nasledujici prvek s predposlednim
			}
			else{
				(*ptrht)[index] = current->ptrnext; // Jedna se o prvni prvek
			}
			free(current->key);
			free(current);
			return;
		}
		else{
			prev = current;
			current = current->ptrnext; 
		}
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	if(ptrht == NULL){
		return;
	}
	// Postupne se projdou vsechny indexy
	for(int i=0;i<HTSIZE;i++){
		tHTItem * current;
		current = (*ptrht)[i];
		
		// Uvolneni vsech polozek na danem indexu
		while(current != NULL){
			tHTItem* rem = current;
			current = current->ptrnext;
			// Je potreba uvolnit jak klic tak polozku
			free(rem->key);
			free(rem);
		}
		(*ptrht)[i] = NULL;
	}
}
