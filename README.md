# HumidityController

An embedded C project written for a PIC24HJXXXGP202. The aim of the project is to control the
humidity of a terrarium by controlling an ultrasonic fogger unit and a fan. The fogger unit is
placed inside of a container with water, and is turned on for a period. Then, the fan is turned 
on to evacuate the fog into a nearby terrarium. Inside this terrarium I have mounted an SHT10
sensor which measures the humidity. Via the use of a proportional controller, the fooger- and
fan on-time is set.

The project also includes a text based interface to set the various parameters and get current 
setpoints. This allows the user to remotely monitor and control the device over the bluetooth 
interface I have connected to the PIC24.

The Fan is driven by a PWM controlled MOSFET, while the fogger is switched using a mosfet via 
a GPIO.
