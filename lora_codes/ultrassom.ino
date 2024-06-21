const int trigger_port = 3;
const int echo_port = 4;
const float longest_distance = 336.74;

void setup() {
  pinMode(echo_port,INPUT);
  pinMode(trigger_port,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(trigger_port, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_port, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigger_port, LOW);

  float duration = pulseIn(echo_port, HIGH);

  float current_distance = microsecondsToCentimeters(duration);
  float percentage_completeness  = pertencege_liquid(longest_distance, current_distance);

  Serial.print("Percentage completness: ");
  Serial.println(percentage_completeness );

  delay(100);
}

float pertencege_liquid(float longest_distance, float current_distance) {
  float percentege = 100 - (current_distance / longest_distance * 100);

  if (percentege > 100) return 100;
  else if (percentege < 0) return 0;
  else return percentege;
}

float microsecondsToCentimeters(float microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object
  // we take half of the distance traveled.
  return microseconds / 29 / 2;
}
