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

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 8

#define PIR_POWER_PIN    3
#define PIR_SIGNAL_PIN A1
#define RELAY_PIN_2  5 // Arduino Digital I/O pin number for relay
#define BUTTON_PIN   4 // Arduino Digital I/O pin number for button 
#define BUTTON_PIN_2 7
#define CHILD_ID_3 13
#define CHILD_ID_2 12
#define CHILD_ID 11 // Id of the sensor child
#define RELAY_ON 1
#define RELAY_OFF 0

Bounce debouncer = Bounce(); 
int oldValue=0;
bool state;
Bounce debouncer2 = Bounce(); 
int oldValue2=0;
bool state2;

MyMessage msg(CHILD_ID, V_TRIPPED);
MyMessage msg2(CHILD_ID_2,V_LIGHT);
MyMessage msg3(CHILD_ID_3,V_TEMP);

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass the oneWire reference to Dallas Temperature. 

void setup()  
{  
  // Startup up the OneWire library
  sensors.begin();
  // requestTemperatures() will not block current thread
  sensors.setWaitForConversion(false);
  
   // Setup the button
  pinMode(BUTTON_PIN_2,INPUT);
  // Activate internal pull-up
  digitalWrite(BUTTON_PIN_2,HIGH);
  
  debouncer2.attach(BUTTON_PIN_2);
  debouncer2.interval(5);

  digitalWrite(RELAY_PIN_2, RELAY_OFF);
  // Then set relay pins in output mode
  pinMode(RELAY_PIN_2, OUTPUT);   
      
  // Set relay to last known state (using eeprom storage) 
  state2 = loadState(CHILD_ID_2);
  digitalWrite(RELAY_PIN_2, state2?RELAY_ON:RELAY_OFF);
  
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("PIR with Relay & Button", "0.1");
  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID, S_MOTION);
  present(CHILD_ID_2, S_LIGHT);
  present(CHILD_ID_3, S_TEMP);
 
}

/*
*  Example on how to asynchronously check for new messages from gw
*/
void loop() 
{
  debouncer.update();
  debouncer2.update();
  // Get the update value
  int value = debouncer.read();
  int value2 = debouncer2.read();
  
  if (value != oldValue) {
      send(msg.set(state?false:true), true); // Send new state and request ack back
  }
  oldValue = value;
  
  if (value2 != oldValue2) {
      send(msg2.set(state2?false:true), true); // Send new state and request ack back
  }
  oldValue2 = value2;  

  // Fetch temperatures from Dallas sensors
  sensors.requestTemperatures();
  // query conversion time and sleep until conversion completed
  // Fetch and round temperature to one decimal
  float temperature = static_cast<float>(static_cast<int>((getConfig().isMetric?sensors.getTempCByIndex(0):sensors.getTempFByIndex(0)) * 10.)) / 10.;

  if (temperature != -127.00 && temperature != 85.00) {
          // Send in the new temperature
      send(msg3.set(temperature,1));
  }
} 
 
void incomingMessage(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.isAck()) {
     Serial.println("This is an ack from gateway");
  }

  if (message.type == V_LIGHT && message.sensor == 11) {
//     // Change relay state
//     state = message.getBool();
//     digitalWrite(RELAY_PIN, state?RELAY_ON:RELAY_OFF);
//     // Store state in eeprom
//     saveState(CHILD_ID, state);
   }
  else if(message.type == V_LIGHT && message.sensor == 12){  
    
     state2 = message.getBool();
     digitalWrite(RELAY_PIN_2, state2?RELAY_ON:RELAY_OFF);
     // Store state in eeprom
     saveState(CHILD_ID_2, state2);
    
    
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}
