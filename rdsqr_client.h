#ifndef RDSQR_CLIENT_H
#define RDSQR_CLIENT_H

#include <stdint.h>
#include <fstream>
#include <string>
#include "protocol.h"

class Global_Parameters{

public:

    Global_Parameters();
    uint16_t period;
    uint16_t iteration_num;
    uint8_t dev_num;
};

class Device{

public:

    uint8_t dev_id;
    uint8_t inst_num;
    uint8_t *inst_id;
    uint8_t param_wr_num;
    uint8_t *param_wr_addr;
    uint8_t *param_wr_size;
    std::ifstream  *param_wr_file;
    uint8_t param_rd_num;
    uint8_t *param_rd_addr;
    uint8_t *param_rd_per;

    Device();
    ~Device();
};

class Robot_Configuration{

private:

    int8_t Get_Dev_Id(std::string dev_name);
    int8_t Get_Param_Addr(uint8_t dev_id, std::string param_name);
    int8_t Get_Param_Size(uint8_t dev_id, uint8_t param_addr);
    bool Set_Param_Val(const std::string param_name, void *param_p);
    bool Set_Param_Array(const std::string param_name, uint8_t *param_p, uint8_t length, uint8_t dev_id = 0, std::ifstream  *param_wr_file_p = 0, uint8_t *param_wr_size_p = 0);
    bool Set_Dev_Id(uint8_t *dev_id_p);
    bool Find_Char(char ch);

public:

    std::ifstream  config_file;
    Global_Parameters glob_param;
    Device *device;

    Robot_Configuration();
    ~Robot_Configuration();
    bool Open_Config_File();
    bool Set_Parameters();
    void Print_Configuration();
};

class RDsqr_Client{

private:

    uint8_t flags;
    uint16_t time_step_count;

    Command cmd;
    Package pkg;
    Response rpse;

    bool Open_Log_File();

public:

    RDsqr_Client();
    ~RDsqr_Client();
    std::ofstream  log_file;
    Robot_Configuration robot_config;
    uint16_t iteration_count;

    bool Set_Up();
    bool Send_Config();
    bool Send_Wr_Values();
    bool Save_Rd_Values();

    bool Set_Param_Wr_Data(uint8_t dev_idx, uint8_t param_wr_idx);
    bool Send_Inst_Pkg(uint8_t dev_idx, uint8_t param_wr_idx);
    bool Wait_Server_Response();
    void Send_Token();
};


#endif // RDSQR_CLIENT_H
