/************************************************************************/
/* PIR (BISS0001-based) Class Module                                    */
/* calls tripped/armed handler on "main thread"                         */
/************************************************************************/
#ifndef pir_h__
#define pir_h__

typedef void(*PirEventHandler)(bool);

class Pir {
private:
	static uint8_t mSignalPin;
	static bool mPirHigh;
	static bool mPirPrev;
	static PirEventHandler mEventHandler;
	static void onPirEvent();

public:
	//************************************
	// Method:    init: initializes PIR module singleton.
	// Parameter: uint8_t signalPin: pin with attached PIR signal line. Must be one of the pins with an interrupt support (2,3 for arduino pro mini).
	//************************************
	static void init(uint8_t signalPin);
	//************************************
	// Method:    loop: must be called from main loop, executes callback for PIR motion event.
	//************************************
	static void loop();

	//************************************
	// Method:    setPirEventHandler: installs PIR event handler, or clears if NULL/nullptr.
	// Parameter: PirEventHandler handler, which called on main loop after hardware interrupt received, 
	// has one parameter: tripped - true when tripped, false - when back to armed (idle) state.
	//************************************
	static void setPirEventHandler(PirEventHandler handler);
};

#endif // pir_h__
