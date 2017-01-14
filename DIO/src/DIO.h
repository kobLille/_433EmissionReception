/*
 DIO
 */

namespace DIO {

class SDClass {

  public:
    int cpt1;
    int cpt0;
    int sEmis;
    int index;
    int enCours;
    int fin;
    int tailleRequete, tailleRequete1, tailleRequete2;
    int toggle;
    byte tEnvoieRequete[40];
    long trame, pointer;
    int repetition;
    int debut;

    void begin(uint8_t sortie);  //Fontion permettant l'initialisation
    void maj();  //Fonction permettant la mise à jour en fonction du timer
    void envoieCours(long emetteur, bool groupe, bool etat, int interupteur, int rept); //Fonction permettant de lancer l'envoie de la requete
    void bitSuivant();
    void initCpt(int etat); //La valeur du cpt correspond à la valeur du cpt x la base de temps

  private:


};

