const int buttonPin = 7;
const int ledPin = 12;
const int relayPin = 8; // Add this line to set the relay pin

int buttonPushCounter = 0;
int buttonState = 0;
int lastButtonState = 0;

bool ledStatus = false;
long ledStartMillis = 0;

const unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;

void setup() {
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    pinMode(relayPin, OUTPUT); // Add this line to set the relay pin as output
    Serial.begin(115200);
}

void loop() {
    handleButtonPress();
    SerialEvent();
    controlLED();
}

void handleButtonPress() {
    int reading = digitalRead(buttonPin);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;

            if (buttonState == HIGH) {
                buttonPushCounter++;
                Serial.println("on");
                Serial.print("number of button pushes:  ");
                Serial.println(buttonPushCounter);
            } else {
                Serial.println("off");
            }
        }
    }

    lastButtonState = reading;
}

void controlLED() {
    switch (buttonPushCounter % 5) { // Change to 5 to add a new case
        case 0:
            digitalWrite(ledPin, HIGH);
            break;
        case 1:
            digitalWrite(ledPin, LOW);
            break;
        case 2:
            toggleLED(1000);
            break;
        case 3:
            toggleLED(500);
            break;
        case 4: // New case for Morse code SOS signal
            sendMorseSOS();
            break;
        default:
            digitalWrite(ledPin, LOW);
            break;
    }
}

void sendMorseSOS() {
    const int dotDuration = 200; // Duration of a Morse code "dot"
    const int dashDuration = dotDuration * 3; // Duration of a Morse code "dash"
    const int symbolSpace = dotDuration; // Space between symbols in a character
    const int charSpace = dotDuration * 3; // Space between characters
    const int wordSpace = dotDuration * 7; // Space between words

    static int state = 0;
    static unsigned long lastMillis = 0;
    unsigned long currentMillis = millis();

    if (state == 0 && currentMillis - lastMillis >= symbolSpace) {
        digitalWrite(ledPin, HIGH);
        digitalWrite(relayPin, HIGH); // Turn on relay
        lastMillis = currentMillis;
        state = 1;
    } else if (state == 1 && currentMillis - lastMillis >= dotDuration) {
        digitalWrite(ledPin, LOW);
        digitalWrite(relayPin, LOW); // Turn off relay
        lastMillis = currentMillis;
        state = 2;
    } else if (state == 2 && currentMillis - lastMillis >= charSpace) {
        digitalWrite(ledPin, HIGH);
        digitalWrite(relayPin, HIGH); // Turn on relay
        lastMillis = currentMillis;
        state = 3;
    } else if (state == 3 && currentMillis - lastMillis >= dashDuration) {
        digitalWrite(ledPin, LOW);
        digitalWrite(relayPin, LOW); // Turn off relay
        lastMillis = currentMillis;
        state = 4;
    } else if (state == 4 && currentMillis - lastMillis >= charSpace) {
        digitalWrite(ledPin, HIGH);
        digitalWrite(relayPin, HIGH); // Turn on relay
        lastMillis = currentMillis;
        state = 5;
    } else if (state == 5 && currentMillis - lastMillis >= dotDuration) {
        digitalWrite(ledPin, LOW);
        digitalWrite(relayPin, LOW); // Turn off relay
        lastMillis = currentMillis;
        state = 0;
    }
}

void toggleLED(unsigned long interval) { 
    if (millis() - ledStartMillis >= interval) { // Check if interval has elapsed
        ledStartMillis = millis(); // Reset the interval
        ledStatus = !ledStatus; // Toggle LED status
        digitalWrite(ledPin, ledStatus ? HIGH : LOW); // Set LED to HIGH or LOW
    }
    
}

void SerialEvent() {
    if (Serial.available()) {
        String inputString = Serial.readString();
        inputString.trim(); // Remove any leading/trailing whitespace

        if (inputString.equals("One")) { // Check for "One" input
            buttonPushCounter = 2;
        } else if (inputString.equals("Two")) { // Check for "Two" input
            buttonPushCounter = 3;
        } else if (inputString.equals("Three")) { // Check for "Three" input
            buttonPushCounter = 0;
        } else if (inputString.equals("Four")) { // Check for "Four" input
            buttonPushCounter = 1;
        } else if (inputString.equalsIgnoreCase("SOS")) { // Check for "SOS" input, case insensitive
            buttonPushCounter = 4;
        }
    }
}

