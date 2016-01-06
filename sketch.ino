#include <stdlib.h>
#define START 2400
#define PAUSE 600
#define INFRARED_PIN 12
#define NTH_BIT(number, nth) ((number) & (1 << nth))

enum Devices
{
	TV = 1,
	VCR_1 = 2,
	VCR_2 = 3,
	LASER_DISK = 6,
	SURROUND_SOUND = 12,
	TUNER = 16,
	CD_PLAYER = 17,
	EQUALIZER = 18
};

enum Buttons
{
	KEY_0 = 0,
	KEY_1 = 1,
	KEY_2 = 2,
	KEY_3 = 3,
	KEY_4 = 4,
	KEY_5 = 5,
	KEY_6 = 6,
	KEY_7 = 7,
	KEY_8 = 8,
	KEY_9 = 9,
	CHANNEL_UP = 16,
	CHANNEL_DOWN = 17,
	VOLUME_UP = 18,
	VOLUME_DOWN = 19,
	MUTE = 20,
	POWER = 21,
	RESET = 22,
	AUDIO_MODE = 23,
	CONTRAST_UP = 24,
	CONTRAST_DOWN = 25,
	COLOR_UP = 26,
	COLOR_DOWN = 27,
	BRIGHTNESS_UP = 30,
	BRIGHTNESS_DOWN = 31,
	BALANCE_LEFT = 38,
	BALANCE_RIGHT = 39,
	STANDBY = 47
};

boolean sent = false;
boolean buttonReady = false;
int command;

void modulate(unsigned long duration_usecs);
void sendCommand(int command, int address);
void repeatCommand(int command, int address, int amount = 3);

void setup()
{
	Serial.begin(9600);
	pinMode(INFRARED_PIN, OUTPUT);
}

void loop()
{
	if(!sent)
	{
		repeatCommand(KEY_1, TV);
		delay(1000);
		repeatCommand(KEY_0, TV);
		delay(1000);
		sent = true;
	}
	if(Serial.available() > 0)
	{
		int cmd = Serial.parseInt();
		Serial.print("Sending ");
		Serial.print(cmd);
		Serial.print("... ");
		unsigned long cur = micros();
		repeatCommand(cmd , TV);
		Serial.print("Done ! [");
		Serial.print(micros() - cur);
		Serial.println(" us]");
	}
	/*if(sent)
		return;
	repeatCommand(KEY_0, TV, 6);
	sent = true;*/
	/*
	digitalWrite(INFRARED_PIN, HIGH);
	delay(1000);
	digitalWrite(INFRARED_PIN, LOW);
	delay(1000);
	digitalWrite(INFRARED_PIN, HIGH);
	delay(1000);
	*/
	/*unsigned long start = micros();
	for(int i = 0; i < 3; i++)
	{
		sendCommand(KEY_1, TV);
		delayMicroseconds(45000 - start);
		start = micros();
  	}
	sendCommand(KEY_1, TV);
	delay(1000);
	sendCommand(POWER, TV);
	delay(1000);
	sendCommand(VOLUME_DOWN, TV);
	sent = true;*/
}

/* Sends a 40 KHz series of pulses that last a total of duration_usecs microseconds through INFRARED_PIN */
void modulate(unsigned long duration_usecs)
{
	unsigned long start = micros();
	while(1)
	{
		digitalWrite(INFRARED_PIN, HIGH);
		delayMicroseconds(8);
		digitalWrite(INFRARED_PIN, LOW);
		delayMicroseconds(17);
		if(micros() - start >= duration_usecs)
			break;
	}
}

void repeatCommand(int command, int address = TV, int amount)
{
	unsigned long start = 0L;
	for(int i = 0; i < amount; i++)
	{
		start = micros();
		sendCommand(command, address);
		//delayMicroseconds(wait_for - micros() + start);
		delay(45 - ((micros() - start) / 1000));
	}
}

void sendCommand(int command, int address = TV)
{
	// Start burst
	modulate(START);
	delayMicroseconds(PAUSE);
	// Sending command
	int i;
	for(i = 0; i < 7; i++)
	{
		modulate(NTH_BIT(command, i) ? 1200 : 600);
		digitalWrite(INFRARED_PIN, LOW);
		delayMicroseconds(PAUSE);
	}
	// Sending address
	for(i = 0; i < 5; i++)
	{
		modulate(NTH_BIT(address, i) ? 1200 : 600);
		digitalWrite(INFRARED_PIN, LOW);
		delayMicroseconds(PAUSE);
	}
}
