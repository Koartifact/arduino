#include <Wire.h> // I2C 통신 라이브러리를 포함합니다.

const uint8_t AHT20_ADDRESS = 0x38; // AHT20 센서의 I2C 주소를 지정합니다.
const uint8_t AHT20_INIT = 0xE1; // 센서 초기화를 위한 명령어입니다.
const uint8_t AHT20_START_MEASUREMENT = 0xAC; // 온도 및 습도 측정을 시작하는 명령어입니다.

void setup() {
  Serial.begin(115200); // 시리얼 통신 속도를 115200bps로 설정합니다.
  Serial.println("Humidity AHT20 example without library"); // 시리얼 모니터에 메시지를 출력합니다.

  Wire.begin(); // I2C 통신을 시작합니다.

  if (initializeSensor()) { // 센서 초기화를 시도합니다.
    Serial.println("AHT20 acknowledged."); // 초기화 성공 메시지를 출력합니다.
  } else {
    Serial.println("AHT20 not detected. Please check wiring. Freezing."); // 초기화 실패 메시지를 출력하고 무한 루프에 걸립니다.
    while (1);
  }
}

void loop() {
  float temperature, humidity; // 온도와 습도를 저장할 변수를 선언합니다.
  if (getTemperatureAndHumidity(temperature, humidity)) { // 온도와 습도를 읽어옵니다.
    Serial.print("Temperature: "); // 온도를 출력합니다.
    Serial.print(temperature, 2);
    Serial.print(" C\t");
    Serial.print("Humidity: "); // 습도를 출력합니다.
    Serial.print(humidity, 2);
    Serial.println("% RH");
  }
  delay(2000); // 2초 동안 대기합니다.
}

bool initializeSensor() {
  Wire.beginTransmission(AHT20_ADDRESS); // AHT20 센서와 통신을 시작합니다.
  Wire.write(AHT20_INIT); // 초기화 명령어를 전송합니다.
  Wire.write(0x08); // 설정 바이트를 전송합니다.
  Wire.write(0x00); // 두 번째 설정 바이트를 전송합니다.
  return (Wire.endTransmission() == 0); // 통신이 성공적으로 종료되었는지 확인합니다.
}

bool getTemperatureAndHumidity(float &temperature, float &humidity) {
  Wire.beginTransmission(AHT20_ADDRESS); // AHT20 센서와 통신을 시작합니다.
  Wire.write(AHT20_START_MEASUREMENT); // 측정 시작 명령어를 전송합니다.
  Wire.write(0x33); // 설정 바이트를 전송합니다.
  Wire.write(0x00); // 두 번째 설정 바이트를 전송합니다.
  if (Wire.endTransmission() != 0) { // 통신이 성공적으로 종료되었는지 확인합니다.
    return false;
  }

  delay(80); // 센서가 측정 값을 준비하는 데 시간이 걸리므로 대기합니다.

  if (Wire.requestFrom(AHT20_ADDRESS, (uint8_t)6) != 6) {
    return false;
  } // 센서로부터 6바이트의 데이터를 요청하고, 정상적으로 요청되었는지 확인합니다.

  Wire.read(); // 첫 번째 바이트는 읽고 무시합니다. (상태 바이트)
  uint32_t rawHumidity = Wire.read();  // 습도 데이터의 첫 번째 바이트를 읽습니다.
  rawHumidity <<= 8; // 8비트 왼쪽으로 이동시킵니다.
  rawHumidity |= Wire.read(); // 습도 데이터의 두 번째 바이트를 읽어 추가합니다.
  uint8_t midByte = Wire.read(); // 온도와 습도 데이터 사이의 바이트를 읽습니다.
  rawHumidity <<= 4; // 4비트 왼쪽으로 이동시킵니다.
  rawHumidity |= (midByte >> 4); // 중간 바이트의 상위 4비트를 습도 데이터에 추가합니다.

  uint32_t rawTemperature = midByte & 0x0F; // 중간 바이트의 하위 4비트를 온도 데이터의 시작으로 사용합니다.
  rawTemperature <<= 8; // 8비트 왼쪽으로 이동시킵니다.
  rawTemperature |= Wire.read(); // 온도 데이터의 두 번째 바이트를 읽어 추가합니다.
  rawTemperature <<= 8; // 8비트 왼쪽으로 이동시킵니다.
  rawTemperature |= Wire.read(); // 온도 데이터의 세 번째 바이트를 읽어 추가합니다.

  humidity = (rawHumidity * 100.0) / 1048576.0; // 습도 데이터를 백분율로 변환합니다.
  temperature = ((rawTemperature * 200.0) / 1048576.0) - 50; // 온도 데이터를 섭씨로 변환합니다.

  return true; // 온도와 습도 값을 성공적으로 읽었습니다.
}