#include <CapacitiveSensor.h>

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

#define distancePin 2
#define whitePin 9
#define yellowPin 10

#define awakeTime 60000 // milliseconds cube will stay awake
#define evaluateTime 5000 // evaluate next state every N seconds

CapacitiveSensor capSensor = CapacitiveSensor(4, 6);       // 10M resistor between pins 4 & 6, pin 6 is sensor pin
long capSensorThreshold = 1000; // if capacitive sensor reading is over threshold, then cube is touched

// when user is not interacting with cube
// then state will be revaluated every N seconds
int possibleNextStates[5][3] = {
  {PEACE, SENSITIVE, 80}, // from PEACE, probability of staying PEACE is 80%
  {SENSITIVE, PEACE, 80}, // from SENSITIVE
  {HAPPY, PEACE, 60}, // from HAPPY
  {ANGRY, SENSITIVE, 60}, // from ANGRY
  {PEACE, SENSITIVE, 60} // from ASLEEP
};
// when user touches the cube, reevaluate state
int touchNextStates[5][3] = {
  {HAPPY, ANGRY, 90}, // from PEACE
  {ANGRY, ANGRY, 100}, // from SENSITIVE
  {HAPPY, HAPPY, 100}, // from HAPPY
  {ANGRY, ANGRY, 100}, // from ANGRY
  {ANGRY, ANGRY, 100} // from ASLEEP
};

int currentState = ASLEEP;
int lastWakeupTime = -1;
int lastEvaluateTime = -1;
int prevDistanceValue = LOW;

void setup() {
  Serial.begin(9600);

  // temporary for button, will be replaced with sensor later
  pinMode(distancePin, INPUT);
  pinMode(whitePin, OUTPUT);
  pinMode(yellowPin, OUTPUT);

  // make sure random() is truly random
  randomSeed(analogRead(0));
}

void loop() {
  // get distance sensor reading (temp button)
  int distanceReading = digitalRead(distancePin);
  long touchReading = capSensor.capacitiveSensor(30);

  int seePerson = distanceReading == HIGH && prevDistanceValue == LOW;
  int touchCube = touchReading > capSensorThreshold;
  int currentTime = millis();

  // if we have positive reading on distance sensor
  // update lastWakeupTime to current time so we can reset countdown for cube sleeping
  if (seePerson) {
    lastWakeupTime = currentTime;
    Serial.println("saw person");
  }

  if (touchCube) {
    // if cube is touched, determine next state based on
    // probabilities outlined in touchNextStates
    determineNextState(touchNextStates);
    // since we've evaluated state, also update lastEvaluateTime to now
    lastEvaluateTime = currentTime;
    // also reset wakeup time since we've touched it
    lastWakeupTime = currentTime;
  } else {
    // if cube is not touched

    // case when cube is asleep
    if (currentState == ASLEEP) {
      // cube sees person so it wakes up
      if (seePerson) {
        // first light LED to white to give visual indicator that cube is awake
        analogWrite(whitePin, 255); // turn on white LED
        delay(1000);
        analogWrite(whitePin, 0); // turn off white LED

        // determine next state
        determineNextState(possibleNextStates);
      }
    } else {
      // cube is awake

      // check if it should go to sleep
      // if current time is more than lastWakeupTime + awakeTime
      if (currentTime > (lastWakeupTime + awakeTime)) {
        currentState = ASLEEP;
        Serial.println("went to sleep!");
      } else {
        // if it doesn't need to go to sleep yet
        // evaluate what next state should be every N seconds
        if (currentTime > (lastEvaluateTime + evaluateTime)) {
          determineNextState(possibleNextStates);
          lastEvaluateTime = currentTime;
        }
      }
    }
  }

  // SET LED VALUES
  int value = 0; // default to value 0 for ASLEEP
  if (currentState != ASLEEP) {
    // if awake
    if (currentState == ANGRY) {
      value = (currentTime / 100) % 2;
      value = value * 255;
    } else {
      float speed = 0.25; // HAPPY & PEACE
      if (currentState == SENSITIVE) {
        speed = 1.5; // SENSITIVE
      }
      // brightness of LED is a sine wave of time (current - lastEvaluateTime)
      float time = speed * currentTime / 1000.0;
      value = 128.0 + 128 * sin( time * 2.0 * PI  );
    }
  }

  if (currentState == HAPPY) {
    // just for happy turn on yellow LED instead of white
    analogWrite(yellowPin, value);
    analogWrite(whitePin, 0);
  } else {
    // for everything else turn on white pin
    analogWrite(whitePin, value);
    analogWrite(yellowPin, 0);
  }

  prevDistanceValue = distanceReading;
}

void determineNextState(int nextStates[5][3]) {
  int randomNum = random(100);
  // get probability of current state going to the first option of next state
  int probability = nextStates[currentState][2];
  Serial.print("current state: ");
  Serial.print(currentState);
  Serial.print(", random number: ");
  Serial.print(randomNum);
  // update current state
  if (randomNum < probability) {
    currentState = nextStates[currentState][0];
  } else {
    currentState = nextStates[currentState][1];
  }

  Serial.print(", next state: ");
  Serial.println(currentState);
}
