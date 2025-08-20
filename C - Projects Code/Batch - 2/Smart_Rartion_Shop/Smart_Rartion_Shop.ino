#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Item info
String items[] = {"Rice", "Sugar", "Wheat", "Oil", "Dhal", "Salt", "Tea"};
int prices[] = {20, 50, 15, 25, 30, 25, 10}; // per kg
int numItems = sizeof(items) / sizeof(items[0]);
int quantities[10]; // stores purchased quantities

// Login credentials
String validUsers[] = {"1001", "2002"};
String validPINs[] = {"1234", "5678"};
int numUsers = sizeof(validUsers) / sizeof(validUsers[0]);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print("Smart Ration Shop");
  delay(2500);
  lcd.clear();
}

void loop() {
  lcd.clear();
  lcd.print("Enter Username:");
  String username = getInput(4, true);

  lcd.clear();
  lcd.print("Enter PIN:");
  String pin = getInput(4, false);

  if (validateUser(username, pin)) {
    lcd.clear();
    lcd.print("Login Success");
    delay(2500);
    selectItems();
    showBill();
  } else {
    lcd.clear();
    lcd.print("Access Denied");
    delay(2500);
  }
}

bool validateUser(String u, String p) {
  for (int i = 0; i < numUsers; i++) {
    if (validUsers[i] == u && validPINs[i] == p) {
      return true;
    }
  }
  return false;
}

String getInput(int length, bool showChar) {
  String input = "";
  lcd.setCursor(0, 1);
  while (input.length() < length) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') break;
      else if (key == '*') {
        if (input.length() > 0) {
          input.remove(input.length() - 1);
          lcd.setCursor(input.length(), 1);
          lcd.print(" ");
          lcd.setCursor(input.length(), 1);
        }
      } else {
        input += key;
        lcd.setCursor(input.length() - 1, 1);
        lcd.print(showChar ? key : '*');
      }
    }
  }
  return input;
}

char waitForKey() {
  char key = 0;
  while (!key) key = keypad.getKey();
  return key;
}

void selectItems() {
  for (int i = 0; i < numItems; i++) quantities[i] = 0;
  int index = 0;
  bool done = false;

  while (!done) {
    lcd.clear();
    lcd.print(String(index + 1) + ". " + items[index]);
    lcd.setCursor(0, 1);
    lcd.print("Rate: ");
    lcd.print(prices[index]);
    lcd.print("/kg");
    delay(500);  // reduced delay

    char key = waitForKey();
    if (key == 'A') {
      if (index < numItems - 1) index++;
    } else if (key == 'B') {
      if (index > 0) index--;
    } else if (key == 'C') {
      lcd.clear();
      lcd.print("Enter Qty (kg):");
      String qtyStr = getInput(2, true);
      int qty = qtyStr.toInt();
      if (qty > 0) {
        quantities[index] += qty;
        lcd.clear();
        lcd.print("Added: ");
        lcd.print(qty);
        lcd.print(" kg");
        delay(1500);
      } else {
        lcd.clear();
        lcd.print("Invalid qty");
        delay(1000);
      }
    } else if (key == 'D') {
      done = true;
    } else {
      lcd.clear();
      lcd.print("Invalid Key");
      delay(1000);
    }
  }
}

void showBill() {
  int total = 0;
  lcd.clear();
  lcd.print("Calculating Bill");
  delay(2500);

  for (int i = 0; i < numItems; i++) {
    if (quantities[i] > 0) {
      lcd.clear();
      lcd.print(items[i] + ": " + String(quantities[i]) + "kg");
      int itemTotal = prices[i] * quantities[i];
      lcd.setCursor(0, 1);
      lcd.print("Total: ");
      lcd.print(itemTotal);
      total += itemTotal;
      delay(2000);
    }
  }

  lcd.clear();
  lcd.print("Pay: ");
  lcd.print(total);
  lcd.print(" Rupees");
  delay(3000);
  lcd.clear();
  lcd.print("Thank You!");
  delay(2500);
}
