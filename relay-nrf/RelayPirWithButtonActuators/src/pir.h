/************************************************************************/
/* PIR (BISS0001-based) Class Module                                    */
/************************************************************************/
#ifndef pir_h__
#define pir_h__

class Pir {
private:
	static uint8_t mSignalPin;
	static bool mPirHigh;
	static bool mPirPrev;
	static void onPirEvent();

public:
	static void init(uint8_t signalPin);
	static void loop();
};

#endif // pir_h__
