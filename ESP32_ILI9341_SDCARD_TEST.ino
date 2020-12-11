// Do not remove the include below
#include "ESP32_ILI9341_SDCARD_TEST.h"

#include "Arduino.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"
#include "tft.h"
#include <LITTLEFS.h>

#define TFT_CS        22    // do not use GPI032 or GPIO33 here
#define TFT_DC        21    // do not use GPI032 or GPIO33 here

#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18

#define TP_IRQ        39
#define TP_CS         16

#define SD_CS          5

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);

TFT tft(0);   //0=ILI9341, 1= HX8347D

void setup() {
	Serial.begin(115200);

	pinMode(TP_CS, OUTPUT);
	digitalWrite(TP_CS, HIGH); // disable the touchpad

	Serial.println("SPI begin");
	SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

	Serial.println("TFT begin");
	tft.begin(TFT_CS, TFT_DC, SPI_MOSI, SPI_MISO, SPI_SCK);

	//Serial.println("SD begin");
	//SD.begin(SD_CS, SPI, 8000000);
	//SD.begin(SD_CS,SPI,16000000); // faster speed

	if (!LITTLEFS.begin(false))
	{
		Serial.println("LITTLEFS Mount Failed.");
		return;
	} else
	{
		Serial.println("LITTLEFS Mount SUCCESSFUL.");
	}

	// List directory, from root, 2 levels deep
	Serial.println("\n================================");
	listDir(LITTLEFS, "/", 2);

	tft.setRotation(0);
	tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 0);
	tft.setTextColor(TFT_WHITE);
	tft.setTextSize(2);
}

//-------------------------------------------------------------------------------------
void loop(void) {
	Serial.println("Birds.gif");
	tft.setRotation(0); //portait
	tft.drawGifFile(LITTLEFS, "/birds.gif", 0, 0, 1);
	delay(2000);

	Serial.println("Clock.gif");
	tft.drawGifFile(LITTLEFS, "/clock.gif", 0, 0, 3);
	delay(2000);

	tft.setRotation(3); //landscape
	Serial.print("Radar.gif");
	tft.drawGifFile(LITTLEFS, "/Radar.gif", 0, 0, 3);
	delay(2000);

	Serial.println("Tiger2.bmp");
	tft.drawBmpFile(LITTLEFS, "/Tiger2.bmp", 0, 0);
	delay(2000);

	Serial.println("Tiger2.jpg");
	tft.drawJpgFile(LITTLEFS, "/Tiger2.jpg", 0, 0, 320, 240);
	delay(2000);
}

void tft_info(const char * message) {
	Serial.println(message);
}

// Stolen from the example sketch on how to list files in a dir
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
	Serial.printf("Listing directory: %s\r\n", dirname);

	File root = fs.open(dirname);
	if (!root)
	{
		Serial.println("- failed to open directory");
		return;
	}
	if (!root.isDirectory())
	{
		Serial.println(" - not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file)
	{
		if (file.isDirectory())
		{
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels)
			{
				listDir(fs, file.name(), levels - 1);
			}
		} else
		{
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("\tSIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}
