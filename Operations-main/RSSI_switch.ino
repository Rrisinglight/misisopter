const int analogPin1 = 34;   // Верхний аналоговый пин
const int analogPin2 = 35;
const int pwmPin = 26;     // Пин для управления PWM

const int numSamples = 5;       // Количество измерений для усреднения

int rawValuesLeft[numSamples];  // Массив для хранения последних значений с пинов 
int rawValuesRight[numSamples];

int currentSampleIndex = 0; // Индекс текущего измерения

const int pwmFreq = 50;       // Частота PWM (50 Гц)
const int pwmResolution = 8;  // Разрешение PWM (8 бит)
const int pwmChannel = 0;

const int servoMinUs = 800;
const int servoMaxUs = 2500;
const int servoAngleRange = 180;

void setup() {
  Serial.begin(115200);
  
  // Разрешение 12 бит (0-4095)
  analogReadResolution(12);
  ledcAttach(pwmPin, pwmFreq, pwmResolution);
  
  setServoAngle(90);
}

int average(int* values, int numValues) {
  long sum = 0;
  for (int i = 0; i < numValues; i++) {
    sum += values[i];
  }
  return sum / numValues;
}

void setServoAngle(int angle) {
  int us = map(angle, 0, 180, servoMinUs, servoMaxUs);

  uint32_t duty = (us * ((1 << pwmResolution) - 1)) / (1000000 / pwmFreq);
  
  ledcWrite(pwmChannel, duty);
}

void loop() {
  rawValuesLeft[currentSampleIndex] = analogRead(analogPin1); 
  rawValuesRight[currentSampleIndex] = analogRead(analogPin2); 

  currentSampleIndex = (currentSampleIndex + 1) % numSamples;

  // Когда массив заполнился проводим усреднение
  if (currentSampleIndex == 0) {
    int avgValueLeft = average(rawValuesLeft, numSamples);
    int avgValueRight = average(rawValuesRight, numSamples);

    // разница сигналов
    int rssiDiff = avgValueLeft - avgValueRight;

    static int servoAngle = 90;
    float Kp = 0.05;       

    int adjustment = Kp * rssiDiff;

    servoAngle -= adjustment;   // плавная корректировка угола сервопривода

    setServoAngle(servoAngle);

    Serial.print("Left: ");
    Serial.print(avgValueLeft);
    Serial.print(" | Right: ");
    Serial.print(avgValueRight);
    Serial.print(" | Diff: ");
    Serial.print(rssiDiff);
    Serial.print(" | Servo Angle: ");
    Serial.println(servoAngle);
  }

  delay(50);
}
