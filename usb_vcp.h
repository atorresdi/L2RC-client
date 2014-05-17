/* usb_vcp.h Definitions for the virtual com port module */

#if !defined(USB_VCP_H)
#define USB_VCP_H

#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <string>

class Virtual_COM_Port
{
private:
    std::string path;
    boost::asio::serial_port *port;

public:
    Virtual_COM_Port(boost::asio::serial_port *pp);
    bool Open(std::string p_path);
    bool Read(uint8_t *dest);
    bool Write(uint8_t data);
    bool Close();
};


#endif
