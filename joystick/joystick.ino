#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int joystickButton = 7;
bool isJumping = false;
unsigned long obstacleTimer = 0;
int obstaclePosition = 15;

void createCustomChars() {
  byte frame1[8] = {
    0b00100,
    0b01110,
    0b00100,
    0b01110,
    0b10101,
    0b00100,
    0b01010,
    0b10001
  };

  byte frame2[8] = {
    0b00100,
    0b01110,
    0b00100,
    0b01110,
    0b10101,
    0b00100,
    0b10001,
    0b01010
  };

  lcd.createChar(1, frame1);
  lcd.createChar(2, frame2);
}

void setup() {
  lcd.init();
  lcd.backlight();
  createCustomChars();
  pinMode(joystickButton, INPUT_PULLUP);
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - obstacleTimer > 1000) {
    obstacleTimer = currentTime;
    obstaclePosition--;
    if (obstaclePosition < 0) {
      obstaclePosition = 15;
    }
  }

  lcd.clear();
  if (digitalRead(joystickButton) == LOW) {
    isJumping = true;
  } else {
    isJumping = false;
  }

  if (isJumping) {
    lcd.setCursor(1, 0);
  } else {
    lcd.setCursor(1, 1);
  }
  lcd.write(byte(1));

  lcd.setCursor(obstaclePosition, 1);
  lcd.print("*");

  delay(150);

  lcd.clear();
  if (isJumping) {
    lcd.setCursor(1, 0);
  } else {
    lcd.setCursor(1, 1);
  }
  lcd.write(byte(2));

  lcd.setCursor(obstaclePosition, 1);
  lcd.print("*");

  if (obstaclePosition == 2 && !isJumping) {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Game Over");
    delay(2000);
    obstaclePosition = 15;
  }

  delay(150);
}
