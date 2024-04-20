#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#define SS_PIN 7
#define RST_PIN 9
#define SD_CS_PIN 6
#define FINGERPRINT_RX 2
#define FINGERPRINT_TX 3

MFRC522 mfrc522(SS_PIN, RST_PIN);
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
SoftwareSerial mySerial(FINGERPRINT_RX, FINGERPRINT_TX);
Adafruit_Fingerprint finger(&mySerial);

void setup() {
  Serial.begin(9600);
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  mySerial.begin(57600);
  SPI.begin();
  mfrc522.PCD_Init();
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("SD Card initialization failed!"));
    return;
  }
  Wire.begin();
  
  // Setup for the RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  } else {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if (rtc.lostPower()) {
    Serial.println("RTC is not running!");
  }

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Scan your card"));
  DateTime currentTime = getCurrentDateTime();
  displayTime(currentTime);

  finger.begin(57600);

  initCSV("DATA.csv", "UID,FingerprintID,Timestamp");
  initCSV("HISTORY.csv", "UID,Date");

  Serial.println(F("System Initialized"));
}

void initCSV(const char* filename, const char* header) {
    digitalWrite(SD_CS_PIN, LOW);
    if (!SD.exists(filename)) {
        File file = SD.open(filename, FILE_WRITE);
        if (file) {
            file.println(header);
            file.close();
        }
    }
    digitalWrite(SD_CS_PIN, HIGH);
}

void loop() {
  DateTime currentTime = getCurrentDateTime();
  displayTime(currentTime);
  if (readRFID()) {
    char uid[10];
    getUID(uid);
    if (checkCard(uid)) {
      processRegisteredUser(uid);
    } else {
      processNewUser(uid);
    }
  }
}

void displayTime(DateTime logTime) {
  lcd.setCursor(0, 2);
  lcd.print(F("Date: "));
  lcd.print(logTime.day(), DEC);
  lcd.print('/');
  lcd.print(logTime.month(), DEC);
  lcd.print('/');
  lcd.print(logTime.year(), DEC);
  lcd.setCursor(0, 3);
  lcd.print(F("Time: "));
  lcd.print(logTime.hour(), DEC);
  lcd.print(':');
  lcd.print(logTime.minute(), DEC);
  lcd.print(':');
  lcd.print(logTime.second(), DEC);
}

bool readRFID() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println(F("RFID card detected"));
    return true;
  }
  return false;
}

void getUID(char* buf) {
  byte* id = mfrc522.uid.uidByte;
  sprintf(buf, "%02X%02X%02X%02X", id[0], id[1], id[2], id[3]);
  mfrc522.PICC_HaltA();
  Serial.print(F("RFID UID: "));
  Serial.println(buf);
}

bool checkCard(const char* uid) {
    digitalWrite(SD_CS_PIN, LOW);
    File dataFile = SD.open("DATA.csv", FILE_READ);
    if (!dataFile) {
        Serial.println(F("Opening DATA.csv failed"));
        digitalWrite(SD_CS_PIN, HIGH);
        return false;
    } else {
        Serial.println(F("DATA.csv opened successfully"));
    }
    char dataLine[100];
    while (dataFile.available()) {
        dataFile.readStringUntil('\n').toCharArray(dataLine, sizeof(dataLine));
        if (strncmp(dataLine, uid, strlen(uid)) == 0) {
            dataFile.close();
            digitalWrite(SD_CS_PIN, HIGH);
            return true;
        }
    }
    dataFile.close();
    digitalWrite(SD_CS_PIN, HIGH);
    return false;
}

void processRegisteredUser(char* uid) {
    int fingerprintID = getFingerprintIDez();  // Obtain the fingerprint ID

    if (fingerprintID > 0 && checkFingerprintAndCard(uid, fingerprintID)) {
        DateTime logTime = getCurrentDateTime();
        logAttendance(uid, logTime);
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Welcome, "));
        lcd.print(uid);
        displayTime(logTime);
    } else {
        lcd.clear();
        lcd.print(fingerprintID == -1 ? F("Fingerprint mismatch") : F("Fingerprint not linked to card"));
    }

    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Scan your card"));
}

