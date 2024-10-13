# Starter Kit Pump Module

## Description
STL files, original sldprt files, PCB design files, and firmware for the Starter Kit Pump Module

This module uses six stepper-motor driven peristaltic pumps, manufactured by Takasago Fluidic Systems.  Specifically: [Stepper Motor Type of Ring Pump RP-QⅡ/RP-QⅢ Series [Discharge Rate: 0.18 ~ 180 μL/min]](https://www.takasago-fluidics.com/products/rp-q-stepper-2?variant=45644386238630)

A custom, small form factor, pump driver PCB is also included here. An Arduino Nano 33 IoT is mounted on this driver and controls the module. The provided firmware allows control of the pumps via BLE.

Design files for a PMMA Pump Block Base that makes fluidic contact between the pumps and the FCB are also included.

Six unpopulated spaces for through-hole resistors are left in the PCB for pump-specific feedback resistors to be added.  These resistors control the current sourced by the driver and must be added during or after manufacturing.  For the pumps listed here, a resistor value of 56 kOhm was used.

The PCB is connected to the PMMA carrier with M2 PCB standoffs.

A photo of the assembled module is shown below to assist with assembly. Note that the PCB is conformal coated with epoxy before use in high humidity settings.

![PumpModulePicture](https://github.com/user-attachments/assets/e27e87b6-33cc-46fd-b079-faf1f96512c1)


## Credits

### Designer Credits:
Aniruddha Paul
Eric Safai
Joshua Loessberg-Zahl

### Manufacturing Credits:
This design was manufactured in-house at University of Twente.  PCB manufacturing was outsourced to both JLCPCB and Eurocircuits.

### Extra Credits:
The design work was done at University of Twente in the Netherlands, supported by the university’s Organ-on-Chip Centre Twente (www.utwente.nl/oocct), specifically at the groups: Applied Stem Cell Technologies and BIOS Lab-on-a-Chip Group. 

The work was funded by SMART OoC, an NWO-TTW Perspective Programme of the Dutch Research Council (NWO; project number P19-03), and the Netherlands Organ-on-Chip Initiative, an NWO Gravitation project (024.003.001) by the Ministry of Education, Culture and Science of the government of the Netherlands.

