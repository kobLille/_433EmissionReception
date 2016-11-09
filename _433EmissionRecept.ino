
#define ledPin 8
#define maxi 300

typedef struct requete requetes;
struct requete
{
  bool emetteur[26] ;
  bool groupe ;
  bool etat ;
  bool interupteur[4];
};

class DIOClass {

  public:
    int cpt1;
    int cpt0;
    int sEmis;
    int index;
    int enCours;
    int tailleRequete;
    int toggle;
    byte tEnvoieRequete[40];

    void begin(uint8_t sortie) { //Fontion permettant l'initialisation
      Serial.println("begin");
      sEmis = sortie;
      pinMode(sEmis, OUTPUT);
      digitalWrite(sEmis, LOW);
      infoSortie = 1;
      index = 0;
      tailleRequete = sizeof(requetes);
    }

    void maj() {  //Fonction permettant la mise à jour en fonction du timer
      if (enCours) {
        if (cpt1) {
          digitalWrite(sEmis, LOW);
          infoSortie = 1;
          cpt1--;
        }
        else {
          if (cpt0) {
            digitalWrite(sEmis, HIGH);
            infoSortie = 0;
            cpt0--;
          }
          else {
            digitalWrite(sEmis, LOW);
            infoSortie = 1;
            bitSuivant();
          }
        }
      }
    }

    void envoieCours(requetes envoieRequete) { //Fonction permettant de lancer l'envoie de la requete
      Serial.println("Lancement");
      enCours = 1;
      initCpt(2);
      index = -1;
      toggle = 0;
      memcpy(&tEnvoieRequete, &envoieRequete, tailleRequete);
      Serial.println("requete : ");

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
        enCours = 0;
        Serial.println("Fin");
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
          cpt0 = 3;
          break;
        case 2:  //cpt pour le début de l'envoie d'une trame
          cpt1 = 0;
          cpt0 = 10;
          break;
      }
    }

  private:
    int infoSortie;

};

unsigned int val[maxi];
unsigned long  memo, tampon;
unsigned long  memo1, tampon1;
bool trame[100];
int i, j, k, l, ok;
requetes envoie;
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

  DIO.begin(8);

  delay(1000);

  // initialiser le timer1
  noInterrupts(); // désactiver toutes les interruptions
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  //OCR1A = 31250; // 16MHz/256/2Hz
  //OCR1A = 6250; //toutes les secondes
   OCR1A = 30; //toutes les secondes
  //OCR1A = 500000; //toutes les secondes
  TCCR1B |= (1 << WGM12); // CTC mode
  // TCCR1B |= (1 << CS12); // 256 prescaler
  TCCR1B |= (1 << CS11); // 256 prescaler
  TCCR1B |= (1 << CS10); // 256 prescaler

  TIMSK1 |= (1 << OCIE1A); // Activer le mode de comparaison
  interrupts(); // activer toutes les interruptions

}

void loop() {
  //  0011 1010 1101 0111 0010 1010 1000 1001

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

  if (Serial.available() > 0) {
    DIO.envoieCours(envoie);
    Serial.println("recu");
    Serial.read();
  }

  if (j != i)
  {
    if (val[j])
    {
      noInterrupts();
      tampon = val[j];
      Serial.print("valeur : ");
      Serial.println(tampon );
      interrupts();
      if (tampon > 2500)
      {
        ok = 1;
        Serial.println();
      }
      else
      {
        if (tampon > 750)
        {
          trame[k] = 1;
          Serial.print("1");
        }
        else
        {
          Serial.print("0");
          trame[k] = 0;
        }
        k++;
      }
      val[j] = 0;

    }
    if (j++ >= maxi) j = 0;
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
  val[i] = (int)(micros() - memo);
//  Serial.print("valeur : ");
//  Serial.println(val[i]);

  memo = micros();
  if (i++ >= maxi) i = 0;
}


void period() {
  DIO.maj();
  //  if (DIO.etatSortie() != memoEtatSorti) {
  //    Serial.print("sortie : ");
  //    Serial.print(memoEtatSorti);
  //    Serial.print(", valBit : ");
  //    Serial.print(DIO.etatIndex());
  //    Serial.print(", index : ");
  //    Serial.print(DIO.valIndex());
  //    Serial.print(", pulse : ");
  //    tampon = millis();
  //    Serial.println(tampon - memo);
  //    memo = tampon;
  //    if (!memoEtatSorti) {
  //      Serial.println("");
  //    }
  //    memoEtatSorti = DIO.etatSortie();
  //  }
}


ISR(TIMER1_COMPA_vect) // fonction périodique
{
  //digitalWrite(ledPin, digitalRead(ledPin) ^ 1); // Basculer la LED allumée/éteinte
  period();
}

