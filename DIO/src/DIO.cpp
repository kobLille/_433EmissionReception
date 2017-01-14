/*

 Dio - 

 */


#include "Dio.h"

namespace 433Dio {

class DIOClass {

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

    void begin(uint8_t sortie) { //Fontion permettant l'initialisation
      Serial.println("begin");
      sEmis = sortie;
      pinMode(sEmis, OUTPUT);
      digitalWrite(sEmis, LOW);
      index = 0;
    }

    void maj() {  //Fonction permettant la mise à jour en fonction du timer
      if (enCours) {
        if (cpt1) {
          digitalWrite(sEmis, HIGH);
          cpt1--;
        }
        else {
          if (cpt0) {
            digitalWrite(sEmis, LOW);
            cpt0--;
          }
          else {
            if (fin)
            {
              if (--repetition)
              {
                pointer = 0x80000000;
                initCpt(2);
                fin = 0;
              }
              else
              {
                digitalWrite(sEmis, LOW);
                enCours = 0;
                Serial.println("fin" );

              }
            }
            bitSuivant();
          }
        }
      }
    }

    void envoieCours(long emetteur, bool groupe, bool etat, int interupteur, int rept) { //Fonction permettant de lancer l'envoie de la requete

      int a;
      trame = 0;
      trame = (long) emetteur;
      trame = trame << 1;
      if (groupe) trame = trame + 1;
      trame = trame << 1;
      if (etat) trame = trame + 1;
      trame = trame << 4;
      trame = trame + (long)interupteur;
      pointer = 0x80000000;
      repetition = rept;
      fin = 0;
      initCpt(2);
      index = -1;
      toggle = 0;
      enCours = 1;
  //    Serial.println("Lancement : ");
  //    Serial.println(trame, BIN);
  //    Serial.println(pointer, BIN);
    }

    void bitSuivant() {

      if (!debut)
      {
        if (pointer && enCours)
        {
          if (!toggle) {
            toggle = 1;
            if (trame & pointer) {
              initCpt(1);
            }
            else
            {
              initCpt(0);
            }
          }
          else {
            toggle = 0;
            if (trame & pointer)
              initCpt(0);
            else
              initCpt(1);

            pointer = pointer >> 1;
            pointer = pointer & 0x7FFFFFFF;
          }
        }
        else
        {
          initCpt(3);
          fin = 1;
          // Serial.println("Fin");
        }
      }
      else
        debut = 0;
      maj();
    }

 
    void initCpt(int etat) {  //La valeur du cpt correspond à la valeur du cpt x la base de temps
      switch (etat) {
        case 0:  //cpt pour un bit à 0
          cpt1 = 1;
          cpt0 = 1;
          break;
        case 1:   //cpt pour un bit à 1
          cpt1 = 1;
          cpt0 = 6;
          break;
        case 2:  //cpt pour le début de l'envoie d'une trame
          cpt1 = 1;
          cpt0 = 10;
          debut = 1;
          break;
        case 3:  //cpt pour la fin de l'envoie d'une trame
          cpt1 = 1;
          cpt0 = 41;
          break;
      }
    }

  private:


};


};
