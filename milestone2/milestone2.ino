// pin 9, 11 are fwd
// pin 8, 10 are back


int main(void){ // setup code that only runs once
  // set pins 8 and 9 as outputs for "left" motor
  // set pins 10 and 11 as outputs for "right" motor
  DDRB = 0b00001111;
  while(1){ 

      // Drive forward 1 foot
      drive_forward(8000);
      // Turn right 90 degrees
      turn_right(470);
      // Drive forward 1 foot
      drive_forward(8000);
      // Turn left 90 degrees
      turn_left(470);
      // Drive forward 6 inches
      drive_forward(4000);
      // Drive backward 1.5 feet
      drive_backward(12000);
      // Turn left 90 degrees
      turn_left(470);
      // Drive forward 1 foot, returning to the starting position
      drive_forward(8000);
      // Stop
      break;
  }
}

// option 1: function with no delay
// void drive_forward(){
   // code to control appropriate pins here:
  
// }

void drive_forward(int time){
  PORTB = 0b00001010;
  _delay_ms(time);
  stop(); 
}

void drive_backward(int time){
  PORTB = 0b00000101;
  _delay_ms(time);
  stop(); 
}

void turn_right(int time){
  PORTB = 0b00000110;
  _delay_ms(time);
  stop(); 
}

void turn_left(int time){
  PORTB = 0b00001001;
  _delay_ms(time);
  stop();
}

void stop(){
  PORTB = 0b00000000;
}
// option 2: function with built-in delay
// void drive_forward_delay(int t){ 
   // code to control appropriate pins here:
  
  // _delay_ms(t);         // wait for t milliseconds
// }