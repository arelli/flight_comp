#include <SparkFunMPU9250-DMP.h>
#include <Wire.h>
#define PI 3.1415926535897932384626433832795
#include <Servo.h>

MPU9250_DMP imu;
Servo servo;

void setup() {
  Serial.begin(115200);
  
  if (imu.begin() != INV_SUCCESS)
  {
    while (1)
    {
        // Failed to initialize MPU-9250, loop forever
    }
    imu.setSensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
    imu.setGyroFSR(2000); 
    imu.setAccelFSR(16);
  }

  servo.attach(11);

}

void loop() {
  imu.update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS);

  float accelX = imu.calcAccel(imu.ax); // accelX is x-axis acceleration in g's
  float accelY = imu.calcAccel(imu.ay); // accelY is y-axis acceleration in g's
  float accelZ = imu.calcAccel(imu.az); // accelZ is z-axis acceleration in g's
  
  float gyroX = imu.calcGyro(imu.gx); // gyroX is x-axis rotation in dps
  float gyroY = imu.calcGyro(imu.gy); // gyroY is y-axis rotation in dps
  float gyroZ = imu.calcGyro(imu.gz); // gyroZ is z-axis rotation in dps
  
  float magX = imu.calcMag(imu.mx); // magX is x-axis magnetic field in uT
  float magY = imu.calcMag(imu.my); // magY is y-axis magnetic field in uT
  float magZ = imu.calcMag(imu.mz); // magZ is z-axis magnetic field in uT

  Serial.println(accelX);
  //servo.write(90+90*accelX);     //Stabilization
  servo.write(90+3*gyroZ*PI/180);  //Anti-roll
  // put your main code here, to run repeatedly:

}
