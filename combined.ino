#include <Wire.h>

#define humidity_lsb 0xfe
#define humidity_msb 0xFD

// for the bmp 180
#define temperature_xlsb 0xFC  // the xlsb is used only for extra accuracy(and not needed in our case)
#define temperature_lsb 0xFB
#define temperature_msb 0xFA

#define pressure_xlsb 0xF9  // the xlsb is used only for extra accuracy as above 
#define pressure_lsb 0xF8
#define pressure_msb 0xf7

#define config_register 0xF5
#define ctrl_meas 0xf4
#define status_register 0xf3
#define ctrl_humidity 0xf2

#define reset_register 0xe0
#define chip_id 0xD0

// the hardcoded i2c address of the sensor(found with arduino 12c scanner sketch)
int BME280adress = 0x76;


#include <TinyGPS++.h> // Include the TinyGPS++ library
TinyGPSPlus tinyGPS; // Create a TinyGPSPlus object




int gps_address = 0x42;

void setup()
{
  Serial.begin(9600);
  Wire.begin();        //join the i2c bus as a master
  set_register(BME280adress,reset_register,B10110110);   //   B10110110 is the required sequence to reset the device
  set_register(BME280adress,ctrl_humidity ,B001);  
}

// for the altimeter
int temp1,temp2;
int pres1,pres2;
int hum1, hum2;

void loop()
{
Wire.beginTransmission(gps_address);
Wire.requestFrom(gps_address, 32);

 while (Wire.available()>0){
   int c= Wire.read();
   tinyGPS.encode(c);
    }
Wire.endTransmission();
 printGPSInfo();

 //delay(1000);
 // for the altimeter
 set_register(BME280adress,ctrl_meas, B00100101);  // do one forced measurement cycle, the only right way to address specific registers
  delay(50);  // wait for all the measurements to be done
  
  //initiate a transmission and  ask for the data from the following registers
  read_register(BME280adress,ctrl_meas, "control measuremetn register", "BIN");
  read_register(BME280adress,status_register, "status_register", "BIN");
  
  temp1 = read_register(BME280adress,temperature_msb, "temperature_msb", "BIN");
  temp2 = read_register(BME280adress,temperature_lsb, "temperature_lsb", "BIN");
  unsigned int temperature = temp1 * 256 + temp2;
  float temperature_float = float(temperature)/1000;
  Serial.print("Temp.:");
  Serial.print(temperature_float,4);  //print only 4 decimals
  Serial.print("Â°C, ");

  hum1 = read_register(BME280adress,humidity_msb, "humidity_msb", "BIN");
  hum2 = read_register(BME280adress,humidity_lsb, "humidity_lsb", "BIN");
  unsigned int humidity = hum1 * 256 + hum2;  // humidity is in Q10.22 format -> divide by 1024
  Serial.print("Hum.: ");
  Serial.print(humidity, DEC);
  Serial.print("%, ");

  pres1 = read_register(BME280adress,pressure_msb, "pressure_lsb", "BIN");
  pres2 = read_register(BME280adress,pressure_lsb, "pressure_lsb", "BIN");
  float pressure = float(pres1 + float(pres2)/256);  // is represented in Q24.8 format  -> divide by 256
  Serial.print("Pres.: ");
  Serial.print(pressure, 4);
  Serial.print(" Pa");
 
  delay(500);

  // clear the screen in terminals like putty
  Serial.print("\n");
  Serial.write(12);  // new page, see ASCII
  Serial.print("\n");

}


int read_register(int ic_address,int reg_address, String reg_name, String readmode){
  Wire.beginTransmission(ic_address);
  //Wire.write(chip_id);
  Wire.write(reg_address);
  Wire.endTransmission();
  int returned_value;
  
  Wire.requestFrom(ic_address,byte(1));
  if( Wire.available() == 1){  // check if there are 2 available bytes
//    Serial.print(reg_name);
//    Serial.print(" = ");
    returned_value = Wire.read();
//    if (readmode=="BIN")
//      Serial.print(returned_value, BIN);
//    else if (readmode=="DEC")
//      Serial.print(returned_value, DEC);
//    else if (readmode=="HEX")
//       Serial.print(returned_value, HEX);
  }
  else
    Serial.println( "requestFrom failed");

  //Serial.print("\n");
  return returned_value;
  }

  
void set_register(int ic_address, int reg_address, byte bytes_to_write){
  Wire.beginTransmission(ic_address);
  Wire.write(reg_address);
  Wire.write(bytes_to_write); 
  Wire.endTransmission();
  }



void printGPSInfo()
{
  Serial.print("Lat: "); Serial.println(tinyGPS.location.lat(), 6);
  Serial.print("Long: "); Serial.println(tinyGPS.location.lng(), 6);
  Serial.print("Alt(m): "); Serial.println(tinyGPS.altitude.meters());
  Serial.print("Course(degrees): "); Serial.println(tinyGPS.course.deg());
  Serial.print("Speed(km/h): "); Serial.println(tinyGPS.speed.kmph());
  Serial.print("Date: "); printDate();
  Serial.print("Time: "); printTime();
  Serial.print("Sats visible: "); Serial.println(tinyGPS.satellites.value());
  Serial.println();
}




void printDate()
{
  Serial.print(tinyGPS.date.day());
  Serial.print("/");
  Serial.print(tinyGPS.date.month());
  Serial.print("/");
  Serial.println(tinyGPS.date.year());
}
void printTime()
{
  Serial.print(tinyGPS.time.hour());
  Serial.print(":");
  if (tinyGPS.time.minute() < 10) Serial.print('0');
  Serial.print(tinyGPS.time.minute());
  Serial.print(":");
  if (tinyGPS.time.second() < 10) Serial.print('0');
  Serial.println(tinyGPS.time.second());
}
