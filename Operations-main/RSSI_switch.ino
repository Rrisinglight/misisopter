const int analogPin1 = 34;  // Верхний аналоговый пин
const int analogPin2 = 35;
const int analogPin3 = 32;
const int analogPin4 = 33;  // Нижний аналоговый пин (USB это низ)
const int ledPin = 2;       // Пин для управления светодиодом
const int pwmPin = 26;      // Пин для управления PWM

const int numSamples = 5;   // Количество измерений для усреднения

int rawValues1[numSamples]; // Массив для хранения последних значений с пинов
int rawValues2[numSamples];
int rawValues3[numSamples];
int rawValues4[numSamples];

int currentSampleIndex = 0; // Индекс текущего измерения

const int pwmFreq = 50;       // Частота PWM (50 Гц)
const int pwmResolution = 8;  // Разрешение PWM (8 бит, значение от 0 до 255)

void setup() {
  Serial.begin(115200);
  
  // Разрешение 12 бит (0-4095)
  analogReadResolution(12);
  pinMode(ledPin, OUTPUT);

  ledcAttach(pwmPin, pwmFreq, pwmResolution); // Новый хуеблядский API
}

int average(int* values, int numValues) {
  int sum = 0;
  for (int i = 0; i < numValues; i++) {
    sum += values[i];
  }
  return sum / numValues;
}

void loop() {
  rawValues1[currentSampleIndex] = analogRead(analogPin1); 
  rawValues2[currentSampleIndex] = analogRead(analogPin2); 
  rawValues3[currentSampleIndex] = analogRead(analogPin3); 
  rawValues4[currentSampleIndex] = analogRead(analogPin4); 

  currentSampleIndex = (currentSampleIndex + 1) % numSamples;

  // Когда массив заполнился проводим усреднение
  if (currentSampleIndex == 0) {
    int avgValue1 = average(rawValues1, numSamples);
    int avgValue2 = average(rawValues2, numSamples);
    int avgValue3 = average(rawValues3, numSamples);
    int avgValue4 = average(rawValues4, numSamples);

    // Максимальное значения среди всех пинов
    int maxAvgValue = max(max(avgValue1, avgValue2), max(avgValue3, avgValue4));

    if (maxAvgValue == avgValue1 || maxAvgValue == avgValue2) {
      digitalWrite(ledPin, HIGH);
      ledcWrite(pwmPin, 13);
    } 
    else if (maxAvgValue == avgValue3 || maxAvgValue == avgValue4) {
      digitalWrite(ledPin, LOW);
      ledcWrite(pwmPin, 26);
    }

    Serial.print("Value (34): ");
    Serial.print(avgValue1);
    Serial.print(" | Value (35): ");
    Serial.print(avgValue2);
    Serial.print(" |||||||");
    Serial.print("| Value (32): ");
    Serial.print(avgValue3);
    Serial.print(" | Value (33): ");
    Serial.print(avgValue4);
    Serial.print(" | Max: ");
    Serial.println(maxAvgValue);
  }

  // Задержка между измерениями
  delay(300);
}