bool checkFingerprintAndCard(const char* uid, int fingerprintID) {
    // Open the data file
    digitalWrite(SD_CS_PIN, LOW);
    File dataFile = SD.open("DATA.csv", FILE_READ);
    if (!dataFile) {
        Serial.println(F("Failed to open DATA.csv"));
        digitalWrite(SD_CS_PIN, HIGH);
        return false;
    }

    // Check each record
    char line[100];
    while (dataFile.available()) {
        dataFile.readStringUntil('\n').toCharArray(line, sizeof(line));
        int storedFingerprintID;
        char storedUID[10];
        // Parse UID and FingerprintID from the line
        sscanf(line, "%[^,],%d", storedUID, &storedFingerprintID);
        if (strcmp(storedUID, uid) == 0 && storedFingerprintID == fingerprintID) {
            dataFile.close();
            digitalWrite(SD_CS_PIN, HIGH);
            return true; // Fingerprint ID and UID match
        }
    }

    dataFile.close();
    digitalWrite(SD_CS_PIN, HIGH);
    return false; // No matching record found
}

DateTime getCurrentDateTime() {
  return rtc.now();
}

void processNewUser(char* uid) {
  lcd.clear();
  lcd.print(F("New card, tap again"));
  delay(2000);
  if (scanFingerprint()) {
    lcd.clear();
    lcd.print(F("Registration Complete"));
    saveNewUser(uid);
    delay(2000);
  } else {
    lcd.clear();
    lcd.print(F("Registration Failed"));
    delay(2000);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Scan your card"));
}

bool scanFingerprint() {
  int id = getNewFingerprintID();  // Ensure 'id' is declared and assigned correctly
  if (id == -1) {
    lcd.clear();
    lcd.print(F("File error!"));
    return false;
  }
  
  Serial.print(F("Enrolling ID #"));
  Serial.println(id);
  lcd.clear();
  lcd.print(F("Place your finger"));

  int p = finger.getImage();
  while (p != FINGERPRINT_OK) {
    if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");
    } else {
      Serial.println(F("Error getting image"));
      lcd.clear();
      lcd.print(F("Error!"));
      return false;
    }
    p = finger.getImage();
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println(F("Error converting image"));
    lcd.clear();
    lcd.print(F("Error!"));
    return false;
  }

  lcd.clear();
  lcd.print(F("Remove finger"));
  delay(1000);
  while (finger.getImage() != FINGERPRINT_NOFINGER) {
    // Wait for finger removal
  }

  lcd.clear();
  lcd.print(F("Place same finger again"));
  delay(1000);
  p = finger.getImage();
  while (p != FINGERPRINT_OK) {
    if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");
    } else {
      Serial.println(F("Error getting image"));
      lcd.clear();
      lcd.print(F("Error!"));
      return false;
    }
    p = finger.getImage();
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println(F("Error converting image"));
    lcd.clear();
    lcd.print(F("Error!"));
    return false;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println(F("Error creating model"));
    lcd.clear();
    lcd.print(F("Error!"));
    return false;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Fingerprint stored!"));
    lcd.clear();
    lcd.print(F("Fingerprint stored!"));
    return true;
  } else {
    Serial.print(F("Error storing fingerprint model: "));
    lcd.clear();
    lcd.print(F("Error!"));
    return false;
  }
}

int getNewFingerprintID() {
    // Enable SD card chip select pin
    digitalWrite(SD_CS_PIN, LOW);

    File dataFile = SD.open("DATA.csv", FILE_READ);
    if (!dataFile) {
        Serial.println(F("Failed to open DATA.csv for reading"));
        digitalWrite(SD_CS_PIN, HIGH); // Disable SD card after failure
        return; // Return -1 as an error indicator
    } else {
      Serial.println(F("DATA.csv opened successfully"));
    }

    int id = 0; // Start ID from 0, increment for each entry
    while (dataFile.available()) {
        String line = dataFile.readStringUntil('\n');
        if (line.length() > 0) { // Ensure it's not a blank line
            id++; // Increment ID for each valid line found
        }
    }
    dataFile.close();
    digitalWrite(SD_CS_PIN, HIGH);

    return id + 1; // Return the next ID (last ID + 1)
}

