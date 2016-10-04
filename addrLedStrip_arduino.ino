#include "LedStrip_interface.h"

LedStrip_interface strip = LedStrip_interface();

String received_value;
void setup()
{

  /* add setup code here */
	Serial.begin(56000);
	Serial.println("Hi, I am a fancy interface for addressable LED strip");
	strip.init();
	
}

void loop()
{
	//strip.setPixelColor(15, strip.Color(0, 0, 100));
	//6strip.show();
	if (Serial.available()) {
		received_value += Serial.readString();
		strip.set_one_LED_in_stack(15, 100, 0, 0);
		strip.show_full_RGB_stack();
		Serial.println(received_value);
	}


}
