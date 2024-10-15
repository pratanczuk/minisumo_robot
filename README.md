# Minisumo Robot Project

A project for a Minisumo robot that complies with the official [Robothon Sumo Robot Competition Rules](https://robothon.org/rules-sumo/). This robot is designed to autonomously detect and push opponents out of the ring, with optional manual control via a PS2 controller for debugging purposes.

## Features

-   **Official Compliance**: Built according to the official Minisumo competition rules.
-   **Autonomous Operation**: Uses sensors and algorithms to detect opponents and navigate the ring.
-   **PS2 Controller Support**: Optional control for debugging and manual operation.
-   **Modular Codebase**: Organized into separate modules for easy understanding and modification.
-   **Documentation and Schematics**: Includes PCB designs and detailed documentation.

## Repository Structure

-   `default/` and `doc/`: Documentation and additional resources.
-   `pcb/`: PCB schematics and layouts for the robot.
-   Source Code Files:
    -   `eeprom.c/h`: EEPROM handling for storing configuration.
    -   `engines.c/h`: Motor control functions.
    -   `fsm.h`: Finite State Machine definitions.
    -   `hc_sr04.c/h`: Ultrasonic sensor integration.
    -   `psx_pad.c/h`: PS2 controller interface.
    -   `trace.c/h`: Debugging utilities.
    -   `usart.c/h`: Serial communication functions.
-   Project Files:
    -   `minisumo1.aps` and `minisumo1.aws`: Project files for the development environment.
    -   `minisumo1.c`: Main source code for the robot's operation.
-   Scripts:
    -   `flash_atmega32.bat`: Script for flashing the firmware to the microcontroller.

## Getting Started

1.  **Clone the Repository**:
    
    bash
    
    Skopiuj kod
    
    `git clone https://github.com/pratanczuk/minisumo_robot.git` 
    
2.  **Hardware Setup**:
    
    -   Assemble the robot according to the provided PCB designs.
    -   Connect sensors, motors, and optional PS2 controller as per the schematics.
3.  **Software Setup**:
    
    -   Open the project using the provided project files (`.aps` and `.aws`).
    -   Compile the source code using your preferred AVR development environment.
    -   Use `flash_atmega32.bat` or your flashing tool to upload the firmware.

## Demonstrations

Check out videos from the robot's first fights:

-   [Fight 1](https://www.youtube.com/watch?v=1EaqE86ZQDg)
-   [Fight 2](https://www.youtube.com/watch?v=W36dLinUjCs)
-   [Fight 3](https://www.youtube.com/watch?v=mWIT7kKi6hg)
-   [Fight 4](https://www.youtube.com/watch?v=_5zr2T7BA24)
-   [Fight 5](https://www.youtube.com/watch?v=7P6vkmQnmVI)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

For any questions or suggestions, feel free to open an issue or contact the project maintainers.
