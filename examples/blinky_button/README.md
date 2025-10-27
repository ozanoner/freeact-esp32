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

The example uses configurable GPIO assignments through ESP-IDF's menuconfig system:

**Default GPIO Assignments:**
- **LED0 (Red)**: GPIO 18 - Button feedback LED
- **LED1 (Blue)**: GPIO 19 - Blinking pattern LED  
- **Button**: GPIO 22 - User input (active low with internal pull-up)

**Configuration:**
GPIO pins can be customized via `idf.py menuconfig` under "Blinky Button Example" menu.

**Hardware Setup:**
- LEDs: Connect with appropriate current limiting resistors
- Button: Connected between GPIO and GND (internal pull-up enabled)

## Behavior

### LED1 (Blinking)
- **Pattern**: Asymmetric blinking
- **ON period**: 200ms (short flash)
- **OFF period**: 800ms (long pause)
- **Automatic**: Starts immediately when the application launches

### LED0 (Button Feedback) 
- **ON**: When button is pressed
- **OFF**: When button is released
- **Debounced**: Uses ESP-IDF iot_button component for reliable operation

### Button Events
- **BUTTON_PRESSED_SIG**: Generated on button press using iot_button component
- **BUTTON_RELEASED_SIG**: Generated on button release using iot_button component
- **Debouncing**: Automatic debouncing handled by ESP-IDF iot_button component

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

### Memory Allocation

- **Stack**: `configMINIMAL_STACK_SIZE * 2` (static allocation)
- **Queue**: 10 events (static allocation)
- **Priority**: 1 (lowest user priority above idle)

### Dependencies

- **ESP-IDF**: v5.0 or higher
- **FreeAct ESP32**: Main component
- **ESP-IDF iot_button**: v4.1.4+ for reliable button handling

## Building and Running

### Prerequisites

- ESP-IDF v5.0 or higher
- FreeAct ESP32 component installed

### Build Commands

```bash
# Navigate to the example directory
cd examples/blinky_button

# Update the component version
vi main/idf_component.yml

# Configure GPIOs 
idf.py menuconfig
# Navigate to: Component config -> Blinky Button Example

# Build the project
idf.py build

# Flash and monitor
idf.py flash monitor
```

### Component Configuration

The example uses component dependencies defined in `main/idf_component.yml`:
- **FreeAct ESP32**: Main Active Object framework  
- **ESP-IDF iot_button**: Professional button handling with debouncing

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

### 3. Professional Button Handling
- ESP-IDF iot_button component integration with Active Objects
- Automatic debouncing and noise filtering
- Task-context event posting (not ISR) using `Active_post()`
- Event-driven callback mechanism


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

```bash
idf.py menuconfig
# Navigate to: Component config -> Blinky Button Example
# Configure: Red LED GPIO, Blue LED GPIO, Button GPIO
```

## Troubleshooting

### LEDs Not Working
- Check GPIO pin assignments in BSP
- Verify LED polarity and current limiting resistors
- Ensure adequate power supply

### Button Not Responding
- Verify button wiring (connect between GPIO and GND)
- Check GPIO configuration in menuconfig
- Ensure button is active-low (pressed = connected to GND)
- Verify iot_button component is working in logs

### Build Errors
- Ensure ESP-IDF v5.0+ is installed
- Verify FreeAct component is properly installed  
- Check component dependencies in `main/idf_component.yml`
- Ensure iot_button component v4.1.4+ is available

## Advanced Features

This example can be extended to demonstrate:
- **Multiple Active Objects**: Add more AOs for complex interactions
- **State Machines**: Implement hierarchical state machines within the dispatch function
- **Inter-AO Communication**: Show how Active Objects communicate with each other

## License

This example is part of the FreeAct ESP32 port and follows the same MIT License as the main component.