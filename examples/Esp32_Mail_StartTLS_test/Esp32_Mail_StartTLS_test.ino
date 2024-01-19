/* EMailSender library for Arduino, esp8266 and esp32

  STARTSSL example

   Inline upgrading from a clear-text connection to an SSL/TLS connection.

   Some protocols such as SMTP, XMPP, Mysql, Postgress and others allow, or require,
   that you start the connection without encryption; and then send a command to switch
   over to encryption.

   E.g. a typical SMTP submission would entail a dialogue such as this:

   1. client connects to server in the clear
   2. server says hello
   3. client sents a EHLO
   4. server tells the client that it supports SSL/TLS
   5. client sends a 'STARTTLS' to make use of this faciltiy
   6. client/server negiotiate a SSL or TLS connection.
   7. client sends another EHLO
   8. server now tells the client what (else) is supported; such as additional authentication options.
   ... conversation continues encrypted.

  This is enabled by default when you connect in inSecure mode; and TLS/SSL is
  available on your platform. Disable eithet by setting setIsSecure(true) -- so
  SSL/TLS is spoken right away; or by setting setTrySecure(false) -- so this
  upgrade is not tried.

  Note that most mailservers will allow additional (plaintext) authentication methods once
  you are upgraded. And/or disallow login when not.
  
*/

#include "Arduino.h"
#include <EMailSender.h>
#include <WiFi.h>

#ifndef WIFI_NETWORK
#define WIFI_NETWORK "YOUR Wifi SSID"
#endif

#ifndef WIFI_PASSWD
#define WIFI_PASSWD "your-secret-password"
#endif

#ifndef EMAIL_ADDRESS
#define EMAIL_ADDRESS "test@yourowndomain.com"
#endif

#ifndef SMTP_HOST
#define SMTP_HOST "outbound-mail-server.yourowndomain.comn"
#endif

#ifndef SMTP_USERNAME
#define SMTP_USERNAME "Marcello Mastroianni"
#endif

#ifndef SMTP_USER
#define SMTP_USER "userid" // commonly just a username or the full canonical email address.
#endif

#ifndef SMTP_PASSWORD
#define SMTP_PASSWORD "emailpassword"
#endif

#ifndef SMTP_PORT
#define SMTP_PORT 25
#endif

#ifndef DEST_ADDR
#define DEST_ADDR EMAIL_ADDRESS // sent the test mail to yourself.
#endif 

const char* ssid     = WIFI_NETWORK;     // your network SSID (name of wifi network)
const char* password = WIFI_PASSWD; // your network password
const char*  server = SMTP_HOST;  // Server URL
const int submission_port = SMTP_PORT; // submission port.


uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;

EMailSender emailSend(SMTP_USER, SMTP_PASSWORD, EMAIL_ADDRESS, SMTP_USERNAME, SMTP_HOST, SMTP_PORT);

  
static uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);
        Serial.println(nSSID);
    }

    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 50)
    {
        delay(200);
        Serial.print(".");
    }
    ++attempt;
    Serial.println("");
    if(i == 51) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Check if access point available or SSID and Password\r\n");
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}

static void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state) {
        delay(50);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}

void setup()
{
    Serial.begin(115200);

    connection_state = WiFiConnect(WIFI_NETWORK, WIFI_PASSWD);
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect

    EMailSender::EMailMessage message;
    message.subject = "Soggetto";
    message.message = "Ciao come stai<br>io bene.<br>www.mischianti.org";

    emailSend.setEHLOCommand(true); // needed to detect StartTLS capability

    EMailSender::Response resp = emailSend.send(DEST_ADDR, message);

    Serial.println("Sending status: ");

    Serial.print("Status:"); Serial.println(resp.status);
    Serial.print("Code:  "); Serial.println(resp.code);
    Serial.print("Desc:  "); Serial.println(resp.desc);
}

void loop()
{

}