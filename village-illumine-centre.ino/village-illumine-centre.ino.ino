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

// Temps de cycle de maintien des LEDs min et max
// Ce temps défini toute la beauté du dispositif : éloge de la lenteur des changements !
#define WAIT_CYCLE_MIN 2
#define WAIT_CYCLE_MAX 30

#define BUILDIN_LED 13
#define DATA_PIN    3
#define LED_TYPE    NEOPIXEL
#define NUM_LEDS3   32 //12 //432
#define NB_MAISONS3 7 //4 //94

Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUM_LEDS3, 3, NEO_GRB + NEO_KHZ800);

uint8_t gBrightness = 255;
int gTimeToChange = 1;

int r = 255; //80;
int g = 255; //160;
int b = 255; //40;

volatile byte seconds = 0;
int End_cycle_wait = 1;

// Structure Maison
typedef struct {
  int num_ilot;
  int num_maison;
  int num_deb;
  int num_fin;
  boolean fixe = false;
  boolean on = false;
} Maison;

Maison maisons[NB_MAISONS3];
int index_maison = 0; // index de la derniere maison allumée

Maison initMaison(int ilot, int maison, int deb, int fin) {
  Maison m;
  m.num_ilot = ilot;
  m.num_maison = maison;
  m.num_deb = deb;
  m.num_fin = fin;
  m.fixe = false;
  m.on = false; //par defaut on est à True si l'eclairage doit etre fixe
  return m;
}

void toggle_maison(int idx) {
  for (int i = maisons[idx].num_deb; i <= maisons[idx].num_fin; i++) {
    if (maisons[idx].on) {
      //leds[i] = CRGB::Black;
      strip3.setPixelColor(i, 0, 0, 0);
    } else {
      //leds[i] = gColor;
      strip3.setPixelColor(i, g, r, b);
    }
  }
  maisons[idx].on = !maisons[idx].on;
  //FastLED.show();
  strip3.show();
}

int get_random_index() {
  return random(0, NB_MAISONS3);
}

void writeDebug() {
  Serial.print(" Index #");
  Serial.print(index_maison);
  Serial.print(", Ilot #");
  Serial.print(maisons[index_maison].num_ilot);
  Serial.print(", Maison #");
  Serial.print(maisons[index_maison].num_maison);
  Serial.print(", - wait : ");
  Serial.print(End_cycle_wait);
  Serial.print( "s, ");
  if (maisons[index_maison].on) {
    Serial.print( "ON");
  } else {
    Serial.print( "OFF");
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("Setting up the village...");
  Serial.print(" -> "); Serial.print(NB_MAISONS3); Serial.println(" houses...");
  Serial.print(" -> "); Serial.print(NUM_LEDS3); Serial.println(" leds...");
  strip3.begin();
  strip3.setBrightness(gBrightness);
  // set master brightness control
  Serial.print(" -> Setting brigthness to "); Serial.println(gBrightness);

  /* Init des maisons du village */
  maisons[0] = initMaison(2, 1, 0, 3);
  maisons[1] = initMaison( 2, 2, 4, 7);
  maisons[2] = initMaison( 2, 3, 8, 11);
  maisons[3] = initMaison( 2, 4, 12, 15);
  maisons[4] = initMaison( 2, 5, 16, 19);
  maisons[5] = initMaison( 3, 1, 20, 23);
  maisons[6] = initMaison( 3, 2, 24, 31);

  Serial.print(" -> Setting timer...");
  // Init timer 1 pour Les changements d'état
  // initialize Timer1
  cli();          // disable global interrupts
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B

  // set compare match register to desired timer count:
  OCR1A = (1 / 6.4e-5) - 1; // = 15624; Une seconde.
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  // enable global interrupts:
  sei();
  Serial.println("Setup complete !\n\n");
    pinMode(BUILDIN_LED, OUTPUT);
  digitalWrite(BUILDIN_LED, HIGH);
}

void loop() {
}

/*
   Timer1 pour piloter les changement d'états des LEDs
*/
ISR(TIMER1_COMPA_vect)
{
  if (seconds == 0) {
    writeDebug();
  }
  seconds++;
  Serial.print(".");
  if (seconds == End_cycle_wait)
  {
    Serial.println("");
    seconds = 0;
    index_maison = get_random_index();
    toggle_maison(index_maison); // Allumage d'une maison
    End_cycle_wait = random(WAIT_CYCLE_MIN, WAIT_CYCLE_MAX);   // Calcul du prochain temps d'attente
  }
}

