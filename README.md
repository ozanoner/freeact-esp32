# FreeAct for ESP32

[![Component Registry](https://components.espressif.com/components/ozanoner/freeact-esp32/badge.svg?version=latest)](https://components.espressif.com/components/ozanoner/freeact-esp32)

A port of the [FreeAct library](https://github.com/QuantumLeaps/FreeACT) for ESP32 microcontrollers, providing a lightweight Active Object pattern implementation based on FreeRTOS.

## Overview

FreeAct is a free, open-source Active Object pattern implementation based on FreeRTOS. This component ports the original FreeAct library to ESP32, enabling event-driven programming with active objects on ESP32 platforms.

The Active Object pattern encapsulates:
- State behavior
- Event queue  
- Event loop
- Thread of execution

This results in inherently thread-safe objects that communicate via asynchronous message passing.

## Features

- ✅ **Lightweight**: Minimal memory footprint
- ✅ **Thread-safe**: Built on FreeRTOS primitives
- ✅ **Event-driven**: Asynchronous message passing
- ✅ **Portable**: Works across all ESP32 variants
- ✅ **Time events**: Built-in timer support
- ✅ **ISR-safe**: Functions for interrupt contexts

## Supported Platforms

- **ESP32** (all variants)

## Requirements

- **ESP-IDF**: v5.0 or higher
- **FreeRTOS**: Included with ESP-IDF


## Quick Start

### Basic Active Object

```c
#include "FreeAct.h"

// Define signals
enum Signals
{
    TIMEOUT_SIG = USER_SIG,
    // other signals
};

// Define the Active Object
typedef struct
{
    Active super; /* inherit Active base class */
    /* add private data for the AO... */
    TimeEvent te;
    // others
} BlinkyButton;

// Define instances
static BlinkyButton blinkyButton;
Active*             AO_blinkyButton = &blinkyButton.super;


// State handler function
static void BlinkyButton_dispatch(BlinkyButton* const me, Event const* const e)
{
    switch (e->sig)
    {
        case INIT_SIG:
        case TIMEOUT_SIG:
    }
}
    
// In your main function
void app_main()
{
    /* create and start the BlinkyButton AO */
    BlinkyButton_ctor(&blinkyButton);
    Active_start(AO_blinkyButton,                                             ///< Active object to start
                 1U,                                                          ///< Task priority (1-based)
                 blinkyButton_queue,                                          ///< event queue storage
                 sizeof(blinkyButton_queue) / sizeof(blinkyButton_queue[0]),  ///< queue length
                 blinkyButton_stack,                                          ///< stack storage
                 sizeof(blinkyButton_stack),                                  ///< stack size in bytes
                 0U);
}

```

### Posting Events

```c
// From task context
Event const buttonEvt = { BUTTON_SIG };
Active_post(&blinky.super, &buttonEvt);
```

## API Reference

### Core Functions

- `Active_ctor()` - Constructor for Active Objects
- `Active_start()` - Start an Active Object
- `Active_post()` - Post event from task context
- `Active_postFromISR()` - Post event from ISR context

### Time Events

- `TimeEvent_ctor()` - Constructor for Time Events  
- `TimeEvent_arm()` - Arm a time event
- `TimeEvent_disarm()` - Disarm a time event

For detailed API documentation, see [`include/FreeAct.h`](include/FreeAct.h).

## Examples

Check the `examples/` directory for complete working examples:

- **Blinky**: Basic LED blinking with time events

## Original FreeAct

This component is based on the original FreeAct library by Quantum Leaps:
- **Repository**: https://github.com/QuantumLeaps/FreeACT
- **License**: MIT
- **Author**: Quantum Leaps, LLC

## License

This ESP32 port maintains the same MIT License as the original FreeAct library.

```
MIT License

Copyright (C) 2020 Quantum Leaps, LLC. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Support

For issues related to this ESP32 port, please open an issue on this repository.
For questions about the original FreeAct library, visit the [Quantum Leaps forum](https://sourceforge.net/p/qpc/discussion/668726).