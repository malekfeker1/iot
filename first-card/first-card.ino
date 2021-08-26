#include <TinyGPS++.h> // library for GPS module
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(4, 5); // The serial connection to the GPS device PINS of GPS


WiFiClient client;
WiFiServer server(80);





/******************************************  wifi steup  **************************************************/
const char* ssid = ""; //ssid of your wifi
const char* password = ""; //password of your wifi




/******************************************  define variables  **************************************************/

/* data received from application fo rmanuel driving */
String  data =""; 

int totalwidth;
int totallength;
//X = Longitude, Y = Latitude.
float longitudeS ; //Start point lat and long
float LatitudeS; 
float longitudeE; //End point lat and long ---- GET IT FROM DATABASE
float LatitudeE ;
float longitudeO; //OBSTACLE position lat and long
float LatitudeO ;
float longitudeT; //TRASH position lat and long
float LatitudeT;
int upsn;
int upsw;
int robotwidth;
int distance;
bool obstacle;
int checksN;
long duration;
float speed;
float avoiddelay;
bool trash;

/******************************************  define Pins  **************************************************/

// define ultrasonic pins for obstacle detection
const int ObstacleTrigP = 14;  //D5 Or GPIO-2 of nodemcu 
const int ObstacleEchoP = 12;  //D6 Or GPIO-0 of nodemcu



// define motor driver pins for path gen & manuel driving
/* motor control pins */
int leftMotorForward = 2;     /* GPIO2(D4) -> IN3   */
int rightMotorForward = 15;   /* GPIO15(D8) -> IN1  */
int leftMotorBackward = 0;    /* GPIO0(D3) -> IN4   */
int rightMotorBackward = 13;  /* GPIO13(D7) -> IN2  */
/*  enable=  speed pins */
int rightMotorENB = 14; /* GPIO14(D5) -> Motor-A Enable */
int leftMotorENB = 12;  /* GPIO12(D6) -> Motor-B Enable */


// define GPS pins 






/****************************************** void setup  **************************************************/
void setup() {
  
// SERIAL MONITOR CONNECTION
Serial.begin(9600);

/* initialize motor control pins as output */
  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT); 
  pinMode(leftMotorBackward, OUTPUT);  
  pinMode(rightMotorBackward, OUTPUT);

/* initialize motor enable pins as output */
  pinMode(leftMotorENB, OUTPUT); 
  pinMode(rightMotorENB, OUTPUT);

/* start server communication */
  server.begin();

//WIFI  SET UP AND CONNECTION
  ss.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //connecting to wifi
  while (WiFi.status() != WL_CONNECTED)// while wifi not connected
  {
    delay(500);
    Serial.print("."); //print "...."
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());  // Print the IP address
  // GPS CONNECTION && START POINT LOCATION
  if (gps.location.isValid() ) 
  {    
    float latitudeS = (gps.location.lat());     //Storing the Lat. and Lon. 
    float longitudeS = (gps.location.lng());
  }
  if (obstacle=true)
  {
    float latitudeO = (gps.location.lat());   //lat & lng of the obstacle 
    float longtitudeO = (gps.location.lng());
  }
  if (trash=true)
  {
    float LatitudeT = (gps.location.lat());   //lat & lng of the trash detected
    float longitudeT = (gps.location.lng());
  }
  // define ultrasonic pins for TRASH detection
 int TrashTrigP  ;  
 int TrashEchoP  ;  
// define right inductive sensors 
int rightIndcP;   
//define left inductive sensors
int leftIndcP;  
  //calculate the delay time needed to get past 4m
  speed= ((2pi*3,25)/60)*200;
  avoiddelay= 400,5/speed;
  //calculate widths
  totallength = floor (abs(LatitudeE-LatitudeS));
  totalwidth = floor (abs(LongitudeE-LongitudeS)); 
  upsn= floor(floor(totallength/robotwidth)*0.6);
  checksN = floor (totalwidth / 400);  //en cm

  upsw= floor(totallength/upsn);
  if (upsn%2 != 0 ){
    upsn= upsn-1;
    }

  
}


/****************************************** void loop  **************************************************/
void loop() {
analogWrite(leftMotorENB, 200); //ENA pin
analogWrite(rightMotorENB, 200); 

  if ( /*data de manuel driving*/ ) {
//manuel driving code

/* If the server available, run the "checkClient" function */  
    client = server.available();
    if (!client) return; 
    data = checkClient ();

/************************ Run function according to incoming data from application *************************/
    /* If the incoming data is "forward", run the "MotorForward" function */
    if (data == "forward") MotorForward();
    /* If the incoming data is "backward", run the "MotorBackward" function */
    else if (data == "backward") MotorBackward();
    /* If the incoming data is "left", run the "TurnLeft" function */
    else if (data == "left") TurnLeft();
    /* If the incoming data is "right", run the "TurnRight" function */
    else if (data == "right") TurnRight();
    /* If the incoming data is "stop", run the "MotorStop" function */
    else if (data == "stop") MotorStop();
}


    else {
  //gen path & obstacle avoiding
  for (int i = 0; i <upsn; i++){
    for ( int j=0 ; j <checksN ; j++ ) {
      float latitudeC = (gps.location.lat());     //Storing CURRENT Lat. and Lon. 
      float longitudeC = (gps.location.lng());
      checkObstacle();
      checktrash();

      if (obstacle == false || trash== false){
        MotorForward();
        delay(floor(405/speed)); // shoudl be a delay of 4m distance
        }
      else{
        float latitudeO = (gps.location.lat());     //Storing the Lat. and Lon. OF OBSTACLE
        float longitudeO = (gps.location.lng()); }
          if (upsn%2=0){
            //ykareb lin distance twali 20 then avoid
            aviodRight()
            MotorForward();
            delay(floor(405/speed)); //SHOULD BE A delay of 4m distance
            }
          else{
            avoidLeft() 
            MotorForward();
            delay(floor(405/speed)); // SHOULD BE A delay of 4m distance
            
            }
          }       
      } 
  if (upsn%2=0){
    MotorRight();
    delay(400);
    MotorForward();
    delay(floor(upsw/speed));
    MotorRight();
    delay(400);
    }
    else {
    MotorLeft();
    delay(400); MotorForward();
    delay(floor(upsw/speed));
    MotorRLeft();
    delay(400);
    }
    
    }
    }
 }

