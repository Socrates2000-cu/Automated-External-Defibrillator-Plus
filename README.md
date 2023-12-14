# Automated-External-Defibrillator-Plus
Automated External Defibrillator Plus system simulation

## Video
https://youtu.be/KQlV2cWFG3Y

## Team Members
- Frances Fang
  - Program Logic - Heart rhythm analysis, shock delivery, multi-thread function invocation, AED power off, merging steps and added safety checks in steps.
  - Class Design - MainWindow, AED, Patient, Electrode.
  - Diagrams - UML and use case diagram.
- Sore Adedotun
  - Program Logic - AED startup, self-test, battery, electrode check and initial states of the AED along with signals/slots required for this stage.
  - Class Design - MainWindow, AED.
  - Diagrams - Safety scenarios and state diagram.
- Sukhrobjon Eshmirzaev - 101169793
  - Program Logic - Simulated delivering CPR by analayzing CPR depth to deliver at proper depth, giving real-time CPR feedback, updating CPR depth,
  - Class Design - AED, MainWindow
  - Diagrams - No shockable case sequence diagram, Main use case,
- Xuejiao Qing
  - UI
  - Program Logic
  - Class Design
  - Diagrams

## Organization of submission
- Documentation - use cases, demo video, UML diagrams
- final_project_team32 - all header files, source files and images required to run the program

## Design Explanation
Our overall design decision takes into account what the AED has 7 states in total. It has a "shut down" state, a "start-up" state, and five states corresponding to the five steps.

We write function calls in a "chain", with the help of Qt signal and slots. Except for the first two steps which don't have some real analysis, we connect the other steps (3-5) with signals. Once a step is finished, it calls the signal to trigger the next step. The advantage is these "linked-list" like function "chains" are more flexible, if sometime in the treatment the connection is lost, or if the shock is not advised, we can easily decide the next step.

We also applied model-view separation. Our main window class acts as the view and controller, whereas the AED class acts as the model. The AED does the Cardiac Arrhythmia Detection on the (connected) patient and gives Real-Time CPR Feedback. The Mainwindow prints voice prompts, updates GUI components including LCD, and handles user simulations.

The approach to the power and start-up sequence was chosen to be an automated one which initiates the self-test on power up and on the occasion of a failed self-test, prompts the user about the possible reasons, gives them time to act, and then prompts them to restart the device as it powers down automatically. This approach is popular for most electrical devices or machinery and is safe for making changes to connections or power on an active device.