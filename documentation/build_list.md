# Connections
- Atlas Scientific Probes (pH, DO): connected according to documentation/data sheet, easy plug-in/plug-out via BNC, pins see script
- Atlas Scientific temperature sensor: connected according to documentation/data sheet, pin see script. I soldered a servo cable to a 3-pin header and connected this header to the breadboard, then I soldered another 3-pin header to the temperature sensor cable and can now easily plug in/plug out the sensor
- Peristaltic pumps: connected to M1, M2, M3 of the motor shield, motor shield set to VIN (12V) power
- I used cheap CO2 valves from which I removed the actual valve to connect the peristaltic pump tube to CO2 tubing running from the fertilizer bottles to the pumps, then from the pumps to the aquarium. Using non-transparent tubing is a good idea (breakdown of weakly chelated fertilizers). The fertilizer outlet tubes should ideally end above the aquarium water line to prevent water backflow (there is however only a tiny chance of backflow since the peristaltic pumps will block any liquid flow when not in operation)
- Wenshing RF module connected according to data sheet, 23cm copper cable as antenna. (I learned in the meantime that 17.3cm,1/4 wavelength, would be the ideal length for that cable, but in my situation, 23cm worked just fine. I tried the 17.3cm with a 433Mhz module and got a quite impressive reach.). Observe DIP switch positions in power outlets, compare with script (no modifications to power outlets needed, only DIP switch setting!)
- RTC connected according to documentation, I used the „true“ SDA/SCL input of the Uno.
- IMPORTANT: a FAT copper cable should be run from the GND connector between the M1 and M2 connectors of the motor shield into the aquarium water. This will help deviate alternate ground (ground loop) currents from the very sensitive Atlas Scientific probes. This took me weeks to discover.
- Connect PC case cooling fans to M4 output of motor shield, mount fans on aquarium cover as appropriate (airflow: the fans should blow air into the hood/towards the water, not suck it out to prevent moisture damage). Only two cables need to be connected, the cable with the black marker (tachometer signal) can be clipped.
- Connect 12 DC adaptor to Arduino
- Connect USB cable to Arduino
- Connect LCD Display I2C and power to Arduino according to Adafruit documentation. Make sure to connect power + ground directly to the Arduino, not via the breadboard any other connection that is close to the sensors, this would influence your sensor readings

# Build
I used a single-bottle wine cask for this Alpha setup, fixing the Arduino to the cask with screws, taping the breadboard next to it, drilling some holes for the BNC, temperature, ground, USB, power connecters and some large holes for the peristaltic pumps. The cask can (IMO) be closed after you’re done because the Arduino won’t produce much heat.

See the images to get a rough idea of how I built this.

I currently use a raspberry PI to pick up the serial output from the Arduino, do some simple statistics with Python scripts and some output in the form of a web page. 