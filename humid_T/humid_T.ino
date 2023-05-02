int DHT11 = 2;      // DHT11의 DATA핀이 연결된 아두이노 핀 번호
int data[40];       // DHT11로 40개의 bit (5 byte) 읽어오므로 40개 배열변수 준비
int humidity;       // 첫번째 byte인 습도데이터를 위한 변수
int humidity2;      // 두번째 byte인 습도데이터를 위한 변수
int temperature;    // 세번째 byte인 온도데이터를 위한 변수
int temperature2;   // 네번째 byte인 온도데이터를 위한 변수
int checksum;       // 다섯번째 byte인 checksum데이터를 위한 변수
int expect;         // checksum과 비교할 앞의 4byte의 합을 저장할 변수

void setup(){  
  // 모니터를 위해 시리얼 통신을 시작합니다.
  Serial.begin(9600);  
}

void loop(){
  // 뒤에 정의되어 있는 void readDHT11()함수를 호출합니다.
  // 온도와 습도를 읽어옵니다
  readDHT11(); 

  // 읽어온 습도와 온도 데이터를 시리얼 모니터에 출력합니다.
  Serial.print("humidity : ");  
  Serial.print(humidity);
  Serial.print("    temperature : ");
  Serial.println(temperature);

  // 데이터를 읽는 간격이 1Hz 즉 1초에 한번이므로 넉넉하게 1.5초 쉽니다
  delay(1500);
}

// 이 부분이 라이브러리 ccp화일의 내용을 가져온 것입니다.
void readDHT11(){

  // 온도 습도 값을 -1로 초기화합니다. 통신이 오류가 날 경우 
  // 이 값이 출력됩니다.
  humidity = -1;
  temperature = -1;
  
  // 데이터시트에 따르면 통신을 시작하려면 아두이노에서 먼저
  // LOW신호를 20ms, HIGH신호를 20~40us 보낸다고 되어 있습니다.
  
  // 먼저 DHT11의 DATA핀과 연결된 아두이노 핀을 출력으로 설정합니다
  pinMode (DHT11, OUTPUT);     
  digitalWrite (DHT11, LOW);    // DATA핀에 LOW를 보내고
  delay(20);                    // 20ms 유지합니다
  digitalWrite (DHT11, HIGH);   // DATA핀에 HIGH를 보냅니다

  // 이후에는 DHT11로 부터 데이터를 전송받으므로 핀설정을 입력으로 변경합니다
  pinMode (DHT11, INPUT);       
  delayMicroseconds (30);       // 30us 동안 유지합니다.

  // 아두이노에서 LOW 20ms, HIGH 20~40us 유지하면 DHT11에서 
  // LOW 80us, HIGH 80us의 신호를 보내어 데이터 전송의 시작을 알립니다
  // confirm이라는 함수는 뒤에 정의 되어 있습니다. 
  
  // 입력이 LOW로 80us가 되는지 확인하고 그렇지 않다면 
  // 데이터 전송 시작을 알리는 LOW 신호에서 에러가 났다고 표시합니다
  if (confirm (DHT11, 80, LOW)) {
    Serial.println("Error on Start LOW"); 
    return;
  }

  // 입력이 HIGH로 80us가 되는지 확인하고 그렇지 않다면
  // 데이터 전송 시작을 알리는 HIGH 신호에서 에러가 났다고 표시합니다
  if (confirm (DHT11, 80, HIGH)) {
    Serial.println("Error on Start HIGH");
    return;
  }

  // 데이터 전송을 알리는 LOW, HIGH신호가 보내졌으면 40개의 비트를 전송합니다
  // 데이터는 50us의 LOW신호 뒤에 HIGH신호가 26~28us이면 "0", 70us이면 "1"입니다

  // 40개의 비트를 전송받기 위해 40번 반복합니다.
  for (int i = 0; i < 40; i++){
    if (confirm(DHT11, 50, LOW)) {   // DATA bit앞에 50us의 LOW신호를 확인합니다
      Serial.println("Error on Data LOW");  // 확인되지 않으면 에러를 표시합니다
      return;
    }

    // 이 부분은 50us의 LOW신호 뒤에 HIGH신호의 길이를 확인하는 코드입니다
    bool ok = false;    // 불 변수를 선언하고 false로 지정합니다
    int tick = 0;       // tick이라는 정수형 변수를 지정합니다

    // 10us 주기로 8번 DATA라인이 HIGH인지 확인합니다. 
    for (int j = 0; j < 8; j++, tick++){
      if (digitalRead(DHT11) != HIGH){ // DATA핀이 LOW로 바뀌면
        ok = true;          // 불변수를 true로 변경합니다.
        break;              // 반복을 끝까지 하지 않고 멈춥니다.
      }
      delayMicroseconds(10); // 반복 주기가 10us입니다.
      // 10us마다 확인하므로 데이터 값이 0일경우 HIGH의 길이가 26~28us
      // 밖에 되지 않으므로 ok가 true로 바뀔 때 tick 값이 3보다 작을 것입니다.
      // 데이터 값이 1일 경우 HIGH의 길이가 70us이므로 ok가 true로 바뀔때
      // tick값이 3보다 클것입니다.
    }
    if (!ok) { // !ok가 true, 즉 ok가 false일 경우 
      // 이 경우는 70us가 지나도 계속 HIGH신호가 유지되는 경우입니다
      // 데이터 읽기에 오류가 있습니다라고 표시합니다    
      Serial.println("Error on Data Read"); 
      return;
    }
    // HIGH신호가 유지된 시간을 측정한 tick 값이 3보다 크면 1, 작으면 0을
    // data[i]에 저장합니다.
    data[i] = (tick > 3? 1:0); 
  }
  // DATA전송이 끝나면 마지막 50us LOW신호를 전송합니다
  // 이 신호가 확인되지 않으면 읽기 종료에 오류가 있다고 출력합니다.
  if (confirm(DHT11, 50, LOW)) {
    Serial.println("Error on End of Reading");
    return;
  }

  // 설정된 변수에 읽어온 데이터를 변환하여 저장합니다.
  // bit2byte()는 뒤에 정의되어 있습니다
  humidity = bits2byte(data);
  humidity2 = bits2byte(data + 8);
  temperature = bits2byte(data + 16);
  temperature2 = bits2byte(data + 24);
  checksum = bits2byte(data + 32);

  // 5번째 byte인 checksum과 앞의 4개 byte 합을 비교합니다.
  expect = humidity + humidity2 + temperature + temperature2;
  if (checksum != expect) {  // 같아야 되지만 다르다면
      // 잘못된 값이 전송되었다고 표시하고 읽은 값을 -1로 초기화합니다
      Serial.println("there are some error on Transmission");
      humidity = -1;
      temperature = -1;
  }
}

