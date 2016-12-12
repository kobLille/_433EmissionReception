
#define ledPin 8
#define maxi 200

typedef struct requete1 requetes1;
struct requete1
{
  bool emetteur[26] ;
  bool groupe ;
  bool etat ;
  bool interupteur[4];
};

typedef struct requete2 requetes2;
struct requete2
{
  bool emetteur[26] ;
  bool groupe ;
  bool etat ;
  bool interupteur[4];
  bool level[4];
};

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

    void begin(uint8_t sortie) { //Fontion permettant l'initialisation
      Serial.println("begin");
      sEmis = sortie;
      pinMode(sEmis, OUTPUT);
      digitalWrite(sEmis, LOW);
      infoSortie = 1;
      index = 0;
      tailleRequete1 = sizeof(requetes1);
      tailleRequete2 = sizeof(requetes2);
    }

    void maj() {  //Fonction permettant la mise à jour en fonction du timer
      if (enCours) {
        if (cpt1) {
          digitalWrite(sEmis, HIGH);
          infoSortie = 1;
          cpt1--;
        }
        else {
          if (cpt0) {
            digitalWrite(sEmis, LOW);
            infoSortie = 0;
            cpt0--;
          }
          else {
            digitalWrite(sEmis, LOW);
            infoSortie = 1;
            if (fin)
              enCours = 0;
            bitSuivant();
          }
        }
      }
    }

    void envoieCours(bool emetteur[28], bool groupe, bool etat, bool interupteur[4]) { //Fonction permettant de lancer l'envoie de la requete
      // Serial.println("Lancement");
      int a;
      requetes1 envoieRequete;
      memcpy(&tEnvoieRequete, &emetteur, 26);
      memcpy(&tEnvoieRequete + 26, &interupteur, 4);
      tEnvoieRequete[26] = groupe;
      tEnvoieRequete[27] = etat;
      enCours = 1;
      fin = 0;
      initCpt(2);
      index = -1;
      toggle = 0;
      tailleRequete = tailleRequete1;
      //memcpy(&tEnvoieRequete, &envoieRequete, tailleRequete1);
      Serial.print("requete : ");
      for (a = 0; a < tailleRequete1; a++)
      { Serial.print(a);
        Serial.print(": ");
        Serial.print(tEnvoieRequete[a]);
        Serial.print("; ");
      }
      Serial.println();

    }

    void envoieCours1(requetes1 envoieRequete) { //Fonction permettant de lancer l'envoie de la requete
      // Serial.println("Lancement");
      enCours = 1;
      fin = 0;
      initCpt(2);
      index = -1;
      toggle = 0;
      tailleRequete = tailleRequete1;
      memcpy(&tEnvoieRequete, &envoieRequete, tailleRequete1);
      // Serial.println("requete : ");

    }

    void envoieCours2(requetes2 envoieRequete) { //Fonction permettant de lancer l'envoie de la requete
      // Serial.println("Lancement");
      enCours = 1;
      fin = 0;
      initCpt(2);
      index = -1;
      toggle = 0;
      tailleRequete = tailleRequete2;
      memcpy(&tEnvoieRequete, &envoieRequete, tailleRequete2);
      // Serial.println("requete : ");

    }

    void bitSuivant() {
      if (!toggle)
        index++;
      if ((index < tailleRequete ) && enCours)
      {

        if (!toggle) {
          toggle = 1;
          //        Serial.println("------------------------");
          if (tEnvoieRequete[index] && 1)
            initCpt(1);
          else
            initCpt(0);
        }
        else {
          toggle = 0;
          if (tEnvoieRequete[index] && 1)
            initCpt(0);
          else
            initCpt(1);
        }
      }
      else
      {
        initCpt(3);
        fin = 1;
        //  Serial.println("Fin");
      }

      maj();
    }

    int etatSortie() {
      return infoSortie;
    }

    int etatIndex() {
      return tEnvoieRequete[index] && 1;
    }
    int valIndex() {
      return index;
    }

    int envoieEncours() {
      if (enCours)
        return 0;
      else
        return 1;
    }

    int compt(int val) {
      if (val)
        return cpt1;
      else
        return cpt0;
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
          cpt0 = 9;
          break;
        case 3:  //cpt pour la fin de l'envoie d'une trame
          cpt1 = 1;
          cpt0 = 41;
          break;
      }
    }

  private:
    int infoSortie;

};

//unsigned long val[maxi];
int val[maxi];
unsigned long  memo, tampon;
bool trame[100];
int i, j, k, l, ok, recept, requete,caractere;
requetes1 envoie, envoieO, envoieF;
requetes2 envoie1, envoieToto[10];
int memoEtatSorti = 1;
DIOClass DIO;

