/*
  Joystick.cpp

  Copyright (c) 2015-2017, Matthew Heironimus

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Joystick.h"
#include "JoystickBuilder.h"


#define JOYSTICK_REPORT_ID_INDEX 7
#define JOYSTICK_AXIS_MINIMUM 0
#define JOYSTICK_AXIS_MAXIMUM 65535
#define JOYSTICK_SIMULATOR_MINIMUM 0
#define JOYSTICK_SIMULATOR_MAXIMUM 65535

#define JOYSTICK_INCLUDE_X_AXIS  B00000001
#define JOYSTICK_INCLUDE_Y_AXIS  B00000010
#define JOYSTICK_INCLUDE_Z_AXIS  B00000100
#define JOYSTICK_INCLUDE_RX_AXIS B00001000
#define JOYSTICK_INCLUDE_RY_AXIS B00010000
#define JOYSTICK_INCLUDE_RZ_AXIS B00100000

#define JOYSTICK_INCLUDE_RUDDER      B00000001
#define JOYSTICK_INCLUDE_THROTTLE    B00000010
#define JOYSTICK_INCLUDE_ACCELERATOR B00000100
#define JOYSTICK_INCLUDE_BRAKE       B00001000
#define JOYSTICK_INCLUDE_STEERING    B00010000


static uint8_t tempHidReportDescriptor[150];

Joystick_::Joystick_(JoystickBuilder &builder) : _hidSubDescriptor(_hidReportDescriptor, builder.getHidSize()) {
    // Set the USB HID Report ID
    _hidReportId = builder.getReportId();

    // Save Joystick Settings
    _buttonCount = builder.getButtonCount();
    _hatSwitchCount = builder.getSwitchCount();
    _includeAxisFlags = builder.getAxisFlags();
    _includeSimulatorFlags = builder.getSimulatorFlags();

    builder.buildDescriptor(_hidReportDescriptor);
    HID().AppendDescriptor(&_hidSubDescriptor);

    // Setup Joystick State
    if (_buttonCount > 64) {
        Serial.println("Unable to use more than 64 buttons");
        _buttonCount = 64;
    }

    if (_buttonCount > 0) {
        _buttonValuesArraySize = _buttonCount / 8;
        if ((_buttonCount % 8) > 0) {
            _buttonValuesArraySize++;
        }
    }

    // Calculate HID Report Size
    _hidReportSize = _buttonValuesArraySize;
    _hidReportSize += (_hatSwitchCount > 0);
    _hidReportSize += (builder.getAxisCount() * 2);
    _hidReportSize += (builder.getSimulatorCount() * 2);

    // Initialize Joystick State
    _xAxis = 0;
    _yAxis = 0;
    _zAxis = 0;
    _xAxisRotation = 0;
    _yAxisRotation = 0;
    _zAxisRotation = 0;
    _throttle = 0;
    _rudder = 0;
    _accelerator = 0;
    _brake = 0;
    _steering = 0;
    for (short &_hatSwitchValue: _hatSwitchValues) {
        _hatSwitchValue = JOYSTICK_HATSWITCH_RELEASE;
    }
    for (int index = 0; index < _buttonValuesArraySize; index++) {
        _buttonValues[index] = 0;
    }
}

void Joystick_::begin(bool initAutoSendState) {
    _autoSendState = initAutoSendState;
    sendState();
}

void Joystick_::end() {
}

void Joystick_::setButton(uint8_t button, uint8_t value) {
    if (value == 0) {
        releaseButton(button);
    } else {
        pressButton(button);
    }
}

void Joystick_::pressButton(uint8_t button) {
    if (button >= _buttonCount) return;

    int index = button / 8;
    int bit = button % 8;

    bitSet(_buttonValues[index], bit);
    if (_autoSendState) sendState();
}

void Joystick_::releaseButton(uint8_t button) {
    if (button >= _buttonCount) return;

    int index = button / 8;
    int bit = button % 8;

    bitClear(_buttonValues[index], bit);
    if (_autoSendState) sendState();
}

void Joystick_::setXAxis(int32_t value) {
    _xAxis = value;
    if (_autoSendState) sendState();
}

void Joystick_::setYAxis(int32_t value) {
    _yAxis = value;
    if (_autoSendState) sendState();
}

void Joystick_::setZAxis(int32_t value) {
    _zAxis = value;
    if (_autoSendState) sendState();
}

void Joystick_::setRxAxis(int32_t value) {
    _xAxisRotation = value;
    if (_autoSendState) sendState();
}

void Joystick_::setRyAxis(int32_t value) {
    _yAxisRotation = value;
    if (_autoSendState) sendState();
}

void Joystick_::setRzAxis(int32_t value) {
    _zAxisRotation = value;
    if (_autoSendState) sendState();
}

void Joystick_::setRudder(int32_t value) {
    _rudder = value;
    if (_autoSendState) sendState();
}

void Joystick_::setThrottle(int32_t value) {
    _throttle = value;
    if (_autoSendState) sendState();
}

void Joystick_::setAccelerator(int32_t value) {
    _accelerator = value;
    if (_autoSendState) sendState();
}

void Joystick_::setBrake(int32_t value) {
    _brake = value;
    if (_autoSendState) sendState();
}

void Joystick_::setSteering(int32_t value) {
    _steering = value;
    if (_autoSendState) sendState();
}

void Joystick_::setHatSwitch(int8_t hatSwitchIndex, int16_t value) {
    if (hatSwitchIndex >= _hatSwitchCount) return;

    _hatSwitchValues[hatSwitchIndex] = value;
    if (_autoSendState) sendState();
}

int Joystick_::buildAndSet16BitValue(bool includeValue, int32_t value, int32_t valueMinimum, int32_t valueMaximum,
                                     int32_t actualMinimum, int32_t actualMaximum, uint8_t dataLocation[]) {
    int32_t convertedValue;
    uint8_t highByte;
    uint8_t lowByte;
    int32_t realMinimum = min(valueMinimum, valueMaximum);
    int32_t realMaximum = max(valueMinimum, valueMaximum);

    if (!includeValue) return 0;

    if (value < realMinimum) {
        value = realMinimum;
    }
    if (value > realMaximum) {
        value = realMaximum;
    }

    if (valueMinimum > valueMaximum) {
        // Values go from a larger number to a smaller number (e.g. 1024 to 0)
        value = realMaximum - value + realMinimum;
    }

    convertedValue = map(value, realMinimum, realMaximum, actualMinimum, actualMaximum);

    highByte = (uint8_t) (convertedValue >> 8);
    lowByte = (uint8_t) (convertedValue & 0x00FF);

    dataLocation[0] = lowByte;
    dataLocation[1] = highByte;

    return 2;
}

int Joystick_::buildAndSetAxisValue(bool includeAxis, int32_t axisValue, int32_t axisMinimum, int32_t axisMaximum,
                                    uint8_t dataLocation[]) {
    return buildAndSet16BitValue(includeAxis, axisValue, axisMinimum, axisMaximum, JOYSTICK_AXIS_MINIMUM,
                                 JOYSTICK_AXIS_MAXIMUM, dataLocation);
}

int Joystick_::buildAndSetSimulationValue(bool includeValue, int32_t value, int32_t valueMinimum, int32_t valueMaximum,
                                          uint8_t dataLocation[]) {
    return buildAndSet16BitValue(includeValue, value, valueMinimum, valueMaximum, JOYSTICK_SIMULATOR_MINIMUM,
                                 JOYSTICK_SIMULATOR_MAXIMUM, dataLocation);
}

void Joystick_::sendState() {
    uint8_t data[_hidReportSize];
    int index = 0;

    // Load Button State
    for (; index < _buttonValuesArraySize; index++) {
        data[index] = _buttonValues[index];
    }

    // Set Hat Switch Values
    if (_hatSwitchCount > 0) {

        // Calculate hat-switch values
        uint8_t convertedHatSwitch[JOYSTICK_HATSWITCH_COUNT_MAXIMUM];
        for (int hatSwitchIndex = 0; hatSwitchIndex < JOYSTICK_HATSWITCH_COUNT_MAXIMUM; hatSwitchIndex++) {
            if (_hatSwitchValues[hatSwitchIndex] < 0) {
                convertedHatSwitch[hatSwitchIndex] = 8;
            } else {
                convertedHatSwitch[hatSwitchIndex] = (_hatSwitchValues[hatSwitchIndex] % 360) / 45;
            }
        }

        // Pack hat-switch states into a single byte
        data[index++] = (convertedHatSwitch[1] << 4) | (B00001111 & convertedHatSwitch[0]);

    } // Hat Switches

    // Set Axis Values
    index += buildAndSetAxisValue(_includeAxisFlags & JOYSTICK_INCLUDE_X_AXIS, _xAxis, _xAxisMinimum, _xAxisMaximum,
                                  &(data[index]));
    index += buildAndSetAxisValue(_includeAxisFlags & JOYSTICK_INCLUDE_Y_AXIS, _yAxis, _yAxisMinimum, _yAxisMaximum,
                                  &(data[index]));
    index += buildAndSetAxisValue(_includeAxisFlags & JOYSTICK_INCLUDE_Z_AXIS, _zAxis, _zAxisMinimum, _zAxisMaximum,
                                  &(data[index]));
    index += buildAndSetAxisValue(_includeAxisFlags & JOYSTICK_INCLUDE_RX_AXIS, _xAxisRotation, _rxAxisMinimum,
                                  _rxAxisMaximum, &(data[index]));
    index += buildAndSetAxisValue(_includeAxisFlags & JOYSTICK_INCLUDE_RY_AXIS, _yAxisRotation, _ryAxisMinimum,
                                  _ryAxisMaximum, &(data[index]));
    index += buildAndSetAxisValue(_includeAxisFlags & JOYSTICK_INCLUDE_RZ_AXIS, _zAxisRotation, _rzAxisMinimum,
                                  _rzAxisMaximum, &(data[index]));

    // Set Simulation Values
    index += buildAndSetSimulationValue(_includeSimulatorFlags & JOYSTICK_INCLUDE_RUDDER, _rudder, _rudderMinimum,
                                        _rudderMaximum, &(data[index]));
    index += buildAndSetSimulationValue(_includeSimulatorFlags & JOYSTICK_INCLUDE_THROTTLE, _throttle, _throttleMinimum,
                                        _throttleMaximum, &(data[index]));
    index += buildAndSetSimulationValue(_includeSimulatorFlags & JOYSTICK_INCLUDE_ACCELERATOR, _accelerator,
                                        _acceleratorMinimum, _acceleratorMaximum, &(data[index]));
    index += buildAndSetSimulationValue(_includeSimulatorFlags & JOYSTICK_INCLUDE_BRAKE, _brake, _brakeMinimum,
                                        _brakeMaximum, &(data[index]));
    index += buildAndSetSimulationValue(_includeSimulatorFlags & JOYSTICK_INCLUDE_STEERING, _steering, _steeringMinimum,
                                        _steeringMaximum, &(data[index]));

    HID().SendReport(_hidReportId, data, _hidReportSize);
}
