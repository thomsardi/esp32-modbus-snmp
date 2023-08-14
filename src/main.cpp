#if defined (ESP8266)
    #include <ESP8266WiFi.h>        // ESP8266 Core WiFi Library         
#else
    #include <WiFi.h>               // ESP32 Core WiFi Library    
#endif

#include <WiFiUdp.h>
#include <SNMP_Agent.h>
#include <SNMPTrap.h>

#include <ModbusClientRTU.h>
#include <Logging.h>

#include <ModbusData.h>
#include <MpptSrne.h>

const char* ssid = "RnD_Sundaya";
const char* password = "sundaya22";

TaskHandle_t modbusTaskHandler;
QueueHandle_t modbusTaskQueue = xQueueCreate(16, sizeof(int));

WiFiUDP udp;
// Starts an SMMPAgent instance with the read-only community string 'public', and read-write community string 'private
SNMPAgent snmp = SNMPAgent("public", "private");  

// Numbers used to response to Get requests
int changingNumber = 0;
int settableNumber = 0;
uint32_t tensOfMillisCounter = 0;

// arbitrary data will be stored here to act as an OPAQUE data-type
uint8_t* stuff = 0;


// If we want to change the functionaality of an OID callback later, store them here.
ValueCallback* changingNumberOID;
ValueCallback* modbusDataOID;
ValueCallback* settableNumberOID;
TimestampCallback* timestampCallbackOID;

std::string staticString = "This value will never change";

// Setup an SNMPTrap for later use
SNMPTrap* settableNumberTrap = new SNMPTrap("public", SNMP_VERSION_2C);
char* changingString;

ModbusClientRTU MB;
uint32_t Token = 1111;

int modbusDump = 0;

MpptSrne mpptSrne(&MB, 1);

// Define an onData handler function to receive the regular responses
// Arguments are Modbus server ID, the function code requested, the message data and length of it, 
// plus a user-supplied token to identify the causing request
void handleData(ModbusMessage response, uint32_t token) 
{
    // Only print out result of the "real" example - not the request preparing the field
    if (token > 1111) {
        LOG_N("Response: serverID=%d, FC=%d, Token=%08X, length=%d:\n", response.getServerID(), response.getFunctionCode(), token, response.size());
        HEXDUMP_N("Data", response.data(), response.size());
    }
    mpptSrne.parseData(response, token);
}

void handleError(Error error, uint32_t token) 
{
    // ModbusError wraps the error code and provides a readable error message for it
    ModbusError me(error);
    LOG_E("Error response: %02X - %s\n", (int)me, (const char *)me);
}

void modbusTask(void *args)
{
    while (1)
    {
        mpptSrne.print();
        Error err = MB.addRequest(1000, 1, READ_HOLD_REGISTER, 0x100, 10);
        if (err!=SUCCESS) {
            ModbusError e(err);
            LOG_E("Error creating request: %02X - %s\n", (int)e, (const char *)e);
        }
        else
        {
            Serial.println("Success add request");
        }
        vTaskDelay(200);
        // ModbusData md;
        // if(xQueueReceive(modbusTaskQueue, &md, 200)==pdTRUE)
        // {
        //     Serial.println("==Parsing Incoming Data==");
        //     Serial.println("Id : " + String(md.msg.getServerID()));
        //     Serial.println("Function Code : " + String(md.msg.getFunctionCode()));
        //     Serial.println("Token : " + String(md.token));
        //     Serial.println("Response Size : " + String(md.msg.size()));
        //     Serial.print("Data : ");
        //     for (auto &data : md.msg)
        //     {
        //         Serial.print(data, HEX);
        //         Serial.print(" ");
        //     }
        //     Serial.println();
        //     int16_t temp;
        //     md.msg.get(0, temp);
        //     Serial.println("Value : " + String(temp));
        //     vTaskDelay(20);
        // }
        // else
        // {
        //     // Create request by
        //     // (Fill in your data here!)
        //     // - token to match the response with the request.
        //     // - server ID = 1
        //     // - function code = 0x03 (read holding register)
        //     // - address to read = word 1
        //     // - data words to read = 1
        //     //
        //     // If something is missing or wrong with the call parameters, we will immediately get an error code 
        //     // and the request will not be issued

        //     // Read register
        //     Error err = MB.addRequest(Token++, 1, READ_HOLD_REGISTER, 1, 1);
        //     if (err!=SUCCESS) {
        //         ModbusError e(err);
        //         LOG_E("Error creating request: %02X - %s\n", (int)e, (const char *)e);
        //     }
        //     else
        //     {
        //         Serial.println("Success add request");
        //     }
        // }
        
    }
    
}

