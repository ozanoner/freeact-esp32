# BlinkyButton Example

This example demonstrates the FreeAct Active Object pattern with LED blinking and button handling on ESP32.

## Overview

The BlinkyButton example showcases:
- **Event-Driven Architecture**: Using FreeAct Active Objects for clean separation of concerns
- **Time Events**: Demonstration of FreeRTOS timer integration through TimeEvents
- **LED Blinking**: Automated LED1 blinking with asymmetric timing (200ms ON, 800ms OFF)
- **Button Handling**: Immediate LED0 response to button press/release events

## Hardware Requirements

- ESP32 development board (any variant)
- 2 LEDs connected to GPIO pins (configured in BSP)
- 1 button connected to GPIO pin (configured in BSP)
- Pull-up/pull-down resistors as needed

## GPIO Configuration

The example uses a Board Support Package (BSP) that defines:
- **LED0**: Connected to a GPIO pin for button feedback
- **LED1**: Connected to a GPIO pin for blinking pattern
- **Button**: Connected to a GPIO pin with interrupt capability

> **Note**: GPIO pin assignments are defined in the BSP module.

## Behavior

### LED1 (Blinking)
- **Pattern**: Asymmetric blinking
- **ON period**: 200ms (short flash)
- **OFF period**: 800ms (long pause)
- **Automatic**: Starts immediately when the application launches

### LED0 (Button Feedback)
- **ON**: When button is pressed
- **OFF**: When button is released
- **Immediate**: No debouncing delay for instant feedback

### Button Events
- **BUTTON_PRESSED_SIG**: Generated on button press (falling edge)
- **BUTTON_RELEASED_SIG**: Generated on button release (rising edge)

## Code Structure

### Active Object Design

```c
typedef struct {
    Active super;      // Inherit Active base class
    TimeEvent te;      // Time event for LED blinking
    bool isLedOn;      // Current LED1 state tracking
} BlinkyButton;
```

### Event Handling

The `BlinkyButton_dispatch()` function processes:

1. **INIT_SIG**: Initial event to start LED blinking
2. **TIMEOUT_SIG**: Timer expiry to toggle LED1 and re-arm timer
3. **BUTTON_PRESSED_SIG**: Turn LED0 ON immediately
4. **BUTTON_RELEASED_SIG**: Turn LED0 OFF immediately

### FreeRTOS task

- **Stack**: `configMINIMAL_STACK_SIZE * 2`
- **Queue**: 10 events
- **Priority**: 1 (lowest user priority above idle)

## Building and Running

### Prerequisites

- ESP-IDF v5.0 or higher
- FreeAct ESP32 component installed

### Build Commands

```bash
# Navigate to the example directory
cd examples/blinky_button

# Configure the project (optional - uses defaults)
idf.py menuconfig

# Build the project
idf.py build

# Flash and monitor
idf.py flash monitor
```

### Expected Output

```
I (xxx) app: BlinkyButton example start
I (xxx) bsp: BSP initialized
I (xxx) bsp: Button interrupt enabled
```

You should observe:
- LED1 blinking with the specified pattern
- LED0 responding to button presses
- No console output after initialization (normal behavior)

## Key Learning Points

### 1. Active Object Pattern
- Encapsulation of state, behavior, and execution context
- Thread-safe communication via message passing
- Clean separation of event handling logic

### 2. Time Events
- Integration with FreeRTOS timers
- One-shot timer re-arming for periodic behavior
- ISR-safe timer operations

### 3. Interrupt Handling
- GPIO interrupt integration with Active Objects
- Event posting from ISR context using `Active_postFromISR()`
- Proper priority task waking


## Customization

### Timing Adjustment

Modify the timing in `BlinkyButton_dispatch()`:

```c
TimeEvent_arm(&me->te, 200);  // ON period (ms)
TimeEvent_arm(&me->te, 800);  // OFF period (ms)
```

### Adding More Events

1. Define new signals in the enum
2. Add cases to the dispatch function
3. Post events from appropriate contexts

### Hardware Configuration

Modify the BSP files (`bsp.h` and `bsp*.c`) to:
- Change GPIO pin assignments
- Adjust interrupt triggering (edge/level)
- Add more LEDs or buttons

## Troubleshooting

### LEDs Not Working
- Check GPIO pin assignments in BSP
- Verify LED polarity and current limiting resistors
- Ensure adequate power supply

### Button Not Responding
- Verify button wiring and pull-up/pull-down resistors
- Check interrupt configuration in BSP
- Monitor for bounce issues (consider adding debouncing)

### Build Errors
- Ensure ESP-IDF v5.0+ is installed
- Verify FreeAct component is properly installed
- Check include paths and dependencies

## Advanced Features

This example can be extended to demonstrate:
- **Multiple Active Objects**: Add more AOs for complex interactions
- **State Machines**: Implement hierarchical state machines within the dispatch function
- **Inter-AO Communication**: Show how Active Objects communicate with each other

## License

This example is part of the FreeAct ESP32 port and follows the same MIT License as the main component.