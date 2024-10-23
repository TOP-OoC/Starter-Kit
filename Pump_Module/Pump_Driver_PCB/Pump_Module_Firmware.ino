#include <ArduinoBLE.h>
#include <FlashStorage.h>

//board name
const char board_name[] = "PumpModule_0003";

//permanent variables
FlashStorage(not_first_boot, bool);
FlashStorage(pump_1_freq, int);
FlashStorage(pump_2_freq, int);
FlashStorage(pump_3_freq, int);
FlashStorage(pump_4_freq, int);
FlashStorage(pump_5_freq, int);
FlashStorage(pump_6_freq, int);

//Pins
const int DIR_PIN_1 = 3;
const int STEP_PIN_1 = 2;
const int DIR_PIN_2 = 5;
const int STEP_PIN_2 = 4;
const int DIR_PIN_3 = 7;
const int STEP_PIN_3 = 6;
const int DIR_PIN_4 = 9;
const int STEP_PIN_4 = 8;
const int DIR_PIN_5 = 11;
const int STEP_PIN_5 = 10;
const int DIR_PIN_6 = 13;
const int STEP_PIN_6 = 12;

//intital values
int init_pump_freq = 0; //set to 0 KHz to start

//timers for each pump
int delay_time_1 = 0;    //microseconds time to delay
unsigned long last_time_1 = 0; // time of the next switch
int delay_time_2 = 0;
unsigned long last_time_2 = 0;
int delay_time_3 = 0;
unsigned long last_time_3 = 0;
int delay_time_4 = 0;
unsigned long last_time_4 = 0;
int delay_time_5 = 0;
unsigned long last_time_5 = 0;
int delay_time_6 = 0;
unsigned long last_time_6 = 0;

//Universal timer
unsigned long current_time = 0; //stores the last measured time (in uSeconds)

//Pump states
bool pump_1_step = LOW; //State of the first pump step pin
bool pump_2_step = LOW;
bool pump_3_step = LOW;
bool pump_4_step = LOW;
bool pump_5_step = LOW;
bool pump_6_step = LOW;

bool pump_1_active = LOW; //if the first pump is on
bool pump_2_active = LOW;
bool pump_3_active = LOW;
bool pump_4_active = LOW;
bool pump_5_active = LOW;
bool pump_6_active = LOW;

BLEService pumpService("180A"); // BLE pump Service

// BLE characteristics for controlling pump
BLEIntCharacteristic Pump1Characteristic("2A57", BLERead | BLEWrite);
BLEDescriptor Pump1CharacteristicDescriptor("2901", "Pump 1 Frequency (int | Hz)");
BLEIntCharacteristic Pump2Characteristic("2A57", BLERead | BLEWrite);
BLEDescriptor Pump2CharacteristicDescriptor("2901", "Pump 2 Frequency (int | Hz)");
BLEIntCharacteristic Pump3Characteristic("2A57", BLERead | BLEWrite);
BLEDescriptor Pump3CharacteristicDescriptor("2901", "Pump 3 Frequency (int | Hz)");
BLEIntCharacteristic Pump4Characteristic("2A57", BLERead | BLEWrite);
BLEDescriptor Pump4CharacteristicDescriptor("2901", "Pump 4 Frequency (int | Hz)");
BLEIntCharacteristic Pump5Characteristic("2A57", BLERead | BLEWrite);
BLEDescriptor Pump5CharacteristicDescriptor("2901", "Pump 5 Frequency (int | Hz)");
BLEIntCharacteristic Pump6Characteristic("2A57", BLERead | BLEWrite);
BLEDescriptor Pump6CharacteristicDescriptor("2901", "Pump 6 Frequency (int | Hz)");

