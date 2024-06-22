#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   26  
#define SS_PIN    5 
#define SCK_PIN   18 
#define MOSI_PIN  23 
#define MISO_PIN  19 

#define RX 32
#define TX 33

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
void setup() {
	Serial.begin(115200);       // Initialize serial communications with the PC
	while (!Serial);        // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);
	mfrc522.PCD_Init();     // Init MFRC522
	delay(4);               // Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
	//Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
	Serial1.begin(115200, SERIAL_8N1, RX, TX);
}

void loop() {
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}
	//auto piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
	//Serial.println(mfrc522.PICC_GetTypeName(piccType));

	{
		MFRC522::StatusCode status;
		byte byteCount;
		byte buffer[18];
		byte buf[132*4];
		uint16_t i;
		byteCount = sizeof(buffer);
		
		for (uint16_t page = 0; page < 132; page +=4) { 
			status = mfrc522.MIFARE_Read(page, buffer, &byteCount);
			if (status != MFRC522::STATUS_OK) {
				Serial.print(F("MIFARE_Read() failed: "));
				Serial.println(mfrc522.GetStatusCodeName(status));
				return;
			}
			for(uint16_t j = 0; j < 16; j++){
				buf[page*4+j] = buffer[j];
			}
		}
		

		String str = "";

		for (uint16_t c = 25; c< 528; c++) {
			if(buf[c] == 0xFE)
				break;
			str += (char)buf[c];
		}

		Serial.println(str);
		Serial1.println(str);

		mfrc522.PICC_HaltA();
		delay(500);
	}
}
