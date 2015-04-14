//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 15;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn; 
long unsigned int pause = 250;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 3;    //the digital pin connected to the PIR sensor's output
int pirPin2 = 4; 
int ledPin1 = 7;
int ledPin12 = 8; 
int ledPin21 = 9;
int ledPin22 = 10;
int buzzerPin = 11;
boolean guestActive = false; 


/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(pirPin2, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin12, OUTPUT);
  pinMode(ledPin21, OUTPUT);
  pinMode(ledPin22, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(pirPin, LOW);
  digitalWrite(pirPin2, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);
  }

////////////////////////////
//LOOP
void loop(){
  
     if(digitalRead(pirPin) == HIGH){
       guestHigh();  
       guestActive = true;
     }
       
     if(digitalRead(pirPin2) == HIGH){
        homeHigh();
        homeActive = true; 
     }
'
     if (guestActive) {
       if(digitalRead(pirPin) == LOW){       
         guestLow();  
         guestActive = false; 
       }
     }
     
     if (homeActive) {
       if(digitalRead(pirPin2) == LOW){       
         homeLow();
         homeActive = false; 
       }
     }
  }
  
void homeHigh(){
   digitalWrite(ledPin21, HIGH);
   digitalWrite(ledPin22, HIGH);   //the led visualizes the sensors output pin state
   analogWrite(buzzerPin, 250);
       
   if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("Home motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         delay(50);
     }         
     takeLowTime = true;
}

void homeLow() {
  digitalWrite(ledPin21, LOW);
  digitalWrite(ledPin22, LOW);  //the led visualizes the sensors output pin state
  analogWrite(buzzerPin, LOW);

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("Home motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           delay(50);
           }
}

void guestHigh() {
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin12, HIGH);   //the led visualizes the sensors output pin state
  analogWrite(buzzerPin, 250);
  
  if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("Guest motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         delay(50);
         }         
         takeLowTime = true;
}

void guestLow() {
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin12, LOW);  //the led visualizes the sensors output pin state
  analogWrite(buzzerPin, LOW);

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("Guest motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           delay(50);
           } 
}
