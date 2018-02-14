/*****************************************************************
            Illumination d'un village de maisons.
                PGL Coding & Making 2018

  L'état initial des maisons allumées ou éteintes est définit aléatoirement
  sur le tableau de toutes les maisons sauf pour celles qui sont en lumière fixe
  qui restent toujours allumées.

  Un timer déclenché toute les secondes vérifie s'il faut eteindre
  une maison allumée en parcourant toute la structure village et en
  vérifiant que le temps d'allumage est atteint
 *****************************************************************/
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

// Temps de cycle de maintien des LEDs min et max en micro-secondes
// Ce temps défini toute la beauté du dispositif : éloge de la lenteur des changements !
#define WAIT_CYCLE_MIN 10000000 // 10 secondes
#define WAIT_CYCLE_MAX 60000000 // 60 secondes
// Intervalle de déclenchement du timer de mise à jour
#define TIMER_INTERVAL 100000   // 0.1 seconde 

#define BUILDIN_LED 13
#define LED_TYPE    NEOPIXEL
#define NUM_LEDS3  64
#define NB_MAISONS3 11

Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUM_LEDS3, 3, NEO_GRB + NEO_KHZ800);

uint8_t gBrightness = 255;
int r = 255; //80;
int g = 255; //160;
int b = 255; //40;

// Create an IntervalTimer object
IntervalTimer myTimer;

// Structure Maison
typedef struct {
  int num_ilot;
  String num_maison;
  int num_deb;
  int num_fin;
  boolean fixe = false;
  boolean on = false;
  uint32_t cycle = 1000000; // temps allumé ou éteint
  uint32_t start = 0;
  int pinout = 5;
} Maison;

Maison maisons[NB_MAISONS3]; // Le village tout entier

Maison initMaison(int pinout, int ilot, String maison, int deb, int fin) {
  Maison m;
  m.num_ilot = ilot;
  m.num_maison = maison;
  m.num_deb = deb;
  m.num_fin = fin;
  m.fixe = false;
  m.on = false; //par defaut on est à True si l'eclairage doit etre fixe
  m.cycle = 1000000;
  m.start = 0;
  m.pinout = pinout;
  return m;
}

void initVillage() {
  maisons[0] = initMaison(3, 1, 1, 0, 3);
  maisons[1] = initMaison(3, 1, 2, 4, 7);
  maisons[2] = initMaison(3, 1, 3, 8, 23);
  maisons[3] = initMaison(3, 1, 4, 12, 15);
  maisons[4] = initMaison(3, 2, 1, 16, 19);
  maisons[5] = initMaison(3, 2, 2, 20, 23);
  maisons[6] = initMaison(3, 2, 3, 24, 27);
  maisons[7] = initMaison(3, 3, 1, 28, 31);
  maisons[8] = initMaison(3, 3, 2, 32, 35);
  maisons[9] = initMaison(3, 3, 3, 36, 39);
}

void switchHome(int idx, boolean on) {
  String change = "#" + String(idx) + " -> Ilot #" + String(maisons[idx].num_ilot) + ", maison #" + String(maisons[idx].num_maison) + " : ";
  String etat = "";
  for (int i = maisons[idx].num_deb; i <= maisons[idx].num_fin; i++) {
    if (!on) {
      strip3.setPixelColor(i, 0, 0, 0);
      etat = "OFF";
    } else {
      strip3.setPixelColor(i, g, r, b);
      etat = "ON";
    }
  }
  change += etat + " pour " + String( maisons[idx].cycle / 1000000) + " s.";
  change += "\t" + infoMaxCurrent();
  Serial.println(change);
}

void changeHomesState() {
  // Parcours de toutes les maisons
  for (int i = 0; i < NB_MAISONS3; i++) {
    // l'eclairage n'est pas fixe
    if (!maisons[i].fixe) {
      if (micros() - maisons[i].start >= maisons[i].cycle) {
        maisons[i].cycle = random(WAIT_CYCLE_MIN, WAIT_CYCLE_MAX);
        maisons[i].start = micros();
        maisons[i].on = !maisons[i].on;
        switchHome(i, maisons[i].on);
        strip3.show();
      }
    }
  }
}

void dumpHomeState() {
  // Parcours de toutes les maisons
  for (int i = 0; i < NB_MAISONS3; i++) {
    String change = "#" + String(i) + " -> Ilot #" + String(maisons[i].num_ilot) + ", maison #" + String(maisons[i].num_maison) + " : ";
    String etat =  (maisons[i].on) ? "ON" : "OFF";
    change += etat + " pour " + String( maisons[i].cycle / 1000000) + " s.";
    change += "\t" + infoMaxCurrent();
    Serial.println(change);
  }
}

String infoMaxCurrent() {
  int nb_leds = 0;
  for (int i = 0; i < NUM_LEDS3; i++) {
    if ( strip3.getPixelColor(i) > 0 ) {
      nb_leds++;
    }
  }
  return String(nb_leds * 60) + "mA";
}

void setRandomState() {
  String seq = "110010000111011001100101010100110110111101110010011010010110000101101110011000010101000001101001011011000110111101110101";
  int on = 0;
  int off = 0;
  int fixes = 0;
  for (int i = 0; i < NB_MAISONS3; i++) {
    if (maisons[i].fixe) {
      maisons[i].on = true;
      maisons[i].cycle = 1000000;
      fixes++;
    } else {
      maisons[i].cycle = random(WAIT_CYCLE_MIN, WAIT_CYCLE_MAX);
      if (seq[i] == '1') {
        maisons[i].on = true;
        on++;
      } else {
        maisons[i].on = false;
        off++;
      }
    }
  }
  Serial.print(on); Serial.println(" maisons éclairées.");
  Serial.print(off); Serial.println(" maisons éteintes.");
  Serial.print(fixes); Serial.println(" maisons en lumière fixe.");
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();

  Serial.print(" -> "); Serial.print(NUM_LEDS3); Serial.println(" leds...");
  // set master brightness control
  Serial.print(" -> Set brigthness to "); Serial.println(gBrightness);
  strip3.begin();
  strip3.setBrightness(gBrightness);
  // All leds black
  Serial.println(" -> Toutes les leds à noir.");
  strip3.clear();
  strip3.show();

  //Init des maisons du village
  initVillage();
  Serial.print(" -> "); Serial.print(NB_MAISONS3); Serial.println(" maisons...");

  Serial.println("Etat intial des maisons aléatoire...");
  // setRandomState();
  for (int i = 0; i < NB_MAISONS3; i++) {
    // switchHome(i, maisons[i].on);
    switchHome(i, true);
  }
  strip3.show();

  // Allumer les maisons en éclairage fixe.

  Serial.println(" -> Initialisation du timer toutes les 0.1 seconde");
  myTimer.begin(changeHomesState, TIMER_INTERVAL);  // runs every 0.10 seconds

  Serial.println("------- Fin du setup ! -------- \n\n");
  //  Serial.println("---------------------- ETATS INITIAUX -------------------------");
  //  dumpHomeState();
  //  Serial.println("---------------------------------------------------------------");
  // Led 13 on.
  pinMode(BUILDIN_LED, OUTPUT);
  digitalWrite(BUILDIN_LED, HIGH);
}

void loop() {
}

