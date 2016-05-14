#define PIR_SIGNAL_PIN 3
#define LED_ATTACHED_PIN 9

bool g_PirHigh = false;
bool g_PirPrev = false;

void setup() {
   Serial.begin(115200);
   pinMode(PIR_SIGNAL_PIN, INPUT);
   pinMode(LED_ATTACHED_PIN, OUTPUT);
   attachInterrupt(digitalPinToInterrupt(PIR_SIGNAL_PIN), onPirEvent, CHANGE);
   Serial.println("PIR detector initialized");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (g_PirHigh != g_PirPrev) {
    if (g_PirHigh)
        Serial.println("PIR detected");
    else
        Serial.println("PIR lost");
  }

  g_PirPrev = g_PirHigh;
}

void onPirEvent() {

  int pirSig = digitalRead(PIR_SIGNAL_PIN);
  if (pirSig == HIGH) {
    digitalWrite(LED_ATTACHED_PIN, HIGH);
    g_PirHigh = true;
  }
  else {
    digitalWrite(LED_ATTACHED_PIN, LOW);
    g_PirHigh = false;
  }
}

