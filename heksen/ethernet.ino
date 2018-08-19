// interfacing with this module requires implementation of a handleApiRequest function in the main file
// this function must handle incoming api path Strings

#include <Ethernet.h>

// api url string to match 
const char* API_URL = "/api/";
int charCounter = 0; // counts how many characters of API_URL are already matched
bool apiUrlMatch = false; // true if API_URL has been matched

// mac address WARNING must be different for each controllino
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// server at port 80
EthernetServer server(80);

// the connecting client
// TODO bad design because only one client can be handled
// initial code did not have this global variable
// instead, the client was passed as a parameter to the handleClient function
// but the compiler refused because EthernetClient was not a known parameter type...
EthernetClient client; 

void ethernetSetup() {
  Ethernet.begin(mac);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP()); 
}

void listenOnEthernet() {
  // listen for incoming clients
  client = server.available();
  if (client) handleClient();  
}

void handleClient() {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true; 
    String apiCommand = ""; 
    
    while (client.connected()) {
      if (client.available()) {

        // read incoming characters and read out REST api path: /api</path> (space terminates a path)
        char c = client.read();
        if(!apiUrlMatch && c == API_URL[charCounter]) {
          charCounter++;
          if(charCounter == 5) apiUrlMatch = true; // 5 is number of characters in "/api/" 
        }
        if(c == ' ') {apiUrlMatch = false; charCounter = 0; }
        if(apiUrlMatch) apiCommand += String(c);

        
        // a http request end upon receiving \n and the current line contained no characters (blank)
        if (c == '\n' && currentLineIsBlank) {
          sendHttpResponse();           
          handleApiRequest(apiCommand); 
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true; // starting a new line
        } else if (c != '\r') {
          currentLineIsBlank = false; // there is a character on the current line
        }
      }
    }
    
    delay(1); // give the web browser time to receive the data
    client.stop(); // close the connection
}

void sendHttpResponse() {
  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println();
}


