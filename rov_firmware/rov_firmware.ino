// UDP firmware for internet enabled ROV
// By Jeremy Ruhland
// Based off example code by Brian Lee <cybexsoft@hotmail.com>

#define stopAllMotors() PORTC &= ~((1<<PC5)|(1<<PC4)|(1<<PC3)|(1<<PC2)|(1<<PC1)|(1<<PC0)); PORTD &= ~((1<<PD1)|(1<<PD0))
#define lightsOff() PORTB &= ~(1<<PB0)
#define lightsOn() PORTB |= (1<<PB0)
#define forwardThrust() PORTC |= (1<<PC0)
#define reverseThrust() PORTC |= (1<<PC1)
#define forwardReverseStop() PORTC &= ~((1<<PC1)|(1<<PC0)) 
#define starboardThrust() PORTC |= (1<<PC2)
#define portThrust() PORTC |= (1<<PC3)
#define starboardPortStop() PORTC &= ~((1<<PC3)|(1<<PC2))
#define diveThrust() PORTC |= (1<<PC4)
#define surfaceThrust() PORTC |= (1<<PC5)
#define diveSurfaceStop() PORTC &= ~((1<<PC5)|(1<<PC4))
#define clockwiseThrust() PORTD |= (1<<PD0)
#define anticlockwiseThrust() PORTD |= (1<<PD1)
#define clockwiseAnticlockwiseStop() PORTD &= ~((1<<PD1)|(1<<PD0))

#include <EtherCard.h>
#include <avr/eeprom.h>

// ethernet interface ip address
static byte myip[] = { 192,168,1,200 };
// ethernet interface mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[1200];   // tcp/ip send and receive buffer
static BufferFiller bfill; // used as cursor while filling the buffer

char functionName[2];	// query key of sent command

char okHeader[] PROGMEM = 
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
;

char indexPage[] PROGMEM = 
    "<html><b>ROV Web Interface</b>"
    "<table><tr><td><iframe width=\"480\" height=\"302\" src=\"http://www.ustream.tv/embed/9948292\?v=3\"></iframe></td>"
    "<td><hr width=1 size=250></td><td>Keyboard controlls:<br>Arrow Keys and<br>A (Ascend)<br>Z (Descend)<br><br><label><input type=\'checkbox\' onchange=\'e(this);\'>Enable Lighting</label></td></tr></table>"
    "<script>var a;var b={};var i=document.createElement(\"img\");function c(e){if(a&&a.keyCode==e.keyCode)return;if(!e)e=window.event;a=e;b[e.keyCode]=true;if(e.keyCode==65)i.src=\"\?c=1\";if(e.keyCode==90)i.src=\"\?c=2\";if(e.keyCode==37)i.src=\"\?c=4\";if(e.keyCode==39)i.src=\"\?c=5\";if(e.keyCode==38)i.src=\"\?c=7\";if(e.keyCode==40)i.src=\"\?c=8\";if(e.keyCode==33)i.src=\"\?c=c\";if(e.keyCode==34)i.src=\"\?c=d\";}function d(e){a=null;delete b[e.keyCode];if(!e)e=window.event;if(e.keyCode==65||e.keyCode==90)i.src=\"\?c=3\";if(e.keyCode==37||e.keyCode==39)i.src=\"\?c=6\";if(e.keyCode==38||e.keyCode==40)i.src=\"\?c=9\";if(e.keyCode==33||e.keyCode==34)i.src=\"\?c=e\";}function e(cb){if(cb.checked==true)i.src=\"\?c=a\";else i.src=\"\?c=b\";}document.onkeydown=c;document.onkeyup=d;</script></html>"
;

static void homePage(BufferFiller& buf) {
    buf.emit_p(indexPage, okHeader);
}

static void commandParse(const char* data, BufferFiller& buf) {
    // execute command, if present
    switch(functionName[0]) {
	case '0':	// All motors off
                stopAllMotors();
		break;
	case '1':	// Up
	            surfaceThrust();
		break;
	case '2':	// Down
	            diveThrust();
		break;
	case '3':	// Up/down off
	            diveSurfaceStop();
		break;
	case '4':   // Left
	            portThrust();
		break;
	case '5':   // Right
	            starboardThrust();
		break;
	case '6':   // Left/right off
	            starboardPortStop();
		break;
	case '7':	// Fwd
	            forwardThrust();
		break;
	case '8':	// Back
	            reverseThrust();
		break;
	case '9':	// Fwd/back off
	            forwardReverseStop();
		break;
	case 'a':	// Lights on
	            lightsOn();
		break;
	case 'b':	// Lights off
	            lightsOff();
		break;
	case 'c':	// Rotate CW
	            clockwiseThrust();
		break;
	case 'd':	// Rotate CCW
	            anticlockwiseThrust();
		break;
    case 'e':   // Rotate off
                clockwiseAnticlockwiseStop();
        break;
	default:    // Emergency case, should never be entered
                stopAllMotors();
		break;
	}
}

void setup(){
    // Disable motors/lights
    PORTB &= ~(1<<PB1);
    PORTC &= ~((1<<PC5)|(1<<PC4)|(1<<PC3)|(1<<PC2)|(1<<PC1)|(1<<PC0));
    PORTD &= ~((1<<PD1)|(1<<PD0));
    // Set ouput ports correctly
    DDRB |= (1<<PB1);
    DDRC |= (1<<PC5)|(1<<PC4)|(1<<PC3)|(1<<PC2)|(1<<PC1)|(1<<PC0);
    DDRD |= (1<<PD1)|(1<<PD0);
    // Init ethernet system
    ether.begin(sizeof Ethernet::buffer, mymac);
    ether.staticSetup(myip);
}

void loop(){
    word len = ether.packetReceive();
    word pos = ether.packetLoop(len);
    // check if valid tcp data is received
    if (pos) {
        bfill = ether.tcpOffset();
        char* data = (char *) Ethernet::buffer + pos;
        // receive buf hasn't been clobbered by reply yet
        if (EtherCard::findKeyVal(data, functionName, 2, "c") != 0)
            commandParse(data, bfill);
        else
            homePage(bfill);
        ether.httpServerReply(bfill.position()); // send web page data
    }
}
