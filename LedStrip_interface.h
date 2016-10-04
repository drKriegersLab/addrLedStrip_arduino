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

#define NUM_OF_LEDS 30
#define STRIP_PIN 6
#define MSG_PARSER_COUNTER_LIMIT 1000

class LedStrip_interface : public Adafruit_NeoPixel
{
private: 
	uint16_t RGB_stack[NUM_OF_LEDS][3];
	uint16_t t_RGB_stack[NUM_OF_LEDS][3];
	String msg;
	long int cyc_parser_msg_wait_counter = 0;
	
	void init_RGB_stack() {
		for (uint16_t led_num = 0; led_num < NUM_OF_LEDS; led_num++) {
			for (uint8_t color_num = 0; color_num < 3; color_num++) {
				RGB_stack[led_num][color_num] = 0;
			}
		}
	}

	void strip_test() {
		
		for (uint16_t led_id = 0; led_id < numPixels() + 15; led_id++) {
			setPixelColor(led_id - 10, Color(0, 0, 0));
		}
		show();
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
		for (uint16_t led_id = 0; led_id < numPixels(); led_id++) {
			delay(20);
			shift_RGB_stack(1, 0, 0, 0);
			show_full_RGB_stack();
		}
		
		
	}
	void print_led_RGB(uint16_t px) {
		Serial.print("l: "); Serial.print(int(px));
		Serial.print("  R:  ");  Serial.print(int(RGB_stack[px][0]));
		Serial.print("  G:  ");  Serial.print(int(RGB_stack[px][1]));
		Serial.print("  B:  ");  Serial.print(int(RGB_stack[px][2]));
		Serial.println("");
	}
	/*
	void shift_RGB_stack(uint16_t steps, uint16_t new_R, uint16_t new_G, uint16_t new_B) {
		//shifting
		for (uint16_t led_id = steps; led_id < numPixels(); led_id++) {
			t_RGB_stack[led_id][0] = RGB_stack[led_id - steps][0];
			t_RGB_stack[led_id][1] = RGB_stack[led_id - steps][1];
			t_RGB_stack[led_id][2] = RGB_stack[led_id - steps][2];
		}
		//fill empty space
		for (uint16_t led_id = 0; led_id < steps+1; led_id++) {
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
	*/
	
public:
	LedStrip_interface() : Adafruit_NeoPixel(NUM_OF_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800) {
		begin();
		
	}
	void init() {
		strip_test();
		Serial.println("LED strip initialized");
	}

	void set_one_LED_in_stack(uint16_t LED_id, uint16_t color_R, uint16_t color_G, uint16_t color_B) {
		RGB_stack[LED_id][0] = color_R;
		RGB_stack[LED_id][1] = color_G;
		RGB_stack[LED_id][2] = color_B;

	}
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
	void show_full_RGB_stack() {
		for (uint16_t led_id = 0; led_id < numPixels(); led_id++) {
			setPixelColor(led_id, Color(RGB_stack[led_id][0], RGB_stack[led_id][1], RGB_stack[led_id][2]));
		}
		show();
	}
	void parseCmd() {
		if (Serial.available()) {
			msg = Serial.readString();
			if ((msg[0] == 'S') && (msg[1] == '_')) {
				init_RGB_stack();
				Serial.println(msg);
				Serial.println(msg.length());

			}
		}
	}

	
	//~LedStrip_interface();
};