// 이부분은 전송되는 신호의 길이를 확인하는 함수입니다
// 확인하는 핀번호, 확인하는 시간, 확인하는 신호레벨(HIGH, LOW)를 입력받습니다
int confirm (int pin, int us, byte level){  
  int count = us / 10; // 10us단위로 확인하므로 10으로 나눈 값을 저장합니다
  if ((us % 10) > 0) count++;  // 혹시 입력받은 값이 10단위가 아니면 1을 더합니다
  // 예를 들어 35라는 시간을 확인한다면 35 / 10 = 3과, 35 % 10 = 5에서 count++을 하여
  // 3 + 1 즉, 10us씩 4번이 지나는지 확인하는 것입니다.

  bool ok = false;    // 앞에서 처럼 불변수를 지정합니다.
  for (int i = 0; i < count; i++){  // 확인할 시간으로 환산한 횟수만큼 반복합니다
    delayMicroseconds(10);  // 10us씩 count만큼 반복합니다

    if (digitalRead(pin) != level){ // 중간에 확인하는 신호레벨과 달라지면
      ok = true;  // 불변수를 바꾸어줍니다
      break;      // 반복을 멈춥니다
    }
  }
  if (!ok){   // 만약 !ok가 참이면 즉, ok가 false이면
    return -1;  // -1을 반환합니다.
  }
  return 0;   // ok가 true이면 0을 반환합니다.
}
/* 예를 들어 앞에 아래와 같은 코드가 있었습니다.
 *  즉, LOW신호를 80us 확인하고 오류가 있으면 시리얼모니터에 오류
 *  표시를 하게됩니다. 
 *  confirm이 0을 반환하면 정상통신이고 다른 값을 반환하면 오류가 발생
 *  한 것으로 판단하는 것입니다. 
   if (confirm (DHT11, 80, LOW)) {
    Serial.println("Error on Start LOW"); 
    return;
  }
  이 코드를 볼때 설정한 시간보다 짧은 신호는 정상통신으로
  설정한 시간보다 긴 신호는 통신오류로 판단하는 것으로 생각됩니다. 
 */
// data[40]의 배열에 저장된 40개 bit를 5개 byte로 변경합니다.
int bits2byte(int data[8]) {
    int v = 0;
    for (int i = 0; i < 8; i++) {
        v += data[i] << (7 - i);
    }
    return v;
}