void setup() {
  //Variable for reading the pump direction to later update the initail value of the BLE characteristics
  int pump_1_dir = -1;
  int pump_2_dir = -1;
  int pump_3_dir = -1;
  int pump_4_dir = -1;
  int pump_5_dir = -1;
  int pump_6_dir = -1;
  
  Serial.begin(9600);
  //while (!Serial);

  // setup output pins
  delayMicroseconds(1000);
  
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  digitalWrite(DIR_PIN_1, LOW);
  digitalWrite(STEP_PIN_1, LOW);
  pinMode(DIR_PIN_2, OUTPUT);
  pinMode(STEP_PIN_2, OUTPUT);
  digitalWrite(DIR_PIN_2, LOW);
  digitalWrite(STEP_PIN_2, LOW);
  pinMode(DIR_PIN_3, OUTPUT);
  pinMode(STEP_PIN_3, OUTPUT);
  digitalWrite(DIR_PIN_3, LOW);
  digitalWrite(STEP_PIN_3, LOW);
  pinMode(DIR_PIN_4, OUTPUT);
  pinMode(STEP_PIN_4, OUTPUT);
  digitalWrite(DIR_PIN_4, LOW);
  digitalWrite(STEP_PIN_4, LOW);
  pinMode(DIR_PIN_5, OUTPUT);
  pinMode(STEP_PIN_5, OUTPUT);
  digitalWrite(DIR_PIN_5, LOW);
  digitalWrite(STEP_PIN_5, LOW);
  pinMode(DIR_PIN_6, OUTPUT);
  pinMode(STEP_PIN_6, OUTPUT);
  digitalWrite(DIR_PIN_6, LOW);
  digitalWrite(STEP_PIN_6, LOW);

  //initialize flash on first boot (when new firmware is flashed all flash variables are set to 0)
  if (!not_first_boot.read()) {     
    not_first_boot.write(1);
    pump_1_freq.write(init_pump_freq);
    pump_2_freq.write(init_pump_freq);
    pump_3_freq.write(init_pump_freq);
    pump_4_freq.write(init_pump_freq);
    pump_5_freq.write(init_pump_freq);
    pump_6_freq.write(init_pump_freq);
  }

  //Setup state of all pumps based on flash memory
  //Pump 1
  int read_value = pump_1_freq.read(); //read the value from memory, then parse read_value                        
  if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
    pump_1_active = HIGH;
    digitalWrite(DIR_PIN_1, LOW);
    pump_1_dir = -1;
    delay_time_1 = (unsigned long)((float)1000000/(-(float)read_value));
  }
  else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
    pump_1_active = LOW;
    digitalWrite(DIR_PIN_1, HIGH);
    pump_1_dir = 1;
    delay_time_1 = (unsigned long)(1000000);
  }
  else{                       //otherwise set pump to active, dir pin to high, and delay time normally
    pump_1_active = HIGH;
    digitalWrite(DIR_PIN_1, HIGH);
    pump_1_dir = 1;
    delay_time_1 = (unsigned long)((float)1000000/(float)read_value);
  }
  
  //Pump2
  read_value = pump_2_freq.read(); //read the value from memory, then parse read_value                        
  if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
    pump_2_active = HIGH;
    digitalWrite(DIR_PIN_2, LOW);
    pump_2_dir = -1;
    delay_time_2 = (unsigned long)((float)1000000/(-(float)read_value));
  }
  else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
    pump_2_active = LOW;
    digitalWrite(DIR_PIN_2, HIGH);
    pump_2_dir = 1;
    delay_time_2 = (unsigned long)(1000000);
  }
  else{                       //otherwise set pump to active, dir pin to high, and delay time normally
    pump_2_active = HIGH;
    digitalWrite(DIR_PIN_2, HIGH);
    pump_2_dir = 1;
    delay_time_2 = (unsigned long)((float)1000000/(float)read_value);
  }

  //Pump3
  read_value = pump_3_freq.read(); //read the value from memory, then parse read_value                        
  if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
    pump_3_active = HIGH;
    digitalWrite(DIR_PIN_3, LOW);
    pump_3_dir = -1;
    delay_time_3 = (unsigned long)((float)1000000/(-(float)read_value));
  }
  else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
    pump_3_active = LOW;
    digitalWrite(DIR_PIN_3, HIGH);
    pump_3_dir = 1;
    delay_time_3 = (unsigned long)(1000000);
  }
  else{                       //otherwise set pump to active, dir pin to high, and delay time normally
    pump_3_active = HIGH;
    digitalWrite(DIR_PIN_3, HIGH);
    pump_3_dir = 1;
    delay_time_3 = (unsigned long)((float)1000000/(float)read_value);
  }

  //Pump4
  read_value = pump_4_freq.read(); //read the value from memory, then parse read_value                        
  if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
    pump_4_active = HIGH;
    digitalWrite(DIR_PIN_4, LOW);
    pump_4_dir = -1;
    delay_time_4 = (unsigned long)((float)1000000/(-(float)read_value));
  }
  else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
    pump_4_active = LOW;
    digitalWrite(DIR_PIN_4, HIGH);
    pump_4_dir = 1;
    delay_time_4 = (unsigned long)(1000000);
  }
  else{                       //otherwise set pump to active, dir pin to high, and delay time normally
    pump_4_active = HIGH;
    digitalWrite(DIR_PIN_4, HIGH);
    pump_4_dir = 1;
    delay_time_4 = (unsigned long)((float)1000000/(float)read_value);
  }

  //Pump5
  read_value = pump_5_freq.read(); //read the value from memory, then parse read_value                        
  if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
    pump_5_active = HIGH;
    digitalWrite(DIR_PIN_5, LOW);
    pump_5_dir = -1;
    delay_time_5 = (unsigned long)((float)1000000/(-(float)read_value));
  }
  else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
    pump_5_active = LOW;
    digitalWrite(DIR_PIN_5, HIGH);
    pump_5_dir = 1;
    delay_time_5 = (unsigned long)(1000000);
  }
  else{                       //otherwise set pump to active, dir pin to high, and delay time normally
    pump_5_active = HIGH;
    digitalWrite(DIR_PIN_5, HIGH);
    pump_5_dir = 1;
    delay_time_5 = (unsigned long)((float)1000000/(float)read_value);
  }

  //Pump6
  read_value = pump_6_freq.read(); //read the value from memory, then parse read_value                        
  if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
    pump_6_active = HIGH;
    digitalWrite(DIR_PIN_6, LOW);
    pump_6_dir = -1;
    delay_time_6 = (unsigned long)((float)1000000/(-(float)read_value));
  }
  else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
    pump_6_active = LOW;
    digitalWrite(DIR_PIN_6, HIGH);
    pump_6_dir = 1;
    delay_time_6 = (unsigned long)(1000000);
  }
  else{                       //otherwise set pump to active, dir pin to high, and delay time normally
    pump_6_active = HIGH;
    digitalWrite(DIR_PIN_6, HIGH);
    pump_6_dir = 1;
    delay_time_6 = (unsigned long)((float)1000000/(float)read_value);
  }

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName(board_name);
  BLE.setAdvertisedService(pumpService);

  //Add descriptors to characteristics
  Pump1Characteristic.addDescriptor(Pump1CharacteristicDescriptor);
  Pump2Characteristic.addDescriptor(Pump2CharacteristicDescriptor);
  Pump3Characteristic.addDescriptor(Pump3CharacteristicDescriptor);
  Pump4Characteristic.addDescriptor(Pump4CharacteristicDescriptor);
  Pump5Characteristic.addDescriptor(Pump5CharacteristicDescriptor);
  Pump6Characteristic.addDescriptor(Pump6CharacteristicDescriptor);
    
  // add the characteristic to the service
  pumpService.addCharacteristic(Pump1Characteristic);
  pumpService.addCharacteristic(Pump2Characteristic);
  pumpService.addCharacteristic(Pump3Characteristic);
  pumpService.addCharacteristic(Pump4Characteristic);
  pumpService.addCharacteristic(Pump5Characteristic);
  pumpService.addCharacteristic(Pump6Characteristic);

  // add service
  BLE.addService(pumpService);

  // set the initial value for the characteristic (note that this is based on the flash memory which has set delay_time):
  Pump1Characteristic.writeValue((int)(pump_1_dir*(float)1000000/(float)delay_time_1)*(int)pump_1_active);
  Pump2Characteristic.writeValue((int)(pump_2_dir*(float)1000000/(float)delay_time_2)*(int)pump_2_active);
  Pump3Characteristic.writeValue((int)(pump_3_dir*(float)1000000/(float)delay_time_3)*(int)pump_3_active);
  Pump4Characteristic.writeValue((int)(pump_4_dir*(float)1000000/(float)delay_time_4)*(int)pump_4_active);
  Pump5Characteristic.writeValue((int)(pump_5_dir*(float)1000000/(float)delay_time_5)*(int)pump_5_active);
  Pump6Characteristic.writeValue((int)(pump_6_dir*(float)1000000/(float)delay_time_6)*(int)pump_6_active);

  // start advertising
  BLE.advertise();

  //start timers
  current_time = micros();
  last_time_1 = current_time;
  last_time_2 = current_time;
  last_time_3 = current_time;
  last_time_4 = current_time;
  last_time_5 = current_time;
  last_time_6 = current_time;

  Serial.println("Pump Module Online");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    if (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to set control values for pumps:
      
      if (Pump1Characteristic.written()) {
        int read_value = Pump1Characteristic.value();
        pump_1_freq.write(read_value);
        Serial.println(String(read_value));

        //parse read_value
        if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
          pump_1_active = HIGH;
          digitalWrite(DIR_PIN_1, LOW);
          delay_time_1 = (unsigned long)((float)1000000/(-(float)read_value));
        }
        else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
          pump_1_active = LOW;
          digitalWrite(DIR_PIN_1, HIGH);
          delay_time_1 = (unsigned long)(1000000);
        }
        else{                       //otherwise set pump to active, dir pin to high, and delay time normally
          pump_1_active = HIGH;
          digitalWrite(DIR_PIN_1, HIGH);
          delay_time_1 = (unsigned long)((float)1000000/(float)read_value);
        }

        Serial.println(String(delay_time_1));
        last_time_1 = micros(); //puts next step time into the future
        Serial.println("value updated!");
      }
      
      if (Pump2Characteristic.written()) {
        int read_value = Pump2Characteristic.value();
        pump_2_freq.write(read_value);

        //parse read_value
        if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
          pump_2_active = HIGH;
          digitalWrite(DIR_PIN_2, LOW);
          delay_time_2 = (unsigned long)((float)1000000/(-(float)read_value));
        }
        else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
          pump_2_active = LOW;
          digitalWrite(DIR_PIN_2, HIGH);
          delay_time_2 = (unsigned long)(1000000);
        }
        else{                       //otherwise set pump to active, dir pin to high, and delay time normally
          pump_2_active = HIGH;
          digitalWrite(DIR_PIN_2, HIGH);
          delay_time_2 = (unsigned long)((float)1000000/(float)read_value);
        }
        
        last_time_2 = micros(); //puts next step time into the future
        Serial.println("value updated!");
      }

      if (Pump3Characteristic.written()) {
        int read_value = Pump3Characteristic.value();
        pump_3_freq.write(read_value);

        //parse read_value
        if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
          pump_3_active = HIGH;
          digitalWrite(DIR_PIN_3, LOW);
          delay_time_3 = (unsigned long)((float)1000000/(-(float)read_value));
        }
        else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
          pump_3_active = LOW;
          digitalWrite(DIR_PIN_3, HIGH);
          delay_time_3 = (unsigned long)(1000000);
        }
        else{                       //otherwise set pump to active, dir pin to high, and delay time normally
          pump_3_active = HIGH;
          digitalWrite(DIR_PIN_3, HIGH);
          delay_time_3 = (unsigned long)((float)1000000/(float)read_value);
        }
        
        last_time_3 = micros(); //puts next step time into the future
        Serial.println("value updated!");
      }

      if (Pump4Characteristic.written()) {
        int read_value = Pump4Characteristic.value();
        pump_4_freq.write(read_value);

        //parse read_value
        if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
          pump_4_active = HIGH;
          digitalWrite(DIR_PIN_4, LOW);
          delay_time_4 = (unsigned long)((float)1000000/(-(float)read_value));
        }
        else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
          pump_4_active = LOW;
          digitalWrite(DIR_PIN_4, HIGH);
          delay_time_4 = (unsigned long)(1000000);
        }
        else{                       //otherwise set pump to active, dir pin to high, and delay time normally
          pump_4_active = HIGH;
          digitalWrite(DIR_PIN_4, HIGH);
          delay_time_4 = (unsigned long)((float)1000000/(float)read_value);
        }
        
        last_time_4 = micros(); //puts next step time into the future
        Serial.println("value updated!");
      }

      if (Pump5Characteristic.written()) {
        int read_value = Pump5Characteristic.value();
        pump_5_freq.write(read_value);

        //parse read_value
        if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
          pump_5_active = HIGH;
          digitalWrite(DIR_PIN_5, LOW);
          delay_time_5 = (unsigned long)((float)1000000/(-(float)read_value));
        }
        else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
          pump_5_active = LOW;
          digitalWrite(DIR_PIN_5, HIGH);
          delay_time_5 = (unsigned long)(1000000);
        }
        else{                       //otherwise set pump to active, dir pin to high, and delay time normally
          pump_5_active = HIGH;
          digitalWrite(DIR_PIN_5, HIGH);
          delay_time_5 = (unsigned long)((float)1000000/(float)read_value);
        }
        
        last_time_5 = micros(); //puts next step time into the future
        Serial.println("value updated!");
      }
      
      if (Pump6Characteristic.written()) {
        int read_value = Pump6Characteristic.value();
        pump_6_freq.write(read_value);

        //parse read_value
        if (read_value < 0){     //set pump to active, flip sign of read value, and set DIR pin to low
          pump_6_active = HIGH;
          digitalWrite(DIR_PIN_6, LOW);
          delay_time_6 = (unsigned long)((float)1000000/(-(float)read_value));
        }
        else if (read_value == 0) {  //set dir pin high, pump to inactive and speed to 1Hz(speed will be ignored later)
          pump_6_active = LOW;
          digitalWrite(DIR_PIN_6, HIGH);
          delay_time_6 = (unsigned long)(1000000);
        }
        else{                       //otherwise set pump to active, dir pin to high, and delay time normally
          pump_6_active = HIGH;
          digitalWrite(DIR_PIN_6, HIGH);
          delay_time_6 = (unsigned long)((float)1000000/(float)read_value);
        }
        
        last_time_6 = micros(); //puts next step time into the future
        Serial.println("value updated!");
        
      }
    }    
  }

     
  //Checks timers and updates outputs accordingly
  current_time = micros();
  
  if (pump_1_active) {
    if (current_time - last_time_1 >= delay_time_1){
      last_time_1 = current_time;
      pump_1_step = !pump_1_step;               
      digitalWrite(STEP_PIN_1, pump_1_step); 
    }
  }
  if (pump_2_active) {
    if (current_time - last_time_2 >= delay_time_2){
      last_time_2 = current_time;
      pump_2_step = !pump_2_step;              
      digitalWrite(STEP_PIN_2, pump_2_step); 
    }
  }
  if (pump_3_active) {
    if (current_time - last_time_3 >= delay_time_3){
      last_time_3 = current_time;
      pump_3_step = !pump_3_step;              
      digitalWrite(STEP_PIN_3, pump_3_step); 
    }
  }
  if (pump_4_active) {
    if (current_time - last_time_4 >= delay_time_4){
      last_time_4 = current_time;
      pump_4_step = !pump_4_step;               
      digitalWrite(STEP_PIN_4, pump_4_step); 
    }
  }
  if (pump_5_active) {
    if (current_time - last_time_5 >= delay_time_5){
      last_time_5 = current_time;
      pump_5_step = !pump_5_step;               
      digitalWrite(STEP_PIN_5, pump_5_step); 
    }
  }
  if (pump_6_active) {
    if (current_time - last_time_6 >= delay_time_6){
      last_time_6 = current_time;
      pump_6_step = !pump_6_step;               
      digitalWrite(STEP_PIN_6, pump_6_step); 
    }
  }
}
