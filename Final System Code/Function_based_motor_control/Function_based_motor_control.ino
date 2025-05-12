// ======= MOTOR CONTROL PINS =======
// Front Left Motor
const int FL_IN1 = 25;
const int FL_IN2 = 23;
const int FL_PWM = 10;   // PWM capable

// Front Right Motor
const int FR_IN1 = 29;
const int FR_IN2 = 31;
const int FR_PWM = 11;  // PWM capable

// Rear Left Motor
const int RL_IN1 = 47;
const int RL_IN2 = 45;
const int RL_PWM = 13;  // PWM capable

// Rear Right Motor
const int RR_IN1 = 51;
const int RR_IN2 = 53;
const int RR_PWM = 12;   // PWM capable (not pin 13)

// ======= ENCODER PINS (UNIQUE + INTERRUPT CAPABLE - MEGA) =======
const int FL_ENCA = 21;  // INT5
const int FL_ENCB = 20;

const int FR_ENCA = 19;  // INT3
const int FR_ENCB = 18;

const int RL_ENCA = 2;   // INT4
const int RL_ENCB = 3;

const int RR_ENCA =22;   // INT1
const int RR_ENCB = 24; // THESE PINS DO NOTHING, WE JUST DONT HAVE AN ENCODER FOR THE FOURTH MOTOR

// ======= ENCODER COUNTS =======
volatile long FL_forwardCount = 0;
volatile long FL_backwardCount = 0;

volatile long FR_forwardCount = 0;
volatile long FR_backwardCount = 0;

volatile long RL_forwardCount = 0;
volatile long RL_backwardCount = 0;

volatile long RR_forwardCount = 0;
volatile long RR_backwardCount = 0;

void setup() {
  // Motor pin setup
  pinMode(FL_IN1, OUTPUT); pinMode(FL_IN2, OUTPUT); pinMode(FL_PWM, OUTPUT);
  pinMode(FR_IN1, OUTPUT); pinMode(FR_IN2, OUTPUT); pinMode(FR_PWM, OUTPUT);
  pinMode(RL_IN1, OUTPUT); pinMode(RL_IN2, OUTPUT); pinMode(RL_PWM, OUTPUT);
  pinMode(RR_IN1, OUTPUT); pinMode(RR_IN2, OUTPUT); pinMode(RR_PWM, OUTPUT);

  // Encoder pin setup
  pinMode(FL_ENCA, INPUT); pinMode(FL_ENCB, INPUT);
  pinMode(FR_ENCA, INPUT); pinMode(FR_ENCB, INPUT);
  pinMode(RL_ENCA, INPUT); pinMode(RL_ENCB, INPUT);
  pinMode(RR_ENCA, INPUT); pinMode(RR_ENCB, INPUT);

  // Attach interrupts for each encoder (on ENCA pin)
  attachInterrupt(digitalPinToInterrupt(FL_ENCA), FL_encoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(FR_ENCA), FR_encoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RL_ENCA), RL_encoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RR_ENCA), RR_encoderISR, RISING);

  Serial.begin(115200);
}

void loop() {
  // Move at 90° (right) with full translational speed, no rotation
  move(90, 200, 0);
  delay(2000);

  // Move at 0° (forward) with rotation clockwise
  move(0, 150, 50);
  delay(2000);

  // Move backward with counter-clockwise rotation
  move(180, 150, -50);
  delay(2000);

  // Stop
  move(0, 0, 0);
  delay(1000);
}

// ======= MOVEMENT LOGIC USING TB6612FNG =======
void move(float angle_deg, float max_wheel_speed, float rotation_speed) {
  float angle_rad = radians(angle_deg);
  float vx = cos(angle_rad);
  float vy = sin(angle_rad);

  // Unscaled wheel speeds with rotation
  float fl = vy + vx - rotation_speed;
  float fr = vy - vx + rotation_speed;
  float rl = vy - vx - rotation_speed;
  float rr = vy + vx + rotation_speed;

  float max_raw = max(max(abs(fl), abs(fr)), max(abs(rl), abs(rr)));
  float scale = (max_raw > 0) ? (max_wheel_speed / max_raw) : 0;

  fl *= scale;
  fr *= scale;
  rl *= scale;
  rr *= scale;

  setMotor(FL_IN1, FL_IN2, FL_PWM, (int)abs(fl), fl >= 0);
  setMotor(FR_IN1, FR_IN2, FR_PWM, (int)abs(fr), fr >= 0);
  setMotor(RL_IN1, RL_IN2, RL_PWM, (int)abs(rl), rl >= 0);
  setMotor(RR_IN1, RR_IN2, RR_PWM, (int)abs(rr), rr >= 0);
}

void setMotor(int in1, int in2, int pwm, int speed, bool forward) {
  digitalWrite(in1, forward ? HIGH : LOW);
  digitalWrite(in2, forward ? LOW : HIGH);
  analogWrite(pwm, constrain(speed, 0, 255));
}

// ======= ENCODER ISR HANDLERS =======
void FL_encoderISR() {
  if (digitalRead(FL_ENCB) == HIGH)
    FL_forwardCount++;
  else
    FL_backwardCount++;
}

void FR_encoderISR() {
  if (digitalRead(FR_ENCB) == HIGH)
    FR_forwardCount++;
  else
    FR_backwardCount++;
}

void RL_encoderISR() {
  if (digitalRead(RL_ENCB) == HIGH)
    RL_forwardCount++;
  else
    RL_backwardCount++;
}

void RR_encoderISR() {
  if (digitalRead(RR_ENCB) == HIGH)
    RR_forwardCount++;
  else
    RR_backwardCount++;
}
