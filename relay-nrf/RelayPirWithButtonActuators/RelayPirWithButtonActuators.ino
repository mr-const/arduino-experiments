// Example sketch för a "light switch" where you can control light or something 
// else from both vera and a local physical button (connected between digital
// pin 3 and GND).
// This node also works as a repeader for other nodes
#define MY_DEBUG
#define MY_RADIO_NRF24
#include <MySensors.h>
#include <SPI.h>
#include <Bounce2.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "src/pir.h"

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 8

#define PIR_POWER_PIN    A1
#define PIR_SIGNAL_PIN 3
#define RELAY_PIN  5 // Arduino Digital I/O pin number for relay
#define BUTTON_PIN   4 // Arduino Digital I/O pin number for button 
#define BUTTON_PIN_2 7
#define CHILD_DSB_ID 13
#define CHILD_RELAY_ID 12
#define CHILD_PIR_ID 11 // Id of the sensor child
#define RELAY_ON 1
#define RELAY_OFF 0

#define BUILTIN_LED 13

Bounce debouncer2 = Bounce();
int oldValue2 = 0;
bool state2;

MyMessage msgTripped(CHILD_PIR_ID, V_TRIPPED);
MyMessage msg2(CHILD_RELAY_ID, V_LIGHT);
MyMessage msg3(CHILD_DSB_ID, V_TEMP);

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass the oneWire reference to Dallas Temperature. 

void onPirEvent(bool tripped);

void setup()
{
	// Startup up the OneWire library
	sensors.begin();
	// requestTemperatures() will not block current thread
	sensors.setWaitForConversion(false);

	// Setup the button
	pinMode(BUTTON_PIN_2, INPUT);
	// Activate internal pull-up
	digitalWrite(BUTTON_PIN_2, HIGH);

	debouncer2.attach(BUTTON_PIN_2);
	debouncer2.interval(5);

	digitalWrite(RELAY_PIN, RELAY_OFF);
	// Then set relay pins in output mode
	pinMode(RELAY_PIN, OUTPUT);

	// Set relay to last known state (using eeprom storage) 
	state2 = loadState(CHILD_RELAY_ID);
	digitalWrite(RELAY_PIN, state2 ? RELAY_ON : RELAY_OFF);
	Serial.println("App initialized");

	// Using POWER PIN as power source for PIR
	pinMode(PIR_POWER_PIN, OUTPUT);
	digitalWrite(PIR_POWER_PIN, HIGH);
	// PIR tripped state shown with built-in led
	pinMode(LED_BUILTIN, OUTPUT);
	Pir::init(PIR_SIGNAL_PIN);
	Pir::setPirEventHandler(&onPirEvent);
}

void presentation() {
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("PIR with Relay & Button", "0.2");
	// Register all sensors to gw (they will be created as child devices)
	present(CHILD_PIR_ID, S_MOTION);
	present(CHILD_RELAY_ID, S_LIGHT);
	present(CHILD_DSB_ID, S_TEMP);

}

void loop()
{
	static float prevTemp = 0;

	Pir::loop();
	debouncer2.update();
	// Get the update value
	int value2 = debouncer2.read();

	if (value2 != oldValue2) {
		send(msg2.set(state2 ? false : true), true); // Send new state and request ack back
	}
	oldValue2 = value2;

	// Fetch temperatures from Dallas sensors
	sensors.requestTemperatures();
	// Fetch and round temperature to one decimal
	float temperature = static_cast<float>(static_cast<int>(sensors.getTempCByIndex(0) * 10.f)) / 10.f;

	if (temperature != -127.00f && temperature != 85.00f && prevTemp != temperature) {
		// Send in the new temperature
		send(msg3.set(temperature, 1));
		Serial.print("Sent temperature: ");
		Serial.println(temperature);
		prevTemp = temperature;
	}
}

void receive(const MyMessage &message) {
	// We only expect one type of message from controller. But we better check anyway.
	if (message.isAck()) {
		Serial.println("This is an ack from gateway");
	}

	if (message.type == V_LIGHT && message.sensor == CHILD_RELAY_ID) {
		state2 = message.getBool();
		digitalWrite(RELAY_PIN, state2 ? RELAY_ON : RELAY_OFF);
		// Store state in eeprom
		saveState(CHILD_RELAY_ID, state2);

		// Write some debug info
		Serial.print("Incoming change for sensor:");
		Serial.print(message.sensor);
		Serial.print(", New status: ");
		Serial.println(message.getBool());
	}
	else {
		Serial.print("Unhandled message: ");
		Serial.println(message.sensor);
	}
}

void onPirEvent(bool tripped) {
	digitalWrite(LED_BUILTIN, tripped ? HIGH : LOW);
	Serial.print("PIR ");
	if (tripped) {
		Serial.println("tripped");
		send(msgTripped.set(1));
	}
	else {
		Serial.println("armed");
		send(msgTripped.set(0));
	}
}
