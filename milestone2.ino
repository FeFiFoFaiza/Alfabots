// pin 9, 11 are fwd
// pin 8, 10 are back


int main(void){ // setup code that only runs once
  // set pins 8 and 9 as outputs for "left" motor
  // set pins 10 and 11 as outputs for "right" motor
  DDRB = 0b00001111;
  while(1){ // code that loops forever
    
    // call functions for different directions
    
    // option 1: function with no argument followed by delay
    // drive_forward();
    // _delay_ms(1000);
    
    // option 2: function with argument for delay
    // drive_forward_delay(1000);
      // drive_forward();
      // _delay_ms(1000);

      // turn_right();
      // _delay_ms(1000);

      // drive_forward();
      // _delay_ms(1000);

      // turn_left();
      // _delay_ms(1000);

      // drive_forward();
      // _delay_ms(1000);

      // drive_backward();
      // _delay_ms(1000);

      // turn_left();
      // _delay_ms(1000);

      // drive_forward();
      // _delay_ms(1000);

    turn_left();
    _delay_ms(470);
    stop();
    _delay_ms(10000);
    
  }
}

// option 1: function with no delay
// void drive_forward(){
   // code to control appropriate pins here:
  
// }

void drive_forward(){
  PORTB = 0b00001010; 
}

void drive_backward(){
  PORTB = 0b00000101; 
}

void turn_right(){
  PORTB = 0b00000110; 
}

void turn_left(){
  PORTB = 0b00001001;
}

void stop(){
  PORTB = 0b00000000;
}
// option 2: function with built-in delay
// void drive_forward_delay(int t){ 
   // code to control appropriate pins here:
  
  // _delay_ms(t);         // wait for t milliseconds
// }