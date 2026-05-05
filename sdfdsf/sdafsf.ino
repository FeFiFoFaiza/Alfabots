#include <Arduino.h>

// --- Motor Functions ---
void stop(){
  PORTD = 0b00000000;
}

void drive_forward(){
  PORTD = 0b01100000;
}

void drive_backward(){
  PORTD = 0b10010000;
}

void turn_left(){
  PORTD = 0b00110000;
}

void turn_right(int time){
  PORTD = 0b11000000;
}

void forward(int time){
  PORTD = 0b01100000;
  _delay_ms(time);
  stop();
  _delay_ms(10);
}


void backward(int time){
  PORTD = 0b10010000;
  _delay_ms(time);
  stop();
  _delay_ms(10);  
}


void left(int time){
  PORTD = 0b00110000;
  _delay_ms(time);
  stop();
  _delay_ms(10);
}


void right(int time){
  PORTD = 0b00110000;
  _delay_ms(time);
  stop();
  _delay_ms(10);
}

// --- FSM States ---
enum State {
  FORWARD,
  TURN_LEFT_BLACK,
  BACKWARD_STUCK,
  TURN_LEFT_DELAYED,
  STOPPED
};

State state = FORWARD;

// --- Timing variables ---
unsigned long start_time;
unsigned long state_start;
unsigned long forward_start;
unsigned long forward_since_black_start;

volatile int period = 0;
volatile bool newReading = false;

bool sonar_activated;

bool first_black = false;
bool delayed_turn_done = false;

void setup(){
  init();
  Serial.begin(9600);
  start_time = millis();
  DDRD = 0b11110000;
  sonar_activated = false;
  start_time = millis();
  state_start = millis();
  forward_start = millis();
}

void sonor_init()
{
  DDRB &= ~0b00010000;
  PORTB &= ~0b00010000;
  TCCR1A = 0;
  TCCR1B = (1 << CS11);      // prescaler 8
}

void loop(){

  unsigned long now = millis();
  // --- Stop after 60 seconds ---
  if (now - start_time >= 60000){
    state = STOPPED;
  }

  switch(state){

    case FORWARD:
      forward(100);
      int dist = 1000;
      if (sonar_activated) {
        dist = get_distance_cm();
      }
      if (dist > 3) {
        //we carry on as if sonar doesn't exist
        // black detected
        if (PIND & 0b00000100){
          Serial.println("BLACK");
          state = TURN_LEFT_BLACK;
          state_start = now;
        }

        // stuck (15s forward)
        else if (now - forward_start >= 15000){
          Serial.println("STUCK");
          state = BACKWARD_STUCK;
          state_start = now;
        }

        // delayed turn (10s after first black)
        else if (first_black && !delayed_turn_done &&
                (now - forward_since_black_start >= 10000)){
          Serial.println("DELAYED TURN");
          state = TURN_LEFT_DELAYED;
          state_start = now;
        }
      } else {
        //we've detected something within the threshhold 
        Serial.println("hdthf");
        turn_right(470);
        state = FORWARD;
      }
      

      break;


    case TURN_LEFT_BLACK:
      Serial.println("GO");
      left(470);

      if (!first_black){
        first_black = true;
        forward_since_black_start = now;
      }

      if (now - state_start >= 470){
        stop();
        forward_start = now;
        state = FORWARD;
      }
      break;


    case BACKWARD_STUCK:
      backward(20);

      if (now - state_start >= 500){
        state = TURN_LEFT_BLACK;
        state_start = now;
      }
      break;


    case TURN_LEFT_DELAYED:
      left(470);

      if (now - state_start >= 500){
        stop();
        delayed_turn_done = true;
        forward_start = now;
        state = FORWARD;
        sonor_init();
        sonar_activated = true;
      }
      break;


    case STOPPED:
      stop();
      Serial.println("STOPPED");
      while(1); // halt
      break;
  }
}

void send_trigger()
{
  cli();                      // disable interrupts during trigger
  DDRB |= 0b00010000;        // switch to output
  PORTB &= ~0b00010000;
  _delay_us(2);
  PORTB |= 0b00010000;        // pulse high
  _delay_us(5);
  PORTB &= ~0b00010000;        // pull low
  DDRB &= ~0b00010000;        // switch back to input
  PORTB &= ~0b00010000;        // no pull-up

  PCIFR |= 0b00000001;      // clear any pending PCINT flag before enabling
  PCICR |= 0b00000001;
  PCMSK0 |= 0b00010000;
  sei();                      // re-enable interrupts
}

int get_distance_cm()
{
  newReading = false;
  send_trigger();

  unsigned long timeout = millis();
  while (!newReading) {
    if (millis() - timeout > 50) {
      break;
    }
  }

  // disarm interrupt until next trigger
  PCMSK0 &= ~0b00010000;

  return period / 116;
}

ISR(PCINT0_vect)
{
  if (PINB & 0b00010000) {
    TCNT1 = 0;
  } else {
    period = TCNT1;
    newReading = true;
  }
}
