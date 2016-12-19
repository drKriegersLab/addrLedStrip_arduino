#include "LedStrip_interface.h"

LedStrip_interface strip = LedStrip_interface();
byte received_value[8];
bool flag_receive_strip_codes;

void setup()
{
	flag_receive_strip_codes = false;
	Serial.begin(9600);
	Serial.println("Hi, I am a fancy interface for addressable LED strip");
	strip.init();

	strip.set_one_LED_in_stack(15, 50, 0, 0);
	strip.show_full_RGB_stack();

	for (uint8_t i = 0; i < sizeof(received_value); i++) {
		received_value[i] = 0;
	}
	
}

size_t size_of_buffer  = 8;
void loop()
{
	

	
	if (Serial.available()) {

		// handling of the bytes on the Serial line
		Serial.readBytes(received_value, size_of_buffer);

		// sending the received bytes to the parser
		strip.parse_strip_codes(received_value);
		
		
		

	}


}
