// Code by: Jody McAdams
// Twitter: @MegaJiXiang
// Website: http://www.jodymcadams.com
// Original code/instructions from: http://bildr.org/2011/06/l3g4200d-arduino/
// License: http://www.opensource.org/licenses/mit-license.php (Go crazy)
// L3G4200D data sheet: http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/CD00265057.pdf

#include <Wire.h>
#include <MathUtil.h>

const int sensorRotationScaleDPS = 250; //(can be 250, 500, or 2000)

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

//Sensitivity from data sheet in mdps/digit (millidegrees per second) with DPS conversion
const float SoDR_250 = 8.75f/1000.0f;
const float SoDR_500 = 17.50f/1000.0f;
const float SoDR_2000 = 70.0f/1000.0f;

//Digital zero-rate level from data sheet in dps (degrees per second)
/*const float DVoff_250 = 10.0f;
const float DVoff_500 = 15.0f;
const float DVoff_2000 = 75.0f;*/

//Save the scale value that will be used when the program starts
float scaleFactor = 0.0f;

//Digital zero-rate values (calculated from calibration instead of data sheet)
float DVoff_X = 0.0f;
float DVoff_Y = 0.0f;
float DVoff_Z = 0.0f;

const float DVOffScale = 1.2f;

int L3G4200D_Address = 105; //I2C address of the L3G4200D

//actual sensor readings
float xVal = 0.0f;
float yVal = 0.0f;
float zVal = 0.0f;
float tVal = 0.0f;

float temperature = 0.0f;

//Temporarily made up numbers
//TODO: do some measurements to figure these numbers out
const float temperature_Min = 35.0f;
const float temperature_Max = 100.0f;

//last time in ms
float lastTimeMS;

//Program init
void setup() {
  Wire.begin();
  Serial.begin(9600);

  Serial.println("starting up L3G4200D...");
  setupL3G4200D(sensorRotationScaleDPS); // Configure L3G4200  - 250, 500 or 2000 deg/sec

  delay(1500); //wait for the sensor to be ready
  Serial.println("L3G4200D started!");
  
  Serial.println("Calibrating...");
  
  lastTimeMS = millis();
}

float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
int timeline = 0; 

float spamTimer = -1.0f;
float spamInterval = 0.125f; //seconds

float timeElapsed = 0.0f;

bool isCalibrating = true;
float calibrationTimer = 3.0f;

//Program loop
void loop() {
  const float currTimeMS = millis();
  timeElapsed = (currTimeMS-lastTimeMS)/1000.0f;
  lastTimeMS = currTimeMS;
  
  getGyroValues();  // This will update x, y, and z with new values
  
  if(isCalibrating == true) {
    const float absValX = abs(xVal);
    const float absValY = abs(yVal);
    const float absValZ = abs(zVal);
    
    if(absValX > DVoff_X) {
      DVoff_X = absValX;
    }
    
    if(absValY > DVoff_Y) {
      DVoff_Y = absValY;
    }
    
    if(absValZ > DVoff_Z) {
      DVoff_Z = absValZ;
    }
    
    calibrationTimer -= timeElapsed;
    
    if(calibrationTimer < 0.0f) {
      Serial.println("Calibration complete!");
      
      isCalibrating = false;
      
      //Some fudging to account for the slight
      //amount of movement that slips through
      DVoff_X *= DVOffScale;
      DVoff_Y *= DVOffScale;
      DVoff_Z *= DVOffScale;
    }
    
    return;
  }
  
  spamTimer -= timeElapsed;
  if(spamTimer < 0.0f) {
    spamTimer = spamInterval;
    
   if (angleX > 40 ) {
      Serial.print(1); 
    }
    
    else if (angleZ > 40 ) {
      Serial.print(4);
    }
    
    /*
    Serial.print("X:");
    Serial.print(angleX);
    Serial.print(", ");
 
    Serial.print("Y:");
    Serial.print(angleY);
    Serial.print(", ");

    Serial.print("Z:");
    Serial.print(angleZ);
    Serial.print(", ");
    
    Serial.print("Temp:");
    Serial.print(temperature);
    
    Serial.println(""); */
  }
}

void getGyroValues() {

  tVal = readRegister(L3G4200D_Address, 0x26);
  
  //Convert temp reading into a 0 to 1 float value
  float tempT = 1.0f - ((float)tVal+128.0f)/255.0f;
  
  //Get DPS values from registers
  byte xMSB = readRegister(L3G4200D_Address, 0x29);
  byte xLSB = readRegister(L3G4200D_Address, 0x28);
  xVal = scaleFactor * ((xMSB << 8) | xLSB);

  byte yMSB = readRegister(L3G4200D_Address, 0x2B);
  byte yLSB = readRegister(L3G4200D_Address, 0x2A);
  yVal = scaleFactor * ((yMSB << 8) | yLSB);

  byte zMSB = readRegister(L3G4200D_Address, 0x2D);
  byte zLSB = readRegister(L3G4200D_Address, 0x2C);
  zVal = scaleFactor * ((zMSB << 8) | zLSB);
  
  if(isCalibrating == false) {
      //If the DPS values are less than or equal to the Digital zero-rate levels from
      //the datasheet, set the values to 0
      if(abs(xVal) <= DVoff_X) {
        xVal = 0.0f;
      }
      
      if(abs(yVal) <= DVoff_Y) {
        yVal = 0.0f;
      }
      
      if(abs(zVal) <= DVoff_Z)  {
        zVal = 0.0f;
      }
      
      angleX += xVal*timeElapsed;
      angleY += yVal*timeElapsed;
      angleZ += zVal*timeElapsed;
      
      angleX = WrapAngle(angleX);
      angleY = WrapAngle(angleY);
      angleZ = WrapAngle(angleZ);
  }  
}

int setupL3G4200D(int scale){
  //From  Jim Lindblom of Sparkfun's code

  // Enable x, y, z and turn off power down:
  writeRegister(L3G4200D_Address, CTRL_REG1, 0b00001111);

  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  writeRegister(L3G4200D_Address, CTRL_REG2, 0b00000000);

  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  writeRegister(L3G4200D_Address, CTRL_REG3, 0b00001000);

  switch(scale){
    case 250: {
      writeRegister(L3G4200D_Address, CTRL_REG4, 0b00000000);
      scaleFactor = SoDR_250;
      //DVoff = DVoff_250;
      
      break;
    }
    case 500: {
      writeRegister(L3G4200D_Address, CTRL_REG4, 0b00010000);
      scaleFactor = SoDR_500;
      //DVoff = DVoff_500;
      
      break;
    }
    case 2000: {
      writeRegister(L3G4200D_Address, CTRL_REG4, 0b00110000);
      scaleFactor = SoDR_2000;
      //DVoff = DVoff_2000;
      
      break;
    }
  }  

  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister(L3G4200D_Address, CTRL_REG5, 0b00000000);
}

void writeRegister(int deviceAddress, byte address, byte val) {
    Wire.beginTransmission(deviceAddress); // start transmission to device 
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address){

    int v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1); // read a byte

    while(!Wire.available()) {
        // waiting
    }

    v = Wire.read();
    return v;
}


//If the angle goes beyond 360, wrap it around
float WrapAngle(float angle) {
  static const float maxAngle = 360.0f;
  
  float finalAngle;
  if(angle > maxAngle) {
    return angle - maxAngle;
  }
  else if(angle < -maxAngle) {
    return angle + maxAngle;
  } else {
    return angle;
  }
}
