//
// Created by f.wimmer on 02.06.2023.
//



#include "JoystickBuilder.h"

#define JOYSTICK_INCLUDE_X_AXIS  0b00000001
#define JOYSTICK_INCLUDE_Y_AXIS  0b00000010
#define JOYSTICK_INCLUDE_Z_AXIS  0b00000100
#define JOYSTICK_INCLUDE_RX_AXIS 0b00001000
#define JOYSTICK_INCLUDE_RY_AXIS 0b00010000
#define JOYSTICK_INCLUDE_RZ_AXIS 0b00100000

#define JOYSTICK_INCLUDE_RUDDER      0b00000001
#define JOYSTICK_INCLUDE_THROTTLE    0b00000010
#define JOYSTICK_INCLUDE_ACCELERATOR 0b00000100
#define JOYSTICK_INCLUDE_BRAKE       0b00001000
#define JOYSTICK_INCLUDE_STEERING    0b00010000

JoystickBuilder::JoystickBuilder(uint8_t hidReportId, uint8_t joystickType)
        : _hidReportId(hidReportId), _joystickType(joystickType) {}

JoystickBuilder &JoystickBuilder::includeXAxis(bool include) {
    _includeXAxis = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::includeYAxis(bool include) {
    _includeYAxis = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::includeZAxis(bool include) {
    _includeZAxis = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::includeRxAxis(bool include) {
    _includeRxAxis = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::includeRyAxis(bool include) {
    _includeRyAxis = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::includeRzAxis(bool include) {
    _includeRzAxis = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::includeRudder(bool include) {
    _includeRudder = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::includeThrottle(bool include) {
    _includeThrottle = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::includeAccelerator(bool include) {
    _includeAccelerator = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::includeBrake(bool include) {
    _includeBrake = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::includeSteering(bool include) {
    _includeSteering = include;
    return *this;
}

JoystickBuilder &JoystickBuilder::setButtonCount(uint8_t buttonCount) {
    if (buttonCount >= 64) {
        _buttonCount = 64;
    } else {
        _buttonCount = buttonCount;
    }
    return *this;
}

JoystickBuilder &JoystickBuilder::setHatSwitchCount(uint8_t hatSwitchCount) {
    _hatSwitchCount = hatSwitchCount;
    return *this;
}

uint8_t JoystickBuilder::getHidSize() const {
    // Button
    uint8_t buttonPaddingBits = getButtonPaddingBits();
    // Axis Calculations
    uint8_t axisCount = getAxisCount();
    uint8_t simulationCount = getSimulatorCount();

    int hidReportDescriptorSize = 8;

    if (_buttonCount > 0) {

        hidReportDescriptorSize += 20;

        if (buttonPaddingBits > 0) {
            hidReportDescriptorSize += 6;
        } // Padding Bits Needed

    } // Buttons

    if ((axisCount > 0) || (_hatSwitchCount > 0)) {
        hidReportDescriptorSize += 2;
    }

    if (_hatSwitchCount > 0) {

        hidReportDescriptorSize += 19;
        if (_hatSwitchCount > 1) {
            hidReportDescriptorSize += 19;
        } else {
            hidReportDescriptorSize += 6;
        } // One or Two Hat Switches?

    } // Hat Switches

    if (axisCount > 0) {
        hidReportDescriptorSize += 15;

        if (_includeXAxis) {
            hidReportDescriptorSize += 2;
        }

        if (_includeYAxis) {
            hidReportDescriptorSize += 2;
        }

        if (_includeZAxis) {
            hidReportDescriptorSize += 2;
        }

        if (_includeRxAxis) {
            hidReportDescriptorSize += 2;
        }

        if (_includeRyAxis) {
            hidReportDescriptorSize += 2;
        }

        if (_includeRzAxis) {
            hidReportDescriptorSize += 2;
        }
        hidReportDescriptorSize += 3;

    } // X, Y, Z, Rx, Ry, and Rz Axis

    if (simulationCount > 0) {

        hidReportDescriptorSize += 15;

        if (_includeRudder) {
            hidReportDescriptorSize += 2;
        }

        if (_includeThrottle) {
            hidReportDescriptorSize += 2;
        }

        if (_includeAccelerator) {
            hidReportDescriptorSize += 2;
        }

        if (_includeBrake) {
            hidReportDescriptorSize += 2;
        }

        if (_includeSteering) {
            hidReportDescriptorSize += 2;
        }

        hidReportDescriptorSize += 3;

    } // Simulation Controls

    // END_COLLECTION
    hidReportDescriptorSize += 1;

    return hidReportDescriptorSize;
}

uint8_t JoystickBuilder::getAxisFlags() const {
    uint8_t includeAxisFlags = 0;
    includeAxisFlags |= (_includeXAxis ? JOYSTICK_INCLUDE_X_AXIS : 0);
    includeAxisFlags |= (_includeYAxis ? JOYSTICK_INCLUDE_Y_AXIS : 0);
    includeAxisFlags |= (_includeZAxis ? JOYSTICK_INCLUDE_Z_AXIS : 0);
    includeAxisFlags |= (_includeRxAxis ? JOYSTICK_INCLUDE_RX_AXIS : 0);
    includeAxisFlags |= (_includeRyAxis ? JOYSTICK_INCLUDE_RY_AXIS : 0);
    includeAxisFlags |= (_includeRzAxis ? JOYSTICK_INCLUDE_RZ_AXIS : 0);

    return includeAxisFlags;
}

uint8_t JoystickBuilder::getSimulatorFlags() const {
    uint8_t includeSimulatorFlags = 0;
    includeSimulatorFlags |= (_includeRudder ? JOYSTICK_INCLUDE_RUDDER : 0);
    includeSimulatorFlags |= (_includeThrottle ? JOYSTICK_INCLUDE_THROTTLE : 0);
    includeSimulatorFlags |= (_includeAccelerator ? JOYSTICK_INCLUDE_ACCELERATOR : 0);
    includeSimulatorFlags |= (_includeBrake ? JOYSTICK_INCLUDE_BRAKE : 0);
    includeSimulatorFlags |= (_includeSteering ? JOYSTICK_INCLUDE_STEERING : 0);

    return includeSimulatorFlags;
}

void JoystickBuilder::buildDescriptor(uint8_t *buffer) const {
    // Button
    uint8_t buttonPaddingBits = getButtonPaddingBits();
    // Axis Calculations
    uint8_t axisCount = getAxisCount();
    uint8_t simulationCount = getSimulatorCount();

    int hidReportDescriptorSize = 0;

    // USAGE_PAGE (Generic Desktop)
    buffer[hidReportDescriptorSize++] = 0x05;
    buffer[hidReportDescriptorSize++] = 0x01;

    // USAGE (Joystick - 0x04; Gamepad - 0x05; Multi-axis Controller - 0x08)
    buffer[hidReportDescriptorSize++] = 0x09;
    buffer[hidReportDescriptorSize++] = _joystickType;

    // COLLECTION (Application)
    buffer[hidReportDescriptorSize++] = 0xa1;
    buffer[hidReportDescriptorSize++] = 0x01;

    // REPORT_ID (Default: 3)
    buffer[hidReportDescriptorSize++] = 0x85;
    buffer[hidReportDescriptorSize++] = _hidReportId;

    if (_buttonCount > 0) {

        // USAGE_PAGE (Button)
        buffer[hidReportDescriptorSize++] = 0x05;
        buffer[hidReportDescriptorSize++] = 0x09;

        // USAGE_MINIMUM (Button 1)
        buffer[hidReportDescriptorSize++] = 0x19;
        buffer[hidReportDescriptorSize++] = 0x01;

        // USAGE_MAXIMUM (Button 32)
        buffer[hidReportDescriptorSize++] = 0x29;
        buffer[hidReportDescriptorSize++] = _buttonCount;

        // LOGICAL_MINIMUM (0)
        buffer[hidReportDescriptorSize++] = 0x15;
        buffer[hidReportDescriptorSize++] = 0x00;

        // LOGICAL_MAXIMUM (1)
        buffer[hidReportDescriptorSize++] = 0x25;
        buffer[hidReportDescriptorSize++] = 0x01;

        // REPORT_SIZE (1)
        buffer[hidReportDescriptorSize++] = 0x75;
        buffer[hidReportDescriptorSize++] = 0x01;

        // REPORT_COUNT (# of buttons)
        buffer[hidReportDescriptorSize++] = 0x95;
        buffer[hidReportDescriptorSize++] = _buttonCount;

        // UNIT_EXPONENT (0)
        buffer[hidReportDescriptorSize++] = 0x55;
        buffer[hidReportDescriptorSize++] = 0x00;

        // UNIT (None)
        buffer[hidReportDescriptorSize++] = 0x65;
        buffer[hidReportDescriptorSize++] = 0x00;

        // INPUT (Data,Var,Abs)
        buffer[hidReportDescriptorSize++] = 0x81;
        buffer[hidReportDescriptorSize++] = 0x02;

        if (buttonPaddingBits > 0) {

            // REPORT_SIZE (1)
            buffer[hidReportDescriptorSize++] = 0x75;
            buffer[hidReportDescriptorSize++] = 0x01;

            // REPORT_COUNT (# of padding bits)
            buffer[hidReportDescriptorSize++] = 0x95;
            buffer[hidReportDescriptorSize++] = buttonPaddingBits;

            // INPUT (Const,Var,Abs)
            buffer[hidReportDescriptorSize++] = 0x81;
            buffer[hidReportDescriptorSize++] = 0x03;

        } // Padding Bits Needed

    } // Buttons

    if ((axisCount > 0) || (_hatSwitchCount > 0)) {

        // USAGE_PAGE (Generic Desktop)
        buffer[hidReportDescriptorSize++] = 0x05;
        buffer[hidReportDescriptorSize++] = 0x01;

    }

    if (_hatSwitchCount > 0) {

        // USAGE (Hat Switch)
        buffer[hidReportDescriptorSize++] = 0x09;
        buffer[hidReportDescriptorSize++] = 0x39;

        // LOGICAL_MINIMUM (0)
        buffer[hidReportDescriptorSize++] = 0x15;
        buffer[hidReportDescriptorSize++] = 0x00;

        // LOGICAL_MAXIMUM (7)
        buffer[hidReportDescriptorSize++] = 0x25;
        buffer[hidReportDescriptorSize++] = 0x07;

        // PHYSICAL_MINIMUM (0)
        buffer[hidReportDescriptorSize++] = 0x35;
        buffer[hidReportDescriptorSize++] = 0x00;

        // PHYSICAL_MAXIMUM (315)
        buffer[hidReportDescriptorSize++] = 0x46;
        buffer[hidReportDescriptorSize++] = 0x3B;
        buffer[hidReportDescriptorSize++] = 0x01;

        // UNIT (Eng Rot:Angular Pos)
        buffer[hidReportDescriptorSize++] = 0x65;
        buffer[hidReportDescriptorSize++] = 0x14;

        // REPORT_SIZE (4)
        buffer[hidReportDescriptorSize++] = 0x75;
        buffer[hidReportDescriptorSize++] = 0x04;

        // REPORT_COUNT (1)
        buffer[hidReportDescriptorSize++] = 0x95;
        buffer[hidReportDescriptorSize++] = 0x01;

        // INPUT (Data,Var,Abs)
        buffer[hidReportDescriptorSize++] = 0x81;
        buffer[hidReportDescriptorSize++] = 0x02;

        if (_hatSwitchCount > 1) {

            // USAGE (Hat Switch)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0x39;

            // LOGICAL_MINIMUM (0)
            buffer[hidReportDescriptorSize++] = 0x15;
            buffer[hidReportDescriptorSize++] = 0x00;

            // LOGICAL_MAXIMUM (7)
            buffer[hidReportDescriptorSize++] = 0x25;
            buffer[hidReportDescriptorSize++] = 0x07;

            // PHYSICAL_MINIMUM (0)
            buffer[hidReportDescriptorSize++] = 0x35;
            buffer[hidReportDescriptorSize++] = 0x00;

            // PHYSICAL_MAXIMUM (315)
            buffer[hidReportDescriptorSize++] = 0x46;
            buffer[hidReportDescriptorSize++] = 0x3B;
            buffer[hidReportDescriptorSize++] = 0x01;

            // UNIT (Eng Rot:Angular Pos)
            buffer[hidReportDescriptorSize++] = 0x65;
            buffer[hidReportDescriptorSize++] = 0x14;

            // REPORT_SIZE (4)
            buffer[hidReportDescriptorSize++] = 0x75;
            buffer[hidReportDescriptorSize++] = 0x04;

            // REPORT_COUNT (1)
            buffer[hidReportDescriptorSize++] = 0x95;
            buffer[hidReportDescriptorSize++] = 0x01;

            // INPUT (Data,Var,Abs)
            buffer[hidReportDescriptorSize++] = 0x81;
            buffer[hidReportDescriptorSize++] = 0x02;

        } else {

            // Use Padding Bits

            // REPORT_SIZE (1)
            buffer[hidReportDescriptorSize++] = 0x75;
            buffer[hidReportDescriptorSize++] = 0x01;

            // REPORT_COUNT (4)
            buffer[hidReportDescriptorSize++] = 0x95;
            buffer[hidReportDescriptorSize++] = 0x04;

            // INPUT (Const,Var,Abs)
            buffer[hidReportDescriptorSize++] = 0x81;
            buffer[hidReportDescriptorSize++] = 0x03;

        } // One or Two Hat Switches?

    } // Hat Switches

    if (axisCount > 0) {

        // USAGE (Pointer)
        buffer[hidReportDescriptorSize++] = 0x09;
        buffer[hidReportDescriptorSize++] = 0x01;

        // LOGICAL_MINIMUM (0)
        buffer[hidReportDescriptorSize++] = 0x15;
        buffer[hidReportDescriptorSize++] = 0x00;

        // LOGICAL_MAXIMUM (65535)
        buffer[hidReportDescriptorSize++] = 0x27;
        buffer[hidReportDescriptorSize++] = 0XFF;
        buffer[hidReportDescriptorSize++] = 0XFF;
        buffer[hidReportDescriptorSize++] = 0x00;
        buffer[hidReportDescriptorSize++] = 0x00;

        // REPORT_SIZE (16)
        buffer[hidReportDescriptorSize++] = 0x75;
        buffer[hidReportDescriptorSize++] = 0x10;

        // REPORT_COUNT (axisCount)
        buffer[hidReportDescriptorSize++] = 0x95;
        buffer[hidReportDescriptorSize++] = axisCount;

        // COLLECTION (Physical)
        buffer[hidReportDescriptorSize++] = 0xA1;
        buffer[hidReportDescriptorSize++] = 0x00;

        if (_includeXAxis) {
            // USAGE (X)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0x30;
        }

        if (_includeYAxis) {
            // USAGE (Y)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0x31;
        }

        if (_includeZAxis) {
            // USAGE (Z)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0x32;
        }

        if (_includeRxAxis) {
            // USAGE (Rx)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0x33;
        }

        if (_includeRyAxis) {
            // USAGE (Ry)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0x34;
        }

        if (_includeRzAxis) {
            // USAGE (Rz)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0x35;
        }

        // INPUT (Data,Var,Abs)
        buffer[hidReportDescriptorSize++] = 0x81;
        buffer[hidReportDescriptorSize++] = 0x02;

        // END_COLLECTION (Physical)
        buffer[hidReportDescriptorSize++] = 0xc0;

    } // X, Y, Z, Rx, Ry, and Rz Axis

    if (simulationCount > 0) {

        // USAGE_PAGE (Simulation Controls)
        buffer[hidReportDescriptorSize++] = 0x05;
        buffer[hidReportDescriptorSize++] = 0x02;

        // LOGICAL_MINIMUM (0)
        buffer[hidReportDescriptorSize++] = 0x15;
        buffer[hidReportDescriptorSize++] = 0x00;

        // LOGICAL_MAXIMUM (65535)
        buffer[hidReportDescriptorSize++] = 0x27;
        buffer[hidReportDescriptorSize++] = 0XFF;
        buffer[hidReportDescriptorSize++] = 0XFF;
        buffer[hidReportDescriptorSize++] = 0x00;
        buffer[hidReportDescriptorSize++] = 0x00;

        // REPORT_SIZE (16)
        buffer[hidReportDescriptorSize++] = 0x75;
        buffer[hidReportDescriptorSize++] = 0x10;

        // REPORT_COUNT (simulationCount)
        buffer[hidReportDescriptorSize++] = 0x95;
        buffer[hidReportDescriptorSize++] = simulationCount;

        // COLLECTION (Physical)
        buffer[hidReportDescriptorSize++] = 0xA1;
        buffer[hidReportDescriptorSize++] = 0x00;

        if (_includeRudder) {
            // USAGE (Rudder)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0xBA;
        }

        if (_includeThrottle) {
            // USAGE (Throttle)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0xBB;
        }

        if (_includeAccelerator) {
            // USAGE (Accelerator)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0xC4;
        }

        if (_includeBrake) {
            // USAGE (Brake)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0xC5;
        }

        if (_includeSteering) {
            // USAGE (Steering)
            buffer[hidReportDescriptorSize++] = 0x09;
            buffer[hidReportDescriptorSize++] = 0xC8;
        }

        // INPUT (Data,Var,Abs)
        buffer[hidReportDescriptorSize++] = 0x81;
        buffer[hidReportDescriptorSize++] = 0x02;

        // END_COLLECTION (Physical)
        buffer[hidReportDescriptorSize++] = 0xc0;

    } // Simulation Controls

    // END_COLLECTION
    buffer[hidReportDescriptorSize++] = 0xc0;
}

uint8_t JoystickBuilder::getAxisCount() const {
    return (_includeXAxis)
           + (_includeYAxis)
           + (_includeZAxis)
           + (_includeRxAxis)
           + (_includeRyAxis)
           + (_includeRzAxis);
}

uint8_t JoystickBuilder::getSimulatorCount() const {
    return (_includeRudder)
           + (_includeThrottle)
           + (_includeAccelerator)
           + (_includeBrake)
           + (_includeSteering);
}

uint8_t JoystickBuilder::getButtonPaddingBits() const {
    uint8_t buttonsInLastByte = _buttonCount % 8;
    uint8_t buttonPaddingBits = 0;
    if (buttonsInLastByte > 0) {
        buttonPaddingBits = 8 - buttonsInLastByte;
    }

    return buttonPaddingBits;
}

uint8_t JoystickBuilder::getReportId() const {
    return _hidReportId;
}

uint8_t JoystickBuilder::getButtonCount() const {
    return _buttonCount;
}

uint8_t JoystickBuilder::getSwitchCount() const {
    return _hatSwitchCount;
}
