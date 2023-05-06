#include <Arduino.h>
#include <Keyboard.h>
#include <FastLED.h>

/*
* Rotary Encoder Pins
*/ 
#define CLK 5
#define DT 3
#define SW 21

/*
 * LED Pins
*/
#define DATA_PIN 10
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 16
#define BRIGHTNESS 255

/* 
* Key Pins
*/
const uint8_t keyPins[8] = {
  2, 4, 6, 7, 8, 9, 20, 19 
};

int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastButtonPress = 0;

uint8_t brightness = 255;

CRGB leds[NUM_LEDS];

/*
* Prototypes
*/
void init_keys();
void init_leds();
void update_leds();
void read_encoder_btn_state();
void read_encoder_rotation_state();
void print_key_state();

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    yield();

  init_keys();
  init_leds();

  Serial.println(F("Ready"));

  // Set encoder pins as inputs
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  digitalWrite(SW, HIGH);

  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip)
      .setDither(brightness < 255);

  // set master brightness control
  FastLED.setBrightness(brightness);
  FastLED.show();

  // Take over keyboard
  Keyboard.begin();
}

void loop()
{
  read_encoder_btn_state();
  read_encoder_rotation_state();
  update_leds();
  FastLED.show();
  print_key_state();
  delay(1000);
}

/*
* Set each key's input mode to INPUT_PULLUP.
* Each pin is set to HIGH to activate built in 
* pull-up resistor.
*/
void init_keys()
{
  for(int i = 0; i < 8; i++)
  {
    uint8_t pin = keyPins[i];
    pinMode(pin, INPUT_PULLUP);
    digitalWrite(pin, HIGH);

  }
}

void read_encoder_btn_state()
{
  // Read encoder button state
  int btnState = digitalRead(SW);
  if (btnState == LOW)
  {
    if (millis() - lastButtonPress > 50)
    {
      Serial.println(F("Button Pressed"));
    }
    lastButtonPress = millis();
  }
  delay(1);
}

void read_encoder_rotation_state()
{
  currentStateCLK = digitalRead(CLK);

  if (currentStateCLK != lastStateCLK && currentStateCLK == 1)
  {

    if (digitalRead(DT) != currentStateCLK)
    {
      currentDir = "CW";
    }
    else
    {
      currentDir = "CCW";
    }

    Serial.print(F("Direction: "));
    Serial.println(currentDir);
  }

  lastStateCLK = currentStateCLK;
}

void init_leds()
{
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);

  FastLED.setBrightness(BRIGHTNESS);
}

void update_leds()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16; // gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV(hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 64);

    FastLED.show();
  }
}

void print_key_state()
{
  for(int i = 0; i < 8; i++)
  {
    uint8_t pin = keyPins[i];

    Serial.print(F("BTN "));
    Serial.print(pin);
    Serial.print(F(": "));
    Serial.println(digitalRead(pin) == HIGH ? F("HIGH") : F("LOW"));
  }
}