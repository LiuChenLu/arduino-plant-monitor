/*
  SD card read/write

  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  Forked from Arduino ReadWrite example

*/

#include <SPI.h>
#include <SD.h>

const String FILE_NAME = "DATA.txt";

File myFile;

void readFile()
{
  // re-open the file for reading:
  myFile = SD.open(FILE_NAME);
  if (myFile) {
    Serial.println(FILE_NAME + ':');

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void deleteFile()
{
  if (SD.exists(FILE_NAME)) {
    Serial.println(FILE_NAME + " exists.");
  } else {
    Serial.println(FILE_NAME + " doesn't exist.");
    return;
  }
  SD.remove(FILE_NAME);
  
  if (SD.exists(FILE_NAME)) {
    Serial.println("ERROR! failed to delete " + FILE_NAME);
  } else {
    Serial.println(FILE_NAME + " deleted.");
  }
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  deleteFile();
}

void loop() {
  // nothing happens after setup
}
