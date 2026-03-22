Overview:
This project is a secure electronic voting system built using Arduino that uses dual fingerprint authentication for enhanced security. It ensures that only authorized users can cast votes by verifying both voter and officer fingerprints. The system provides real-time feedback through an LCD display and ensures transparency and reliability in the voting process.

Problem:
Traditional voting systems may face issues such as unauthorized access, identity fraud, and lack of secure verification mechanisms. Ensuring that only valid users can vote is a critical challenge.

Solution:
The system uses fingerprint sensors to authenticate users before allowing them to vote. Both voter and officer verification are required to proceed, adding an extra layer of security. The system is programmed using Embedded C and integrates LCD feedback, buzzer alerts, and button inputs to create a secure and user-friendly voting mechanism.

Features:
1. Dual fingerprint authentication
2. Secure voting mechanism
3. Real-time LCD feedback
4. Buzzer and LED indicators
5. Controlled voting access

Technologies Used:
Software:
1. Embedded C code

Hardware:
1. Arduino Uno
2. Fingerprint Sensors
3. LCD Display (I2C)
4. Push Buttons
5. Buzzer
6. LEDs

System Workflow:
1. User places fingerprint for verification
2. System checks stored fingerprint data
3. Officer verification is performed
4. If both match, voting is enabled
5. User casts vote using buttons
6. System confirms vote via LCD and buzzer

My Contribution:
1. Developed system logic using Embedded C
2. Implemented fingerprint authentication flow
3. Integrated LCD feedback and buzzer alerts
4. Assisted in hardware integration and system design
