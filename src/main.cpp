#include <Arduino.h>
#include <HX711.h>

#define SHIFT_DATA 2
#define SHIFT_CLOCK 3

#define HX_CLK 11
#define HX_DT 12

HX711 scale;

#define DIGITS 5

const int DIGIT_PIN[DIGITS] = {4,5,6,7,8};

volatile int display_value = 0;
volatile int current_digit = 0;

int num_to_print[DIGITS];

long weight = 0;

int num[11] = {
  0b00000001,
  0b01001111,
  0b10010010,
  0b00000110,
  0b01001100,
  0b00100100,
  0b00100000,
  0b00001111,
  0b00000000,
  0b00000100,
  0b11111111
};

void displaySingle(int i)
{
  shiftOut(SHIFT_DATA, SHIFT_CLOCK, LSBFIRST, num[i]);
}

ISR(TIMER2_COMPA_vect)
{
  int f = display_value;

  num_to_print[4] = f % 10;
  num_to_print[3] = (f % 100) / 10;
  num_to_print[2] = (f % 1000) / 100;
  num_to_print[1] = (f % 10000) / 1000;
  num_to_print[0] = (f % 100000) / 10000;

  displaySingle(num_to_print[current_digit]);

  digitalWrite(DIGIT_PIN[current_digit], LOW);
  delayMicroseconds(200);
  digitalWrite(DIGIT_PIN[current_digit], HIGH);

  current_digit++;
  if(current_digit >= DIGITS) current_digit = 0;
}

void setupTimer()
{
  cli();

  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  OCR2A = 249;

  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS22);

  TIMSK2 |= (1 << OCIE2A);

  sei();
}

void setup()
{
  Serial.begin(9600);

  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLOCK, OUTPUT);

  for(int i=0;i<DIGITS;i++)
  {
    pinMode(DIGIT_PIN[i], OUTPUT);
    digitalWrite(DIGIT_PIN[i], HIGH);
  }

  scale.begin(HX_DT, HX_CLK);

  delay(1000);

  if(scale.is_ready())
  {
    Serial.println("HX711 ready");

    scale.set_scale();
    Serial.println("Remove weight");

    display_value = 199;
    delay(2000);

    scale.tare();

    Serial.println("Tare complete");
  }
  else
  {
    Serial.println("HX711 not found");
  }

  setupTimer();
}

unsigned long weightTimer = 0;

void loop()
{
  if(millis() - weightTimer > 200)
  {
    weightTimer = millis();

    if(scale.is_ready())
    {
      weight = scale.get_units(10) * 0.007633 * (-1);

      display_value = weight;

      Serial.print("Weight: ");
      Serial.println(weight);
    }
  }
}