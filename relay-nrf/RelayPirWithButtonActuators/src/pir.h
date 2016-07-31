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
	static void init(uint8_t signalPin);
	static void loop();

	static void setPirEventHandler(PirEventHandler handler);
};

#endif // pir_h__
