unsigned int period;
volatile unsigned int timeVal;

#define BLUE_THRESHOLD    3000
#define YELLOW_THRESHOLD  1000


typedef enum {
  START,
  FORWARD,
  TURN,
  RETURN,
  END
} State;

State state = START;
int startColor = 0;

ISR(PCINT0_vect)
{
  static uint8_t lastState = 0;
  uint8_t currState = PINB & 0b00000001; // PD0

  // rising edge → reset timer
  if (currState && !lastState) {
    TCNT1 = 0;
  }

  // falling edge → store timer value
  else if (!currState && lastState) {
    timeVal = TCNT1;
  }

  lastState = currState;
}

void initColor()
{
  // // a. I/O setup (S0–S3 outputs, PD2 input)
  // DDRD = 0b11110000;

  // // S0 = 1, S1 = 0, S2 = 0, S3 = 0 (red filter @20%)
  // PORTD = 0b10000000;

  // b. Pin change interrupt setup (PORTD)
  PCICR  = 0b00000001;   // enable PCIE2
  PCMSK0 = 0b00000001;   // enable PD2 (PCINT18)

  sei();

  // c. Timer1 setup (no prescaler)
  TCCR1A = 0b00000000;
  TCCR1B = 0b00000001;
}

int getColor()
{
  PCMSK0 |= 0b00000001;
  _delay_ms(10);
  PCMSK0 &= 0b11111110;
  Serial.print("Color: ");
  Serial.println(timeVal);
  return timeVal;
}
uint8_t qti_state = (PINB & 0b00000001) || (PINB & 0b00000011);

int classifyColor(int reading) {
  if (reading <= YELLOW_THRESHOLD) {
    return 1; //YELOW
  } else if ((reading <= BLUE_THRESHOLD)) {
    return 0; //BLUE
  } else {
    return 2; //BLACK
  }
}

//Motor Funcs

void drive_forward(int time){
  PORTD = 0b01100000;
  _delay_ms(time);
  stop();
  _delay_ms(100);
}


void drive_backward(int time){
  PORTD = 0b10010000;
  _delay_ms(time);
  stop();
  _delay_ms(100);  
}


void turn_left(int time){
  PORTD = 0b00110000;
  _delay_ms(time);
  stop();
  _delay_ms(100);
}


void turn_right(int time){
  PORTD = 0b00110000;
  _delay_ms(time);
  stop();
  _delay_ms(100);
}

void stop(){
  PORTD = 0b00000000;
}



int main(void)
{

  DDRD = 0b11110000;

  initColor();
  state = START;

  Serial.begin(9600);

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
        if (currentColor == 2) { //Black border is priority
        //QTI sensor left is reading the line
          Serial.println("Black Border?");
          stop();
          _delay_ms(100);
          drive_backward(200);
          turn_right(100);
          drive_forward(200);
          state = FORWARD;
        } else if (currentColor != startColor){
          Serial.println("Yellow?");
          stop();
          _delay_ms(400);
          state = TURN;
        } else {
          Serial.println("FORWARD");
          drive_forward(50);
        }
        break;


      case TURN:
        Serial.println("TURN");
        turn_right(1100); // CHECK IF 180 DEGREES
        state = RETURN;
        break;


      case RETURN:
        Serial.println("RETURN");
        drive_forward(300); //Drive forward a lil bit
        stop();
        if (currentColor == 2){
          state = END;
        } else {
          state = RETURN;
        }
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
