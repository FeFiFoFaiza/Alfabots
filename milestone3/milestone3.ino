//TCS3200 Pins
// S0  Pin 4
// S1  Pin 5
// S2  Pin 6
// S3  Pin 7
// OUT Pin 2  

// Global
volatile unsigned int timeVal = 0;
unsigned int period = 0;

#define BLUE_THRESHOLD    1000
#define YELLOW_THRESHOLD  2000

typedef enum {
  START,
  FORWARD,
  TURN,
  RETURN,
  END
} State;

State state = START;
int startColor = 0;

// Interrupt

ISR(PCINT2_vect){ //CHECK VECTOR PIN
    static uint8_t lastState = 0;
    uint8_t currState = PIND & (1 << PD2); //CHECK THIS

    if (currState && !lastState) {
        TCNT1 = 0;
    }
    else if (!currState && lastState) {
        timeVal = TCNT1;
    }

    lastState = currState;
}

// Sensor setup
void initColor()
{
    DDRD &= ~(1 << PD2); // OUT pin input

    PCICR |= (1 << PCIE2);      // enable PCINT for PORTD
    PCMSK2 |= (1 << PCINT18);   // PD2 interrupt

    TCCR1A = 0;
    TCCR1B = 0b00000001;

    sei();
}

// Read Color
int getColor() {
  PCMSK2 |= (1 << PCINT18);
  _delay_ms(10);
  PCMSK2 &= ~(1 << PCINT18);
  return timeVal;
}

int classifyColor(int reading) {
  if (reading <= BLUE_THRESHOLD) {
    return 0; //BLUE
  } else if (reading <= YELLOW_THRESHOLD) {
    return 1; //YELLOW
  } else {
    return 2; //BLACK
  }
}

//Motor Pins
// Right Forward   Pin 10
// Right Backward  Pin 11
// Left Forward    Pin 8
// Left Backward   Pin 9

// Motor Functions
void drive_forward(int time){
  PORTB = 0b00000101;
  _delay_ms(time);
  stop();
  _delay_ms(100); 
}

void drive_backward(int time){
  PORTB = 0b00001010;
  _delay_ms(time);
  stop();
  _delay_ms(100);  
}

void turn_right(int time){
  PORTB = 0b00000110;
  _delay_ms(time);
  stop(); 
  _delay_ms(100); 
}

void turn_left(int time){
  PORTB = 0b00001001;
  _delay_ms(time);
  stop();
  _delay_ms(100); 
}

void stop(){
  PORTB = 0b00000000;
}


// MAIN
int main(void){
  DDRB = 0b00001111; //CHECK WIRING
  initColor();

  while(1) {
    period = getColor();
    int currentColor = classifyColor(period);

    switch(state){
      case START:
        startColor = currentColor;
        Serial.println("START");
        Serial.println(startColor);
        if (currentColor == 2){ //Starting on black edge case
          state = END;
        } else {
          state = FORWARD;
        }
        break;

      case FORWARD:
        Serial.println("FORWARD");
        drive_forward(20);

        if (currentColor == 2) { //Black border is priority
          stop();
          _delay_ms(100);
          turn_right(470); //CHECK IF 90 DEGREES
          state = FORWARD;
        } else if (currentColor != startColor){
          stop();
          _delay_ms(400);
          state = TURN;
        }
        break;

      case TURN:
        Serial.println("TURN");
        turn_right(940); // CHECK IF 180 DEGREES
        state = RETURN;
        break;

      case RETURN:
        Serial.println("RETURN");
        drive_forward(300); //Drive forward a lil bit
        stop();
        state = END;
        break;

      case END:
        Serial.println("END");
        stop();
        while(1);
        break;
    }

    _delay_ms(50);
  }
}