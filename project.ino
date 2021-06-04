// This #include statement was automatically added by the Particle IDE.
#include <HttpClient.h>
#include <SparkFunMMA8452Q.h>

#define START 0
#define WINDUP 1
#define DONE 2
#define WINDDOWN 3
#define OFF 4

#define TIMER 30000

#define BUZZER D2
#define TOUCH D6

//Accelerometer
MMA8452Q accel;

//HTTP Stuff
HttpClient http;
http_header_t headers[] = {
    //  { "Content-Type", "application/json" },
    //  { "Accept" , "application/json" },
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headerswill NULL
    };
http_request_t request;
http_response_t response;

unsigned short state = OFF;
unsigned short reps = 0;
int startTime;

void setup() {
    //Initalize Variables
    Serial.begin(9600);
    //BUZZER
    pinMode(BUZZER, OUTPUT);
    //TOUCH
    pinMode(TOUCH, INPUT);
    //GYRO
    accel.begin();
    //Initalize Account
    request.hostname = "18.212.246.152";
    request.port = 5000;
    request.path = String::format("/?id=%d&a=%d&b=%d&c=%d&d=%d", 1234, 0, 0, 0, 0);
    http.get(request, response, headers);
}

void loop() {
    int touchValue = digitalRead(TOUCH);
    
    accel.read();
    char buffer[17];
    sprintf(buffer, "reps: %d", reps);
    Serial.println(buffer);
    
    //Start the Timer
    if(state == OFF && digitalRead(TOUCH))
    {
        state = START;
        digitalWrite(BUZZER,HIGH);
        delay(100);
        digitalWrite(BUZZER,LOW);
        delay(50);
        digitalWrite(BUZZER,HIGH);
        delay(300);
        digitalWrite(BUZZER,LOW);
        startTime = millis();
        reps = 0;
    }
    
    //Finish Game when time is up
    if(state != OFF && millis() - startTime >= TIMER)
    {
        state = OFF;
        request.path = String::format("/?id=%d&a=%d&b=%d&c=%d&d=%d", 1234, reps, 0, 0, 0);
        http.get(request, response, headers);
        digitalWrite(BUZZER,HIGH);
        delay(100);
        digitalWrite(BUZZER,LOW);
        delay(50);
        digitalWrite(BUZZER,HIGH);
        delay(300);
        digitalWrite(BUZZER,LOW);
    }
    
    //Initial Position
    if(accel.x < 0 && state == START)
    {
        state = WINDUP;
    }
    
    //Spread Legs Out
    if(accel.x > 990 && state == WINDUP)
    {
        reps++;
        digitalWrite(BUZZER,HIGH);
        delay(50);
        digitalWrite(BUZZER,LOW);
        state = DONE;
    }
    
    //Spread Position
    if(accel.x < 0 && state == DONE)
    {
        state = WINDDOWN;
    }
    
    //Spread Legs In
    if(accel.x > 990 && state == WINDDOWN)
    {
        state = START;
    }
}