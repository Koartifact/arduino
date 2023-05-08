#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int dataSet[] = {4307, 8195, 7732, 1102, 3176, 3668, 9020, 3105, 6779, 3545, 3388, 8477, 3736, 2715, 9803, 788, 8918, 5022, 870, 4289, 8712, 5149, 2022, 260, 370, 3433, 5991, 7120, 4586, 2173, 9579, 4489, 9046, 8448, 2462, 2586, 309, 8181, 9636, 2613, 2443, 9589, 3349, 7803, 4448, 3120, 6376, 5227, 4841, 6038, 3847, 4750, 8369, 4502, 9216, 8295, 2771, 6595, 3702, 3165, 10000, 3965, 3646, 7069, 2889, 8724, 9873, 6654, 2994, 3054, 5384, 2732, 1742, 1239, 1779, 5695, 6664, 3214, 8994, 7404, 8283, 6599, 8339, 9279, 3588, 1452, 7012, 6807, 4712, 1168, 3415, 2716, 4235, 4119, 4921, 3617, 2705, 2017, 1977, 2161, 9378, 5368, 4915, 5039, 5852, 1838, 1891, 6676, 5309, 6879, 8802, 5226, 6692, 3957, 6861, 3280, 3667, 1513, 8363, 3613, 2763, 9416, 6031, 5547, 7292, 2742, 2307, 7223, 1810, 2219, 8189, 3267, 8392, 9096, 1548, 4311, 3241, 9126, 1425, 6924, 7594, 8696, 4434, 9024, 8904, 7756, 2315, 2187, 8512, 6046, 7315, 5049, 2485, 3456, 8915, 8478, 8753, 8736, 9905, 4911, 4255, 7005, 4139, 5155, 5366, 3195, 9398, 8933, 2276, 5435, 9811, 6899, 1544, 5014, 6927, 9614, 4426, 6345, 3754, 2581, 9092, 8865, 6697, 2389, 8270, 1562, 7342, 6037, 1196, 5051, 9877, 7835, 2236, 6823, 9293, 5787, 8983, 3473, 5238, 6772, 7902, 1195, 5283, 1415, 3926, 4785, 3643, 9155, 1216, 9263, 2578, 4655, 2341, 7895, 9984, 7204, 7564, 4384, 3827, 738, 8185, 2398, 3565, 1256, 5781, 7690, 6609, 7169, 3352, 7937, 4261, 4051, 9198, 2333, 2457, 9026}; // 여기에 1000개의 데이터를 입력해주세요.
int dataSize = sizeof(dataSet) / sizeof(dataSet[0]);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  printData(F("Before Sort: "), 10);
  sortData();
  printData(F("After Sort: "), 10);
}

void loop() {
}

void printData(const __FlashStringHelper* label, int count) {
  Serial.println(label);
  for (int i = 0; i < count; i++) {
    Serial.print(dataSet[i]);
    Serial.print(" ");
  }
  Serial.println();
  for (int i = dataSize - count; i < dataSize; i++) {
    Serial.print(dataSet[i]);
    Serial.print(" ");
  }
  Serial.println();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label);
  lcd.setCursor(0, 1);
  lcd.print(F("LCD:"));

  // 첫 10개 출력
  for (int i = 0; i < count; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Top "));
    lcd.print(i + 1);
    lcd.setCursor(0, 1);
    lcd.print(dataSet[i]);
    delay(2000);
  }

  // 마지막 10개 출력
  for (int i = dataSize - count; i < dataSize; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Bot "));
    lcd.print(i + 1 - dataSize + count);
    lcd.setCursor(0, 1);
    lcd.print(dataSet[i]);
    delay(2000);
  }
}

void sortData() {
  for (int i = 0; i < dataSize - 1; i++) {
    for (int j = 0; j < dataSize - i - 1; j++) {
      if (dataSet[j] > dataSet[j + 1]) {
        int temp = dataSet[j];
        dataSet[j] = dataSet[j + 1];
        dataSet[j + 1] = temp;
      }
    }
  }
}
