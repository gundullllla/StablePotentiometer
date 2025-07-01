# StablePotentiometer Library

Advanced filtering for stable potentiometer readings on ESP32.

## Features
- 7 filtering algorithms
- Touch-controlled mode switching
- Configurable parameters
- Automatic ADC calibration

## Installation
1. Download ZIP
2. Arduino IDE: Sketch > Include Library > Add .ZIP Library
3. Select downloaded file

## Basic Usage
```cpp
#include <StablePotentiometer.h>

Potentiometer pot(15);  // GPIO pin

void setup() {
  pot.configureADC();
}

void loop() {
  int raw = pot.raw();
  pot.updateExponential(raw);  // Apply filter
  float filtered = pot.read();
}
```

## Examples
1. **BasicReading** - Simple filtered reading
2. **FullDemo** - Complete demo with touch control

## Filter Algorithms
1. Exponential Smoothing
2. Moving Average
3. Median (5/9/13 samples)
4. Kalman Filter
5. Combined (Median+Exponential+MovingAvg)
