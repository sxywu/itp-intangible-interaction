// A curious cube that acts like a cat
// its default state is asleep
// when it detects a person approaching, it lights up in recognition
// and enters either PEACE or SENSITIVE mode
// if person pets cube in PEACE mode, it can enter either HAPPY or ANGRY mode
// if person pets cube in SENSITIVE mode, it will 100% enter ANGRY mode
// from ANGRY mode, it can either stay in ANGRY mode or go back to SENSITIVE mode
// from SENSITIVE mode it can either stay in SENSITIVE mode or go to PEACE mode

#define PEACE 0
#define SENSITIVE 1
#define HAPPY 2
#define ANGRY 3
#define ASLEEP 4

#define distancePin 2 // currently a button, replace with actual sensor later
#define awakeTime 1000 // milliseconds cube will stay awake

int possibleNextStates[5][2] = {
  {PEACE, SENSITIVE}, // from PEACE
  {SENSITIVE, PEACE}, // from SENSITIVE
  {HAPPY, PEACE}, // from HAPPY
  {ANGRY, SENSITIVE}, // from ANGRY
  {PEACE, SENSITIVE} // from ASLEEP
};

int currentState = ASLEEP;
int lastWakeupTime = -1;

void setup() {
  Serial.begin(9600);

  // temporary for button, will be replaced with sensor later
  pinMode(distancePin, INPUT_PULLUP);

  // make sure random() is truly random
  randomSeed(analogRead(0));
}

void loop() {
  // get distance sensor reading (temp button)
  int distanceReading = digitalRead(distancePin);
  int seePerson = distanceReading == LOW;

  // if we have positive reading on distance sensor
  // update lastWakeupTime to current time so we can reset countdown for cube sleeping
  if (seePerson) {
    lastWakeupTime = millis();
  }
    
  // case when cube is asleep
  if (currentState == ASLEEP) {
    // cube sees person so it wakes up
    if (seePerson) {
      // TODO: first light up cube for 1 second
      // determine next state
      determineNextState();
      Serial.println(currentState);
    }
  } else {
    // cube is awake

    // check if it should go to sleep
    // if current time is more than lastWakeupTime + awakeTime
    if (millis() > (lastWakeupTime + awakeTime)) {
      currentState = ASLEEP;
      Serial.println("went to sleep!");
    }
  }

  
}

void determineNextState() {
  int randomNum = random(100);
  Serial.println(randomNum);
  // update current state
  if (randomNum < 50) {
    currentState = possibleNextStates[currentState][0];
  } else {
    currentState = possibleNextStates[currentState][1];
  } 
}