void setup(){
    Serial.begin(115200);
    
    RTUutils::prepareHardwareSerial(Serial2);
    Serial2.begin(115200);
    MB.onDataHandler(&handleData);
    MB.onErrorHandler(&handleError);
    MB.setTimeout(2000);
    MB.begin(Serial2);

    WiFi.begin(ssid, password);
    // WiFi.begin(ssid);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    } 
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // give snmp a pointer to the UDP object
    snmp.setUDP(&udp);
    snmp.begin();

    // setup our OPAQUE data-type
    stuff = (uint8_t*)malloc(4);
    stuff[0] = 1;
    stuff[1] = 2;
    stuff[2] = 24;
    stuff[3] = 67;
    
    // add 'callback' for an OID - pointer to an integer
    changingNumberOID = snmp.addIntegerHandler(".1.3.6.1.4.1.5.0", &changingNumber);
    
    // Using your favourite snmp tool:
    // snmpget -v 1 -c public <IP> 1.3.6.1.4.1.5.0
    
    // you can accept SET commands with a pointer to an integer 
    settableNumberOID = snmp.addIntegerHandler(".1.3.6.1.4.1.5.1", &settableNumber, true);
    // snmpset -v 1 -c public <IP> 1.3.6.1.4.1.5.0 i 99

    modbusDataOID = snmp.addIntegerHandler(".1.3.6.1.4.1.5.2", mpptSrne.getBatteryVoltageAddr());

    // More examples:
    snmp.addIntegerHandler(".1.3.6.1.4.1.4.0", &changingNumber);
    snmp.addOpaqueHandler(".1.3.6.1.4.1.5.9", stuff, 4, true);
    snmp.addReadOnlyStaticStringHandler(".1.3.6.1.4.1.5.11", staticString);
    
    // Setup read/write string
    changingString = (char*)malloc(25 * sizeof(char));
    snprintf(changingString, 25, "This is changeable");
    snmp.addReadWriteStringHandler(".1.3.6.1.4.1.5.12", &changingString, 25, true);


    // Setup SNMP TRAP
    // The SNMP Trap spec requires an uptime counter to be sent along with the trap.
    timestampCallbackOID = (TimestampCallback*)snmp.addTimestampHandler(".1.3.6.1.2.1.1.3.0", &tensOfMillisCounter);

    settableNumberTrap->setUDP(&udp); // give a pointer to our UDP object
    settableNumberTrap->setTrapOID(new OIDType(".1.3.6.1.2.1.33.2")); // OID of the trap
    settableNumberTrap->setSpecificTrap(1); 

    // Set the uptime counter to use in the trap (required)
    settableNumberTrap->setUptimeCallback(timestampCallbackOID);

    // Set some previously set OID Callbacks to send these values with the trap (optional)
    settableNumberTrap->addOIDPointer(changingNumberOID);
    settableNumberTrap->addOIDPointer(settableNumberOID);

    settableNumberTrap->setIP(WiFi.localIP()); // Set our Source IP

    // Ensure to sortHandlers after adding/removing and OID callbacks - this makes snmpwalk work
    snmp.sortHandlers();

    xTaskCreatePinnedToCore(
        modbusTask,
        "modbusTask",
        2048,
        NULL,
        tskIDLE_PRIORITY,
        NULL,
        1
    );
} 

void loop(){
    snmp.loop(); // must be called as often as possible
    if(settableNumberOID->setOccurred){
        
        Serial.printf("Number has been set to value: %i\n", settableNumber);
        if(settableNumber%2 == 0){
            // Sending an SNMPv2 INFORM (trap will be kept and re-sent until it is acknowledged by the IP address it was sent to)
            settableNumberTrap->setVersion(SNMP_VERSION_2C);
            settableNumberTrap->setInform(true); // set this to false and send using `settableNumberTrap->sendTo` to send it without the INFORM request
        } else {
            // Sending regular SNMPv1 trap
            settableNumberTrap->setVersion(SNMP_VERSION_1);
            settableNumberTrap->setInform(false);
        }
        // Serial.println("Lets remove the changingNumber reference");
        // snmp.sortHandlers();
        // if(snmp.removeHandler(settableNumberOID)){
        //     Serial.println("Remove succesful");
        // }
        settableNumberOID->resetSetOccurred();

        // Send the trap to the specified IP address
        // If INFORM is set, snmp.loop(); needs to be called in order for the acknowledge mechanism to work.
        IPAddress destinationIP = IPAddress(192, 168, 2, 132);
        if(snmp.sendTrapTo(settableNumberTrap, destinationIP, true, 2, 5000) != INVALID_SNMP_REQUEST_ID){ 
            Serial.println("Sent SNMP Trap");
        } else {
            Serial.println("Couldn't send SNMP Trap");
        }
    }
    changingNumber++;
    if (changingNumber > 255)
    {
      changingNumber = 0;
    }
    // changingNumber = 100;
    tensOfMillisCounter = millis()/10;
    delay(20);
}