void saveNewUser(const char* uid) {
    digitalWrite(SD_CS_PIN, LOW);
    File dataFile = SD.open("DATA.csv", FILE_WRITE);
    if (!dataFile) {
        Serial.println(F("Failed to open DATA.csv for writing"));
        digitalWrite(SD_CS_PIN, HIGH);
        return;
    } else {
        Serial.println(F("DATA.csv opened successfully"));
    }
    int fingerprintID = getNewFingerprintID();
    DateTime now = rtc.now();
    char dataEntry[100];
    snprintf(dataEntry, sizeof(dataEntry), "%s,%d,%ld", uid, fingerprintID, now.unixtime());
    dataFile.println(dataEntry);
    dataFile.close();
    digitalWrite(SD_CS_PIN, HIGH);
    Serial.print(F("New user saved: "));
    Serial.println(dataEntry);
}

void logAttendance(const char* uid, DateTime logTime) {
    digitalWrite(SD_CS_PIN, LOW);
    File logFile = SD.open("HISTORY.csv", FILE_WRITE);
    if (!logFile) {
        Serial.println(F("Failed to open HISTORY.csv for writing"));
        digitalWrite(SD_CS_PIN, HIGH);
        return;
    } else {
        Serial.println(F("HISTORY.csv opened successfully"));
    }
    DateTime now = rtc.now();
    char logEntry[100];
    snprintf(logEntry, sizeof(logEntry), "%s,%ld", uid, logTime.unixtime());
    logFile.println(logEntry);
    logFile.close();
    digitalWrite(SD_CS_PIN, HIGH);
    Serial.print(F("Attendance logged for UID: "));
    Serial.println(uid);
}

int getFingerprintIDez() {
    lcd.clear();
    lcd.print(F("Place your finger"));
    delay(500); // Give time for the user to place the finger

    uint8_t p = finger.getImage();
    while (p != FINGERPRINT_OK) {
        if (p == FINGERPRINT_NOFINGER) {
            delay(50);  // Wait a bit and try again
            p = finger.getImage();
        } else {
            Serial.println(F("Error getting fingerprint image"));
            return -1;  // Indicate an error
        }
    }

    // Convert the image into a template
    p = finger.image2Tz(1);
    if (p != FINGERPRINT_OK) {
        Serial.println(F("Error converting image"));
        return -1;  // Indicate an error
    }

    lcd.clear();
    lcd.print(F("Remove finger"));
    delay(1000);

    // Ensure no finger is on the sensor
    do {
        p = finger.getImage();
        if (p != FINGERPRINT_NOFINGER) {
            lcd.clear();
            lcd.print(F("Remove finger"));
            delay(100); // Ensure the user has removed the finger
        }
    } while (p != FINGERPRINT_NOFINGER);

    lcd.clear();
    lcd.print(F("Place same finger again"));

    // Try to get the image of the same finger again
    p = finger.getImage();
    while (p != FINGERPRINT_OK) {
        if (p == FINGERPRINT_NOFINGER) {
            delay(50);  // Wait a bit and try again
            p = finger.getImage();
        } else {
            Serial.println(F("Error getting second image"));
            return -1;  // Indicate an error
        }
    }

    // Convert the second image into a template
    p = finger.image2Tz(2);
    if (p != FINGERPRINT_OK) {
        Serial.println(F("Error converting second image"));
        return -1;  // Indicate an error
    }

    // Attempt to find the fingerprint in the system
    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK) {
        Serial.println(F("Fingerprint not found"));
        return -1;  // Indicate an error
    }

    // Fingerprint match found
    Serial.print("Found ID #"); Serial.print(finger.fingerID);
    Serial.print(" with confidence of "); Serial.println(finger.confidence);
    return finger.fingerID;  // Return the found ID
}
