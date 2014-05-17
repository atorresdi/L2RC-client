/* usb_vcp.c Implementation of the virtual com port module */

#include "usb_vcp.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;

Virtual_COM_Port::Virtual_COM_Port(boost::asio::serial_port *pp) : port(pp) {}

bool Virtual_COM_Port::Open(std::string p_path)
{
    path = p_path;

    if (!boost::filesystem::exists(path.data()))
    {
        cout << "Port " << path << " not found" << endl;
        return 0;
    };

    (*port).open(path.data());

    if (!(*port).is_open())
    {
        cout << "Port " << path << " could not be opened" << endl;
        return 0;
    };

    return 1;
}

bool Virtual_COM_Port::Read(uint8_t *dest)
{
    return ( (*port).read_some(boost::asio::buffer(dest, 1)) );
}


bool Virtual_COM_Port::Write(uint8_t data)
{
    return ( (*port).write_some(boost::asio::buffer(&data, 1)) );
}

bool Virtual_COM_Port::Close()
{
    (*port).close();

    return (*port).is_open();
}
