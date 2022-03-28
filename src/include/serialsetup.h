/* serialsetup.h
 *
 * contiene la funzione
 *
 * int init(char *comname, int mode);
 *
 * che inizializza la seriale impostandola in modo "raw".
 * La seriale può essere reale (utilizzando il dispositivo definito nella
 * #define SERIALE nel file principale) oppure una seriale viruale (usando ù
 * i dispositivi pty).
 * La funzione ritorna il file handle del terminale aperto
 *
 * ATTENZIONE!
 * La prima invocazione della funzine init per la seriale virtuale apre
 * il PTY, tutte le altre invocazioni si collegano a questo primo PTY,
 * quindi prima di rilnciare client  server occorre far terminare tutte
 * le vecchie istanze, altiemtni si connetteranno tutte al primo server.
 *
 * Problemi:
 * i caratteri:
 * CTRL-C (0x03 - 3 - ETX)
 * CTRL-Z (0x1A - 26 - SUB)
 * CTRL-\ (0x1C - 28 - FS)
 * creano problemi di trasmissione
 */

#ifndef SERIALSETUP
#define SERIALSETUP

static int init(char *comname, int mode);        // Funzione che apre una seriale e la inizializza

#ifdef VIRTUALTTY

/**** init in caso di seriale virtuale ****/
#include <errno.h>
#include <signal.h>
int grantpt(int fd);
int unlockpt(int fd);
char *ptsname(int fd);
void __finish(){
  unlink("/tmp/ttyLOCAL.ctl");
}
int init (char *comname, int mode) {
  const char *ttyControlFile = "/tmp/ttyLOCAL.ctl";
  const char *ttyControlFormat = "%d %s\n";
  struct termios tattr;                // Struttura per impostare le caratteristiche della seriale
  int com = -1;
  FILE *control = fopen(ttyControlFile, "r");
  if (control) {
    int pid;
    char slave[30];
    if (fscanf(control, ttyControlFormat, &pid, slave) == 2) {
      char pidname [15];
      int pt;
      sprintf (pidname, "/proc/%d", pid);
      if ((pt = open(pidname, O_RDONLY)) >= 0){
        com = open (slave, mode | O_SYNC);
      }
    }
    fclose (control);
  }
  if (com < 0) {
    com = open("/dev/ptmx", mode | O_SYNC);
    if (com >= 0) {
      control = fopen(ttyControlFile, "w");
      if (control) {
        atexit(__finish);
        signal(SIGINT, __finish);
        grantpt(com);
        unlockpt(com);
        fprintf(control, ttyControlFormat, getpid(), ptsname(com));
        fclose (control);
      } else {
        perror ("Non riesco a creare il file di controllo");
        close (com);
        exit (2);
      }
    } else {
      perror ("Non riesco a creare il file master della seriale virtuale");
      exit (1);
    }
  }
  /* Impostazione della seriale
   * fonte info libc->"Low-Level Terminal Interface"->"Noncanonical Mode Example" */
  tcgetattr (com, &tattr);                // Recupera gli attributi correnti della seriale

  // Modi di ingresso - spegne i flag indicati
  tattr.c_iflag &= ~(INLCR|IGNCR|ICRNL|IXON);
  // Modi di uscita - spegne i flag indicati
  tattr.c_oflag &= ~(OPOST|ONLCR|OCRNL|ONLRET);
  /* Modi di controllo - imposta 8 bit (CS8), abilita ricezione (CREAD),
   *ignora segnali di controllo (CLOCAL) */
  tattr.c_cflag = CS8 | CREAD | CLOCAL;
  tattr.c_lflag &= ~(ICANON|ECHO);        // elimina traduzioni di carateri (ICANON) ed ECHO.
  tattr.c_cc[VMIN] = 1;                   // restituisce dati dopo aver ricevuto 1 carattere
  tattr.c_cc[VTIME] = 0;                  // nessun timeout

  tcsetattr (com, TCSAFLUSH, &tattr);        // Imposta i nuovo attributi

  return com;
}
#else

/* ***** Init in caso di seriale reale ***** */
int init(char *comname, int mode)
{
int com;                        // Il file per la seriale
struct termios tattr;                // Struttura per impostare le caratteristiche della seriale

  com = open (comname,mode | O_SYNC);        // Apre il dispositivo (Lettura/scrittura, sincrono)

  if (com == -1)                        // Open ritorna errore
  {
    perror ("Non posso aprire la seriale"); // Stampa il messaggio di errore
    exit (2);                                // E termina drasticamente con errore
  }

  /* Impostazione della seriale
   * fonte info libc->"Low-Level Terminal Interface"->"Noncanonical Mode Example" */
  tcgetattr (com, &tattr);                // Recupera gli attributi correnti della seriale

  // Modi di ingresso - spegne i flag indicati
  tattr.c_iflag &= ~(INLCR|IGNCR|ICRNL|IXON);
  // Modi di uscita - spegne i flag indicati
  tattr.c_oflag &= ~(OPOST|ONLCR|OCRNL|ONLRET);
  /* Modi di controllo - imposta 8 bit (CS8),
   * abilita ricezione (CREAD),
   * ignora segnali di controllo (CLOCAL) */
  tattr.c_cflag = CS8 | CREAD | CLOCAL;
  tattr.c_lflag &= ~(ICANON|ECHO);        // elimina traduzioni di carateri (ICANON) ed ECHO.
  tattr.c_cc[VMIN] = 1;                        // restituisce dati dopo aver ricevuto 1 carattere
  tattr.c_cc[VTIME] = 0;                // nessun timeout

  cfsetispeed (&tattr, B115200);                // Imposta la velocita' di ricezione
  cfsetospeed (&tattr, B115200);                // Imposta la velocita' di trasmissione

  tcsetattr (com, TCSAFLUSH, &tattr);        // Imposta i nuovo attributi

  return com;                                // Ritrona l'handle del file aperto
}
#endif

#endif
