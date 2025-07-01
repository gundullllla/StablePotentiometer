#ifndef STABLE_POTENTIOMETER_H
#define STABLE_POTENTIOMETER_H

#include <Arduino.h>
#include <algorithm>
#include <numeric>

class Potentiometer {
    int _pin;
    float _currentValue = 0;
    float _alpha;
    int _bufferIndex = 0;
    float* _valueBuffer = nullptr;
    int _windowSize = 5;

public:
    Potentiometer(int pin, float alpha = 0.2f);
    ~Potentiometer();
    
    float read() const;
    float raw() const;
    void configureADC();
    void resizeBuffer(int newSize);
    
    void updateExponential(int raw);
    void updateMovingAvg(int raw);
    void updateMedian(int raw);
    void updateKalman(int raw);
    void updateCombined(int raw);
    
    void setAlgorithm(int algoId);
};

namespace TouchControl {
    enum Algorithm {
        EXPONENTIAL,
        MOVING_AVG,
        MEDIAN_5,
        MEDIAN_9,
        MEDIAN_13,
        KALMAN_LITE,
        COMBINED,
        ALGO_COUNT
    };

    void begin(int touchPin);
    bool checkRelease();
    const char* getAlgorithmName();
    Algorithm currentAlgorithm();
}

#endif