/* protocol.h Definitions for the communication protocol module */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

class Command
{
private:
    uint8_t command;

public:
    Command();
    Command(uint8_t cmd);
    bool Send();
    void Set(uint8_t cmd);
};

class Response
{
private:
    bool valid_reponse;
    uint8_t type;
    uint8_t data;
    uint8_t *pkg_data;
    uint8_t length;

public:
    Response();
    void Wait();
    uint8_t Get_Type();
    uint8_t Get_Data();
    uint8_t *Get_Pkg_Data();
    uint8_t Get_Length();
};

#endif // PROTOCOL_H
