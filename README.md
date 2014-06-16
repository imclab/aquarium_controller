Aquarium Controller
===================

A simple, Arduino based freshwater aquarium controller.

Version: Alpha 4
Author: Chris Lüscher
Current features:
- Measure: pH, O2, temperature
- Report: pH, O2, temperature, internal time, Arduino RAM usage, fan speed
- Fertilize: 3 fertilizer pumps
- Control: 5 RC power switches
- Cooling: control one set of 12v pc case cooling fans
- Display: 16x2 LCD display

Disclaimer: this is a personal project. Pumping stuff into your aquarium is DANGEROUS and so is controlling your aquarium hardware via RC switches. Use this project as an inspiration only if you know what you are doing. I provide this information 'as is' without warranty of any kind.

## Contents

*documentation*
A list of the parts I used and how they need to be connected.

*sketch_aquarium_a4*
The main Arduino sketch.

*logger*
Some very basic Python scripts used to monitor Arduino output and produce a simple status website.

*calibrate_probes*
*calibrate_pump*
*flush_pumps*
*set_clock*
Some helper scripts.

*LeanAlarms*
*lean_alarm_test*
*lean_alarm_challenger*
RTC Alarm library that is a little leaner than the default library and that allows millisecond-precise timers for the fertilizer pumps.
