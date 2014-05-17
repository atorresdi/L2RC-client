/* protocol.c Implementation of the communication protocol module */

#include "protocol.h"
#include "l2rc_constants.h"
#include "usb_vcp.h"

extern Virtual_COM_Port vcp;
extern boost::asio::serial_port vc_port;

using namespace std;

Command::Command() : command(0) {}

Command::Command(uint8_t cmd) : command(cmd) {}

bool Command::Send()
{
    uint8_t cmd_cmp = ~command;
    uint8_t cmd[] = {'#', command, cmd_cmp};
    uint8_t reply = 0;

    while ( reply != ACK )
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            if ( !vcp.Write(cmd[i]) )
            {
                cout << "Unable to send command" << endl;
                return 0;
            };
        };

        while ( vcp.Read(&reply) <= 0 ) {};

        if (reply == NAK)
            cout << "NAK received\n" << "Retrying to send command" << endl;
    };

    return 1;
}

void Command::Set(uint8_t cmd)
{
    command = cmd;
}

Response::Response() : valid_reponse(0) {}

void Response::Wait()
{
    valid_reponse = 0;

    while (!valid_reponse)
    {
        uint8_t tmp_data;

        while ( vcp.Read(&tmp_data) <= 0 ) {};

        if (tmp_data == '#')
        {
            uint8_t cmd;

            while ( vcp.Read(&cmd) <= 0 ) {};
            while ( vcp.Read(&tmp_data) <= 0 ) {};

            tmp_data = ~tmp_data;

            if (cmd == tmp_data)
            {
                type = PROT_CMD;
                data = cmd;
                valid_reponse = 1;

                vcp.Write(ACK);
            };
        };
    };
}

uint8_t Response::Get_Type()
{
    return type;
}

uint8_t Response::Get_Data()
{
    return data;
}
