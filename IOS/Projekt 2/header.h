#define MAXTIME 5001

// Struktura ve ktere se ukladaji argumenty
typedef struct parametrs{
    int A; // Pocet adultu
    int C; // Pocet childu
    int AGT; // Maximální hodnota doby (v milisekundách), po které je generován nový proces.
    int CGT; // Maximální hodnota doby (v milisekundách), po které je generován nový proces 
    int AWT; // Maximální hodnota doby (v milisekundách), po kterou proces adult simuluje činnost v centru
    int CWT; // Maximální hodnota doby (v milisekundách), po kterou proces child simuluje činnost v centru
}parametrs;

void adult(parametrs *par);
void child(parametrs *par);
int init();
int destruct();
int getArg(int argc,char *argv[],parametrs *par);
void end();