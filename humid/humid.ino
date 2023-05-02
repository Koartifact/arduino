// 전역 변수 설정
long levelTimeout = 500000; // 500ms
uint8_t bitmask = 0xFF; 
uint8_t port = 0xFF; 
int pin = 7; 

// 초기 설정
void setup()
{
    Serial.begin(115200); 
}

// 메인 루프
void loop()
{
    byte temperature = 0;
    byte humidity = 0;

    read(&temperature, &humidity); // 온도와 습도 읽기
    
    // 온도와 습도 출력
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("C, Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    delay(1000); // 데이터를 읽는 간격이 1Hz 즉 1초에 한번
}

// DHT11 센서에서 데이터 읽기
int read(byte *temperature, byte *humidity)
{
    bitmask = digitalPinToBitMask(pin); // 핀 번호를 비트 마스크로 변환
    port = digitalPinToPort(pin); // 핀 번호를 포트 번호로 변환
    byte data[40] = {0};

    sample(data);

        // 가로로 비트 값 출력
    Serial.print("Bit values: ");
    for (size_t i = 0; i < 40; i++)
    {
        Serial.print(data[i]);
        if (i % 8 == 7) {
            Serial.print(" ");
        }
    }
    Serial.println();

    byte humidity2, temperature2, checksum;
    byte expect;

    // 40개의 비트를 올바르게 변환합니다.
    *humidity = bits2byte(data);
    humidity2 = bits2byte(data + 8);
    *temperature = bits2byte(data + 16);
    temperature2 = bits2byte(data + 24);
    checksum = bits2byte(data + 32);

    // 5번째 바이트인 checksum과 앞의 4개 바이트 합을 비교합니다.
    expect = *humidity + humidity2 + *temperature + temperature2;

    if (checksum != expect) // 같아야 되지만 다르다면
    {
        // 잘못된 값이 전송되었다고 표시하고 읽은 값을 -1로 초기화합니다
        Serial.println("there are some error on Transmission");
        *humidity = -1;
        *temperature = -1;
    }

    return 0;
}


// DHT11 데이터 샘플링
int sample(byte data[40])
{
    // 데이터 핀을 출력으로 설정하고 LOW로 설정
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay(18 + 2);
    
    // 데이터 핀을 입력으로 설정하고 HIGH로 설정
    digitalWrite(pin, HIGH);
    pinMode(pin, INPUT);
    delayMicroseconds(25);

    // 센서의 LOW-HIGH 응답 시간 측정
    long t = levelTime(LOW, 10, 6);
    if (t < 40)
    {
        return -1;
    }
    t = levelTime(HIGH, 10, 6);
    if (t < 40)
    {
        return -1;
    }

    // 40 비트 데이터 샘플링
    for (size_t i = 0; i < 40; i++)
    {
        t = levelTime(LOW, 10, 6);
        if (t < 24)
        {
        }
        t = levelTime(HIGH, 10, 6);
        if (t < 11)
        {
        }
        data[ i ] = (t > 40 ? 1 : 0);
    }
}

// 센서 응답 시간 측정 함수
long levelTime(byte level, int firstWait, int interval)
{
    unsigned long time_start = micros();
    long time = 0;

    uint8_t portState = level ? bitmask : 0;

    bool loop = true;
    for (int i = 0; loop; i++)
    {
        if (time < 0 || time > levelTimeout)
        {
            return -1;
        }

        if (i == 0)
        {
            if (firstWait > 0)
            {
                delayMicroseconds(firstWait);
            }
        }
        else if (interval > 0)
        {
            delayMicroseconds(interval);
        }
        time = micros() - time_start;

        // low상태에서 high 상태로 변할때 빠져 나가는 것
        loop = ((*portInputRegister(port) & bitmask) == portState);
    }

    return time;
}

//이 함수는 8개의 비트를 하나의 바이트로 변환.
byte bits2byte(byte *bits)
{
    byte value = 0;

    for (int i = 0; i < 8; i++)
    {
        value <<= 1;
        value |= bits[i];
    }

    return value;
}

