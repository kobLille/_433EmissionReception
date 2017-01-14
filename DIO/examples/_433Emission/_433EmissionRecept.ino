
#define ledPin 8
#define maxi 200


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
      Serial.println("Lancement : ");
      Serial.println(trame, BIN);
      Serial.println(pointer, BIN);
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

int val[maxi];
unsigned long  memo, tampon;
bool trame[100];
int i, j, k, l, ok, recept, requete, caractere;
int memoEtatSorti = 1;
DIOClass DIO;

void setup() {

  // pinMode(ledPin, OUTPUT);
  //start serial connection
  Serial.begin(230400);
  //configure pin2 as an input and enable the internal pull-up resistor
  //pinMode(3, INPUT_PULLUP);
  //attachInterrupt(1, blink, RISING);

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

}

void loop() {
  int toto;

  if (Serial.available() > 0) {
    caractere = Serial.read();
    //   DIO.envoieCours(0xEB5CAA, 0, 1, 0x9, 10);
    if (caractere)
    {
      DIO.envoieCours(0xEB5CAA, 0, 1, 0x9, 3);
      //  break;
          delay(3000);
          DIO.envoieCours(0xEB5CAA, 0, 0, 0x9, 3);
      //  break;
    }
    //caractere = 0;
    //  DIO.envoieCours1(envoieO);
    requete = 5;
    //DIO.envoieCours(0x3AC72A8, 0, 1, 0x9);
    //requete = 5;
    Serial.print("recu : ");
    Serial.println(caractere);


  }

  // if (requete)
  // {
  //  if (!DIO.enCours)
  //  {

  //DIO.envoieCours(0x3AC72A8, 0, 1, 0x9);
  //    switch (caractere)
  //    {
  //       case 111: DIO.envoieCours1(envoieO);
  //         break;
  //      case 102 : DIO.envoieCours1(envoieF);
  //        break;
  //    }
  //     //   DIO.envoieCours1(envoieO);
  //     requete--;

  //   }
  // }
  //  toto = (micros() - memo);
  //
  //  if (recept && (toto > 20000))
  //  { ok = 1;
  //    recept = 0;
  //    //    Serial.print(toto);
  //    //    Serial.print(" : ");
  //    //    Serial.print(micros());
  //    //    Serial.print(" : ");
  //    //          Serial.print(micros() - memo);
  //    //  Serial.println(" -> fini");
  //  }
  //
  //  if ((j != i) && recept)
  //  {
  //    noInterrupts();
  //    if (val[j])
  //    {
  //      tampon = val[j];
  //      Serial.print("valeur : ");
  //      Serial.println(tampon );
  //      if (tampon > 2000)
  //      {
  //        ok = 1;
  //        recept = 0;
  //        //    Serial.println("fini1");
  //      }
  //      else
  //      {
  //        if (tampon > 750)
  //        {
  //          trame[k] = 1;
  //          //     Serial.print("1");
  //        }
  //        else
  //        {
  //          //                Serial.print("0");
  //          trame[k] = 0;
  //        }
  //        k++;
  //      }
  //      val[j] = 0;
  //
  //    }
  //    if (j++ >= maxi) j = 0;
  //    interrupts();
  //  }
  //
  //  if (ok)
  //  {
  //    l = 0;
  //    while (l < k && k != 0)
  //    {
  //      if (trame[l] == 1 && trame[l + 1] == 0)
  //        Serial.print("1");
  //      else
  //        Serial.print("0");
  //      l = l + 2;
  //    }
  //    Serial.println();
  //    ok = 0;
  //    k = 0;
  //  }


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

