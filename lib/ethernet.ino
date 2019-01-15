#include <Ethernet.h>

// mac address WARNING must be different for each controllino
// @something@ will be replaced by aap build process (see readme)
byte mac[] = { @mac_address@ }; // e.g. 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
byte serverIp[] = { @server_ip@ }; // e.g. 192, 168, 100, 185
unsigned int serverPort = @server_port@; // e.g. 13809

// client object
EthernetClient client;

// TCP variables
bool isConnected = false;
String incomingMessage = "";
char messageToSend[] = ""; 
#define MESSAGE_SPLIT_SEQUENCE "_NEXT_ERSCOMMAND_"

void reConnect() {    
  Ethernet.begin(mac);
  delay(1000); // TODO necessary? 
  Serial.print("Controllino ip is ");
  Serial.println(Ethernet.localIP()); 

  if (client.connect(serverIp, serverPort)) {
    client.flush();
    isConnected = true;
    // not used by dokter
    setMessageToSend("@Action.controllino-check.ers");
  } 

  Serial.println(isConnected ? "Connected with server" : "Failed to connect to server"); 
}

// is executed during loop
void handleEthernet() {
  // check if connection is alive
  if (isConnected) {
    if (client.available()) readIncomingMessage();
    writeOutgoingMessage();
  }
  else if (!client.connected()) {
    Serial.println("Disconnected");
    client.stop();
    isConnected = false;
    reConnect(); 
  }  
}

// read an incoming message and dispatch it to handleApiRequest
void readIncomingMessage() {
  char newChar = client.read();
  incomingMessage += String(newChar);

  if (incomingMessage.endsWith(MESSAGE_SPLIT_SEQUENCE)) {
    client.flush();
    incomingMessage = incomingMessage.substring(0, incomingMessage.indexOf(MESSAGE_SPLIT_SEQUENCE)); // removes the MESSAGE_SPLIT_SEQUENCE
    handleApiRequest(incomingMessage); // must be implemented by main file to use this 'library'
    incomingMessage="";
  }
}

// set a new message to be send to the server
// this message will be sent the following time handleEthernet() is called
void setMessageToSend(char message[]) {
  strcpy(messageToSend, message); 
}

// write a message to the server if a message is present
void writeOutgoingMessage() {
  if (strcmp(messageToSend,"") != 0) {
    // escape room software expects an action name
    // e.g. @Action.victory.ers
    client.write(prepMessage(messageToSend));
    strcpy(messageToSend,""); // clear message
  }
}

// append fixed string to message
char *prepMessage(char message[]) {
  return strcat(message, MESSAGE_SPLIT_SEQUENCE);  
}




