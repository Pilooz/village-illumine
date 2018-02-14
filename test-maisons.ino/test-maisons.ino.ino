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

#define BUILDIN_LED 13
#define DATA_PIN    5
#define LED_TYPE    NEOPIXEL
#define NUM_LEDS  16 //60 //12 // 432 
#define NB_MAISONS 4 //15 //6 // 95 

//CRGB leds[NUM_LEDS];
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

uint8_t gBrightness = 255;
//CRGB gColor = CRGB(80, 160, 40);
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
} Maison;

Maison maisons[NB_MAISONS]; // Le village tout entier

Maison initMaison(int ilot, String maison, int deb, int fin, boolean fixe) {
  Maison m;
  m.num_ilot = ilot;
  m.num_maison = maison;
  m.num_deb = deb;
  m.num_fin = fin;
  m.fixe = fixe;
  m.on = fixe; //par defaut on est à True si l'eclairage doit etre fixe
  m.cycle = 1000000;
  m.start = 0;
  return m;
}

void initVillage() {
  // TEst ilot 2
  /*
  maisons[0] = initMaison(2, "0", 0, 7, true); // Ilot 2, maison 1 : 8 leds.
  maisons[1] = initMaison(2, "1", 8, 15, true); // Ilot 2, maison 1 : 8 leds.
  maisons[2] = initMaison(2, "2", 16, 23, true); // Ilot 2, maison 2 : 8 leds.
  maisons[3] = initMaison(2, "3", 24, 31, true); // Ilot 2, maison 3 : 8 leds.
  maisons[4] = initMaison(2, "4", 32, 47, true); // Ilot 2, maison 4 : 8 leds.
*/
  // TEst ilot 3
  maisons[0] = initMaison(2, "1", 0, 3, true); // Ilot 2, maison 1 : 8 leds.
  maisons[1] = initMaison(2, "2", 4, 7, true); // Ilot 2, maison 2 : 8 leds.
  maisons[2] = initMaison(2, "3", 8, 11, true); // Ilot 2, maison 3 : 8 leds.
  maisons[3] = initMaison(2, "4", 12, 15, true); // Ilot 2, maison 4 : 8 leds.

  // Tests Ilot 6
  /*
    maisons[0] = initMaison(6, "1", 4, 7, false); // Ilot 6, maison 1 : 4 leds.
    maisons[1] = initMaison(6, "2", 8, 11, false); // Ilot 6, maison 2 : 4 leds.
    maisons[2] = initMaison(6, "3", 12, 15, false); // Ilot 6, maison 3 : 4 leds.
    maisons[3] = initMaison(6, "4", 16, 19, false); // Ilot 6, maison 4 : 4 leds.
    maisons[4] = initMaison(6, "5", 20, 23, false); // Ilot 6, maison 5 : 4 leds.
    maisons[5] = initMaison(6, "6", 24, 27, false); // Ilot 6, maison 6 : 4 leds.
    maisons[6] = initMaison(6, "7", 28, 31, false); // Ilot 6, maison 7 : 4 leds.
    maisons[7] = initMaison(6, "8", 32, 35, false); // Ilot 6, maison 8 : 4 leds.
    maisons[8] = initMaison(6, "9", 36, 39, false); // Ilot 6, maison 9 : 4 leds.
    maisons[9] = initMaison(6, "10", 40, 43, false); // Ilot 6, maison 10 : 4 leds.
    maisons[10] = initMaison(6, "11", 44, 47, false); // Ilot 6, maison 11 : 4 leds.
    maisons[11] = initMaison(6, "12", 48, 51, false); // Ilot 6, maison 12 : 4 leds.
    maisons[12] = initMaison(6, "13", 52, 55, false); // Ilot 6, maison 13 : 4 leds.
    maisons[13] = initMaison(6, "14", 56, 59, false); // Ilot 6, maison 14 : 4 leds.
    maisons[14] = initMaison(6, "15", 60, -1, false); // Ilot 6, maison 15 : 4 leds.
  */
  // Test
}

