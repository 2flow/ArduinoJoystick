//
// Created by f.wimmer on 02.06.2023.
//

#ifndef SWITCHCUBEV3_JOYSTICKBUILDER_H
#define SWITCHCUBEV3_JOYSTICKBUILDER_H

#include "cstdint"

class JoystickBuilder {
public:
    JoystickBuilder(uint8_t hidReportId, uint8_t joystickType);

    JoystickBuilder &includeXAxis(bool include);

    JoystickBuilder &includeYAxis(bool include);

    JoystickBuilder &includeZAxis(bool include);

    JoystickBuilder &includeRxAxis(bool include);

    JoystickBuilder &includeRyAxis(bool include);

    JoystickBuilder &includeRzAxis(bool include);

    JoystickBuilder &includeRudder(bool include);

    JoystickBuilder &includeThrottle(bool include);

    JoystickBuilder &includeAccelerator(bool include);

    JoystickBuilder &includeBrake(bool include);

    JoystickBuilder &includeSteering(bool include);

    JoystickBuilder &setButtonCount(uint8_t buttonCount);

    JoystickBuilder &setHatSwitchCount(uint8_t hatSwitchCount);

    uint8_t getHidSize() const;

    uint8_t getAxisFlags() const;

    uint8_t getSimulatorFlags() const;

    void buildDescriptor(uint8_t *buffer) const;

    uint8_t getReportId() const;

    uint8_t getButtonCount() const;

    uint8_t getSwitchCount() const;

    uint8_t getAxisCount() const;

    uint8_t getSimulatorCount() const;

private:
    bool _includeXAxis = false;
    bool _includeYAxis = false;
    bool _includeZAxis = false;
    bool _includeRxAxis = false;
    bool _includeRyAxis = false;
    bool _includeRzAxis = false;
    bool _includeRudder = false;
    bool _includeThrottle = false;
    bool _includeAccelerator = false;
    bool _includeBrake = false;
    bool _includeSteering = false;
    uint8_t _hidReportId;
    uint8_t _joystickType;
    uint8_t _buttonCount = 0;
    uint8_t _hatSwitchCount = 0;


    uint8_t getButtonPaddingBits() const;
};


#endif //SWITCHCUBEV3_JOYSTICKBUILDER_H
