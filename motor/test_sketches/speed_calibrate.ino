#include <EEPROM.h>


//******Global settings********
const byte MAX_SPEED_PWM = 180; //All calculated speeds will be measured relative to 0 => MAX_SPEED_PWM
int STEPS = 256; //Total amount of different speeds that need to be calculated
const int DELAY = 3000; //Time in ms between measurements. Total experiment time == DELAY * STEPS
float ACCURACY = 4;

//******Global variables********
int current_addr = 0; //EEPROM adress to write to
int current_pwm = MAX_SPEED_PWM; //PWM value written to engine

int max_value_sens = 0; //Value of speed sensor at speed MAX_SPEED_PWM
float step_size = 0; //max_value_sens / STEPS


 //******Pin allocation********

 //Speed sensor
 const byte PIN_SPEED = A3; //
 
 //Motor
 const byte PIN_MOTOR_A = 9; //Backwad engine
 const byte PIN_MOTOR_V = 5; //Forward engine

//Unused Pins, will be flagged as INPUT
 const byte UNUSED_PINS[] = {A0, A1, A2, A4, A5, 0, 1, 2, 3, 4, 6, 7, 8, 10, 11, 12, 13};
 const byte AMOUNT_UNUSED_PINS = 11; //needed to loop through the above array

void reach_speed(int speed_to_reach, int current_speed_sens){

  if ((current_speed_sens > speed_to_reach) && (current_pwm > 0)){
    current_pwm -=1;
  }

  else if ((current_speed_sens < speed_to_reach) && (current_pwm < 255)){
    current_pwm +=1;
  }
  digitalWrite(PIN_MOTOR_A, LOW);
  analogWrite(PIN_MOTOR_V, current_pwm);
}




void setup() {
  
  pinMode(PIN_SPEED, INPUT);

  pinMode(PIN_MOTOR_V, OUTPUT);
  pinMode(PIN_MOTOR_A, OUTPUT);

  digitalWrite(PIN_MOTOR_V, LOW); //make sure engines are off
  digitalWrite(PIN_MOTOR_A, LOW);

  for (byte i = 0; i < AMOUNT_UNUSED_PINS; i++) { //flag unused pins as input
    pinMode(UNUSED_PINS[i], INPUT);
  }
  
  delay(2000); //boot time
  
  digitalWrite(PIN_MOTOR_A, LOW); //start engines
  analogWrite(PIN_MOTOR_V, MAX_SPEED_PWM);
  
  delay(7000); //wait untill max speed is reached
  
  max_value_sens = analogRead(PIN_SPEED); //Get max speed
  delay(100);

  step_size = (float) max_value_sens / (float) STEPS; //Calculate step size

  for(int current_step = (STEPS - 1); current_step >= 0; current_step--){
    int current_speed_size = round(step_size * current_step);    
    int current_speed_sens = analogRead(PIN_SPEED);
    
    int current_speed_size_rounded = round((float) current_speed_size / ACCURACY);
    int current_speed_sens_rounded = round((float) current_speed_sens / ACCURACY);

    while(current_speed_size_rounded != current_speed_sens_rounded){
      
      reach_speed(current_speed_size, current_speed_sens);
      
      current_speed_sens = analogRead(PIN_SPEED);
      current_speed_sens_rounded = round((float) current_speed_sens / ACCURACY);
      
      delay(350);
    }

    EEPROM.write(current_addr, current_step);
    current_addr += 1;
    EEPROM.write(current_addr, current_speed_size / 4);
    current_addr += 1;

    }
    
}

void loop() {
  digitalWrite(PIN_MOTOR_V, LOW);
  analogWrite(PIN_MOTOR_A, MAX_SPEED_PWM); //drive backwards, max speed
  delay(1000);
}
