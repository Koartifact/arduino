#include <SoftwareSerial.h>

// SoftwareSerial(RX, TX) 형식으로 블루투스 모듈과 교차하여 연결
SoftwareSerial BTSerial(4, 5); // RX: 4, TX: 5
String receivedMessage = "";

void setup() {
  Serial.begin(9600); // 컴퓨터와의 시리얼 통신 초기화
  BTSerial.begin(9600); // 블루투스 모듈 초기화
}

void loop() {
  // 블루투스 모듈 → 아두이노 → 시리얼 모니터
  while (BTSerial.available()) {
    char incomingChar = BTSerial.read();
    receivedMessage += incomingChar;
    
    if (incomingChar == '\n') {
      Serial.print("Received: ");
      Serial.print(receivedMessage);
      if (receivedMessage.startsWith("hello")) {
        // 시리얼 모니터 → 아두이노 → 블루투스 모듈
        BTSerial.print("hello!! I'm Jincheol Auduino~! \n");
        Serial.print("Sent: hello!!! \n");
      }
      receivedMessage = "";
    }
  }
}
