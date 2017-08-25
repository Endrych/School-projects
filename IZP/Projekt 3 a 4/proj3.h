/**
 * @file
 * Kostra hlavickoveho souboru 3. projekt IZP 2016/17
 * a pro dokumentaci Javadoc.
 * @author David Endrych
 * @date Prosinec 2016
 */

/**
 * @defgroup struktury Deklerace datovych typu
 * Jedna se o dekleraci potrebnych datovych typu
 * @{
 */
/**
 * Definovani struktury objektu u ktereho je definovano jeho id, pozice x a pozice y
 */
struct obj_t {
    /// identifikacni cislo objektu, vyuziva se napr. pro razeni objektu ve shluku    
    int id; 
    ///promena udava pozici objektu na ose x
    float x;
    ///y promena udava pozici objektu na ose y
    float y;
};

/**
 * Struktura, ktera slouzi pro ukladani shluku objektu
 */
struct cluster_t {
    /// pocet obejktu ve shluku
    int size;
    /// jedna se o pocet objektu, pro ktere je allokovano misto v pameti
    int capacity;
    /// ukazatel na pole shluku  
    struct obj_t *obj;
};
/** @}
 * @defgroup Zakladni_funkce Zakladni funkce
 * @{
 */
/**
 * Funkce inicializuje shluk c a pokusi se alokovat pamet pro cap objektu
 * @param c ukazatel na shluk, ktery se ma inicializovat
 * @param cap promena, ktera znaci kolik mista ma funkce alokovat
 * @pre cap musi byt vetsi nebo rovno 0
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * Funkce pro odstraneni vsech objektu shluku a inicializovani shluku na prazdny shluk
 * @param c ukazatel na shluk c
 */
void clear_cluster(struct cluster_t *c);

/// CLUSTER_CHUNK hodnota, ktera je doporucena pro realokovani pameti
extern const int CLUSTER_CHUNK;

/**
 * Funkce meni(prealokovava) dosavadni kapacitu shluku, na novou kapacitu new_cap
 * @param c ukazatel na shluk
 * @param new_cap nova kapacita shluku
 * @return c vrati novy shluk
 * @pre new_cap musi byt vetsi nebo rovno 0
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * Funcke prida objekt na konec shluku. Pokud ma shluk malou kapacitu, tak dojde k rozsireni kapacity
 * @param c ukazatel na shluk, do ktereho se ma vlozit objekt
 * @param obj objekt, ktery chceme pridat na konec shluku
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * Funkce prida obekty ze shluku c2 do shluku c1. Pokud bude potreba dojde k rozsireni shluku 1. Nakonec se shluk seradi vzestupne podle identifikacniho cisla.
 * @param c1 ukazatel na shluk, do ktereho budou vkladany objekty
 * @param c2 ukazatel na shluk, ze ktereho se budou vkladat objekty do shluku c1
 * @post Shluk c1 bude na konci serazen podle identifikacniho cisla
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);
/** @}
 * @defgroup pole Prace s polem shluku
 *@{
 */
/**
 * Funkce odstrani shluk z pole.
 * @param carr ukazatel na pole, ze ktereho chceme odstranit shluk
 * @param narr pocet polozek shluku carr
 * @param idx index, na kterem se nachazi shluk, ktery chceme odstranit
 * @return narr-1 Vrati nam novy pocet shluku v poli carr
 * @pre idx musi byt mensi nez narr
 * @pre narr musi byt vetsi nez 0
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * Pocitani vzdalenosti mezi dvema objekty pomoci euklidovske vzdalenosti
 * @param o1 ukazatel na objekt 1
 * @param o2 ukazatel na objekt 2
 * @return result vysledek euklidovske vzdalenosti
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * Pocitani vzdalenosti dvou shluku pomoci nejvzdalenejsiho souseda
 * @param c1 ukazatel na prvni shluk
 * @param c2 ukazatel na druhy shluk
 * @return maximal_distance nejvetsi vzdalenost mezi jejich objekty
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * Funkce hleda dva nejpodobnejsi shluky na zaklade nejvzdalenejsich sousedu
 * @param carr ukazatel na pole shluku
 * @param narr velikost pole shluku
 * @param c1 ukazatel na promenou, do ktere se ulozi index prviho podobneho shluku
 * @param c2 ukazatel na promenou, do ktere se ulozi index druheho podobneho shluku
 * @pre narr musi byt vetsi nez 0
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * Funkce seradi vzestupne objekty shluku podle identifikacniho cisla
 * @param c ukazatel na shluk
 */
void sort_cluster(struct cluster_t *c);

/**
 * Funkce vytiskne shluk na standartni vystup stdout
 * @param c ukazatel na shluk, ktery se ma vypsat
 */
void print_cluster(struct cluster_t *c);

/**
 * Funkce nacte objekty ze souboru a pote pro kazdy objekt vytvori shluk, ktery ulozi do pole shluku. Funkce alokuje prostor pro pole vsech shluku a ukazatel na prvni polozku pole shluku
 * @param filename nazev souboru
 * @param arr ukazatel na pole shluku
 * @return count pocet nactenych shluku(objektu)
 * @return 0 V pripade chyby funkce vrati 0
 * @pre soubor musi existovat
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * Funkce vytiskne pole shluku na standartni vystup stdout
 * @param carr ukazatel na pole shluku
 * @param narr pocet polozek od zacatku, ktere se maji vytisknout
 */
void print_clusters(struct cluster_t *carr, int narr);
/** @} */ promena udava pozici objektu na ose x
