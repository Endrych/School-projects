/**
 * Kostra programu pro 3. projekt IZP 2016/17
 * Autor: David Endrych
 * Jednoducha shlukova analyza
 * Complete linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/clsrov.html
 * Popis: Jedná se o shlukovou analýzu na základě nejvzdálenějšího souseda. Shluková analýza se provádí na dvourozměrných objektech. Každý objekt je identifikován celým číslem. Objekty 	  jsou uloženy v textovém souboru.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
  int id;
	float x;
  float y;
};

struct cluster_t {
  int size;
  int capacity;
  struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
  assert(c != NULL);
  assert(cap >= 0);

  // TODO
	if((c->obj = malloc(cap * sizeof(struct obj_t))) == NULL)		// Pokud se špatně allokuje pamět přiřadí se kapacitu na 0
		c->capacity = 0;
	else
		c->capacity = cap;
	c->size = 0;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
  // TODO
  c->size = 0;
  c->capacity = 0;
	free(c->obj);
  c->obj = NULL;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
  // TUTO FUNKCI NEMENTE
  assert(c);
  assert(c->capacity >= 0);
  assert(new_cap >= 0);

  if (c->capacity >= new_cap)
	  return c;

  size_t size = sizeof(struct obj_t) * new_cap;

  void *arr = realloc(c->obj, size);
  if (arr == NULL)
    return NULL;

  c->obj = arr;
  c->capacity = new_cap;
	return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
  // TODO
  if(c->capacity == c->size)
  	resize_cluster(c,c->capacity + CLUSTER_CHUNK);
  c->obj[c->size] = obj;
	c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
  assert(c1 != NULL);
  assert(c2 != NULL);

  // TODO
	int free_capacity = c1->capacity - c1->size;
  if(free_capacity < c2->size)
  {
  	resize_cluster(c1,c2->size - free_capacity);
  }
  for(int x = 0; x < c2->size; x++)
  	append_cluster(c1,c2->obj[x]);
	sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
  assert(idx < narr);
  assert(narr > 0);

  // TODO
  struct cluster_t tmp_cluster = carr[idx];
	carr[idx] = carr[narr-1];
	carr[narr-1] = tmp_cluster;
  clear_cluster(&carr[narr-1]);
	return narr-1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
	assert(o1 != NULL);
  assert(o2 != NULL);

  // TODO
  return sqrtf((o1->x - o2->x)*(o1->x - o2->x) + (o1->y - o2->y)*(o1->y - o2->y));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
	assert(c1 != NULL);
  assert(c1->size > 0);
  assert(c2 != NULL);
  assert(c2->size > 0);

	// TODO
	float maximal_distance = obj_distance(&c1->obj[0],&c2->obj[0]);
	float actual_distance;
	for(int i = 0; i < c1->size;i++)
	{
		for(int j = 0; j < c2->size;j++)
		{
			actual_distance = obj_distance(&c1->obj[i],&c2->obj[j]);
			if(actual_distance > maximal_distance)
				maximal_distance = actual_distance;
		}
	}
	return maximal_distance;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
  assert(narr > 0);

  // TODO
	float lenght = cluster_distance(&carr[1],&carr[0]);
	float current_lenght = 0;
	for(int i = 0; i < narr; i++)
	{
		for(int j = i+1; j < narr; j++)
		{
			current_lenght = cluster_distance(&carr[i],&carr[j]);
			if(lenght >= current_lenght)
			{
				*c1 = i;
				*c2 = j;
				lenght = current_lenght;
			}
		}
	}
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
	// TUTO FUNKCI NEMENTE
  const struct obj_t *o1 = a;
  const struct obj_t *o2 = b;
  if (o1->id < o2->id) return -1;
  if (o1->id > o2->id) return 1;
  return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
	// TUTO FUNKCI NEMENTE
  qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
	// TUTO FUNKCI NEMENTE
  for (int i = 0; i < c->size; i++)
  {
  	if (i) putchar(' ');
    printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
  }
  putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
  assert(arr != NULL);

  // TODO
	int count;
	int id;
	int x;
	int y;
	FILE *r;
	struct obj_t object;

	r = fopen(filename, "r"); // Pokus o načtení souboru
	if(r == NULL) // Pokud se soubor špatně načte
	{
		*arr = NULL;
		fprintf(stderr,"Nepodařilo se načíst soubor!\n");
		return 0;
	}
	if(fscanf(r, "count=%d\n",&count) != 1) // Pokud se nepodaří správně načíst první řádek
	{
		fprintf(stderr,"První řádek neobsahuje count=CISLO!\n");
		*arr = NULL;
		fclose(r);
		return 0;
	}
	*arr = malloc(count * sizeof(struct cluster_t)); // Alokace paměti
	for(int i = 0; i < count; i++)
	{
		if(fscanf(r, "%d %d %d",&id,&x,&y) == 3) // Pokus o načtení jednotlivých řádků
		{
      if(x <= 1000 && x >= 0 && y <= 1000 && y >= 0)
      {
  			object.id = id;
  			object.x = x;
  			object.y = y;
  			init_cluster(&(*arr)[i],CLUSTER_CHUNK);
  			append_cluster(&(*arr)[i],object);
      }
      else
      {
        fprintf(stderr, "Pozice x a y musí být v intervalu  <0;1000>\n");
        for(int k = i - 1; k >=0; k--) // Dealokace allokované paměti při přerušení cyklu
  				clear_cluster(&(*arr)[k]);
  			free(*arr);
  			*arr = NULL;
  			fclose(r);
  			return 0;
      }
    }
		else
		{
			for(int k = i - 1; k >=0; k--) // Dealokace allokované paměti při přerušení cyklu
				clear_cluster(&(*arr)[k]);
			free(*arr);
			*arr = NULL;
			fclose(r);
			return 0;
		}
	}
	fclose(r);
	return count;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{

	printf("Clusters:\n");
  for (int i = 0; i < narr; i++)
  {
  	printf("cluster %d: ", i);
    print_cluster(&carr[i]);
  }
}

int main(int argc, char *argv[])
{
	struct cluster_t *clusters;
	char *endptr;
  // TODO
	int clusters_number = 0;
	int c1,c2;

	if(argc >= 2 && argc <= 3)
	{
		if(argc == 3)
		{
			long cluster_number_long = strtoul(argv[2],&endptr,0);
			if(*endptr != '\0') // Testovaní jestli se jedná o číslo
			{
				fprintf(stderr,"Špatný argument, nejedná se o číslo!\n");
				return -1;
			}
			if(cluster_number_long > INT_MAX || cluster_number_long < 1) // Testovaní jestli se nepřesáhlo povolených hodnot
			{
				fprintf(stderr,"Číslo není v rozsahu hodnot (1 - %d)!\n",INT_MAX);
				return -1;
			}
			clusters_number = (int) cluster_number_long;	// Přetypování
		}
		else
			clusters_number = 1;
	}
	else
	{
		fprintf(stderr,"Špatně spustěný program. Spustěte v podobě ./proj3 SOUBOR [N] nebo ./proj3 SOUBOR\n");
		return -1;
	}
	int cluster_count = load_clusters(argv[1],&clusters); // Načtení clusterů ze souboru
	if(cluster_count == 0)
		return -1;
	while(cluster_count > clusters_number)
	{
		find_neighbours(clusters,cluster_count, &c1, &c2);
	  merge_clusters(&clusters[c1], &clusters[c2]);
	  cluster_count = remove_cluster(clusters, cluster_count, c2);
	}

	print_clusters(clusters,cluster_count);
	for(int i = 0; i < cluster_count; i++) //Dealokování paměti
	{
		clear_cluster(&clusters[i]);
	}
	free(clusters);
	return 0;
}
