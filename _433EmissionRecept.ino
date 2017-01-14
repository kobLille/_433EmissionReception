#include <DIO.h>


#define ledPin 8
#define maxi 200


int val[maxi];
unsigned long  memo, tampon;
bool trame[100];
int i, j, k, l, ok, recept, requete, caractere;
int memoEtatSorti = 1;
DIOClass DIO;

void setup() {


  Serial.begin(230400);

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
    if (caractere)
    {
      DIO.envoieCours(0xEB5CAA, 0, 1, 0x9, 3);
      //  break;
      delay(3000);
      DIO.envoieCours(0xEB5CAA, 0, 0, 0x9, 3);
      //  break;
    }
    Serial.print("recu : ");
    Serial.println(caractere);
  }
}

ISR(TIMER1_COMPA_vect) // fonction périodique
{
  DIO.maj();
}

