#define SHIFT_DATA 10
#define SHIFT_CLK 12
#define SHIFT_LATCH 11
#define EEPROM_D0 2
#define EEPROM_D7 9
#define WRITE_EN 13

void setAddress(int address, bool outputEnable){
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);
  
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

byte readEEPROM(int address){
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1){
    pinMode(pin, INPUT);
  }
  setAddress(address, true);
  
  byte data = 0;
  for(int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1){
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

void writeEEPROM(int address, byte data){
  setAddress(address, false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1){
    pinMode(pin, OUTPUT);
  }
  
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1){
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }

  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}

void printContents(){
for (int base = 0; base <= 255; base += 16){
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1){
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x: %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
      base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
      data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
    
    Serial.println(buf);
  }
}

// 4-bit hex decoder for common anode 7-segment display
// byte data[] = { 0x81, 0xcf, 0x92, 0x86, 0xcc, 0xa4, 0xa0, 0x8f, 0x80, 0x84, 0x88, 0xe0, 0xb1, 0xc2, 0xb0, 0xb8 };

// 4-bit hex decoder for common cathode 7-segment display
byte data[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x4e, 0x3d, 0x4f, 0x47 };

void setup(){
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);

  // Erase EEPROM
  for (int address = 0; address <= 2047; address += 1){
    writeEEPROM(address, 0xff);
  }

  // Program 16 bytes
  for (int address = 0; address <= 15; address += 1){
    writeEEPROM(address, data[address]);
    delay(10);
  }
  printContents();
}

void loop(){
  
}
