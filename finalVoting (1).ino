#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

SoftwareSerial mySerial1(2, 3);
SoftwareSerial mySerial2(4, 5);
Adafruit_Fingerprint finger1 = Adafruit_Fingerprint(&mySerial1);
Adafruit_Fingerprint finger2 = Adafruit_Fingerprint(&mySerial2);

uint8_t id;
bool matched = false;
int lastVoterID = -1;
bool voted[128] = {false};
int voteCount1 = 0;
int voteCount2 = 0;
const int voteBtn1 = 7;
const int voteBtn2 = 8;
const int buzzerPin = 10;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(voteBtn1, INPUT);
  pinMode(voteBtn2, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(1500);
  mySerial1.begin(57600);
  finger1.begin(57600);
  delay(100);
  bool sensor1Status = finger1.verifyPassword();
  mySerial2.begin(57600);
  finger2.begin(57600);
  delay(100);
  bool sensor2Status = finger2.verifyPassword();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sensor 1: ");
  lcd.print(sensor1Status ? "Working" : "Not Found");
  lcd.setCursor(0, 1);
  lcd.print("Sensor 2: ");
  lcd.print(sensor2Status ? "Working" : "Not Found");
  delay(2500);
  showMainMenu();
}

void loop() {
  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'e' || input == 'E') enrollMode();
    else if (input == 'm' || input == 'M') matchMode();
    else if (input == 'd' || input == 'D') deleteMode();
    else if (input == 'v' || input == 'V') viewVoteResults();
  }

  if (matched && lastVoterID != -1 && !voted[lastVoterID]) {
    if (digitalRead(voteBtn1) == HIGH) {
      voteCount1++;
      voted[lastVoterID] = true;
      showVoteSuccess("Nayi Soch Party.", voteCount1);
    } else if (digitalRead(voteBtn2) == HIGH) {
      voteCount2++;
      voted[lastVoterID] = true;
      showVoteSuccess("Sankalp Bharat Party", voteCount2);
    }
  }
}

void showVoteSuccess(String partyName, int count) {
  lcd.clear();
  lcd.print("Voted: ");
  lcd.print(partyName);
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
  delay(3000);
  matched = false;
  lastVoterID = -1;
  showMainMenu();
}

void viewVoteResults() {
  lcd.clear();
  lcd.print("Place Admin Finger");
  mySerial2.listen();
  delay(100);
  unsigned long start = millis();
  while (millis() - start < 10000) {
    if (finger2.getImage() == FINGERPRINT_OK) {
      if (finger2.image2Tz() == FINGERPRINT_OK && finger2.fingerSearch() == FINGERPRINT_OK) {
        if (finger2.fingerID == 50) {
          lcd.clear();
          lcd.print("P1:");
          lcd.print(voteCount1);
          lcd.print(" P2:");
          lcd.print(voteCount2);
          delay(4000);
          showMainMenu();
          return;
        }
      }
    }
    delay(100);
  }
  lcd.clear();
  lcd.print("Admin Failed");
  delay(2000);
  showMainMenu();
}

void enrollMode() {
  lcd.clear();
  lcd.print("Enroll: 1 or 2?");
  int sensorChoice = waitForSensorChoice();
  lcd.clear();
  if (sensorChoice == 2) {
    lcd.print("Enter ID (1-127)");
    id = waitForID();
    if (id == 50 && finger2.loadModel(50) == FINGERPRINT_OK) {
      lcd.clear();
      lcd.print("Admin Exists");
      delay(2000);
      showMainMenu();
      return;
    }
  } else {
    lcd.print("Enter ID (1-127)");
    id = waitForID();
  }

  if (sensorChoice == 1) {
    mySerial1.listen();
    delay(100);
    enrollFingerprint(finger1);
  } else {
    mySerial2.listen();
    delay(100);
    enrollFingerprint(finger2);
  }

  showMainMenu();
}

