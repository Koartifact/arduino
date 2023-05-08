#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int joystick_x = 0;
int joystick_y = 1;
int attack_button = 7;

int playerX = 8;
int playerY = 1;

int obstacleX = 15;
int obstacleY = 0;

unsigned long prevMillis = 0;
unsigned long prevMillisAI = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(attack_button, INPUT_PULLUP);
  randomSeed(analogRead(0));
}

void loop() {
  int x = analogRead(joystick_x);
  int y = analogRead(joystick_y);
  int attack = digitalRead(attack_button);

  if (x < 400) playerX--;
  if (x > 600) playerX++;
  if (y < 400) playerY--;
  if (y > 600) playerY++;

  playerX = constrain(playerX, 0, 15);
  playerY = constrain(playerY, 0, 1);

  lcd.clear();
  lcd.setCursor(playerX, playerY);
  lcd.print("*");

  lcd.setCursor(obstacleX, obstacleY);
  lcd.print("#");

  if (playerX == obstacleX && playerY == obstacleY) {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Game Over");
    delay(3000);
    playerX = 8;
    playerY = 1;
    obstacleX = 15;
    obstacleY = random(0, 2);
  }

  if (attack == LOW && abs(playerX - obstacleX) <= 1 && playerY == obstacleY) {
    obstacleX = 15;
    obstacleY = random(0, 2);
  }

  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis >= 500) {
    prevMillis = currentMillis;
    obstacleX--;
    if (obstacleX < 0) {
      obstacleX = 15;
      obstacleY = random(0, 2);
    }
  }

  // AI for obstacle to follow the player on Y axis
  if (currentMillis - prevMillisAI >= 1000) {
    prevMillisAI = currentMillis;
    if (obstacleY < playerY) {
      obstacleY++;
    } else if (obstacleY > playerY) {
      obstacleY--;
    }
  }

  delay(100);
}
