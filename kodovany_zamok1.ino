
// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>

char getChar(void);  // deklaracia vlastnej funkcie

// priradenie pinov Arduino ku klávesnici
#define ROW1   9
#define ROW2   8
#define ROW3   7
#define ROW4   6
#define COL1  A5
#define COL2  A4
#define COL3  A3
#define COL4  A2

#define PIEZO A1
#define SERVO 10

#define ROWS  4
#define COLS  4

// Globálne premenné
const char tabulka_znakov[ROWS][COLS] = {{'1','2','3','A'},
                                         {'4','5','6','B'},
                                         {'7','8','9','C'},
                                         {'*','0','#','D'}};

const char pinkod_zamku[] = "1A*D";

unsigned long startTime;				// čas procesora pred vstupom do loop()
unsigned long loopTime;					// čas procesora v loop()
unsigned long cas_rampa = 5000;   // 5000 ms

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// deklarácia skupiny funkcií ku obsluhe serva z knižnice
Servo  servo_p;

void setup() {
  pinMode(ROW1,INPUT_PULLUP);
  pinMode(ROW2,INPUT_PULLUP);
  pinMode(ROW3,INPUT_PULLUP);
  pinMode(ROW4,INPUT_PULLUP);
  pinMode(COL1,OUTPUT); digitalWrite(COL1, HIGH);
  pinMode(COL2,OUTPUT); digitalWrite(COL2, HIGH);
  pinMode(COL3,OUTPUT); digitalWrite(COL3, HIGH);
  pinMode(COL4,OUTPUT); digitalWrite(COL4, HIGH);
  
  pinMode(PIEZO, OUTPUT); digitalWrite(PIEZO, LOW);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  // ovladanie serva cez pin
  servo_p.attach(SERVO);
  servo_p.write(5);	// otočí servo na uhol = 5°
  
  //lcd.setCursor(0, 0);  // nastaví výpis od 1.znaku v 1.riadku
  lcd.print("Zadaj kod");
  lcd.setCursor(0, 1);  // nastaví výpis od 1.znaku v 2.riadku
  lcd.print("----");

  startTime = millis();
  
}  // koniec setup()

void loop() {

  char znaky[4];      // pole znakov, ktoré zadáme z klávesnice
  char znak;          // znak, zadaný z klávesnice
  int  count_char;    // počítadlo znakov
  int  zhoda_pin;  // = 1, ak je zadaný kód zhodný s pin kódom
                      // inak = 0
  
  // 4 miestny PIN = pole znakov "znaky[]"
  // Cyklus1: počíta vypísané znaky
  //   Slučka1: čaká na stlačenie znaku
  //   Výpis stlačeného znaku
  //   Slučka2: čaká na pustenie znaku
  // navrat na Cyklus1 
  // po zadaní 4.znaku porovná reťazec s PIN kódom
  
  lcd.setCursor(0, 1);  // nastaví výpis od 1.znaku v 2.riadku

  // Cyklus1
  for(count_char = 0; count_char < 4; ++ count_char) {
    // Slučka1
    do {
      znaky[count_char] = getChar();
    } while(znaky[count_char] == NULL);   // čaká na stlačenie znaku
   
    lcd.print(znaky[count_char]);   // vypíše stlačený znak
    
    // Slučka2
    do {
      znak = getChar();
    } while(znak != NULL);   // čaká na pustenie znaku

  } // koniec for(;;)

  // porovná znaky v pinkod_zamku[] so zadanými znakmi kod_zamku[]
  zhoda_pin = 1;		// prednastaví
  for(count_char = 0; count_char < 4; ++count_char) {
    if(znaky[count_char] == pinkod_zamku[count_char])
      zhoda_pin = zhoda_pin * 1;
    else
      zhoda_pin = 0;
  }
  
  // ak sú kódy zhodné:
  //   zapípaj na 0.5 sek.
  //   zmaž zadané znaky (nahradí ich "----")
  // inak zapípaj nízkym tónom na 0.5 sek.
  if(zhoda_pin == 1) {
    tone(PIEZO, 3000, 500);		// vysoký tón pípne na 500 ms
    servo_p.write(180);			// otočí servo na uhol = 180°
    delay(500);
    lcd.setCursor(0, 1);    // nastaví výpis od 1.znaku v 2.riadku
    lcd.print("----");		// zmaže zadané znaky
  }
  else tone(PIEZO, 216, 500);	// nízky tón pípne na 500 ms

  // návrat serva po určitom čase
  loopTime = millis();		// načíta aktuálny čas v ms
  if(loopTime >= (startTime + cas_rampa)) {
    startTime = loopTime;
  	servo_p.write(5);		// vráti servo na uhol = 5°
  }
  
} // koniec loop()

