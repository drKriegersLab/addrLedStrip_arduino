#pragma once

#include "Adafruit_NeoPixel.h"

#define CH_UNDERLINE char("_")
#define CH_ZERO char("0")
#define CH_ONE char("1")

#define GET_NUM_FROM_STRING(s, index) s[index] - 40
#define PARSE_2_DIGIT_NUM_FROM_STRING(s, index_start) ((s[index_start] - 48) * 10 + (s[index_start + 1]))
#define PARSE_3_DIGIT_NUM_FROM_STRING(s, index_start) ((s[index_start] - 48) * 100 + (s[index_start + 1] - 48) * 10 + (s[index_start + 2] - 48) )
#define MAX(a,b) a>b ? a:b
#define MIN(a,b) a<b ? a:b

#define NUM_OF_LEDS 60
#define STRIP_PIN 6
#define MSG_PARSER_COUNTER_LIMIT 1000



class LedStrip_interface : public Adafruit_NeoPixel
{
private: 
	uint16_t RGB_stack[NUM_OF_LEDS][3]; // NUM_OF_LEDS x 3 matrix for storing RGB values for each LED on the strip
	uint16_t t_RGB_stack[NUM_OF_LEDS][3]; // temp matrix with same size with original stack
	String msg;
	long int cyc_parser_msg_wait_counter = 0;

	bool flag_waiting_for_strip_codes;
	
	// function for reseting all elements of the RGB stack.
	void init_RGB_stack() {
		for (uint16_t led_num = 0; led_num < NUM_OF_LEDS; led_num++) {
			for (uint8_t color_num = 0; color_num < 3; color_num++) {
				RGB_stack[led_num][color_num] = 0;
			}
		}
	}

	// function for testing each LED on the strip
	void strip_test() {
		// reset all LED
		for (uint16_t led_id = 0; led_id < numPixels() + 15; led_id++) {
			setPixelColor(led_id - 10, Color(0, 0, 0));
		}
		show();
		
		// RGB "sneak"
		for (uint16_t led_id = 0; led_id < numPixels() + 15; led_id++) {
			delay(20);
			//R
			if (led_id < numPixels()) {
				RGB_stack[led_id][0] = 100;
				RGB_stack[led_id][1] = 0;
				RGB_stack[led_id][2] = 0;
				setPixelColor(led_id, Color(RGB_stack[led_id][0], RGB_stack[led_id][1], RGB_stack[led_id][2]));
			}
			//G
			if ((led_id > 4) && (led_id < (numPixels() - 5))) {
				RGB_stack[led_id-5][0] = 0;
				RGB_stack[led_id-5][1] = 100;
				RGB_stack[led_id-5][2] = 0;
				setPixelColor(led_id - 5, Color(RGB_stack[led_id-5][0], RGB_stack[led_id-5][1], RGB_stack[led_id-5][2]));
			}
			//B
			if ((led_id > 9) && (led_id < (numPixels() - 10))) {
				RGB_stack[led_id-10][0] = 0;
				RGB_stack[led_id-10][1] = 0;
				RGB_stack[led_id-10][2] = 100;
				setPixelColor(led_id - 10, Color(RGB_stack[led_id-10][0], RGB_stack[led_id-10][1], RGB_stack[led_id-10][2]));
			}
			show();
		}
		delay(20);

		// RGB sneak's going out  (shifting the stack, and fill with zeros the new elements)
		for (uint16_t led_id = 0; led_id < numPixels(); led_id++) {
			delay(20);
			shift_RGB_stack(1, 0, 0, 0);
			show_full_RGB_stack();
		}
	} //end strip test

	// send a selected LED's RGB values to Serial port
	void print_led_RGB(uint16_t px) {
		Serial.print("l: "); Serial.print(int(px));
		Serial.print("  R:  ");  Serial.print(int(RGB_stack[px][0]));
		Serial.print("  G:  ");  Serial.print(int(RGB_stack[px][1]));
		Serial.print("  B:  ");  Serial.print(int(RGB_stack[px][2]));
		Serial.println("");
	}
	
public:
	// init of Base interface class
	LedStrip_interface() : Adafruit_NeoPixel(NUM_OF_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800) {
		begin();
	}

	// method for initialization in the Setup() function of main ino file
	void init() {
		begin();
		strip_test();
		flag_waiting_for_strip_codes = false;
		Serial.println("LED strip initialized");
	}


	// set RGB values of a specified LED in the _STACK_
	void set_one_LED_in_stack(uint16_t LED_id, uint16_t color_R, uint16_t color_G, uint16_t color_B) {
		RGB_stack[LED_id][0] = color_R;
		RGB_stack[LED_id][1] = color_G;
		RGB_stack[LED_id][2] = color_B;

	}

	// method for shifting each element in the stack with specified steps and filling the new elements with an RGB value
	void shift_RGB_stack(uint16_t steps, uint16_t new_R, uint16_t new_G, uint16_t new_B) {
		//shifting
		for (uint16_t led_id = steps; led_id < numPixels(); led_id++) {
			t_RGB_stack[led_id][0] = RGB_stack[led_id - steps][0];
			t_RGB_stack[led_id][1] = RGB_stack[led_id - steps][1];
			t_RGB_stack[led_id][2] = RGB_stack[led_id - steps][2];
		}
		//fill empty space
		for (uint16_t led_id = 0; led_id < steps + 1; led_id++) {
			t_RGB_stack[led_id][0] = 0;
			t_RGB_stack[led_id][1] = 0;
			t_RGB_stack[led_id][2] = 0;
		}
		//store new values
		for (uint16_t led_id = 0; led_id < numPixels(); led_id++) {
			for (uint16_t c = 0; c < 3; c++) {
				RGB_stack[led_id][c] = t_RGB_stack[led_id][c];
			}
		}
	}

	// put the STACK to the STRIP
	void show_full_RGB_stack() {
		for (uint16_t led_id = 0; led_id < numPixels(); led_id++) {
			setPixelColor(led_id, Color(RGB_stack[led_id][0], RGB_stack[led_id][1], RGB_stack[led_id][2]));
		}
		show();
	}


	// method for parsing RGB cmds for each LED on the strip
	void parse_strip_codes(byte *received_value) {

		//set LEDs' RGB codes
		if (flag_waiting_for_strip_codes) {
			set_one_LED_in_stack(received_value[0], received_value[2], received_value[3], received_value[4]);
			set_one_LED_in_stack(received_value[1], received_value[5], received_value[6], received_value[7]);
			show_full_RGB_stack();
		}
		// reset wait flag
		if (received_value[0] >= NUM_OF_LEDS) {
			//setPixelColor(20, Color(50, 0, 0));
			//show();
			flag_waiting_for_strip_codes = false;
		}
		// set wait flag (overrite reset)
		if (received_value[0] == 255) {
			flag_waiting_for_strip_codes = true;
		}
	}

	
	//~LedStrip_interface();
};

