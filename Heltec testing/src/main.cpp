#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <LoRa.h>
#include <heltec.h>
#include <WiFi.h>

//some links used
//https://github.com/m1k0s/ESP32-HelloWorld3D/blob/master/include/display.h
//https://randomnerdtutorials.com/esp32-web-server-arduino-ide/

//define oled object
U8X8_SSD1306_128X64_NONAME_SW_I2C oled(15, 4, 16);


//definitions for LoRa
#define SS 18
#define RST 14
#define DI0 26

int syncWord = 0xF3;
byte _localAddress = 0xFF; // address of this device
byte _destination = 0xFF;  // destination to send to


//*definitions for wifi
const char *ssid = "Tele2-8c311";
const char *password = "8g6ihj47rb2";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
//*wifi end

//lora message function (this can be read by the current LoPy implementation)
void sendMessage(String outgoing, int lora_message_id)
{
	LoRa.beginPacket();			   // start packet
	LoRa.write(_destination);	   // add destination address
	LoRa.write(_localAddress);	   // add sender address
	LoRa.write(lora_message_id);   // add message ID
	LoRa.write(outgoing.length()); // add payload length
	LoRa.print(outgoing);		   // add payload
	LoRa.endPacket();			   // finish packet and send it
}

void setup()
{
	Serial.begin(115200);
	Serial.println("Heltec test");

	//oled setup
	oled.begin();
	oled.setFont(u8x8_font_chroma48medium8_r);
	oled.drawString(1, 3, "Shalom");

	//Lora
	SPI.begin(5, 19, 27, 18);
	LoRa.setPins(SS, RST, DI0);
	if (!LoRa.begin(433E6))
	{
		Serial.println("Starting LoRa failed!");
		while (1);
	}

	//setting paramaters
	LoRa.setSyncWord(0xF3);
	LoRa.setTxPower(14);
	LoRa.setSpreadingFactor(10);
	LoRa.setCodingRate4(6);
	LoRa.setSignalBandwidth(62.5E3);

	Serial.println("Lora initialized!");

	//*wifi setup
	// Connect to Wi-Fi network with SSID and password
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	// Print local IP address and start web server
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	server.begin();
	//*wifi end
}

void loop()
{
	//uncomment to send messages
	//sendMessage("Lora message", 0);
	//delay(1000);
	//Serial.println("Sent");

	//*wifi functionality - paste ip adress into browser to see webpage
	WiFiClient client = server.available(); // Listen for incoming clients

	if (client)
	{ // If a new client connects,
		currentTime = millis();
		previousTime = currentTime;
		Serial.println("New Client."); // print a message out in the serial port
		String currentLine = "";	   // make a String to hold incoming data from the client
		while (client.connected() && currentTime - previousTime <= timeoutTime)
		{ // loop while the client's connected
			currentTime = millis();
			if (client.available())
			{							// if there's bytes to read from the client,
				char c = client.read(); // read a byte, then
				Serial.write(c);		// print it out the serial monitor
				header += c;
				if (c == '\n')
				{ // if the byte is a newline character
					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0)
					{
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println("Connection: close");
						client.println();

						// Display the HTML web page
						client.println("<!DOCTYPE html><html>");
						client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
						client.println("<link rel=\"icon\" href=\"data:,\">");
						// CSS to style the on/off buttons
						// Feel free to change the background-color and font-size attributes to fit your preferences
						client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
						client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
						client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
						client.println(".button2 {background-color: #555555;}</style></head>");

						// Web Page Heading
						client.println("<body><h1>Chau, Lukass :)</h1>");
						client.println("</body></html>");
						client.println();
						// Break out of the while loop
						break;
					}
					else
					{ // if you got a newline, then clear currentLine
						currentLine = "";
					}
				}
				else if (c != '\r')
				{					  // if you got anything else but a carriage return character,
					currentLine += c; // add it to the end of the currentLine
				}
			}
		}
		// Clear the header variable
		header = "";
		// Close the connection
		client.stop();
		Serial.println("Client disconnected.");
		Serial.println("");
	}
	//*wifi end
}
