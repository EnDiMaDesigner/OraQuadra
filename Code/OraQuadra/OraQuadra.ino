// OraQuadra V1.0
// By Davide Gatti SURVIVAL HACKING  www.survivalhacking.it
// Modified by En.Di.Ma.

// 1. Modificati valori luminosità per lettere stampate in PETG trasparente
// 2. Semplificata inizializzazione facendo rimanere soltanto l'effetto Rainbow
// 3. Cancellata scritta Survival Hacking ed inserito testo EN DI MA
// 4. Aggiunte definizioni 'SONOL' 'EORE' 'ORE' per intervallare le frasi di lettura su pannello
// 5. Fissato colore bianco sul testo 'SONO LE ORE' 'E' 'MINUTI'
// 6. Invertito ciclo IF che va a cambiare la luminosità giorno e notte.
// 7. Corretto 'case 40' in 'AggiornaMinuti' che andava a colorare di rosso la lettera 'E' (ora rimosso)

// Setup:
//
// Pulsante 1 = per la regolazione dell'ora
// Pulsante 2 = per la regolazione deli minuti
// Pulsante 1 + Pulsante 2 = cambio preset 0=Sfumato bianco  1-5=Scrittura lenta vari colori  6-10=Scrittira veloce vari colori
//

#include "RTClib.h"
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

RTC_DS3231 rtc;       // Istanza per usare l'RTC

//Definizioni per neopixel
#define PIN   4        // pin neopixel
#define NUMPIXELS 256  // 16 x 16
#define GIORNO 30     // 64 Luminosità di girno
#define NOTTE 4       // 32 Luminosità di notte

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int intBrightness = 50; // Imposta la luminosità dei NeoPixel (0-255)

//Definizione colori
uint32_t Red = strip.Color(255, 0, 0); 
uint32_t Green = strip.Color( 0, 255, 0);
uint32_t Blue = strip.Color( 0, 0, 255);
uint32_t White = strip.Color( 255, 255, 255);
uint32_t Yellow = strip.Color( 255, 255, 0);
uint32_t Purple = strip.Color( 60, 0, 255);
uint32_t Off = strip.Color( 0, 0, 0);

int P1= 3; // Tasto O  avanzamento ore
int P2= 2; // Tasto M  avanzamento minuti

int oracorrente;        // variabile ora corrente
int oraprecedente;      // variabile ora precedente
int minutocorrente;     // variabile minuto corrente
int minutoprecedente;   // variabile minuto precedente
int mode;               // variabile per la modalità   
int modo;               // variabile per la modalità mirror
int Presets;            // Preset corrente
uint32_t Colore;        // Variabile per il passaggio colore

// Mappatura matrice
// S-015 O-014 N-013 O-012 S-011 L-010 E-009 U-008 O-007 R-006 E-005 R-004 T-003 R-002 E-001 V-000
// D-016 U-017 E-018 I-019 S-020 E-021 I-022 V-023 Q-024 U-025 A-026 T-027 T-028 R-029 O-030 A-031
// U-047 N-046 D-045 I-044 C-043 I-042 L-041 U-040 N-039 A-038 H-037 S-036 E-035 T-034 T-033 E-032
// N-048 O-049 V-050 E-051 A-052 O-053 T-054 T-055 O-056 C-057 D-058 I-059 E-060 C-061 I-062 K-063
// C-079 I-078 N-077 Q-076 U-075 E-074 I-073 D-072 O-071 D-070 I-069 C-068 I-067 N-066 E-065 G-064
// C-080 I-081 N-082 Q-083 U-084 A-085 N-086 T-087 A-088 Z-089 T-090 R-091 E-092 N-093 T-094 A-095
// Q-111 U-110 A-109 R-108 A-107 N-106 T-105 A-104 Q-103 V-102 E-101 N-100 T-099 I-098 U-097 A-096
// D-112 I-113 C-114 I-115 A-116 N-117 N-118 O-119 V-120 E-121 D-122 S-123 E-124 T-125 T-126 E-127
// D-143 O-142 D-141 I-140 C-139 I-138 R-137 D-136 I-135 C-134 I-133 O-132 T-131 T-130 O-129 A-128
// O-144 T-145 T-146 O-147 T-148 Q-149 U-150 A-151 T-152 T-153 O-154 R-155 D-156 I-157 C-158 I-159
// S-175 E-174 D-173 I-172 C-171 I-170 O-169 D-168 I-167 E-166 C-165 I-164 N-163 T-162 R-161 E-160
// U-176 N-177 D-178 I-179 C-180 I-181 E-182 Q-183 U-184 I-185 N-186 D-187 I-188 C-189 I-190 X-191
// D-207 I-206 C-205 I-204 A-203 S-202 S-201 E-200 T-199 T-198 E-197 F-196 N-195 O-194 V-193 E-192
// T-208 R-209 E-210 D-211 I-212 C-213 I-214 J-215 Q-216 U-217 A-218 T-219 T-220 R-221 O-222 P-223
// C-239 I-238 N-237 Q-236 U-235 E-234 W-233 D-232 U-231 E-230 R-229 S-228 E-227 I-226 N-225 O-224
// U-240 N-241 O-242 N-243 U-244 N-245 A-246 R-247 Y-248 M-249 I-250 N-251 U-252 T-253 I-254 B-255


