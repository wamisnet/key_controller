const int OPEN_SIGN = 2;
const int CLOSED_SIGN = 3;
const int SOLENOID_NORMAL = 4;
const int SOLENOID_REVERSE = 5;
const int MOTOR_NORMAL = 6;
const int MOTOR_REVERSE = 7;
const int SETTING_BTN = 8;
const int SOLENOID_FAULT = 9;
const int MOTOR_FAULT = 10;
const int POSITIONMETER = A0;

boolean openSign;
boolean closedSign;
boolean settingSign;

int nowPos_old = 0;
int nowPos;
int targetPos;

const int MAX_POS = 1023;
int OPEN_POS = 0;
int CLOSED_POS = 350;
int MAX_RANGE;


void setup() {
  // put your setup code here, to run once:
  pinMode(OPEN_SIGN, INPUT_PULLUP);
  pinMode(CLOSED_SIGN, INPUT_PULLUP);
  pinMode(MOTOR_NORMAL, OUTPUT);
  pinMode(MOTOR_REVERSE, OUTPUT);
  pinMode(SOLENOID_NORMAL, OUTPUT);
  pinMode(SOLENOID_REVERSE, OUTPUT);
  pinMode(SETTING_BTN, INPUT_PULLUP);
  pinMode(POSITIONMETER, INPUT);
  pinMode(SOLENOID_FAULT, INPUT_PULLUP);
  pinMode(MOTOR_FAULT, INPUT_PULLUP);

  Serial.begin(9600);
  stopMotor();

  MAX_RANGE = abs(OPEN_POS - CLOSED_POS);
  if (MAX_RANGE > MAX_POS / 2) {
    MAX_RANGE = MAX_POS - MAX_RANGE;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //信号取得
  getKeySign();  
  //ポジション取得
  getPositionMeter();

  int targetRange;
  //前のポジションと比較して10以上ずれてたら、nowPosを変更
   if (abs(nowPos_old - nowPos) > 10) {
    Serial.print("nowPos:");
    Serial.println(nowPos);
    nowPos_old = nowPos;
  }
  //オープンポジションとクローズポジションを設定
  if (settingSign == LOW) {
    if (openSign == LOW) {
      Serial.println("setting OpenPos");
      OPEN_POS = nowPos;
    } else if (closedSign == LOW) {
      Serial.println("setting closedPos");
      CLOSED_POS = nowPos;
    }
    //鍵が動ける最大範囲
    MAX_RANGE = abs(OPEN_POS - CLOSED_POS) + 100;
    delay(1000);
    return;
  }

  if (openSign == LOW) {
    targetPos = OPEN_POS;
  } else if (closedSign == LOW) {
    targetPos = CLOSED_POS;
  } else {
    return;
  }
  Serial.print("nowPos");
  Serial.println(nowPos);
  Serial.print("targetPos:");
  Serial.println(targetPos);
 
  

  
  //動く距離が10より小さかったらモーターを止める
  if (abs(targetPos - nowPos) < 10) {
    stopMotor();

  //目的ポジションより現在ポジションのほうが小さく、かつ移動距離が開閉距離より小さかった場合
  } else if ((targetPos > nowPos) && (abs(targetPos - nowPos) < MAX_RANGE)) {
    Serial.println("targetPos > nowPos");
    Serial.print("targetPos:");
    Serial.println(targetPos);
    Serial.print("nowPos:");
    Serial.println(nowPos);
    Serial.print("targetPos - nowPos");
    targetRange = abs(targetPos - nowPos);
    Serial.println(targetRange);
    Serial.print("MAX_RANGE");
    Serial.println(MAX_RANGE);
    startSolenoid();
    digitalWrite(MOTOR_REVERSE, HIGH);
    while (abs(targetPos - nowPos) >= 10) {
      getPositionMeter();
      getMotorFault();
    }
    stopMotor();
    Serial.print("nowPos:");
    Serial.println(nowPos);
    Serial.println();
  //目的ポジションより現在ポジションのほうが大きく、かつ移動距離が開閉距離より小さかった場合
  } else if ((targetPos < nowPos) && (abs(targetPos - nowPos) < MAX_RANGE)) {
    Serial.println("targetPos < nowPos");
    Serial.print("targetPos:");
    Serial.println(targetPos);
    Serial.print("nowPos:");
    Serial.println(nowPos);
    Serial.print("targetPos - nowPos");
    targetRange = abs(targetPos - nowPos);
    Serial.println(targetRange);
    Serial.print("MAX_RANGE");
    Serial.println(MAX_RANGE);
    startSolenoid();
    digitalWrite(MOTOR_NORMAL, HIGH);
    while (abs(targetPos - nowPos) >= 10) {
      getPositionMeter();
      getMotorFault();
    }
    stopMotor();
    Serial.print("nowPos:");
    Serial.println(nowPos);
    Serial.println();
  } else {
    Serial.println("Max Range Error");
  }
}

void getKeySign() {
  openSign = digitalRead(OPEN_SIGN);
  closedSign = digitalRead(CLOSED_SIGN);
  settingSign = digitalRead(SETTING_BTN);
}

void getPositionMeter() {
  nowPos = analogRead(POSITIONMETER);
}

void stopMotor() {
  digitalWrite(MOTOR_NORMAL, LOW);
  digitalWrite(MOTOR_REVERSE, LOW);
  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, LOW);
  endSolenoid();
}

void startSolenoid() {
  digitalWrite(SOLENOID_NORMAL, HIGH);
  digitalWrite(SOLENOID_REVERSE, LOW);
  delay(200);
  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, LOW);
}

void endSolenoid() {
  startSolenoid();
  delay(200);
  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, HIGH);
  delay(200);
  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, LOW);
}

void getMotorFault() {
  if (digitalRead(SOLENOID_FAULT) == LOW) {
    Serial.println("Solenoid Error");
  }
  if (digitalRead(MOTOR_FAULT) == LOW) {
    Serial.println("motor Error");
  }
}
