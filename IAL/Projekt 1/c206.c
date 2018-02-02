
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, říjen 2017
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError()
{
    /*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE; /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList(tDLList *L)
{
    /*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;
}

void DLDisposeList(tDLList *L)
{
    /*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
    
    tDLElemPtr curr = L->First;
    //Prochazeni celeho seznamu a uvolnovnovani jednotlivych prvku
    while (curr)
    {
        tDLElemPtr rem = curr;
        curr = curr->rptr;
        free(rem);
    }
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

void DLInsertFirst(tDLList *L, int val)
{
    /*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    tDLElemPtr old_first = L->First;
    tDLElemPtr new_first = (tDLElemPtr) malloc(sizeof(struct tDLElem));
    if (new_first == NULL)
    {
        DLError();
        return;
    }
    new_first->data = val;
    new_first->lptr = NULL;
    new_first->rptr = old_first;
    
    if (old_first != NULL){
        old_first->lptr = new_first;
    }
    //Pokud neni nastaveny posledni prvek je potreba ho nastavit
    if (L->Last == NULL)
        L->Last = L->First;
    L->First = new_first;
}

void DLInsertLast(tDLList *L, int val)
{
    /*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    tDLElemPtr old_last = L->Last;
    tDLElemPtr new_elem = (tDLElemPtr) malloc(sizeof(struct tDLElem));
    if (new_elem == NULL)
    {
        DLError();
        return;
    }
    new_elem->data = val;
    new_elem->lptr = old_last;
    new_elem->rptr = NULL;

    if(old_last != NULL){
        old_last->rptr = new_elem;
    }
    //Pokud neni nastaveny prvni prvek je potreba ho nastavit
    if (L->First == NULL)
        L->First = L->Last;
    L->Last = new_elem;
}

void DLFirst(tDLList *L)
{
    /*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->First;
}

void DLLast(tDLList *L)
{
    /*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->Last;
}

void DLCopyFirst(tDLList *L, int *val)
{
    /*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->First != NULL)
        *val = L->First->data;
    else
        DLError();
}

void DLCopyLast(tDLList *L, int *val)
{
    /*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->Last != NULL)
        *val = L->Last->data;
    else
        DLError();
}

void DLDeleteFirst(tDLList *L)
{
    /*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if (L->First == NULL)
        return;

    tDLElemPtr newFirst = L->First->rptr;
    if (L->Act == L->First)
        L->Act = NULL;
    free(L->First);
    L->First = newFirst;
}

void DLDeleteLast(tDLList *L)
{
    /*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if (L->Last == NULL)
        return;

    tDLElemPtr new_last = L->Last->lptr;
    new_last->rptr = NULL;
    if (L->Act == L->Last)
        L->Act = NULL;
    free(L->Last);
    L->Last = new_last;
}

void DLPostDelete(tDLList *L)
{
    /*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
    if (L->Act != NULL && L->Act != L->Last)
    {
        tDLElemPtr post = L->Act->rptr;
        L->Act->rptr = post->rptr;
        if (post == L->Last)
            L->Last = L->Act;
        else
            post->rptr->lptr = L->Act;
        free(post);
    }
}

void DLPreDelete(tDLList *L)
{
    /*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
    if (L->Act != NULL && L->Act != L->First)
    {
        tDLElemPtr pre = L->Act->lptr;

        L->Act->lptr = pre->lptr;
        if (pre == L->First)
            L->First = L->Act;
        else
            pre->lptr->rptr = L->Act;
        free(pre);
    }
}

void DLPostInsert(tDLList *L, int val)
{
    /*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if (L->Act != NULL)
    {
        tDLElemPtr post = L->Act->rptr;
        tDLElemPtr new_element = (tDLElemPtr) malloc(sizeof(struct tDLElem));
        if (new_element == NULL)
        {
            DLError();
            return;
        }
        if (post != NULL)
        {
            post->lptr = new_element;
        }
        else
            L->Last = new_element;
        new_element->data = val;
        new_element->rptr = post;
        new_element->lptr = L->Act;
        L->Act->rptr = new_element;
    }
}

void DLPreInsert(tDLList *L, int val)
{
    /*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    if (L->Act != NULL)
    {
        tDLElemPtr pre = L->Act->lptr;
        tDLElemPtr new_element = (tDLElemPtr) malloc(sizeof(struct tDLElem));
        if (new_element == NULL)
        {
            DLError();
            return;
        }
        if (pre != NULL)
        {
            pre->rptr = new_element;
        }
        else
        {
            L->First = new_element;
        }
        new_element->data = val;
        new_element->lptr = pre;
        new_element->rptr = L->Act;
        L->Act->lptr = new_element;
    }
}

void DLCopy(tDLList *L, int *val)
{
    /*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

    if (L->Act == NULL)
        DLError();
    else
        *val = L->Act->data;
}

void DLActualize(tDLList *L, int val)
{
    /*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    L->Act->data = val;
}

void DLSucc(tDLList *L)
{
    /*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
    if (L->Act != NULL)
        L->Act = L->Act->rptr;
}

void DLPred(tDLList *L)
{
    /*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/

    if (L->Act != NULL)
        L->Act = L->Act->lptr;
}

int DLActive(tDLList *L)
{
    /*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
    return (L->Act != NULL);
}

/* Konec c206.c*/
