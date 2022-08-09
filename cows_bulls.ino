#include <LiquidCrystal.h>
#include <Keypad.h>

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5); //4-bit mode; connect d0-d3 in addition for 8-bit mode
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = // Define the Keymap
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = { 5, 4, 3, 2 }; // Connect keypad ROW0, ROW1, ROW2 and ROW3 to associated Arduino pins.
byte colPins[COLS] = { 6, 7, 8, 9 }; // Connect keypad COL0, COL1, COL2 and COL3 to associated Arduino pins.

Keypad pad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
char key;

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
}

int bulls = 0;
int cows = 0;
void game(int digits) {
  int guessednumbers[digits][10];
  int num [digits];
  int guess [digits];
  int rando;
  bool taken;
  for (int i = 0; i < digits; i++) {
    taken = false;
    rando = random(1, 10);
    while (taken == false && i > 0) {
      for (int j = 0; j < i; j++) {
        if (rando == num[j]) {
          taken = true;
          rando = random(1, 10);
          j = -1;
        }
      }
      taken = true;
    }
    num[i] = rando;
    Serial.print(num[i]);
  }
  taken = false;
  Serial.println();
  int attempts = 0;
  bool won = false;
  while (attempts < 7 && won == false) {
    bool validnumber = false;
    while (validnumber == false) {
      lcd.clear();
      lcd.print("Enter ");
      lcd.print(digits);
      lcd.print(" digits");
      lcd.setCursor(0, 1);
      lcd.print("number: ");
      Serial.print("Guessed: ");
      for (int i = 0; i < digits ; i++) {
        key = pad.getKey();
        while (key == NO_KEY) {
          key = pad.getKey();
        }
        lcd.print(key);
        guess[i] = key - '0';
        delay(250);
        Serial.print(guess[i]);
      }
      Serial.println();
      validnumber = checkifvalid(guess, digits, guessednumbers, attempts);
      if (validnumber == false) {
        lcd.clear();
        lcd.print("Not allowed num");
        lcd.setCursor(0, 1);
        lcd.print("Enter num again");
        delay(2000);
      }
    }
    for (int i = 0; i < digits; i++) {
      guessednumbers[i][attempts] = guess[i];
    }
    won = comparenumbers(num, guess, digits);
    delay(4000);
    attempts++;
    if (won == false && attempts < 7) {
      lcd.clear();
      lcd.print("* to continue");
      lcd.setCursor(0, 1);
      lcd.print("# for prev nums");
      key = pad.getKey();
      while (key != '*' && key != '#') {
        key = pad.getKey();
      }
      if (key == '#') {
        showAllNums(num, digits, guessednumbers, attempts);
      }
    }
  }
  if (won == true) {
    lcd.clear();
    lcd.print("Congratz!");
    lcd.setCursor(0, 1);
    lcd.print("You won!");
  }
  else {
    lcd.clear();
    lcd.print("You lost :(");
    lcd.setCursor(0,1);
    lcd.print("Number was:");
    for(int i=0;i<digits;i++){
      lcd.print(num[i]);
    }
  }
  delay(5000);
  lcd.clear();
  lcd.print("Play Again?");
  lcd.setCursor(0, 1);
  lcd.print("Yes=A    No=B");
  key = pad.getKey();
  while (key != 'A' && key != 'B') {
    key = pad.getKey();
  }
  if (key == 'B') {
    lcd.clear();
    lcd.print("Thanks for");
    lcd.setCursor(0,1);
    lcd.print("playing!");
    delay(5000);
  }
}

bool checkifvalid(int arr[], int size, int guessednumbers[][10], int guessednumberssize) {

  bool equal = true;
  for (int i = 0; i < guessednumberssize; i++) {
    equal = true;
    for (int j = 0; j < size; j++) {
      if (arr[j] != guessednumbers[j][i] || arr[j] == 0) {
        equal = false;
        break;
      }
    }
    if (equal == true) return false;
  }
  
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < i; j++) {
      if (arr[j] == arr[i] && i != j) {
        return false;
      }
    }
  }
  return true;
}

bool comparenumbers(int num[], int guess[], int size) {
  bulls = 0;
  cows = 0;
  for (int i = 0; i < size ; i++) {
    for (int j = 0; j < size; j++) {
      if (guess[j] == num[i]) {
        if (j == i) bulls++;
        else cows++;
      }
    }
  }
  lcd.clear();
  print(guess, size);
  lcd.setCursor(7, 0);
  lcd.print(bulls);
  lcd.print(" bulls");
  lcd.setCursor(7, 1);
  lcd.print(cows);
  lcd.print(" cows");
  if (bulls == size) return true;
  else return false;
}
void print(int arr[], int size) {
  for (int i = 0 ; i < size ; i++) {
    lcd.print(arr[i]);
  }
}

void showAllNums(int num[],  int size, int guessednumbers[][10], int guessednumberssize) {
  int arr [size];
  int i = 0;
  while (true) {
    for (int j = 0; j < size; j++) {
      arr[j] = guessednumbers[j][i];
    }
    bulls = 0;
    cows = 0;
    for (int i = 0; i < size ; i++) {
      for (int j = 0; j < size; j++) {
        if (arr[j] == num[i]) {
          if (j == i) bulls++;
          else cows++;
        }
      }
    }
    lcd.clear();
    print(arr, size);
    lcd.setCursor(6, 0);
    lcd.print(bulls);
    lcd.print("Blls ");
    lcd.print(cows);
    lcd.print("Cws");
    lcd.setCursor(0, 1);
    lcd.print("Bck=4 Next=6 X=0");
    key = pad.getKey();
    while (key != '4' && key != '0' && key != '6') {
      key = pad.getKey();
    }
    if (key == '4') {
      if (i > 0) i--;
    }
    if (key == '6') {
      if (i < guessednumberssize - 1) i++;
    }
    if (key == '0') return;
  }
}


void loop()
{
  lcd.print("Welcome to Abdul");
  lcd.setCursor(0, 1);
  lcd.print("rahmans C&B Game");
  delay(4000);
  while (true) {
    lcd.clear();
    lcd.print("Press Any Key to");
    lcd.setCursor(0, 1);
    lcd.print("Start Playing");
    key = pad.getKey();
    while (key == NO_KEY) {
      key = pad.getKey();
    }
    lcd.clear();
    lcd.print("Select");
    lcd.setCursor(0, 1);
    lcd.print("Difficulty:");
    delay(1500);
    lcd.clear();
    lcd.print("Easy=1 Medium=2");
    lcd.setCursor(0, 1);
    lcd.print("Hard=3");
    key = pad.getKey();
    while (key == NO_KEY) {
      key = pad.getKey();
    }
    while (key != '1' && key != '2' && key != '3') {
      lcd.clear();
      lcd.print("Wrong Key!");
      delay(1500);
      lcd.clear();
      lcd.print("Easy=1 Medium=2");
      lcd.setCursor(0, 1);
      lcd.print("Hard=3");
      key = pad.getKey();
      while (key == NO_KEY) {
        key = pad.getKey();
      }
    }
    lcd.clear();
    if (key == '1') {
      lcd.print("Easy difficulty");
      lcd.setCursor(0, 1);
      lcd.print("Chosen");
      delay(1500);
      game(3);
    }
    else if (key == '2') {
      lcd.print("Medium diffculty");
      lcd.setCursor(0, 1);
      lcd.print("Chosen");
      delay(1500);
      game(4);
    }
    else {
      lcd.print("Hard difficulty");
      lcd.setCursor(0, 1);
      lcd.print("Chosen");
      delay(1500);
      game(5);
    }
  }
}
