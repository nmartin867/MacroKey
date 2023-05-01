#include <Arduino.h>
#include <Keyboard.h>
#include <Mouse.h>

#define CLK 5
#define DT 3
#define SW 21

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastButtonPress = 0;

void read_encoder_btn_state();
void read_encoder_rotation_state();

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    yield();
  Serial.println(F("Ready"));

  // Set encoder pins as inputs
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  pinMode(3, INPUT);

  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);

  // Take over keyboard & mouse
  Mouse.begin();
  Keyboard.begin();
}

void loop()
{
  read_encoder_btn_state();
  read_encoder_rotation_state();

  Serial.println(digitalRead(3));
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
      Keyboard.print("w");
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
      counter++;
      currentDir = "CW";
    }
    else
    {
      counter--;
      currentDir = "CCW";
    }

    Serial.print(F("Direction: "));
    Serial.print(currentDir);
    Serial.print(F(" | Counter: "));
    Serial.println(counter);
  }

  lastStateCLK = currentStateCLK;
}
