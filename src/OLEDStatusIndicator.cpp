/*
 * OLEDStatusIndicator.cpp
 *
 *  Created on: 14.08.2016
 *      Author: ian
 */

#include "OLEDStatusIndicator.h"

#include <ESP8266WiFi.h> // Needed for WiFi Signal Strength


#include "gfx_resources.h"
#include "images.h"

#include <LoggerNode.h>

OLEDStatusIndicator::OLEDStatusIndicator() :
		wifi(false), wifi_quality(-1), mqtt(false),	cfgmode(false) {

}

void OLEDStatusIndicator::Event(const HomieEvent& event) {
	Serial.begin(115200);
	Serial.flush();
	cfgmode = false;
	switch (event.type) {
	case HomieEventType::CONFIGURATION_MODE:
		last_status = "Configuration mode started";
		cfgmode = true;
		break;
	case HomieEventType::NORMAL_MODE:
		last_status = "Normal mode started";
		break;
	case HomieEventType::OTA_STARTED:
		last_status = "OTA started";
		break;
	case HomieEventType::OTA_FAILED:
		last_status = "OTA failed";
		break;
	case HomieEventType::OTA_SUCCESSFUL:
		last_status = "OTA successful";
		break;
	case HomieEventType::ABOUT_TO_RESET:
		last_status = "About to reset";
		break;
	case HomieEventType::WIFI_CONNECTED:
		last_status = "Wi-Fi connected";
		wifi = true;
		break;
	case HomieEventType::WIFI_DISCONNECTED:
		last_status = "Wi-Fi disconnected";
		wifi = false;
		break;
	case HomieEventType::MQTT_CONNECTED:
		last_status = "MQTT connected";
		mqtt = true;
		break;
	case HomieEventType::MQTT_DISCONNECTED:
		last_status = "MQTT disconnected";
		mqtt = false;
		break;
	}
	Serial.printf("Event: %s\n", last_status.c_str());
	Serial.flush();

}

void OLEDStatusIndicator::drawOverlay(OLEDDisplay& display, OLEDDisplayUiState& state, uint8_t idx) {
	static uint8_t count = 0;

	uint32_t now = millis();

	bool blink_state = (now >> 4) % 2;
	uint8_t cycle4 = ++count % 4;
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.setFont(ArialMT_Plain_10);
	if (cfgmode) {
		display.drawXbm(0, 0, cfg_mode_width, cfg_mode_height,
						cfg_mode_bits);
		display.drawString(cfg_mode_width, 3, "Cfg Mode!");
	}
	else if (wifi) {
		String RSSIString;
		int32_t rssi = WiFi.RSSI();
//		RSSIString += rssi;
//		RSSIString += "dbm";
		display.drawXbm(128 - conn_mqtt_width - 16, 0, wireless_full_width, wireless_full_height,
				wireless_full_bits);
//		display.drawString(48+wireless_full_width, 3, RSSIString);
	} else {
		if (blink_state) {
			display.drawXbm(128 - conn_mqtt_width - 16, 0, wireless_full_width, wireless_full_height,
					wireless_full_bits);
		}
	}
	// MQTT indicator
	if (mqtt || (wifi && blink_state)) {
		display.drawXbm(128 - conn_mqtt_width, 0, conn_mqtt_width,
				conn_mqtt_height, conn_mqtt_bits);
	}

	// Lifesign
//	uint8_t quads = (1 << 3 - cycle4); // Draw all quads except 1
//	display.drawCircleQuads(128 - 8, 8, 6, quads);
	if (!blink_state) display.drawRect(127,0,1,1);
}
