/* protocol.c Implementation of the communication protocol module */

#include "protocol.h"
#include "l2rc_constants.h"
#include "usb_vcp.h"

extern Virtual_COM_Port vcp;
extern boost::asio::serial_port vc_port;

/* static variables */
uint8_t pkg_sec_num = 0;

using namespace std;

/* class: Command ------------------------------------ */

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

/* -------------------------------------class: Command */


/* class: Package ------------------------------------ */

bool Package::Set_Attributes(uint16_t d_length, uint8_t p_ptsf, uint8_t *p_data)
{
    if ( (d_length > 0) ||  (d_length <= 256) )
        length = d_length;
    else
    {
        cout << "Package::Set_Attributes: Invalid package length value" << endl;
        return 0;
    };

    ptsf = p_ptsf;
    data = p_data;

    return 1;
}

bool Package::Set_Opts_Field(uint8_t pkg_type, uint8_t device, uint8_t data_size)
{
    opts = 0;

    if (pkg_type < 4)
         opts |= ((pkg_type << 6) & 0xC0);
    else
    {
        cout << "Package::Set_Opts_Field: Invalid pkg_type value" << endl;
        return 0;
    };

    if (device < 16)
         opts |= ((device << 2) & 0x3C);
    else
    {
        cout << "Package::Set_Opts_Field: Invalid device value" << endl;
        return 0;
    };

    if (data_size < 4)
         opts |= (data_size & 0x03);
    else
    {
        cout << "Package::Set_Opts_Field: Invalid data_size value" << endl;
        return 0;
    };

    return 1;
}

void Package::Print_Error()
{
    cout << "Package::Send: NAK received" << endl;
}

void Package::Print_Warning()
{
    cout << "WARNING: Package::Send: Character received other than SYN" << endl;
}

bool Package::Send()
{
    uint8_t cksum = 0;
    uint8_t reply;

    /* calculate checksum */
    cksum = length + opts + ptsf;
    for (int i = 0; i < length; i++)
        cksum += data[i];

    /* send header */
    vcp.Write(SOH);
    vcp.Write(pkg_sec_num);

    /* Wait for SYN... or NAK */
    while ( vcp.Read(&reply) <= 0 ) {};
    if (reply == NAK)
    {
        Print_Error();
        return 0;
    };

    if (length == 256)      /* a 256 length is expressed by zero */
        vcp.Write(0);
    else
        vcp.Write(length);

    vcp.Write(opts);
    vcp.Write(ptsf);

    /* Wait for SYN */
    while ( vcp.Read(&reply) <= 0 ) {};
    if (reply != SYN)
        Print_Warning();

    /* send package data */
    for (int i = 0; i < length; )
    {
        vcp.Write(data[i]);
        i++;

        /* Determine if the maximun consecutive sent bytes num was reached */
        if (i >= MAX_CONSEC_SENT_BYTES)
        {
            if (!(i % MAX_CONSEC_SENT_BYTES))
            {
                /* Wait for SYN */
                while ( vcp.Read(&reply) <= 0 ) {};
                if (reply != SYN)
                    Print_Warning();
            };
        };
    };

    /* send checksum */
    vcp.Write(cksum);

    /* Wait for ACK... or NAK */
    while ( vcp.Read(&reply) <= 0 ) {};
    if (reply == NAK)
    {
        Print_Error();
        return 0;
    }
    else if (reply == ACK)       /* if the reply is ACK, increment the package secuence number */
    {
        pkg_sec_num++;
        return 1;
    }
    else
    {
        while (1)
        {
            cout << "Package::Send: Final reply niether ACK o NAK ---> " <<  (int)reply << endl;
            while ( vcp.Read(&reply) <= 0 ) {};
        }
    }
}

/* -------------------------------------class: Package */


/* class: Response ------------------------------------ */

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

/* -------------------------------------class: Response */