// --------------------------------
// Definicia vlastnej funkcie

char getChar(void) {

  byte riadok;
  byte stlpec;
  
  // Obsluha klávesnice
  // nastav COL1 = LOW, COL2-COL4 = HIGH
  // prečítaj stav na ROW1-ROW4
  //  - ak ROW1 = 0 -> znak = '1' (riadok = 1, stlpec = 1)
  //  - ak ROW2 = 0 -> znak = '4' (riadok = 2, stlpec = 1)
  //  - ak ROW3 = 0 -> znak = '7' (riadok = 3, stlpec = 1)
  //  - ak ROW4 = 0 -> znak = '*' (riadok = 4, stlpec = 1)
  
  stlpec = 1;
  digitalWrite(COL1, LOW);
  digitalWrite(COL2, HIGH);
  digitalWrite(COL3, HIGH);
  digitalWrite(COL4, HIGH);
  delay(20);
  if (digitalRead(ROW1)==LOW) riadok = 1;
  else if (digitalRead(ROW2)==LOW) riadok = 2;
  else if (digitalRead(ROW3)==LOW) riadok = 3;
  else if (digitalRead(ROW4)==LOW) riadok = 4;
  else riadok = 0;
  
if (riadok == 0) {
  stlpec = 2;
  digitalWrite(COL1, HIGH);
  digitalWrite(COL2, LOW);
  digitalWrite(COL3, HIGH);
  digitalWrite(COL4, HIGH);
  delay(20);
  if (digitalRead(ROW1)==LOW) riadok = 1;
  else if (digitalRead(ROW2)==LOW) riadok = 2;
  else if (digitalRead(ROW3)==LOW) riadok = 3;
  else if (digitalRead(ROW4)==LOW) riadok = 4;
  else riadok = 0;
}

if (riadok == 0) {
  stlpec = 3;
  digitalWrite(COL1, HIGH);
  digitalWrite(COL2, HIGH);
  digitalWrite(COL3, LOW);
  digitalWrite(COL4, HIGH);
  delay(20);
  if (digitalRead(ROW1)==LOW) riadok = 1;
  else if (digitalRead(ROW2)==LOW) riadok = 2;
  else if (digitalRead(ROW3)==LOW) riadok = 3;
  else if (digitalRead(ROW4)==LOW) riadok = 4;
  else riadok = 0;
}

if (riadok == 0) {
  stlpec = 4;
  digitalWrite(COL1, HIGH);
  digitalWrite(COL2, HIGH);
  digitalWrite(COL3, HIGH);
  digitalWrite(COL4, LOW);
  delay(20);
  if (digitalRead(ROW1)==LOW) riadok = 1;
  else if (digitalRead(ROW2)==LOW) riadok = 2;
  else if (digitalRead(ROW3)==LOW) riadok = 3;
  else if (digitalRead(ROW4)==LOW) riadok = 4;
  else riadok = 0;
}
 if (riadok != 0) 
   return tabulka_znakov[riadok-1][stlpec-1];
 else
   return NULL;
  
}  // koniec getChar()
