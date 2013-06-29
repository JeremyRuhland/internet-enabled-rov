= Internet Enabled ROV =

=== Theory ===

The ROV will be connected to the surface by cat5 ethernet cable, carrying data and PoE at 12-48 volts depending on what power supply I find.

The underwater component will contain a motherboard, power converter and ip camera.

The motherboard contains a passive ethernet hub, connections to the power converter, microcontroller and motor drivers. The system can be programmed by usb connection.

When the motorboard's ip address is visited in a web browser, it responds with a simple html page showing instructions and a live updating camera feed. Pressing the WASD, QE and up/down arrow keys will send GET requests to the onboard webserver that will control the motor drivers. There is also a checkbox that will control an led array for increased lighting.

=== ROV Design ===

The ROV will have four motors, one each for movement in the X, Y and Z axises and one for rotation of the robot.

All electronics will be encased in a waterproofed pvc pipe that will form the body of the ROV with a window endcap on one side and 5 waterproofed electrical connections on the other. Bits of foam will be used to achieve neutral density.

Motors will be standard brushed dc, placed inside pill bottles filled with paraffin and marine jelly for waterproofing. Propellers will be whatever I can get my hands on.

=== Embedded Webserver ===

Code for the embedded server is based off examples from jcw's [ethercard project](https://github.com/jcw/ethercard).

The javascript keyboard interface was inspired by a number of online tutorials and compressed to minimal size using google's [Closure Compiler](http://closure-compiler.appspot.com/).
