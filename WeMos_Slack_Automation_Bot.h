// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#define WORD_SEPERATORS "., \"'()[]<>;:-+&?!\n\t"

#ifndef _WeMos_Slack_Automation_Bot_H_
#define _WeMos_Slack_Automation_Bot_H_
#include "Arduino.h"
//add your includes for the project WeMos_Slack_Automation_Bot here
#include <WebSocketsClient.h>


//end of add your includes here


//add your function definitions for the project WeMos_Slack_Automation_Bot here
void webSocketEvent(WStype_t type, uint8_t *payload, size_t len);
void heating(String chanCode, String msgText);
void helloWorld(String chanCode);
void processSlackMessage(char *payload);
void connectWifi();



//Do not add code below this line
#endif /* _WeMos_Slack_Automation_Bot_H_ */
