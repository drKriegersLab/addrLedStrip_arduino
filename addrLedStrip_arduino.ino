#include "LedStrip_interface.h"

LedStrip_interface strip = LedStrip_interface();
void setup()
{

  /* add setup code here */
	Serial.begin(115200);
	Serial.println("Hi, I am a fancy interface for addressable LED strip");
	strip.init();
	
}

void loop()
{
	//strip.setPixelColor(15, strip.Color(0, 0, 100));
	//6strip.show();

}