void switchHome(int idx, boolean on) {
  String change = "#" + String(idx) + " -> Ilot #" + String(maisons[idx].num_ilot) + ", maison #" + String(maisons[idx].num_maison) + " : ";
  String etat = "";
  for (int i = maisons[idx].num_deb; i <= maisons[idx].num_fin; i++) {
    if (!on) {
      //leds[i] = CRGB::Black;
      strip.setPixelColor(i, 0, 0, 0);
      etat = "OFF";
    } else {
      //leds[i] = gColor;
      strip.setPixelColor(i, g, r, b);
      etat = "ON";
    }
  }
  change += etat + " pour " + String( maisons[idx].cycle / 1000000) + " s.";
  change += "\t" + infoMaxCurrent();
  Serial.println(change);
}

void dumpHomeState() {
  // Parcours de toutes les maisons
  for (int i = 0; i < NB_MAISONS; i++) {
    String change = "#" + String(i) + " -> Ilot #" + String(maisons[i].num_ilot) + ", maison #" + String(maisons[i].num_maison) + " : ";
    String etat =  (maisons[i].on) ? "ON" : "OFF";
    change += etat + " pour " + String( maisons[i].cycle / 1000000) + " s.";
    change += "\t" + infoMaxCurrent();
    Serial.println(change);
  }
}

String infoMaxCurrent() {
  int nb_leds = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    if ( strip.getPixelColor(i) > 0 ) {
      nb_leds++;
    }
  }
  return String(nb_leds * 60) + "mA";
}

void setRandomState() {
  int on = 0;
  int off = 0;
  int fixes = 0;
  for (int i = 0; i < NB_MAISONS; i++) {
    maisons[i].cycle = 1000000;
    maisons[i].on = false;
    off++;
  }
  Serial.print(on); Serial.println(" maisons éclairées.");
  Serial.print(off); Serial.println(" maisons éteintes.");
  Serial.print(fixes); Serial.println(" maisons en lumière fixe.");
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();

  Serial.print(" -> "); Serial.print(NUM_LEDS); Serial.println(" leds...");
  //FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  Serial.print(" -> Set brigthness to "); Serial.println(gBrightness);
  //FastLED.setBrightness(gBrightness);
  strip.begin();
  strip.setBrightness(gBrightness);
  // All leds black
  Serial.println(" -> Toutes les leds à noir.");
  //leds[0] = CRGB::Black;
  //FastLED.show();
  strip.clear();
  strip.show();

  //Init des maisons du village
  initVillage();
  Serial.print(" -> "); Serial.print(NB_MAISONS); Serial.println(" maisons...");

  Serial.println("Etat intial des maisons aléatoire...");
  setRandomState();
  for (int i = 0; i < NB_MAISONS; i++) {
    switchHome(i, maisons[i].on);
  }
  //FastLED.show();
  strip.show();

  // Allumer les maisons en éclairage fixe.

  Serial.println(" -> Initialisation du timer toutes les 0.1 seconde");
  //myTimer.begin(changeHomesState, TIMER_INTERVAL);  // runs every 0.10 seconds

  Serial.println("------- Fin du setup ! -------- \n\n");
  Serial.println("---------------------- ETATS INITIAUX -------------------------");
  dumpHomeState();
  Serial.println("---------------------------------------------------------------");
  // Led 13 on.
  pinMode(BUILDIN_LED, OUTPUT);
  digitalWrite(BUILDIN_LED, HIGH);
}

void loop() {
  for (int i = 0; i < NB_MAISONS; i++) {
    switchHome(i, true);
    strip.show();
    delay(1000);
  }
  for (int i = 0; i < NB_MAISONS; i++) {
    switchHome(i, false);
    strip.show();
    delay(1000);
  }
}

