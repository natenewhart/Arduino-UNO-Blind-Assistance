// Nate Newhart - ArduinoUnoBlindProject.ino - 2026

// ---------------------- Global Variables ----------------------

const int buttonPin = 9;    // button (other leg to GND)
const int buzzerPin = 10;   // passive buzzer (other leg to GND)

bool systemOn = false;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Ultrasonic distance meter pins
const int trigPin = A0;
const int echoPin = 11;

// Distance noise reduction
int pastIndex = 0; // Most recent distance measurement index
const int distArrSize = 5;
float distanceArr[distArrSize]; // Array with past 5 distances measured
const float minDistance = 0.05;
const float maxDistance = .75;   // 50 cm, in meters

// Buzzer variables
unsigned int buzzerFrequency = 0;
const unsigned int minFreq = 100;
const unsigned int maxFreq = 4000;

unsigned long lastSensorTime = 0;
const unsigned long sensorInterval = 20;   // ms between readings

// ---------------------- Arduino Main ----------------------

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  noTone(buzzerPin);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

  for (int i = 0; i < distArrSize; i++) distanceArr[i] = 1.0;
}

void loop()
{
  // ---- Button: toggles system on/off ----
  bool reading = digitalRead(buttonPin);

  if (reading != lastButtonState && millis() - lastDebounceTime > debounceDelay)
  {
    lastDebounceTime = millis();

    if (reading == LOW) // button just pressed
    {                    
      if (systemOn == false)
      {
        systemOn = true;                 
      } 
      else
      {
        systemOn = false;                    // Turn off buzzer
        noTone(buzzerPin);                   
      }
    }
  }
  lastButtonState = reading;

  // ---- Continuous distance check (only while ON) ----
  if (systemOn && millis() - lastSensorTime >= sensorInterval)
  {
    lastSensorTime = millis();

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 6000);   // 30 ms timeout

    if (duration > 0)
    {
      float currDistance = (duration * 0.000343) / 2.0;  // set object distance to meters
      Serial.println(currDistance, 4);

      distanceArr[pastIndex] = currDistance;
      pastIndex = (pastIndex + 1) % distArrSize;

      float smoothedDistance = median5(distanceArr); // Median of readings
      
      if (smoothedDistance <= maxDistance)
      {
        float distanceRatio = (smoothedDistance - minDistance) / (maxDistance - minDistance);
        distanceRatio = constrain(distanceRatio, 0.0, 1.0);
        buzzerFrequency = lerp(maxFreq, minFreq, distanceRatio);
        tone(buzzerPin, buzzerFrequency);
      } 
      else
      {
        noTone(buzzerPin);
      }
    }
    else
    {
      noTone(buzzerPin);
    }
  }
}

// ------------------- Helper Functions -------------------

inline int lerp(int low, int high, float t)
{
  return (int)(low + t * (high - low));
}
float median5(const float *arr)
{
  float v[5];
  for (int i = 0; i < 5; i++)
  {
    v[i] = arr[i];   // copy so the original buffer keeps its order
  }

  // insertion sort
  for (int i = 1; i < 5; i++)
  {
    float key = v[i];
    int j = i - 1;
    while (j >= 0 && v[j] > key)
    {
      v[j + 1] = v[j];
      j--;
    }
    v[j + 1] = key;
  }

  return v[2];   // middle of 5 sorted values
}