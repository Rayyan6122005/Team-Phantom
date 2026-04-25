# Comprehensive Technical Report: A.R.T.E.M.I.S. v2.0
**Autonomous Radar Thermal Engine Monitoring Illegal Smuggling**

**Prepared For:** PROTONEX 2026 Hackathon  
**Document Version:** 2.0  

---

## 1. Abstract
The A.R.T.E.M.I.S. (Autonomous Radar Thermal Engine Monitoring Illegal Smuggling) project presents a low-latency, full-stack sentry defense system engineered for real-time perimeter monitoring and threat detection. Traditional border and forest security methods rely heavily on human patrols, which are costly, dangerous, and limited in coverage. This project proposes an autonomous, scalable Internet of Things (IoT) network bridging edge-computing microcontrollers with a high-performance, centralized web dashboard. By integrating an Arduino UNO for ultrasonic spatial mapping, an ESP-32 for real-time local telemetry, and an ESP32-CAM for visual evidence logging, the system identifies incursions with sub-100 millisecond latency. The data is pushed via WebSockets to a React-based command center, providing law enforcement with actionable, real-time intelligence.

## 2. Problem and Motivation
Illegal smuggling, wildlife poaching, and timber theft pose critical threats to global biodiversity and border security. These illicit activities often occur in remote, expansive terrains where continuous human surveillance is structurally unfeasible. Consequently, critical response times are delayed, resulting in severe ecological and economic damage.

**Alignment with UN SDG-15: Life on Land**
The primary motivation for A.R.T.E.M.I.S. is deeply rooted in addressing United Nations Sustainable Development Goal 15, which mandates the protection and sustainable management of terrestrial ecosystems and the halting of biodiversity loss. By deploying autonomous sentries in vulnerable forest sectors, A.R.T.E.M.I.S. establishes an early warning infrastructure. This system equips forest officers and border authorities with the necessary technological leverage to preemptively intercept poachers and smugglers, directly contributing to the preservation of fragile ecosystems.

## 3. Methodology
The development of A.R.T.E.M.I.S. followed a modular systems engineering approach, divided into three distinct operational layers: Data Acquisition (Edge), Data Processing (Bridging), and Visualization (Command).

### 3.1 Hardware Integration
The hardware stack was selected to maximize cost-effectiveness while ensuring robust environmental reliability:
*   **Sensor Node (Arduino UNO):** Acts as the primary spatial mapping unit. An HC-SR04 Ultrasonic Sensor is mounted on an SG90 Micro Servo, sweeping across a 180-degree field of view to detect physical anomalies.
*   **Commander Node (ESP-32):** Functions as the local processing brain. It receives raw telemetry from the Arduino via a hardwired TX/RX serial connection, evaluates the data against threat thresholds, and updates a localized 1.8" ST7735 TFT display for on-site personnel.
*   **Optical Node (ESP32-CAM):** Operates independently to prevent blocking the main telemetry loop. Upon receiving a threat trigger, it captures high-resolution imagery and saves it to an onboard SD card, serving as an immutable black box.

### 3.2 Software Stack and Communication Protocols
*   **Firmware:** Developed in C++ using the Arduino IDE, utilizing `Servo.h` for mechanical actuation and `TFT_eSPI` for hardware-accelerated graphics rendering on the local display.
*   **Backend Server:** Engineered using Node.js and Express. It acts as the central router, utilizing the `SerialPort` library to ingest data streams from the ESP-32 and `Socket.io` to establish persistent, bidirectional WebSocket connections with connected clients.
*   **Frontend Dashboard:** Built on React and styled with Tailwind CSS, representing a highly customized tactical military heads-up display (HUD). It leverages HTML5 Canvas to render the incoming radar telemetry smoothly at 60 frames per second without performance degradation.

## 4. Results
The deployed prototype successfully validated the core hypothesis: an edge-to-cloud IoT sentry system can reliably automate perimeter security with near-zero latency.

### 4.1 System Performance
During testing, the system demonstrated a reaction time of under 100 milliseconds from physical breach detection to dashboard visual alert. The integration of an off-screen memory buffer (Canvas) on the ESP-32 TFT display completely eliminated screen tearing and flickering, resulting in a highly stable local readout.

### 4.2 Challenges Overcome
A significant technical hurdle was the serial synchronization between the 5V Arduino logic and the 3.3V ESP-32 logic. By implementing a shared common ground, carefully timed baud rates (115200 bps), and a rigidly structured packet schema (`<ANGLE,DISTANCE,THREAT_LEVEL>`), data corruption and buffer overflows were successfully mitigated. Additionally, offloading the image processing to the standalone ESP32-CAM ensured that the primary radar loop remained unblocked during evidence capture.

## 5. Conclusion
A.R.T.E.M.I.S. effectively demonstrates how low-cost microcontrollers, when paired with a highly optimized full-stack web architecture, can provide enterprise-grade perimeter security. By automating the detection of illegal incursions, the system drastically reduces the danger to human patrols while significantly enhancing response capabilities.

### 5.1 Future Enhancements
Moving forward, the system architecture allows for extensive modular upgrades:
*   **Machine Learning Integration:** Deploying TensorFlow Lite on the ESP32-CAM to autonomously classify targets (differentiating between animals, humans, and vehicles).
*   **Mesh Networking:** Transitioning to an ESP-NOW topology, allowing multiple sentry nodes to communicate across vast forest sectors without relying on centralized Wi-Fi infrastructure.
*   **Active Tracking:** Upgrading the chassis to a 2-DOF pan-tilt system to enable autonomous target locking and following protocols.
