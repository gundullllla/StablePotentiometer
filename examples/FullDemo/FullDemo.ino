#include <StablePotentiometer.h>

#define POT_PIN 15
#define TOUCH_PIN T0

Potentiometer pot(POT_PIN);
constexpr int HISTORY_SIZE = 500;
float valueHistory[HISTORY_SIZE] = {0};
unsigned long timeHistory[HISTORY_SIZE] = {0};
int historyIndex = 0;

void setup() {
    Serial.begin(115200);
    pot.configureADC();
    TouchControl::begin(TOUCH_PIN);
    Serial.println("\nStable Potentiometer Library Demo");
    Serial.println("Touch the sensor to cycle filters");
}

void loop() {
    static unsigned long lastPrint = 0;
    constexpr unsigned long printInterval = 50;
    const int rawValue = pot.raw();
    const unsigned long currentMillis = millis();

    if(TouchControl::checkRelease()) {
        const int newAlgo = static_cast<int>(TouchControl::currentAlgorithm());
        pot.setAlgorithm(newAlgo);
        Serial.printf("\n[Mode] %s\n", TouchControl::getAlgorithmName());
    }

    switch(TouchControl::currentAlgorithm()) {
        case TouchControl::EXPONENTIAL:
            pot.updateExponential(rawValue);
            break;
        case TouchControl::MOVING_AVG:
            pot.updateMovingAvg(rawValue);
            break;
        case TouchControl::MEDIAN_5:
            pot.updateMedian(rawValue);
            break;
        case TouchControl::MEDIAN_9:
            pot.updateMedian(rawValue);
            break;
        case TouchControl::MEDIAN_13:
            pot.updateMedian(rawValue);
            break;
        case TouchControl::KALMAN_LITE:
            pot.updateKalman(rawValue);
            break;
        case TouchControl::COMBINED:
            pot.updateCombined(rawValue);
            break;
    }

    valueHistory[historyIndex] = pot.read();
    timeHistory[historyIndex] = currentMillis;
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;

    if(currentMillis - lastPrint >= printInterval) {
        float maxVal = -INFINITY, minVal = INFINITY;
        for(int i = 0; i < HISTORY_SIZE; i++) {
            if(currentMillis - timeHistory[i] <= 5000) {
                maxVal = fmax(maxVal, valueHistory[i]);
                minVal = fmin(minVal, valueHistory[i]);
            }
        }
        
        Serial.printf("%s: %.1f | Raw: %d | 5sΔ: %.1f | R-SΔ: %.1f\n", 
                     TouchControl::getAlgorithmName(),
                     pot.read(), 
                     rawValue,
                     maxVal - minVal,
                     rawValue - pot.read());
        lastPrint = currentMillis;
    }
    
    delay(10);
}