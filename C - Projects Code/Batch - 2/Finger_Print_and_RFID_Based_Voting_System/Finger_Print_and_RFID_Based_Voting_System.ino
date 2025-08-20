#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

// === LCD Setup ===
const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// === Fingerprint Setup ===
SoftwareSerial fingerSerial(2, 3);  // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

// === RFID Setup ===
#define RST_PIN 9
#define SS_PIN 10
MFRC522 rfid(SS_PIN, RST_PIN);

// === Voting Buttons ===
#define btnC1 4      // TVK
#define btnC2 5      // ADMK
#define btnC3 6      // DMK
#define btnResult 8  // Show Result

// === Voter Database ===
const int numVoters = 4;
int registeredFingerIDs[] = {3, 4, 5, 6};
String registeredNames[] = {
  "Hariprasad N",
  "Aravindhan AP",
  "Aravith Kumar",
  "MadhuSudhanan"
};
String registeredRFIDs[] = {
  "ADC5B502",
  "17C34202",
  "A3FEB113",
  "D3C8072D"
};
bool hasVoted[numVoters] = {false, false, false, false};

// === Party List ===
const int numCandidates = 3;
String partyNames[numCandidates] = {"TVK", "ADMK", "DMK"};
int votes[numCandidates] = {0, 0, 0};

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Voting System");
  delay(2000);
  lcd.clear();

  // Fingerprint Init
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found.");
  } else {
    Serial.println("Fingerprint sensor NOT found!");
    while (1);
  }

  // RFID Init
  SPI.begin();
  rfid.PCD_Init();

  // Buttons
  pinMode(btnC1, INPUT_PULLUP);
  pinMode(btnC2, INPUT_PULLUP);
  pinMode(btnC3, INPUT_PULLUP);
  pinMode(btnResult, INPUT_PULLUP);
}

void loop() {
  static bool promptPrinted = false;

  // Always check result button first
  if (digitalRead(btnResult) == LOW) {
    delay(200); // Debounce
    showResults();
    return;
  }

  if (!promptPrinted) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan Finger...");
    promptPrinted = true;
  }

  int fid = getFingerprintID();
  if (fid == -1) return;

  int userIndex = findUserByFingerprint(fid);

  if (userIndex == -1) {
    promptPrinted = false;
    lcd.clear();
    lcd.print("Finger Not Found");
    delay(2000);
    return;
  }

  promptPrinted = false;
  lcd.clear();
  lcd.print("Welcome");
  lcd.setCursor(0, 1);
  lcd.print(registeredNames[userIndex].substring(0, 16));
  delay(2000);

  if (hasVoted[userIndex]) {
    lcd.clear();
    lcd.print("Already Voted");
    delay(2000);
    return;
  }

  lcd.clear();
  lcd.print("Scan RFID...");
  unsigned long startTime = millis();
  String uid = "";

  while (millis() - startTime < 5000) {
    uid = readRFID();
    if (uid != "") break;
  }

  if (uid == registeredRFIDs[userIndex]) {
    lcd.clear();
    lcd.print("Access Granted");
    delay(1500);
    votingMenu(userIndex);
  } else {
    lcd.clear();
    lcd.print("RFID Mismatch");
    delay(2000);
  }

  delay(100);
}

// === Voting Menu ===
void votingMenu(int voterIndex) {
  lcd.clear();
  lcd.print("Vote: TVK ADMK");
  lcd.setCursor(0, 1);
  lcd.print("DMK -> Btns");

  while (true) {
    if (digitalRead(btnC1) == LOW) {
      votes[0]++;
      confirmVote(0);
      break;
    }
    if (digitalRead(btnC2) == LOW) {
      votes[1]++;
      confirmVote(1);
      break;
    }
    if (digitalRead(btnC3) == LOW) {
      votes[2]++;
      confirmVote(2);
      break;
    }
  }

  hasVoted[voterIndex] = true;
}

void confirmVote(int index) {
  lcd.clear();
  lcd.print("Voted for:");
  lcd.setCursor(0, 1);
  lcd.print(partyNames[index]);
  delay(2000);
  lcd.clear();
}

// === Display Results & Winner ===
void showResults() {
  lcd.clear();
  lcd.print("TVK:");
  lcd.print(votes[0]);
  lcd.setCursor(0, 1);
  lcd.print("ADMK:");
  lcd.print(votes[1]);
  delay(2500);

  lcd.clear();
  lcd.print("DMK:");
  lcd.print(votes[2]);
  delay(2000);

  // Determine highest vote
  int maxVote = max(votes[0], max(votes[1], votes[2]));
  String winners = "";

  for (int i = 0; i < numCandidates; i++) {
    if (votes[i] == maxVote) {
      if (winners != "") winners += " & ";
      winners += partyNames[i];
    }
  }

  lcd.clear();
  lcd.print("Winner:");
  lcd.setCursor(0, 1);
  lcd.print(winners.substring(0, 16));  // Fit LCD width
  delay(4000);
  lcd.clear();
  lcd.print("Election Ended.!");
}

// === Fingerprint Functions ===
int getFingerprintID() {
  finger.getImage();
  if (finger.image2Tz() != FINGERPRINT_OK) return -1;
  if (finger.fingerSearch() != FINGERPRINT_OK) return -1;
  return finger.fingerID;
}

// === RFID Reader Function ===
String readRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return "";

  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(rfid.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();
  rfid.PICC_HaltA();
  return uidStr;
}

// === Match fingerprint ID to user index ===
int findUserByFingerprint(int fid) {
  for (int i = 0; i < numVoters; i++) {
    if (registeredFingerIDs[i] == fid)
      return i;
  }
  return -1;
}
