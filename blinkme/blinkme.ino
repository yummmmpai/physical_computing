int led = 13; 
int timeBetween = 100;
int buttonPin = 2;

boolean locked = false; 
int buttonState = 0; 

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);             //use led as an OUTPUT pin
  pinMode(buttonPin, INPUT); 
}

void loop() {
  buttonState = digitalRead(buttonPin); 
  
  if (buttonState == HIGH && locked == false) {
    buttonPress();
    locked = true;
  } else if (buttonState == HIGH && locked == true){
    buttonPress();
    delay(100);
    buttonPress();
    locked = false; 
  }
  
}

void buttonPress(){
    digitalWrite(led, HIGH);
    delay(timeBetween);
    digitalWrite(led, LOW);
}
