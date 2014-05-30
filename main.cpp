#include "timing.h"
#include "usb_vcp.h"
#include "protocol.h"
#include "rdsqr_client.h"

#include "l2rc_constants.h"

#include <iostream>
#include <stdio.h>

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

using namespace std;

/* Virtual COM port variables */
boost::asio::io_service io_service;
boost::asio::serial_port vc_port(io_service);
Virtual_COM_Port vcp(&vc_port);

/* RDsqr variables */
RDsqr_Client rdd_client;        /* robot device driver client */

int main()
{
    string port_path = "/dev/ttyACM";
    char port_num = '0';
    char user_decision;
    cout << "default serial port is " << "/dev/ttyACM0" << ", change port num? (Y/n)";
    cin >> user_decision;
    if (user_decision == 'y' || user_decision == 'Y')
    {
        cout << "enter port number ";
        cin >> port_num;
        if (port_num < '0' || port_num > '9')
        {
            cerr << "Err: invalid port number" << endl;
            return 0;
        };
    };

    /* Virtual COM port initialization */
    if (vcp.Open(port_path.append(1, port_num)))
        cout << "port " <<  port_path << " open" << endl;
    else
        return 0;

    if (!rdd_client.Set_Up())
        return 0;

    if (!rdd_client.Send_Config())
        return 0;

    for (int dev_idx = 0; dev_idx < rdd_client.robot_config.glob_param.dev_num; dev_idx++)
    {
        for (int param_wr_idx = 0; param_wr_idx < rdd_client.robot_config.device[dev_idx].param_wr_num; param_wr_idx++)
        {
            if(!rdd_client.Set_Param_Wr_Data(dev_idx, param_wr_idx))
                return 0;

            rdd_client.Send_Inst_Pkg(dev_idx, param_wr_idx);
        };
    };

    while (1)
    {
        for (int dev_idx = 0; dev_idx < rdd_client.robot_config.glob_param.dev_num; dev_idx++)
        {
            for (int param_wr_idx = 0; param_wr_idx < rdd_client.robot_config.device[dev_idx].param_wr_num; param_wr_idx++)
            {
                if(!rdd_client.Set_Param_Wr_Data(dev_idx, param_wr_idx))
                {
                    if (rdd_client.robot_config.device[dev_idx].param_wr_file[param_wr_idx].eof())
                    {
                        if (rdd_client.robot_config.glob_param.iteration_num)
                        {
                            (rdd_client.iteration_count)++;         cout << "rdd_client.iteration_count " << rdd_client.iteration_count << endl;

                            if (rdd_client.iteration_count >= rdd_client.robot_config.glob_param.iteration_num)
                            {
                                cout << "matrix file(s) execution complete" << endl;
                                return 0;
                            };
                        };

                        rdd_client.robot_config.device[dev_idx].param_wr_file[param_wr_idx].clear();
                        rdd_client.robot_config.device[dev_idx].param_wr_file[param_wr_idx].seekg(0, ios::beg);
                        if (!rdd_client.Set_Param_Wr_Data(dev_idx, param_wr_idx))
                            return 0;
                    }
                    else
                        return 0;
                };

                rdd_client.Send_Inst_Pkg(dev_idx, param_wr_idx);
            };
        };
    }

    return 0;
}
