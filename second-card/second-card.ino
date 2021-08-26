int ATBuzzerPin = 5; //D1 buzzer pin
const int ATtrigP = 14;  //D5 Or GPIO-2 of nodemcu
const int ATechoP = 12;  //D6 Or GPIO-0 of nodemcu
long ATduration;
int ATdistance;


void setup() {
pinMode(ATtrigP, OUTPUT);  
pinMode(ATechoP, INPUT);   
pinMode(ATBuzzerPin, OUTPUT);

//Set serial monitor
Serial.begin(9600);      
}

void loop() {
digitalWrite ( ATBuzzerPin, LOW);   
digitalWrite(ATtrigP, LOW);   
delayMicroseconds(2);       // 2 micro second delay 
digitalWrite(ATtrigP, HIGH); 
delayMicroseconds(10);      // trigPin high for 10 micro seconds
digitalWrite(ATtrigP, LOW); 
ATduration = pulseIn(ATechoP, HIGH);   //Read echo pin, time in microseconds
ATdistance= duration*0.034/2;        //Calculating actual/real distance en cm

Serial.println(ATdistance);

   if (ATdistance > 50)
   {
      digitalWrite ( ATBuzzerPin, HIGH);
      delay(1000);
      digitalWrite ( ATBuzzerPin, LOW);
   }                    
}