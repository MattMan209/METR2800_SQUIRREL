#define PWM_1 13
#define PWM_2 12
#define PWM_3 11
#define PWM_4 10


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PWM_1,OUTPUT);
  pinMode(PWM_2,OUTPUT);
  pinMode(PWM_3,OUTPUT);
  pinMode(PWM_4,OUTPUT);
  Serial.println("Outpus set");
  analogWrite(PWM_1, 200);
  analogWrite(PWM_2, 200);
  analogWrite(PWM_3, 200);
  analogWrite(PWM_4, 200);
  Serial.println("PWM Set");

  pinMode(36, OUTPUT);
  digitalWrite(36, HIGH);
}

void loop() {
  delay(1000);
  digitalWrite(36, LOW);
  delay(1000);
  digitalWrite(36, HIGH);


}
