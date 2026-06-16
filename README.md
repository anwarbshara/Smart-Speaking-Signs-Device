# Smart Speaking Signs Device

An intelligent wearable glove designed to translate sign language into readable text and audible speech in real time, helping deaf and mute individuals communicate more effectively, especially in healthcare environments.

## Project Overview

The Smart Speaking Signs Device is a graduation project developed to bridge communication barriers between deaf/mute individuals and society. The system uses a smart glove equipped with sensors to recognize hand gestures and convert them into speech and text instantly.

The glove captures finger movements and hand orientation using Flex Sensors and MPU6050, processes the gesture using ESP32, and sends the data wirelessly through Bluetooth Low Energy (BLE) to a Flutter mobile application, where the recognized sign is displayed as text and converted into speech using Text-to-Speech technology.

This project was developed with a focus on:
- Real-time communication
- Ease of use
- Low-cost implementation
- Accessibility support
- Healthcare environments


## Features

- Real-time sign language recognition  
- Instant speech output using Text-to-Speech (TTS)  
- Real-time text display through a mobile application  
- Wireless communication using Bluetooth Low Energy (BLE)  
- Lightweight and wearable smart glove design  
- User-friendly Flutter mobile application  
- Support for healthcare communication scenarios  
- Low-cost assistive technology solution  

---

## Technologies Used

### Hardware
- ESP32
- Flex Sensors (5)
- MPU6050 Motion Sensor
- Bluetooth Low Energy (BLE)

### Software
- Flutter
- Firebase
- Arduino IDE (ESP32 Programming)
- C++ (ESP32 Programming)
- Text-to-Speech (TTS)

---

## Hardware & Software Components

| Component | Purpose |
|------------|----------|
| ESP32 | Main microcontroller for processing data |
| Flex Sensors | Detect finger bending movements |
| MPU6050 | Detect hand motion and orientation |
| BLE | Wireless communication |
| Flutter App | Display translated text and speech |
| Firebase | Store and manage data |
| TTS | Convert text into voice | 

---

 ## How the System Works

The system works through the following process:

1. The user performs a hand gesture using the smart glove.
2. Flex Sensors detect finger bending movements.
3. MPU6050 detects hand motion and orientation.
4. ESP32 processes the sensor readings and identifies the gesture.
5. The recognized sign is transmitted wirelessly through BLE.
6. The Flutter mobile application receives the data.
7. The gesture is displayed as readable text.
8. Text-to-Speech converts the text into audible speech.

---

## System Workflow

Sign Language Gesture  
        ↓ 
Flex Sensors + MPU6050  
        ↓  
ESP32 Processing  
        ↓  
Bluetooth Low Energy (BLE)  
        ↓  
Flutter Mobile Application  
        ↓  
Text Display + Speech Output


### Flowchart Pictures:
     - Mobile App Flowchart:
     
![System Flowchart](System_Flowchart/App_Flowchart.png)


      - Microcontroller(ESP32) Flowchart:

![System Flowchart](System_Flowchart/Microcontroller_ESP32_Flowchart.png)

---

## System Architecture

The system consists of three main layers:

### 1. Hardware Layer
Responsible for gesture sensing and motion detection using:
- Flex Sensors
- MPU6050
- ESP32

### 2. Communication Layer
Responsible for real-time wireless data transmission using:
- Bluetooth Low Energy (BLE)

### 3. Application Layer
Responsible for:
- Displaying translated text
- Generating speech output
- Managing communication through Flutter and Firebase

---

## Project Results

- Successfully implemented more than **40 hand gestures**
- Achieved approximately **87% recognition accuracy**
- Real-time speech and text conversion
- Successfully tested for healthcare communication scenarios

---

## Project Impact

The Smart Speaking Signs Device aimed to improve accessibility and communication for deaf and mute individuals, especially in healthcare environments.

The project gained public recognition and media attention due to its social impact and innovative approach to assistive technology.

It was presented in interviews, media platforms, and public showcases, highlighting the importance of inclusive technological solutions.

---

## Media Recognition

The project received significant public attention and recognition due to its social and technological impact.

It was featured across:

- Social media platforms
- TV channels
- Radio stations
- News interviews
- University events and exhibitions

The project attracted attention for supporting communication for deaf and mute individuals and improving accessibility through assistive technology.

---

## Project Screenshots

### Smart Glove :
![Smart Glove](Images/Glove.png)      ![Smart Glove](Images/Project_Image.jpeg)
![Smart Glove](Images/Project_Image_2.png)


### Mobile Application :
![Mobile App](Images/Mobile_App_1.png)
![Mobile App](Images/Mobile_App_2.png)


### Hardware Components :
![Hardware](Images/Hardware_Components.png)


### Poster :
![Poster](Poster/POSTER.png)


Click Here To Download Project Poster:
[Click Here To Download Project Poster](Poster/POSTER_SSSD.pdf)


---

## Demo Video

🎥 Watch the Smart Speaking Signs Device demonstration here:
[Click Here to Watch the Demo](https://drive.google.com/file/d/1fMt22c8ajauHyEJ4nHGmc-IGWgudZfpw/view?usp=drive_link)

---

## Challenges Faced

During development, several challenges were encountered:

- Sensor calibration accuracy
- Gesture recognition consistency
- Real-time BLE communication
- Hardware integration and optimization

These challenges were addressed through testing, calibration, and iterative improvements.

---

## Future Improvements

Potential future enhancements include:

- Adding Artificial Intelligence (AI) for smarter gesture recognition
- Supporting more sign language gestures
- Multi-language support
- Cloud synchronization
- Higher recognition accuracy
- Integration with healthcare systems

---

## Repository Structure

```text
Smart-Speaking-Signs-Device
│── Embedded_System
│── Flutter_App
│── Documentation
│── Images
│── Poster
│── System_Flowchart
│── Media_Coverage
│── README.md
```
---
## Author

**Anwar Bshara**  
Computer Systems Engineer  

📧 Email: anwarbshara2002@gmail.com  

💼 LinkedIn: [LinkedIn Profile](https://www.linkedin.com/in/anwaar-bshara)

💻 GitHub: [GitHub Profile](https://github.com/anwarbshara)


