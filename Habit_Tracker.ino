
#define interruptPin 2 //Pin we are going to use to wake up the Arduino

#define DEBOUNCE 10  // how many ms to debounce, 5+ ms is usually plenty

//define the buttons that we'll use.
byte buttons[] = {13, 12, 11, 10, 9, A0};

byte leds[] = {8, 7, 6, 5, 4, 3};

bool complete = false;
int animDelay = 100;

//determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)

//track if a button is just pressed, just released, or 'currently pressed'
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];
byte previous_keystate[NUMBUTTONS], current_keystate[NUMBUTTONS];

void setup() {
  Serial.begin(115200);//Start Serial Comunication
  Serial.println("Startup began.");
  pinMode(interruptPin, INPUT_PULLUP); //Set pin d2 to input using the buildin pullup resistor
  // Make input & enable pull-up resistors on switch pins
  for (byte i = 0; i < 5; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }
  for (byte i = 0; i < 6; i++) {
    pinMode(leds[i], OUTPUT);
    //digitalWrite(leds[i], HIGH);
  }
  digitalWrite(leds[5], HIGH);
  //goToSleep();
}

void loop() {
  if (complete)
  {
    for (byte i = 0; i < 5; i++) {
      toggleLED(leds[i]);
      delay(animDelay);
    }
    toggleLED(leds[5]);
  }
  else
  {
    byte thisSwitch = thisSwitch_justPressed();
    switch (thisSwitch)
    {
      case 0:
        Serial.println("switch 1 just pressed");
        toggleLED(leds[thisSwitch]);
        break;
      case 1:
        Serial.println("switch 2 just pressed");
        toggleLED(leds[thisSwitch]);
        break;
      case 2:
        Serial.println("switch 3 just pressed");
        toggleLED(leds[thisSwitch]);
        break;
      case 3:
        Serial.println("switch 4 just pressed");
        toggleLED(leds[thisSwitch]);
        break;
      case 4:
        Serial.println("switch 5 just pressed");
        toggleLED(leds[thisSwitch]);
        break;
      default:
        break;
    }
  }
  if (digitalRead(interruptPin) == HIGH)
  {
    for (byte i = 0; i < 6; i++) {
      digitalWrite(leds[i], LOW);
      complete = false;
    }
  }
  else {
    if (complete) {
      return;
    }
    digitalWrite(leds[5], HIGH);
  }

  checkComplete();
}

void checkComplete()
{
  if (complete) {
    return;
  }
  int completedHabits = 0;
  for (byte i = 0; i < 5; i++) {
    if (digitalRead(leds[i]) == HIGH) {
      completedHabits++;
    }
  }
  if (completedHabits >= 5) {
    complete = true;
    for (byte i = 0; i < 6; i++) {
      digitalWrite(leds[i], LOW);
    }
  }
}



void toggleLED(byte ledPin)
{
  if (digitalRead(ledPin) == LOW)
  {
    digitalWrite(ledPin, HIGH);
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }
}


void check_switches()
{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;
  if (millis() < lasttime) {
    // we wrapped around, lets just try again
    lasttime = millis();
  }
  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return;
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();
  for (index = 0; index < NUMBUTTONS; index++) {
    justpressed[index] = 0;       //when we start, we clear out the "just" indicators
    justreleased[index] = 0;
    currentstate[index] = digitalRead(buttons[index]);   //read the button
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        justreleased[index] = 1; // just released
      }
      pressed[index] = !currentstate[index];  //remember, digital HIGH means NOT pressed
    }
    previousstate[index] = currentstate[index]; //keep a running tally of the buttons
  }
}

byte thisSwitch_justPressed() {
  byte thisSwitch = 255;
  check_switches();  //check the switches &amp; get the current state
  for (byte i = 0; i < NUMBUTTONS; i++) {
    current_keystate[i] = justpressed[i];
    if (current_keystate[i] != previous_keystate[i]) {
      if (current_keystate[i]) thisSwitch = i;
    }
    previous_keystate[i] = current_keystate[i];
  }
  return thisSwitch;
}
