// Example sketch för a "light switch" where you can control light or something 
// else from both vera and a local physical button (connected between digital
// pin 3 and GND).
// This node also works as a repeader for other nodes

#define MY_RADIO_NRF24
#include <MySensors.h>
#include <SPI.h>
#include <Bounce2.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define RELAY_PIN    3  // Arduino Digital I/O pin number for relay 
#define RELAY_PIN_2  5
#define BUTTON_PIN   4  // Arduino Digital I/O pin number for button 
#define BUTTON_PIN_2 7
#define ONE_WIRE_BUS 8

#define CHILD_DSB_ID 13
#define CHILD_ID_2 12
#define CHILD_ID 11 // Id of the sensor child
#define RELAY_ON 1
#define RELAY_OFF 0

Bounce debouncer = Bounce();
int oldValue = 0;
bool state;
Bounce debouncer2 = Bounce();
int oldValue2 = 0;
bool state2;

MyMessage msg(CHILD_ID, V_LIGHT);
MyMessage msg2(CHILD_ID_2, V_LIGHT);
MyMessage msgTemp(CHILD_DSB_ID, V_TEMP);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass the oneWire reference to Dallas Temperature. 


void presentation() {
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("Double Relay & Button", "0.2");
	// Register all sensors to gw (they will be created as child devices)
	present(CHILD_ID, S_LIGHT);
	present(CHILD_ID_2, S_LIGHT);
	present(CHILD_DSB_ID, S_TEMP);
}

void setup()
{
	sensors.begin();
	sensors.setWaitForConversion(false);

	// Setup the button
	pinMode(BUTTON_PIN, INPUT);
	// Activate internal pull-up
	digitalWrite(BUTTON_PIN, HIGH);

	// Setup the button
	pinMode(BUTTON_PIN_2, INPUT);
	// Activate internal pull-up
	digitalWrite(BUTTON_PIN_2, HIGH);

	// After setting up the button, setup debouncer
	debouncer.attach(BUTTON_PIN);
	debouncer.interval(5);

	debouncer2.attach(BUTTON_PIN_2);
	debouncer2.interval(5);


	// Make sure relays are off when starting up
	digitalWrite(RELAY_PIN, RELAY_OFF);
	// Then set relay pins in output mode
	pinMode(RELAY_PIN, OUTPUT);

	digitalWrite(RELAY_PIN_2, RELAY_OFF);
	// Then set relay pins in output mode
	pinMode(RELAY_PIN_2, OUTPUT);

	// Set relay to last known state (using eeprom storage) 
	state = loadState(CHILD_ID);
	digitalWrite(RELAY_PIN, state ? RELAY_ON : RELAY_OFF);

	state2 = loadState(CHILD_ID_2);
	digitalWrite(RELAY_PIN_2, state2 ? RELAY_ON : RELAY_OFF);

}


/*
*  Example on how to asynchronously check for new messages from gw
*/
void loop()
{
	static float prevTemp = 0;

	debouncer.update();
	debouncer2.update();
	// Get the update value
	int value = debouncer.read();
	int value2 = debouncer2.read();

	if (value != oldValue) {
		send(msg.set(state ? false : true), true); // Send new state and request ack back
	}
	oldValue = value;

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
		send(msgTemp.set(temperature, 1));
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

	if (message.type == V_LIGHT && message.sensor == 11) {
		// Change relay state
		state = message.getBool();
		digitalWrite(RELAY_PIN, state ? RELAY_ON : RELAY_OFF);
		// Store state in eeprom
		saveState(CHILD_ID, state);
	}
	else if (message.type == V_LIGHT && message.sensor == 12) {

		state2 = message.getBool();
		digitalWrite(RELAY_PIN_2, state2 ? RELAY_ON : RELAY_OFF);
		// Store state in eeprom
		saveState(CHILD_ID_2, state2);


		// Write some debug info
		Serial.print("Incoming change for sensor:");
		Serial.print(message.sensor);
		Serial.print(", New status: ");
		Serial.println(message.getBool());
	}
}