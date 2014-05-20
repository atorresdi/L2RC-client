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

Package::Package() : new_data_flag(0), length(0), data(0) {}

Package::~Package()
{
    if (new_data_flag)
        delete[] data;
}


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

uint8_t Package::Get_Cksum()
{
    uint8_t cksum = 0;

    if (length < 256)
        cksum += length;

    cksum += opts + ptsf;

    for (int i = 0; i < length; i++)
        cksum += data[i];

    return cksum;
}

void Package::New_Data()
{
    if (length)
    {
        if (new_data_flag)
            delete[] data;

        data = new uint8_t[length];
        new_data_flag = 1;
    }
}

bool Package::Send()
{
    uint8_t cksum = 0;
    uint8_t reply;

    if (!data)
        return 0;

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
        uint8_t rx_data;

        /* wait for start character: # or SOH */
        while ( vcp.Read(&rx_data) <= 0 ) {};

        if (rx_data == '#')        /* Command */
        {
            /* wait for the command and its complement */
            while ( vcp.Read(&cmd) <= 0 ) {};
            while ( vcp.Read(&rx_data) <= 0 ) {};

            rx_data = ~rx_data;
            if (cmd == rx_data)
            {
                type = PROT_CMD;
                valid_reponse = 1;

                vcp.Write(ACK);         /* send ACK */
            };
        }
        else if (rx_data == SOH)       /* Package Start Of Header */
        {
            /* wait for the package secuence num */
            while ( vcp.Read(&rx_data) <= 0 ) {};

            if ( rx_data == pkg_sec_num )
            {
                vcp.Write(SYN);             /* send sync character */

                /* Wait for the package length */
                while ( vcp.Read(&rx_data) <= 0 ) {};

                if (!rx_data)           /* a 256 length is represented by zero */
                    pkg.length = 256;
                else
                    pkg.length = rx_data;

                pkg.New_Data();             /* Create the data buffer after storing package length */

                /* Wait for the package options */
                while ( vcp.Read(&pkg.opts) <= 0 ) {};

                /* Wait for the Package Type Specific Field  */
                while ( vcp.Read(&pkg.ptsf) <= 0 ) {};

                vcp.Write(SYN);             /* send FIRST sync character */

                /* Wait for the package data */
                for (int i = 0; i < pkg.length; )
                {
                    while ( vcp.Read(&pkg.data[i]) <= 0 ) {};
                    i++;

                    if (i >= MAX_CONSEC_RX_BYTES)
                    {
                        if ( (!(i % MAX_CONSEC_RX_BYTES)) && (i < pkg.length) )
                            vcp.Write(SYN);             /* send SECOND sync character */
                    };
                };

                vcp.Write(SYN);             /* send THIRD sync character */

                /* Wait for the package checksum */
                while ( vcp.Read(&rx_data) <= 0 ) {};
                cout << "cksum " << (int)pkg.Get_Cksum() << endl;
                if (pkg.Get_Cksum() == rx_data)
                {
                    vcp.Write(ACK);         /* send ACK */      cout << "ACK sent" << endl;
                    type = PROT_PKG;
                    pkg_sec_num++;
                    valid_reponse = 1;
                }
                else
                    vcp.Write(NAK);         /* send NAK: incorrect package checksum */
            }
            else
                vcp.Write(NAK);         /* send NAK: incorrect package secuence num */

        };

    };
}

uint8_t Response::Get_Type()
{
    return type;
}

uint8_t Response::Get_Cmd()
{
    return cmd;
}

/* -------------------------------------class: Response */
