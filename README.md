# ExamWatch

## A Microcontroller-Based Smart Exam Surveillance System

ExamWatch is an Arduino Uno–based intelligent exam hall monitoring system designed to assist invigilators in detecting suspicious activities during examinations.

The system combines adaptive sound analysis, motion pattern detection, sensor fusion, and intelligent suspicion scoring to reduce false alarms while improving monitoring efficiency.

---

# Features

- Adaptive whisper detection
- Background noise filtering
- Burst-based sound analysis
- PIR motion pattern detection
- Sensor fusion for higher accuracy
- Suspicion score system
- Confidence level classification
- LCD status monitoring
- Visual and audible alert system
- Silent mode support
- False alarm reduction techniques

---

# Hardware Components

- Arduino Uno
- KY-038 Sound Sensor
- HC-SR501 PIR Motion Sensor
- 16x2 I2C LCD Display
- Active Buzzer
- LED Indicator
- Push Button
- Breadboard & Jumper Wires

---

# System Working Principle

The system continuously monitors the environment using sound and motion sensors.

Instead of relying on simple threshold triggering, ExamWatch uses:
- adaptive baseline noise filtering
- variance-based whisper detection
- burst validation
- motion pattern analysis
- sensor fusion

to identify suspicious behavior more accurately.

When repeated suspicious activities are detected, the system:
- increases suspicion score
- classifies confidence level
- triggers visual/audio alerts

---

# Pin Configuration

| Component | Arduino Pin |
|---|---|
| PIR Sensor OUT | D2 |
| Sound Sensor AO | A0 |
| Buzzer | D8 |
| LED | D9 |
| Push Button | D4 |
| LCD SDA | A4 |
| LCD SCL | A5 |

---

# Technologies Used

- Embedded Systems
- Arduino Programming
- Sensor Fusion
- Adaptive Signal Processing
- Real-Time Monitoring
- Intelligent Alert Logic

---

# Future Improvements

- Wireless alert system
- Camera integration
- Cloud-based monitoring
- Data logging
- Multi-zone classroom coverage
- Machine learning–based behavior analysis

---

# Author

Intejar Alif

Department of Computer Science and Engineering

---

# Project Report

The complete detailed project report is included in this repository.

---

# License

This project is developed for academic and educational purposes.