void setup() {

  // pinMode(ledPin, OUTPUT);
  //start serial connection
  Serial.begin(230400);
  //configure pin2 as an input and enable the internal pull-up resistor
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(1, blink, RISING);

  //init envoie


// numéro de la sortie emetteur donnée
  DIO.begin(8);

  delay(1000);

  // initialiser le timer1
  noInterrupts(); // désactiver toutes les interruptions
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TIMSK1 = 0;
  //OCR1A = 31250; // 16MHz/256/2Hz
  //OCR1A = 6250; //toutes les secondes
  OCR1A = 4000; //toutes les secondes
  //OCR1A = 500000; //toutes les secondes
  TCCR1B |= (1 << WGM12); // CTC mode
  // TCCR1B |= (1 << CS12); // 256 prescaler
  //TCCR1B |= (1 << CS11); // 256 prescaler
  TCCR1B |= (1 << CS10); // 256 prescaler

  TIMSK1 |= (1 << OCIE1A); // Activer le mode de comparaison

  delay(1000);

  interrupts(); // activer toutes les interruptions
  Serial.println("fin init");

  // 3AC72A8
  //  0011 1010 1101 0111 0010 1010 1000 1001
  //  0011 1010 1101 0111 0010 1010 1000 1001
  //  0011 1010 1101 0111 0010 1010 1001 1001 0110 mise à 1
  //  0011 1010 1101 0111 0010 1010 1001 1000 0000 envoie

  //0001 0101 0100 1010 0011 1100 1000 0000
  //0001 0101 0100 1010 0011 1100 1001 0000


  envoie.emetteur[0] = 0 ;
  envoie.emetteur[1] = 0 ;
  envoie.emetteur[2] = 1 ;
  envoie.emetteur[3] = 1 ;

  envoie.emetteur[4] = 1 ;
  envoie.emetteur[5] = 0 ;
  envoie.emetteur[6] = 1 ;
  envoie.emetteur[7] = 0 ;

  envoie.emetteur[8] = 1 ;
  envoie.emetteur[9] = 1 ;
  envoie.emetteur[10] = 0 ;
  envoie.emetteur[11] = 1 ;

  envoie.emetteur[12] = 0 ;
  envoie.emetteur[13] = 1 ;
  envoie.emetteur[14] = 1 ;
  envoie.emetteur[15] = 1 ;

  envoie.emetteur[16] = 0 ;
  envoie.emetteur[17] = 0 ;
  envoie.emetteur[18] = 1 ;
  envoie.emetteur[19] = 0 ;

  envoie.emetteur[20] = 1 ;
  envoie.emetteur[21] = 0 ;
  envoie.emetteur[22] = 1 ;
  envoie.emetteur[23] = 0 ;

  envoie.emetteur[24] = 1 ;
  envoie.emetteur[25] = 0 ;
  envoie.groupe = 0 ;
  envoie.etat = 0 ;

  envoie.interupteur[0] = 1;
  envoie.interupteur[1] = 0;
  envoie.interupteur[2] = 0;
  envoie.interupteur[3] = 1;



  //envoie1
  envoie1.emetteur[0] = 0 ;
  envoie1.emetteur[1] = 0 ;
  envoie1.emetteur[2] = 1 ;
  envoie1.emetteur[3] = 1 ;

  envoie1.emetteur[4] = 1 ;
  envoie1.emetteur[5] = 0 ;
  envoie1.emetteur[6] = 1 ;
  envoie1.emetteur[7] = 0 ;

  envoie1.emetteur[8] = 1 ;
  envoie1.emetteur[9] = 1 ;
  envoie1.emetteur[10] = 0 ;
  envoie1.emetteur[11] = 1 ;

  envoie1.emetteur[12] = 0 ;
  envoie1.emetteur[13] = 1 ;
  envoie1.emetteur[14] = 1 ;
  envoie1.emetteur[15] = 1 ;

  envoie1.emetteur[16] = 0 ;
  envoie1.emetteur[17] = 0 ;
  envoie1.emetteur[18] = 1 ;
  envoie1.emetteur[19] = 0 ;

  envoie1.emetteur[20] = 1 ;
  envoie1.emetteur[21] = 0 ;
  envoie1.emetteur[22] = 1 ;
  envoie1.emetteur[23] = 0 ;

  envoie1.emetteur[24] = 1 ;
  envoie1.emetteur[25] = 0 ;
  envoie1.groupe = 1 ;
  envoie1.etat = 1 ;

  envoie1.interupteur[0] = 1;
  envoie1.interupteur[1] = 0;
  envoie1.interupteur[2] = 0;
  envoie1.interupteur[3] = 1;

  envoie1.level[0] = 1;
  envoie1.level[1] = 0;
  envoie1.level[2] = 0;
  envoie1.level[3] = 1;


  envoieO.emetteur[0] = 0 ;
  envoieO.emetteur[1] = 0 ;
  envoieO.emetteur[2] = 0 ;
  envoieO.emetteur[3] = 1 ;

  envoieO.emetteur[4] = 0 ;
  envoieO.emetteur[5] = 1 ;
  envoieO.emetteur[6] = 0 ;
  envoieO.emetteur[7] = 1 ;

  envoieO.emetteur[8] = 0 ;
  envoieO.emetteur[9] = 1 ;
  envoieO.emetteur[10] = 0 ;
  envoieO.emetteur[11] = 0 ;

  envoieO.emetteur[12] = 1 ;
  envoieO.emetteur[13] = 0 ;
  envoieO.emetteur[14] = 1 ;
  envoieO.emetteur[15] = 0 ;

  envoieO.emetteur[16] = 0 ;
  envoieO.emetteur[17] = 0 ;
  envoieO.emetteur[18] = 1 ;
  envoieO.emetteur[19] = 1 ;

  envoieO.emetteur[20] = 1 ;
  envoieO.emetteur[21] = 1 ;
  envoieO.emetteur[22] = 0 ;
  envoieO.emetteur[23] = 0 ;

  envoieO.emetteur[24] = 1 ;
  envoieO.emetteur[25] = 0 ;
  envoieO.groupe = 0 ;
  envoieO.etat = 1 ;

  envoieO.interupteur[0] = 0;
  envoieO.interupteur[1] = 0;
  envoieO.interupteur[2] = 0;
  envoieO.interupteur[3] = 0;

  envoieF.emetteur[0] = 0 ;
  envoieF.emetteur[1] = 0 ;
  envoieF.emetteur[2] = 0 ;
  envoieF.emetteur[3] = 1 ;

  envoieF.emetteur[4] = 0 ;
  envoieF.emetteur[5] = 1 ;
  envoieF.emetteur[6] = 0 ;
  envoieF.emetteur[7] = 1 ;

  envoieF.emetteur[8] = 0 ;
  envoieF.emetteur[9] = 1 ;
  envoieF.emetteur[10] = 0 ;
  envoieF.emetteur[11] = 0 ;

  envoieF.emetteur[12] = 1 ;
  envoieF.emetteur[13] = 0 ;
  envoieF.emetteur[14] = 1 ;
  envoieF.emetteur[15] = 0 ;

  envoieF.emetteur[16] = 0 ;
  envoieF.emetteur[17] = 0 ;
  envoieF.emetteur[18] = 1 ;
  envoieF.emetteur[19] = 1 ;

  envoieF.emetteur[20] = 1 ;
  envoieF.emetteur[21] = 1 ;
  envoieF.emetteur[22] = 0 ;
  envoieF.emetteur[23] = 0 ;

  envoieF.emetteur[24] = 1 ;
  envoieF.emetteur[25] = 0 ;
  envoieF.groupe = 0 ;
  envoieF.etat = 0 ;

  envoieF.interupteur[0] = 0;
  envoieF.interupteur[1] = 0;
  envoieF.interupteur[2] = 0;
  envoieF.interupteur[3] = 0;



}

