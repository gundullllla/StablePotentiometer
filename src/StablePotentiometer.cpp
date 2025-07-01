#include "StablePotentiometer.h"

Potentiometer::Potentiometer(int pin, float alpha) 
    : _pin(pin), _alpha(alpha) {
    _valueBuffer = new float[_windowSize]();
    pinMode(_pin, INPUT);
}

Potentiometer::~Potentiometer() {
    delete[] _valueBuffer;
}

void Potentiometer::configureADC() {
    #ifdef ESP32
    analogReadResolution(12);
    analogSetAttenuation(ADC_6db);
    #endif
}

float Potentiometer::read() const { return _currentValue; }
float Potentiometer::raw() const { return analogRead(_pin); }

void Potentiometer::resizeBuffer(int newSize) {
    if(_windowSize == newSize) return;
    
    float* newBuffer = new float[newSize]();
    const int copyCount = std::min(_windowSize, newSize);
    
    for(int i = 0; i < copyCount; i++) {
        int idx = (_bufferIndex + i) % _windowSize;
        newBuffer[i] = _valueBuffer[idx];
    }
    
    delete[] _valueBuffer;
    _valueBuffer = newBuffer;
    _windowSize = newSize;
    _bufferIndex = copyCount % newSize;
}

void Potentiometer::setAlgorithm(int algoId) {
    switch(static_cast<TouchControl::Algorithm>(algoId)) {
        case TouchControl::EXPONENTIAL: resizeBuffer(1); break;
        case TouchControl::MOVING_AVG: resizeBuffer(5); break;
        case TouchControl::MEDIAN_5: resizeBuffer(5); break;
        case TouchControl::MEDIAN_9: resizeBuffer(9); break;
        case TouchControl::MEDIAN_13: resizeBuffer(13); break;
        case TouchControl::KALMAN_LITE: resizeBuffer(1); break;
        case TouchControl::COMBINED: resizeBuffer(5); break;
    }
}

// Filter Implementations
void Potentiometer::updateExponential(int raw) {
    _currentValue = _alpha * raw + (1 - _alpha) * _currentValue;
}

void Potentiometer::updateMovingAvg(int raw) {
    _valueBuffer[_bufferIndex] = raw;
    _bufferIndex = (_bufferIndex + 1) % _windowSize;
    _currentValue = std::accumulate(_valueBuffer, _valueBuffer + _windowSize, 0.0f) / _windowSize;
}

void Potentiometer::updateMedian(int raw) {
    _valueBuffer[_bufferIndex] = raw;
    _bufferIndex = (_bufferIndex + 1) % _windowSize;
    
    float temp[_windowSize];
    std::copy(_valueBuffer, _valueBuffer + _windowSize, temp);
    std::nth_element(temp, temp + _windowSize/2, temp + _windowSize);
    _currentValue = temp[_windowSize/2];
}

void Potentiometer::updateKalman(int raw) {
    static float estimate = 0.0f;
    static float error = 1.0f;
    const float Q = 0.1f;
    const float R = 0.1f;
    
    error += Q;
    const float gain = error / (error + R);
    estimate += gain * (raw - estimate);
    error *= (1 - gain);
    
    _currentValue = estimate;
}

void Potentiometer::updateCombined(int raw) {
    updateMedian(raw);
    updateExponential(_currentValue);
    updateMovingAvg(_currentValue);
}

// TouchControl Implementation
namespace TouchControl {
    int _touchPin = -1;
    bool _lastTouchState = false;
    Algorithm _currentAlgo = EXPONENTIAL;

    void begin(int touchPin) {
        _touchPin = touchPin;
        #ifdef ESP32
        touchAttachInterrupt(_touchPin, nullptr, 20);
        #endif
    }

    bool checkRelease() {
        if(_touchPin < 0) return false;
        
        #ifdef ESP32
        const bool currentState = touchRead(_touchPin) < 20;
        #else
        const bool currentState = false;
        #endif
        
        if(_lastTouchState && !currentState) {
            _currentAlgo = static_cast<Algorithm>((static_cast<int>(_currentAlgo) + 1) % ALGO_COUNT);
            _lastTouchState = currentState;
            return true;
        }
        _lastTouchState = currentState;
        return false;
    }

    const char* getAlgorithmName() {
        static const char* names[] = {
            "Exponential", "Moving Avg", "Median 5", 
            "Median 9", "Median 13", "Kalman", "Combined"
        };
        return names[_currentAlgo];
    }

    Algorithm currentAlgorithm() { return _currentAlgo; }
}