/******************************************  defineing the functions we have  **************************************************/

/****************************************** Moving Functions - Motor Functions  **************************************************/


/***** FORWARD *****/
void MotorForward(void)   
{
  digitalWrite(leftMotorENB,HIGH);
  digitalWrite(rightMotorENB,HIGH);
  digitalWrite(leftMotorForward,HIGH);
  digitalWrite(rightMotorForward,HIGH);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorBackward,LOW);
}

/***** BACKWARD *****/
void MotorBackward(void)   
{
  digitalWrite(leftMotorENB,HIGH);
  digitalWrite(rightMotorENB,HIGH);
  digitalWrite(leftMotorBackward,HIGH);
  digitalWrite(rightMotorBackward,HIGH);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,LOW);
}

/***** LEFT *****/
void TurnLeft(void)   
{
  digitalWrite(leftMotorENB,HIGH);
  digitalWrite(rightMotorENB,HIGH); 
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,HIGH);
  digitalWrite(rightMotorBackward,LOW);
  digitalWrite(leftMotorBackward,HIGH);  
}

/***** RIGHT *****/
void TurnRight(void)   
{
  digitalWrite(leftMotorENB,HIGH);
  digitalWrite(rightMotorENB,HIGH);
  digitalWrite(leftMotorForward,HIGH);
  digitalWrite(rightMotorForward,LOW);
  digitalWrite(rightMotorBackward,HIGH);
  digitalWrite(leftMotorBackward,LOW);
}

/***** STOP *****/
void MotorStop(void)   
{
  digitalWrite(leftMotorENB,LOW);
  digitalWrite(rightMotorENB,LOW);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorForward,LOW);
  digitalWrite(rightMotorBackward,LOW);
}


/****************************************** Avoiding Obstacles Functions **************************************************/


/***** Avoid to the Left *****/
void AvoidLeft (void)
{
  MotorStop();
  delay(100);
  TurnLeft();
  delay(400);
  MotorForward();
  delay(3000);
  TurnRight();
  delay(400);
  MotorForward();
  delay(3000);
  TurnRight();
  delay(400);
  MotorForward();
  delay(3000);
  TurnLeft();
  delay(400);
  MotorForward();
  delay (floor(totallength-(sqrt((longitudeO-longitudeS)**2-(latitudeO-latitudeS)**2));
  // distance totale - (distance obs+distance done) => 
  }

/***** Avoid to the Right *****/
void AvoidRight(void){

  MotorStop();
  delay(100);
  TurnRight();
  delay(400);
  MotorForward();
  delay(3000);
  TurnLeft();
  delay(400);
  MotorForward();
  delay(3000);
  TurnLeft();
  delay(400);
  MotorForward();
  delay(3000);
  TurnRight();
  delay(400);
  MotorForward();
  delay (floor(totallength-(sqrt((longitudeO-longitudeS)**2-(latitudeO-latitudeS)**2));
  // distance totale - (distance obs+distance done) => 
  }


/********************************** RECEIVE DATA FROM the APP ******************************************/
String checkClient (void)
{
  while(!client.available()) delay(1); 
  String request = client.readStringUntil('\r');
  request.remove(0, 5);
  request.remove(request.length()-9,9);
  return request;
}



/********************************** CHECK OBSTACLE  ******************************************/
bool checkObstacle(void) {
digitalWrite(ObstacleTrigP, LOW);   // Makes trigPin low
delayMicroseconds(2);       // 2 micro second delay 
digitalWrite(ObstacleTrigP, HIGH);  // tigPin high
delayMicroseconds(10);      // trigPin high for 10 micro seconds
digitalWrite(ObstacleTrigP, LOW);   // trigPin low
duration = pulseIn(echoP, HIGH);   //Read echo pin, time in microseconds
distance= duration*0.034/2;        //Calculating actual/real distance en cm

if (distance <400  ) //and ObstacleTrigP,LOW & TrashtrigP,HIGH
{
  obstacle = true;
  }
  return obstacle ;
}
/****************************DETECTING TRASH **********************************/

    void checkTrash(void) {
    digitalWrite(TrashTrigP, LOW);   // Makes trigPin low
    digitalWrite (rightIndcP, LOW);
    digitalWrite (leftIndcP, LOW);
    delayMicroseconds(2);       // 2 micro second delay 
    digitalWrite(TrashTrigP, HIGH);  // tigPin high
    digitalWrite (rightIndcP, HIGH);
    digitalWrite (leftIndcP, HIGH);
    delayMicroseconds(10);      // trigPin high for 10 micro seconds
    digitalWrite(TrashTrigP, LOW);   // trigPin low
    duration = pulseIn(TrashEchoP, HIGH);   //Read echo pin, time in microseconds
    indright=pulseIn (rightIndcP, HIGH);
    indleft=pulseIn (leftIndcP,HIGH);
    distanceTrash= duration*0.034/2;        //Calculating actual/real distance en cm

    if (distanceTrash <0.8)//and ObstacleTrigP LOW 
    {
     trash = true;
    }
    if (indleft==true)
    {
        trash=true;
    }
    if (indright==true)
    {
        trash=true;
    }
    return trash ;
    }
}
