<h3 align="center">Arduino Attendance with RFID and Biometrics System</h3>

<div align="center">

[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![GitHub Issues](https://img.shields.io/github/issues/dnachavez/arduino_attendance_with_rfid_biometrics_system.svg)](https://github.com/dnachavez/arduino_attendance_with_rfid_biometrics_system/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/dnachavez/arduino_attendance_with_rfid_biometrics_system.svg)](https://github.com/dnachavez/arduino_attendance_with_rfid_biometrics_system/pulls)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](/LICENSE)

</div>

<p align="center">This system is based on Arduino Uno R3 and is designed to track attendance using RFID (Radio Frequency Identification) and biometric data.</p>

## 📝 Table of Contents

- [About](#about)
- [Getting Started](#getting_started)
- [Deployment](#deployment)
- [Usage](#usage)
- [Built Using](#built_using)
- [Authors](#authors)

## 🧐 About <a name = "about"></a>

The system utilizes RFID tags or cards to identify individuals and biometric data (such as fingerprints) for additional verification. It includes a central Arduino Uno R3 board that acts as the main controller, along with RFID readers, biometric sensors, and a display for user interaction.

The system allows users to register their RFID tags or cards and biometric data, which are stored in a database (SD Card). During attendance tracking, users can simply present their RFID tags or cards and verify their identity using biometric data. The system records the attendance data and provides real-time feedback on the display.

## 🏁 Getting Started <a name = "getting_started"></a>

These instructions will guide you on how to use the Arduino Uno R3-based Attendance with RFID and Biometrics System.

Follow these steps to get the project up and running on your local machine for development and testing purposes.

### Deployment:
1. Connect the Arduino Uno R3 board to your computer using a USB cable.
2. Install the Arduino IDE (Integrated Development Environment) on your computer.
3. Open the Arduino IDE and create a new sketch.
4. Copy the code from the project and paste it into the Arduino IDE.
5. Connect the RFID module and biometric sensor to the appropriate pins on the Arduino board (based the connections using the `circuit_diagram.png`).
6. Upload the sketch to the Arduino board by clicking the "Upload" button in the Arduino IDE.
7. Once the upload is complete, open the serial monitor in the Arduino IDE to view the system's output.

Note: Make sure to follow any additional instructions provided in the project's README.md file for proper setup and usage.

### Prerequisites

To set up the Arduino Uno R3-based Attendance with RFID and Biometrics System, you will need the following components:

- Arduino Uno R3
- Breadboard
- MFRC522 RFID module
- SD Card Module
- RTC (Real-Time Clock) module
- DY50 Fingerprint Scanner
- 20x4 LCD with I2C interface

Make sure you have all these components before proceeding with the setup.

## 🔧 Running the tests <a name = "tests"></a>

To run the automated tests for this system, follow these steps:

1. Make sure you have all the necessary components connected to the Arduino Uno R3 board as mentioned in the [Deployment](#deployment) section.

2. Open the Arduino IDE and navigate to the sketch that contains the code for the system.

3. Upload the sketch to the Arduino board by clicking the "Upload" button in the Arduino IDE.

4. Once the upload is complete, open the serial monitor in the Arduino IDE to view the results.

## 🎈 Usage <a name="usage"></a>

To use the Arduino Attendance with RFID and Biometrics System, follow these steps:

1. Tap your registered RFID card on the RFID reader.
2. If the card is registered, you will be prompted to scan your fingerprint.
3. Place your finger on the fingerprint scanner to scan your fingerprint.
4. If the fingerprint ID matches with the RFID card, a welcome message will be shown on the display.
5. The system will save a log of the attendance data.
6. If a new card is tapped, it needs to be tapped again within 5 seconds to register it.
7. After tapping the new card, you will be prompted to provide details for the card.
8. Once the card details are entered, you will be prompted to scan your fingerprint.
9. If the fingerprint scan is successful, the data will be saved in the SD card.

Note: Make sure to follow the instructions provided by the system for proper usage and registration of RFID cards.

## ⛏️ Built Using <a name = "built_using"></a>

- [Arduino](https://www.arduino.cc/) - Microcontroller
- MFRC522 RFID module - RFID Reader
- DY50 Fingerprint Scanner - Biometric Sensor
- 20x4 LCD with I2C interface - Display
- SD Card Module - SD Card Reader
- RTC (Real-Time Clock) module - Real-Time Clock

## ✍️ Authors <a name = "authors"></a>

- [@dnachavez](https://github.com/dnachavez) - Idea & code
- [@0Exeqt0](https://github.com/0Exeqt0) - Initial work & circuit diagram

See also the list of [contributors](https://github.com/dnachavez/arduino_attendance_with_rfid_biometrics_system/contributors) who participated in this project.
