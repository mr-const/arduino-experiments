#include <arduino.h>
#include <pins_arduino.h> 

#include "pir.h"

uint8_t Pir::mSignalPin;
bool Pir::mPirHigh = false;
bool Pir::mPirPrev = false;


void Pir::init(uint8_t signalPin)
{
	mSignalPin = signalPin;
	pinMode(mSignalPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(mSignalPin), onPirEvent, CHANGE);
}

void Pir::loop() {
	if (mPirHigh != mPirPrev) {
		if (mPirHigh)
			Serial.println("PIR detected");
		else
			Serial.println("PIR lost");
	}

	mPirPrev = mPirHigh;
}

void Pir::onPirEvent() {

	int pirSig = digitalRead(mSignalPin);
	if (pirSig == HIGH) {
		digitalWrite(LED_BUILTIN, HIGH);
		Serial.println("PIR Tripped");
		mPirHigh = true;
	}
	else {
		digitalWrite(LED_BUILTIN, LOW);
		Serial.println("PIR Armed");
		mPirHigh = false;
	}
}