//Definizione testi vari
int  txtSONOLEORE[] = {15,14,13,12,10,9,7,6,5,-1};
int  txtSONOLE[] = {15,14,13,12,10,9,-1};
int  txtSONOL[] = {15,14,13,12,10,-1};
int  txtEORE[] = {9,7,6,5,-1};
int  txtORE[] = {7,6,5,-1};
int  txtENDIMA[] = {43,52,75,84,107,41,42,74,73,105,106,103,88,71,56,39,57,69,100,91,68,59,36,144,175,176,207,208,145,173,178,205,209,148,171,180,203,212,215,200,183,168,151,167,166,154,165,186,197,218,220,195,188,163,157,158,160,191,192,223,190,189,-1};//{11,8,4,0,19,23,31,41,37,52,57,63,73,66,64,-1};
int  txtMINUTI[] = {249,250,251,252,253,254,-1};

//Definizione delle stringhe delle ore
int  txtUNA[] = {38,39,40,-1};
int  txtDUE[] = {16,17,18,-1};
int  txtTRE[] = {1,2,3,-1};
int  txtQUATTRO[] = {24,25,26,27,28,29,30,-1};
int  txtCINQUE[] = {79,78,77,76,75,74,-1};
int  txtSEI[] = {20,21,22,-1};
int  txtSETTE[] = {32,33,34,35,36,-1};
int  txtOTTO[] = {53,54,55,56,-1};
int  txtNOVE[] = {48,49,50,51,-1};
int  txtDIECI[] = {58,59,60,61,62,-1};
int  txtUNDICI[] = {42,43,44,45,46,47,-1};
int  txtDODOCI[] = {67,68,69,70,71,72,-1};

// Definizione delle stringhe dei minuti
int  txtE[] = {65,-1};
int  txtMUNO[] = {240,241,242,-1};
int  txtMDUE[] = {230,231,232,-1};
int  txtMTRE[] = {160,161,162,-1};
int  txtMQUATTRO[] = {216,217,218,219,220,221,222,-1};
int  txtMCINQUE[] = {234,235,236,237,238,239,-1};
int  txtMSEI[] = {226,227,228,-1};
int  txtMSETTE[] = {123,124,125,126,127,-1};
int  txtMOTTO[] = {144,145,146,147,-1};
int  txtMNOVE[] = {192,193,194,195,-1};
int  txtMDIECI[] = {164,165,166,167,168,-1};
int  txtMUNDICI[] = {176,177,178,179,180,181,-1};
int  txtMDODICI[] = {138,139,140,141,142,143,-1};
int  txtMTREDICI[] = {208,209,210,211,212,213,214,-1};
int  txtMQUATTORDICI[] = {149,150,151,152,153,154,155,156,157,158,159,-1};
int  txtMQUINDICI[] = {183,184,185,186,187,188,189,190,-1};
int  txtMSEDICI[] = {170,171,172,173,174,175,-1};
int  txtMDICIASSETTE[] = {197,198,199,200,201,202,203,204,205,206,207,-1};
int  txtMDICIOTTO[] = {129,130,131,132,133,134,135,136,-1};
int  txtMDICIANNOVE[] = {112,113,114,115,116,117,118,119,120,121,-1};
int  txtMVENT[] = {99,100,101,102,-1};
int  txtMVENTI[] = {98,99,100,101,102,-1};
int  txtMTRENT[] = {90,91,92,93,94,-1};
int  txtMTRENTA[] = {90,91,92,93,94,95,-1};
int  txtMQUARANT[] = {105,106,107,108,109,110,111,-1};
int  txtMQUARANTA[] = {104,105,106,107,108,109,110,111,-1};
int  txtMCINQUANT[] = {80,81,82,83,84,85,86,87,-1};
int  txtMCINQUANTA[] = {80,81,82,83,84,85,86,87,88,-1};


