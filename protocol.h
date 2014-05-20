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

class Package
{
private:
    bool new_data_flag;
    void Print_Error();
    void Print_Warning();

public:
    uint16_t length;
    uint8_t opts;
    uint8_t ptsf;
    uint8_t *data;

    Package();
    ~Package();
    bool Set_Attributes(uint16_t d_length, uint8_t p_ptsf, uint8_t *p_data);
    bool Set_Opts_Field(uint8_t pkg_type, uint8_t device, uint8_t data_size);

    uint8_t Get_Cksum();

    void New_Data();
    bool Send();
};

class Response
{
private:
    bool valid_reponse;
    uint8_t type;
    uint8_t cmd;

public:
    Package pkg;

    Response();
    void Wait();
    uint8_t Get_Type();
    uint8_t Get_Cmd();
    uint8_t *Get_Pkg_Data();
    uint8_t Get_Length();
};

#endif // PROTOCOL_H
