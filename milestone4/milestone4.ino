// ADD QTI LOGIC + US

unsigned int period;
volatile unsigned int timeVal;

#define BLUE_THRESHOLD    3000 //Check
#define YELLOW_THRESHOLD  1000 //Check

int startColor = 0;

unsigned long startTime = millis();

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
  Serial.print("Color: ");
  Serial.println(timeVal);
  return timeVal;
}

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
  _delay_ms(10);
}

void drive_forward_cont(void){
  PORTD = 0b01100000;
}


void drive_backward(int time){
  PORTD = 0b10010000;
  _delay_ms(time);
  stop();
  _delay_ms(10);  
}


void turn_left(int time){
  PORTD = 0b00110000;
  _delay_ms(time);
  stop();
  _delay_ms(10);
}


void turn_right(int time){
  PORTD = 0b00110000;
  _delay_ms(time);
  stop();
  _delay_ms(10);
}

void stop(){
  PORTD = 0b00000000;
}

int main(void){
  DDRD = 0b11110000;

  initColor();

  //Boeris maybe goes backwards a lil bit
  drive_backward(10);
  //Boeris goes to black
  //Boeris turns
  //Boeris goes to middle (Can it detect color change?)
  //Boeris turns
  //Boeris goes straight until black
  //Come back Boeris



  while (1) {

    if (millis() - startTime >= 60000) { // 60 seconds
    stop();
    break; 
    }

    int reading = getColor();
    int color = classifyColor(reading);

    startColor = color;

    if (color == 2) { // BLACK
      stop();
      turn_left(550);   
    } else if (color != startColor) {
      stop();
      turn_left(550);
    } else {
      drive_forward_cont();
    }
  }

  // //Hard Coded Version (must check distances)
  //   drive_forward(3456);
  //   turn_left(550);
  //   drive_forward(3949);
  //   turn_left(550);
  //   drive_forward(6912);
  //   turn_left(550);
  //   drive_forward(3949);
  //   turn_left(550);
  //   drive_forward(3456);

}