void setup () {
  
  if (EEPROM.read(0) != 0x55) {           // read configuration byte. If it is not 55h is not configured, than store a default values
    EEPROM.write(0, 0x55);                // write configuret marker
    EEPROM.write(1, 0);                   // scrivi il Preset di default
  }
  
  Presets=(EEPROM.read(1));               // Leggi eeprom e imposta il preset memorizzato
  Preset(Presets);                        // imposta il modo grafico del relativo preset

  strip.begin();                          // inizializza matrice neo pixel
  strip.clear();                          // spegni tutti i led della matrice  
  rainbow(1);                             // faiun effetto arcobaleno
  strip.clear();
  strip.show();

//Lettera E
strip.setPixelColor(43,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(52,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(75,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(84,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(107,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(41,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(42,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(74,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(73,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(105,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(106,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);

//Lettera N
strip.setPixelColor(103,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(88,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(71,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(56,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(39,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(57,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(69,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(100,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(91,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(68,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(59,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(36,Green);
strip.show();                           // aggiorna matrice neopixel
delay(70);

//Lettera D
strip.setPixelColor(144,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(175,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(176,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(207,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(208,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(145,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(173,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(178,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(205,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(209,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);

//Lettera I
strip.setPixelColor(148,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(171,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(180,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(203,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(212,Yellow);
strip.show();                           // aggiorna matrice neopixel
delay(70);

//Lettera M
strip.setPixelColor(215,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(200,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(183,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(168,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(151,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(167,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(166,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(154,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(165,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(186,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(197,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(218,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);

//Lettera A
strip.setPixelColor(220,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(195,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(188,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(163,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(157,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(158,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(160,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(191,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(192,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(223,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(190,Blue);
strip.show();                           // aggiorna matrice neopixel
delay(70);
strip.setPixelColor(189,Blue);
strip.show();                           // aggiorna matrice neopixel

  delay(3000);

  strip.clear();                          // spegni tutti i led della matrice  
  strip.show();                           // aggiorna matrice neopixel

  pinMode(P1,INPUT_PULLUP);               // definisce il tipo di ingresso digitale con pullup
  pinMode(P2,INPUT_PULLUP);               // definisce il tipo di ingresso digitale con pullup

  oraprecedente =-1;                      // impostazione variabile per aggiornamento forzato ora
  minutoprecedente =-1;                   // impostazione variabile per aggiornamento forzato ora

  if (! rtc.begin()) {                    // inizializza RTC  
    while (1) delay(10);                  // se non trova l'RTC si blocca qui all'infinito
  }

  if (rtc.lostPower()) {                  // se viene rilevata una perdita di alimentazione l'RTC si imposta con la data e ora programmata
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop () {
  if(!digitalRead(P1) && !digitalRead(P2)) {  // Se si schiacciano i due pulsanti in contemporanea si cambia il preset
    Presets+=1;
    if (Presets > 10) Presets=0;
    Preset(Presets);
    EEPROM.write(1, Presets);               // scrivi Preset in EEPROM
  } else if(!digitalRead(P1)) {             // se si preme il pulsante H si regolano le ore
    DateTime now = rtc.now();               // leggi ora dall'RTC
    int ora = now.hour();       
    int minuti = now.minute();
    ora+=1;                                 // incrementa ora 
    if (ora > 23 ) ora = 0;                 // controlla limiti 
    rtc.adjust(DateTime(2024,12,7,ora,minuti,0)); // scrivi ora nell'RTC
    delay(100); 
    mode=2;                                 // imposta modalità aggiornamento led veloce 
  } else if(!digitalRead(P2)) {             // se si preme il pulsante M si regolano i minuti 
    DateTime now = rtc.now();               // leggi ora dall'RTC
    int ora = now.hour();
    int minuti = now.minute();
    minuti+=1;                              // incrementa minuti
    if (minuti > 59 ) minuti = 0;           // controlla limiti 
    rtc.adjust(DateTime(2024,12,7,ora,minuti,0)); // scrivi ora nell'RTC
    delay(100);
    mode=2;                                 // imposta modalità aggiornamento led veloce 
  } else {
    mode=modo;                              // se non sono premuti tasti metti la modalità impostata dai Preset
  }
  DateTime now = rtc.now();               // leggi ora dall'RTC
  if((now.hour() < 7) || (now.hour() >= 19)){  // se nella fascia oraria dalle 7 alle 19 alza la luminosità
    intBrightness =  GIORNO;              // setta la luminosità giorno
  } else {
    intBrightness =  NOTTE;               // setta la luminosità notturna
  }
  strip.setBrightness(intBrightness);     // imposta la luminosità nella matrice neopixel
  strip.show();                           // aggiorna la matrice

  minutocorrente = now.minute();          // imposta variabile locale dei minuti correnti
  oracorrente = now.hour();               // imposta variabile locale delle ore correnti

  //condizione che va a scrivere "SONO L UNA", scrive durante il giorno "ORE", scrive la sera "SONO LE"
  if((now.hour() == 1) || (now.hour() == 13)){ // se sono le 1 o le 13
    paintWord(txtEORE, Off,mode);              // cancella 'SONO LE ORE'
    paintWord(txtSONOL, White,mode);           // scrivi 'SONO L'
  } else if((now.hour() < 0) || (now.hour() >= 12)){ // se si entra nell'intervallo dalle 0 alle 12
    paintWord(txtORE, White, mode);            // scrivi 'ORE'
    paintWord(txtSONOLE, Off,mode);            // cancella 'SONO LE'
  } else {
    paintWord(txtSONOLE, White, mode);        // scrivi 'SONO LE'
    paintWord(txtORE, Off,mode);               // cancella 'ORE'
  }
  
  strip.show();

  //paintWord(txtSONOLEORE, Colore, mode);   // scrivi 'SONO LE ORE'

  if (oracorrente != oraprecedente) {     // verifica se + cambiata l'ora
    AggiornaOre(oraprecedente,Off,2);     // cancella l'ora precedente
    AggiornaOre(oracorrente,Colore,mode); // scrivi l'ora corrente
    oraprecedente=oracorrente;            // imposta l'ora precedente
    strip.show();                         // aggiorna la matrice
  }  

  if (minutocorrente != minutoprecedente) {       // verifica se + cambiati i minuti
    AggiornaMinuti(minutoprecedente,Off,2);       // cancella i minuti precedenti
    if (minutocorrente == 0) {                    // se minuti = 0 non scrivere "E"
      paintWord(txtE, Off,2);                     // cancella "E"
      paintWord(txtMINUTI, Off,2);                // cancella "MINUTI"
    } else {
      paintWord(txtE, White,mode);               // Se minuti <> 0 scrivi "E"
      AggiornaMinuti(minutocorrente,Colore,mode); // scrivi i minuti correnti
      paintWord(txtMINUTI, White,mode);          // Se minuti <> 0 scrivi "MINUTI"
    }
    minutoprecedente=minutocorrente;              // imposta l'ora precedente
    strip.show();                                 // aggiorna la matrice
  }  
  strip.show();                                   // aggiorna la matrice

  delay(500);                                     // attesa di un secondo per far pulsare la scritta se in modalità 0
}


void Preset(int Passedpreset) {                   // imposta preset grafico. 0=sufmato bianco 1-5=Scriturra lenta vari colori  6-10=Scrittura rapida vari colori
  switch (Passedpreset) { 
  case 0:
    modo=0;
    Colore=White;
    break;    
  case 1:
    modo=1;
    Colore=White;
    break;    
  case 2:
    modo=1;
    Colore=Blue;
    break;    
  case 3:
    modo=1;
    Colore=Red;
    break;    
  case 4:
    modo=1;
    Colore=Purple;
    break;    
  case 5:
    modo=1;
    Colore=Green;
    break;    
  case 6:
    modo=2;
    Colore=White;
    break;    
  case 7:
    modo=2;
    Colore=Blue;
    break;    
  case 8:
    modo=2;
    Colore=Red;
    break;    
  case 9:
    modo=2;
    Colore=Purple;
    break;    
  case 10:
    modo=2;
    Colore=Green;
    break;    
  }
  strip.clear();
  strip.show();
  oraprecedente =-1;
  minutoprecedente =-1;
  mode=modo;
  delay(500);
}


void AggiornaOre(int ora,uint32_t Colore, int modo) { // in base all'ora passata, scrive stringa dell'ora nella matrice
 switch (ora) { 
    case -1:
      break;
    case 0:
    case 12: 
    case 24:
      paintWord(txtDODOCI, Colore, modo);
      break;
    case 1:
    case 13:      
      paintWord(txtUNA, Colore, modo);
      break;
    case 2:
    case 14:
      paintWord(txtDUE, Colore, modo);
      break;
    case 3:
    case 15:
      paintWord(txtTRE, Colore, modo);
      break;
    case 4:
    case 16:
      paintWord(txtQUATTRO, Colore, modo);
      break;
    case 5:
    case 17:
      paintWord(txtCINQUE, Colore, modo);
      break;
    case 6:
    case 18:
      paintWord(txtSEI, Colore, modo);
      break;
    case 7:
    case 19:
      paintWord(txtSETTE, Colore, modo);
      break;
    case 8:
    case 20:
      paintWord(txtOTTO, Colore, modo);
      break;
    case 9:
    case 21:
      paintWord(txtNOVE, Colore, modo);
      break;
    case 10:
    case 22:
      paintWord(txtDIECI, Colore, modo);
      break;
    case 11:
    case 23:
      paintWord(txtUNDICI, Colore, modo);
      break;
    }
}

void AggiornaMinuti(int minuti, uint32_t Colore, int modo) { // in base alminuto passato, scrive stringa del minuto nella matrice
  switch (minuti) { 
    case -1:
      break;
    case 0:
      break;
    case 1:
      paintWord(txtMUNO, Colore, modo);
      break;
    case 2:
      paintWord(txtMDUE, Colore, modo);
      break;
    case 3:
      paintWord(txtMTRE, Colore, modo);
      break;
    case 4:
      paintWord(txtMQUATTRO, Colore, modo);
      break;
    case 5:
      paintWord(txtMCINQUE, Colore, modo);
      break;
    case 6:
      paintWord(txtMSEI, Colore, modo);
      break;
    case 7:
      paintWord(txtMSETTE, Colore, modo);
      break;
    case 8:
      paintWord(txtMOTTO, Colore, modo);
      break;
    case 9:
      paintWord(txtMNOVE, Colore, modo);
      break;
    case 10:
      paintWord(txtMDIECI, Colore, modo);
      break;
    case 11:
      paintWord(txtMUNDICI, Colore, modo);
      break;
    case 12:
      paintWord(txtMDODICI, Colore, modo);
      break;
    case 13:
      paintWord(txtMTREDICI, Colore, modo);
      break;
    case 14:
      paintWord(txtMQUATTORDICI, Colore, modo);
      break;
    case 15:
      paintWord(txtMQUINDICI, Colore, modo);
      break;
    case 16:
      paintWord(txtMSEDICI, Colore, modo);
      break;
    case 17:
      paintWord(txtMDICIASSETTE, Colore, modo);
      break;
    case 18:
      paintWord(txtMDICIOTTO, Colore, modo);
      break;
    case 19:
      paintWord(txtMDICIANNOVE, Colore, modo);
      break;
    case 20:
      paintWord(txtMVENTI, Colore, modo);
      break;
    case 21:
      paintWord(txtMVENT, Colore, modo);
      paintWord(txtMUNO, Colore, modo);
      break;
    case 22:
      paintWord(txtMVENTI, Colore, modo);
      paintWord(txtMDUE, Colore, modo);
      break;
    case 23:
      paintWord(txtMVENTI, Colore, modo);
      paintWord(txtMTRE, Colore, modo);
      break;
    case 24:
      paintWord(txtMVENTI, Colore, modo);
      paintWord(txtMQUATTRO, Colore, modo);
      break;
    case 25:
      paintWord(txtMVENTI, Colore, modo);
      paintWord(txtMCINQUE, Colore, modo);
      break;
    case 26:
      paintWord(txtMVENTI, Colore, modo);
      paintWord(txtMSEI, Colore, modo);
      break;
    case 27:
      paintWord(txtMVENTI, Colore, modo);
      paintWord(txtMSETTE, Colore, modo);
      break;
    case 28:
      paintWord(txtMVENT, Colore, modo);
      paintWord(txtMOTTO, Colore, modo);
      break;
    case 29:
      paintWord(txtMVENTI, Colore, modo);
      paintWord(txtMNOVE, Colore, modo);
      break;
    case 30:
      paintWord(txtMTRENTA, Colore, modo);
      break;
    case 31:
      paintWord(txtMTRENT, Colore, modo);
      paintWord(txtMUNO, Colore, modo);
      break;
    case 32:
      paintWord(txtMTRENTA, Colore, modo);
      paintWord(txtMDUE, Colore, modo);
      break;
    case 33:
      paintWord(txtMTRENTA, Colore, modo);
      paintWord(txtMTRE, Colore, modo);
      break;
    case 34:
      paintWord(txtMTRENTA, Colore, modo);
      paintWord(txtMQUATTRO, Colore, modo);
      break;
    case 35:
      paintWord(txtMTRENTA, Colore, modo);
      paintWord(txtMCINQUE, Colore, modo);
      break;
    case 36:
      paintWord(txtMTRENTA, Colore, modo);
      paintWord(txtMSEI, Colore, modo);
      break;
    case 37:
      paintWord(txtMTRENTA, Colore, modo);
      paintWord(txtMSETTE, Colore, modo);
      break;
    case 38:
      paintWord(txtMTRENT, Colore, modo);
      paintWord(txtMOTTO, Colore, modo);
      break;
    case 39:
      paintWord(txtMTRENTA, Colore, modo);
      paintWord(txtMNOVE, Colore, modo);
      break;
    case 40:
      //(paintWord(txtE, Colore, modo);
      paintWord(txtMQUARANTA, Colore, modo);
      break;
    case 41:
      paintWord(txtMQUARANT, Colore, modo);
      paintWord(txtMUNO, Colore, modo);
      break;
    case 42:
      paintWord(txtMQUARANTA, Colore, modo);
      paintWord(txtMDUE, Colore, modo);
      break;
    case 43:
      paintWord(txtMQUARANTA, Colore, modo);
      paintWord(txtMTRE, Colore, modo);
      break;
    case 44:
      paintWord(txtMQUARANTA, Colore, modo);
      paintWord(txtMQUATTRO, Colore, modo);
      break;
    case 45:
      paintWord(txtMQUARANTA, Colore, modo);
      paintWord(txtMCINQUE, Colore, modo);
      break;
    case 46:
      paintWord(txtMQUARANTA, Colore, modo);
      paintWord(txtMSEI, Colore, modo);
      break;
    case 47:
      paintWord(txtMQUARANTA, Colore, modo);
      paintWord(txtMSETTE, Colore, modo);
      break;
    case 48:
      paintWord(txtMQUARANT, Colore, modo);
      paintWord(txtMOTTO, Colore, modo);
      break;
    case 49:
      paintWord(txtMQUARANTA, Colore, modo);
      paintWord(txtMNOVE, Colore, modo);
      break;
    case 50:
      paintWord(txtMCINQUANTA, Colore, modo);
      break;
    case 51:
      paintWord(txtMCINQUANT, Colore, modo);
      paintWord(txtMUNO, Colore, modo);
      break;
    case 52:
      paintWord(txtMCINQUANTA, Colore, modo);
      paintWord(txtMDUE, Colore, modo);
      break;
    case 53:
      paintWord(txtMCINQUANTA, Colore, modo);
      paintWord(txtMTRE, Colore, modo);
      break;
    case 54:
      paintWord(txtMCINQUANTA, Colore, modo);
      paintWord(txtMQUATTRO, Colore, modo);
      break;
    case 55:
      paintWord(txtMCINQUANTA, Colore, modo);
      paintWord(txtMCINQUE, Colore, modo);
      break;
    case 56:
      paintWord(txtMCINQUANTA, Colore, modo);
      paintWord(txtMSEI, Colore, modo);
      break;
    case 57:
      paintWord(txtMCINQUANTA, Colore, modo);
      paintWord(txtMSETTE, Colore, modo);
      break;
    case 58:
      paintWord(txtMCINQUANT, Colore, modo);
      paintWord(txtMOTTO, Colore, modo);
      break;
    case 59:
      paintWord(txtMCINQUANTA, Colore, modo);
      paintWord(txtMNOVE, Colore, modo);
      break;
    }
}

void paintWord(int arrWord[], uint32_t intColor, int mode) { // Scrivi su matrice la strina passata, con eventuale colore e modalità 
  if (mode == 0) {                                           // 0=modo sfumato solo bianco
    if (intColor == 0) {
      paintWordFast(arrWord,intColor);
    } else {
      paintWordFast(arrWord,strip.Color( 30, 30, 30));
      delay(100);  
      paintWordFast(arrWord,strip.Color( 60, 60, 60));
      delay(100);  
      paintWordFast(arrWord,strip.Color( 90, 90, 90));
      delay(100);  
      paintWordFast(arrWord,strip.Color( 120, 120, 120));
      delay(100);  
      paintWordFast(arrWord,strip.Color( 150, 150, 150));
      delay(100);  
      paintWordFast(arrWord,strip.Color( 180, 180, 180));
      delay(100);  
      paintWordFast(arrWord,strip.Color( 210, 210, 210));
      delay(100);  
      paintWordFast(arrWord,strip.Color( 240, 240, 240));
      delay(100);  
      paintWordFast(arrWord,intColor);
    }
  } else if (mode == 1) {                                     // 1=modo lento con gestione colore
    paintWordSlow(arrWord, intColor);
  } else if (mode == 2) {                                     // 2=modo veloce con gestione colore
    paintWordFast(arrWord, intColor);
  }

}


void paintWordSlow(int arrWord[], uint32_t intColor){     // scrittura nella matrice della sctringa passata in modalità lenta
  for(int i = 0; i < strip.numPixels() + 1; i++){
    if(arrWord[i] == -1) {
      strip.show();
      break;
    } else {
      strip.setPixelColor(arrWord[i],intColor);
      strip.show();
      delay(50);
    }
  }
}

void paintWordFast(int arrWord[], uint32_t intColor){         // scrittura nella matrice della stringa passata in modalità veloce
  for(int i = 0; i < strip.numPixels() + 1; i++){
    if(arrWord[i] == -1) {
      strip.show();
      break;
    } else {
      strip.setPixelColor(arrWord[i],intColor);
   }
  }
}


void paintWordSpeed(int arrWord[], uint32_t intColor, int speed) {  // scrittura nella matrice della stringa passata in modalità variabile
  for(int i = 0; i < strip.numPixels() + 1; i++){
    if(arrWord[i] == -1) {
      strip.show();
      break;
    } else {
      strip.setPixelColor(arrWord[i],intColor);
      strip.show();
      delay(speed);
    }
  }
}



void rainbow(uint8_t wait) {                // accendi tutto il display con un arcobaleno animato
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}


uint32_t Wheel(byte WheelPos) {             // calcola il colore nel formato HUE
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