void loop() {
  int toto;

  if (Serial.available() > 0) {
    caractere = Serial.read();
    switch (caractere)
    {
      case 111: DIO.envoieCours1(envoieO);
        break;
      case 102 : DIO.envoieCours1(envoieF);
        break;
    }
    //  DIO.envoieCours1(envoieO);
    requete = 5;
    //DIO.envoieCours(0x3AC72A8, 0, 1, 0x9);
    //requete = 5;
    Serial.print("recu : ");
    Serial.print(caractere);
  }

  if (requete)
  {
    if (!DIO.enCours)
    {

      //DIO.envoieCours(0x3AC72A8, 0, 1, 0x9);
      switch (caractere)
      {
        case 111: DIO.envoieCours1(envoieO);
          break;
        case 102 : DIO.envoieCours1(envoieF);
          break;
      }
      //   DIO.envoieCours1(envoieO);
      requete--;

    }
  }
  toto = (micros() - memo);

  if (recept && (toto > 20000))
  { ok = 1;
    recept = 0;
    //    Serial.print(toto);
    //    Serial.print(" : ");
    //    Serial.print(micros());
    //    Serial.print(" : ");
    //          Serial.print(micros() - memo);
    //  Serial.println(" -> fini");
  }

  if ((j != i) && recept)
  {
    noInterrupts();
    if (val[j])
    {
      tampon = val[j];
      Serial.print("valeur : ");
      Serial.println(tampon );
      if (tampon > 2000)
      {
        ok = 1;
        recept = 0;
        //    Serial.println("fini1");
      }
      else
      {
        if (tampon > 750)
        {
          trame[k] = 1;
          //     Serial.print("1");
        }
        else
        {
          //                Serial.print("0");
          trame[k] = 0;
        }
        k++;
      }
      val[j] = 0;

    }
    if (j++ >= maxi) j = 0;
    interrupts();
  }

  if (ok)
  {
    l = 0;
    while (l < k && k != 0)
    {
      if (trame[l] == 1 && trame[l + 1] == 0)
        Serial.print("1");
      else
        Serial.print("0");
      l = l + 2;
    }
    Serial.println();
    ok = 0;
    k = 0;
  }


}


void blink()
{
  unsigned long top;
  recept = 1;
  top = micros();
  val[i] = top - memo;
  memo = top;
  if (i++ >= maxi) i = 0;
}


void period() {
  DIO.maj();
}


ISR(TIMER1_COMPA_vect) // fonction périodique
{
  period();
}

