#include <arduino.h>
#include <pins_arduino.h> 

#include "pir.h"

uint8_t Pir::mSignalPin;
bool Pir::mPirHigh = false;
bool Pir::mPirPrev = false;
PirEventHandler Pir::mEventHandler = nullptr;


void Pir::init(uint8_t signalPin)
{
	mSignalPin = signalPin;
	pinMode(mSignalPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(mSignalPin), onPirEvent, CHANGE);
}

void Pir::loop() {
	if (mPirHigh != mPirPrev) {
		if (mEventHandler)
			mEventHandler(mPirHigh);
	}
	mPirPrev = mPirHigh;
}

void Pir::setPirEventHandler(PirEventHandler handler)
{
	mEventHandler = handler;
}

void Pir::onPirEvent() {
	mPirHigh = digitalRead(mSignalPin) == HIGH;
}
