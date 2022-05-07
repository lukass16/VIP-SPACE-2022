#pragma once

#include <WiFi.h>

//https://randomnerdtutorials.com/esp32-access-point-ap-web-server/
//https://en.wikipedia.org/wiki/SoftAP

// Replace with your network credentials
const char *ssid = "VIP-SPACE-FC";
const char *password = "VIP-2022";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
String body;

//*testing
int data = 0;

namespace wifiserver
{
    void setup()
    {
        // Set access point
        Serial.print("Setting Access Point...");
        WiFi.softAP(ssid, password);

        // Print IP address
        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);

        // Start server
        server.begin();
    }

    String getBody()
    {
        body = "<br><h1>" + String(data) + "</h1>";
        return body;
    }

	void setData()
	{
        sens_data::IMUData mag = s_data.getIMUData(); //*testing
		data = mag.acc_x;
	}

    void handleClient()
    {
        WiFiClient client = server.available(); // Listen for incoming clients

        if (client)
        {                                    // If a new client connects,
            Serial.println("\nNew Client."); // print a message out in the serial port
            String currentLine = "";         // make a String to hold incoming data from the client

            while (client.connected()) // loop while the client's connected
            {
                if (client.available()) // if there's bytes to read from the client,
                {
                    char c = client.read(); // read a byte, then
                    Serial.write(c);        // print it out the serial monitor
                    header += c;
                    if (c == '\n') // if the byte is a newline character
                    {
                        //* if the current line is blank, you got two newline characters in a row.
                        //* that's the end of the client HTTP request, so send a response:
                        if (currentLine.length() == 0)
                        {

                            client.println("HTTP/1.1 200 OK");        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                            client.println("Content-type:text/html"); // and a content-type so the client knows what's coming, then a blank line:
                            client.println("Connection: close");
                            client.println();

                            //* Display the HTML web page
                            client.println("<!DOCTYPE html><html>");
                            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                            client.println("<link rel=\"icon\" href=\"data:,\">");
                           
                            // CSS to style
                            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}</style></head>");
                            
                            //* Web Page Body
                            client.println("<body><h1>ESP32 Web Server</h1>"); // Web Page Heading
                            client.println(getBody());
                            client.println("</body></html>");

                            // The HTTP response ends with another blank line
                            client.println();

                            break; // Break out of the while loop
                        }
                        else // if you got a newline, then clear currentLine
                        {
                            currentLine = "";
                        }
                    }
                    else if (c != '\r') // if you got anything else but a carriage return character,
                    {
                        currentLine += c; // add it to the end of the currentLine
                    }
                }
            }
            // Clear the header variable
            header = "";
            // Close the connection
            client.stop();
            Serial.println("Client disconnected.\n");
        }
    }

};