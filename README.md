# ESP32-CAM Hazard Detection Robot

University team project involving a sensor-equipped mobile robot and an ESP32-CAM module. The prototype reads environmental and obstacle sensors, responds to configured thresholds, and uses Telegram to send a location message and a captured photo when a danger condition is detected.

## My contributions

- Developed the ESP32-CAM camera code and completed its hardware wiring.
- Collaborated with a teammate on the mobile robot control code.
- The teammate assembled and wired the robot car hardware.

This was formally a group project; the contributions listed above are my own. The two code components are included as project artifacts, not as a claim that I individually implemented every component.

## Hardware and software

- ESP32-CAM and an ESP32-based robot controller
- DHT11 temperature/humidity sensor, MQ-2 gas sensor, ultrasonic distance sensor, GPS receiver, buzzer, and DC motor control
- Arduino C++, Wi-Fi, Telegram Bot API, and UART communication between the controllers

## Repository files

- `camera/ESP32_CAM_Telegram_Photo/ESP32_CAM_Telegram_Photo.ino` — ESP32-CAM photo capture and Telegram upload.
- `robot/ESP32_Hazardous_Area_Monitoring_Robot/ESP32_Hazardous_Area_Monitoring_Robot.ino` — motor control, sensor monitoring, GPS/Telegram location reporting, and camera trigger.

## Configuration and safety

**Credentials have been replaced with placeholders.** Before running, provide your own Wi-Fi name/password, Telegram bot token, and chat ID in *both* sketches. Never commit real credentials to a public repository. Any previously exposed tokens should be revoked or rotated; redaction does not invalidate them.

This is an archived educational prototype, **not a validated or safety-certified hazard detection system**. The thresholds and movement behavior are experiment-specific and have not been revalidated for another vehicle. The code is shared for portfolio documentation; compilation and end-to-end operation have not been independently verified in the current review.

**Implementation considerations:** The camera sketch calls `client.setInsecure()`, which disables TLS certificate verification and should be replaced with certificate validation before real deployment. The camera sketch also uses UART0 for both debug logging and communication; pin/serial conflicts may require adjustment. The vehicle sketch sends placeholder coordinates `(0, 0)` when GPS data is invalid. Check and correct these behaviors before operational use.

## Source history

The `.ino` sketches were extracted from the original project Word documents, with hard-coded credentials redacted. No hardware modifications or new project results are claimed.
