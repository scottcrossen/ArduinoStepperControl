#include <Stepper.h>
#define motorSteps 513
#define motorPin1 6
#define motorPin2 5
#define motorPin3 4
#define motorPin4 7
Stepper myStepper(motorSteps,motorPin1,motorPin3,motorPin2,motorPin4);

int switchState = LOW; //Current value of input
int pastSwitchState = LOW; //value of input last loop
const int triggerIn = 13; //Pin to read input from DAQ-trigger signal
const int resetIn = 12;
int stepNum = 0; //number of steps motors move
int resetState = LOW;
int pastResetState = LOW;

void setup() {
 myStepper.setSpeed(20);
 pinMode(triggerIn,INPUT);
 pinMode(resetIn,INPUT);
 Serial.begin(9600);
 

}

void loop() {
switchState = digitalRead(triggerIn);

  if(switchState != pastSwitchState)
  {
    if(switchState == HIGH)
    {//only triggers when transition of LOW->HIGH
      myStepper.step(1);
      stepNum++;
      Serial.println("on");
      Serial.print("number of steps: ");
      Serial.println(stepNum);
    }
    else
    {//HIGH->LOW
     //HIGH->HIGH
     //LOW->LOW --- No trigger
     Serial.println("off");
    }
    delay(50); //to keep from unwanted bouncing
    } 
   pastSwitchState = switchState;


//Put in reset code
//if another pin == high 
//step backwarsd stepnum
resetState = digitalRead(resetIn);

  if(resetState != pastResetState)
  {
    if(resetState == HIGH)
    {//only triggers when transition of LOW->HIGH
      myStepper.step(-1);
      stepNum--;
      Serial.println("on");
      Serial.print("number of steps: ");
      Serial.println(stepNum);
    }
    else
    {//HIGH->LOW
     //HIGH->HIGH
     //LOW->LOW --- No trigger
     Serial.println("off");
    }
    delay(50); //to keep from unwanted bouncing
  } 
   pastResetState = resetState;
}