void deleteMode() {
  lcd.clear();
  lcd.print("Delete: 1 or 2?");
  int sensorChoice = waitForSensorChoice();
  lcd.clear();
  lcd.print("Enter ID (1-127)");
  id = waitForID();

  if (sensorChoice == 2 && id == 50) {
    lcd.clear();
    lcd.print("Admin Protected");
    delay(2000);
    showMainMenu();
    return;
  }

  uint8_t result;
  if (sensorChoice == 1) {
    mySerial1.listen();
    delay(100);
    result = finger1.deleteModel(id);
  } else {
    mySerial2.listen();
    delay(100);
    result = finger2.deleteModel(id);
  }

  lcd.clear();
  if (result == FINGERPRINT_OK) lcd.print("Deleted Success");
  else lcd.print("Delete Failed");

  delay(2000);
  showMainMenu();
}

int waitForSensorChoice() {
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '1') return 1;
      if (c == '2') return 2;
    }
  }
}

int waitForID() {
  while (Serial.available()) Serial.read();
  while (true) {
    if (Serial.available()) {
      int value = Serial.parseInt();
      if (value >= 1 && value <= 127) return value;
    }
  }
}

void enrollFingerprint(Adafruit_Fingerprint &sensor) {
  int p = -1;
  lcd.clear();
  lcd.print("Place Finger...");
  while ((p = sensor.getImage()) != FINGERPRINT_OK) delay(100);
  p = sensor.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Image Failed");
    delay(2000);
    return;
  }
  lcd.clear();
  lcd.print("Remove Finger");
  while (sensor.getImage() != FINGERPRINT_NOFINGER) delay(100);
  delay(1000);
  lcd.clear();
  lcd.print("Place Again...");
  while ((p = sensor.getImage()) != FINGERPRINT_OK) delay(100);
  p = sensor.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("2nd Img Failed");
    delay(2000);
    return;
  }
  p = sensor.createModel();
  if (p == FINGERPRINT_OK) {
    p = sensor.storeModel(id);
    lcd.clear();
    if (p == FINGERPRINT_OK) lcd.print("Enroll Success");
    else lcd.print("Store Failed");
  } else {
    lcd.clear();
    lcd.print("Match Failed");
  }
  delay(2000);
}

void matchMode() {
  lcd.clear();
  lcd.print("Match Both Now");
  unsigned long startTime = millis();
  const unsigned long timeout = 10000;
  bool s1Matched = false, s2Matched = false;
  int id1 = -1, id2 = -1;

  while (millis() - startTime < timeout) {
    if (!s1Matched) {
      mySerial1.listen();
      delay(50);
      if (finger1.getImage() == FINGERPRINT_OK) {
        if (finger1.image2Tz() == FINGERPRINT_OK && finger1.fingerSearch() == FINGERPRINT_OK) {
          id1 = finger1.fingerID;
          s1Matched = true;
        }
      }
    }

    if (!s2Matched) {
      mySerial2.listen();
      delay(50);
      if (finger2.getImage() == FINGERPRINT_OK) {
        if (finger2.image2Tz() == FINGERPRINT_OK && finger2.fingerSearch() == FINGERPRINT_OK) {
          id2 = finger2.fingerID;
          if (id2 != 50) s2Matched = true;
        }
      }
    }

    if (s1Matched && s2Matched) {
      matched = true;
      lastVoterID = id1;
      lcd.clear();
      lcd.print("Match Success");
      lcd.setCursor(0, 1);
      lcd.print("S1:");
      lcd.print(id1);
      lcd.print(" S2:");
      lcd.print(id2);
      delay(2000);
      lcd.clear();
      lcd.print("Cast Your Vote");
      return;
    }
  }

  lcd.clear();
  lcd.print("Invalid Officer");
  delay(2000);
  showMainMenu();
}

void showMainMenu() {
  lcd.clear();
  lcd.print("Enroll=E Match=M");
  lcd.setCursor(0, 1);
  lcd.print("Delete=D View=V");
}