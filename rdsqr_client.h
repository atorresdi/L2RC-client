#ifndef RDSQR_CLIENT_H
#define RDSQR_CLIENT_H

#include <stdint.h>
#include <fstream>
#include <string>

class Global_Parameters{
public:
    Global_Parameters();
    uint16_t period;
    uint16_t repeat;
    uint8_t dev_num;
};

class Device{

public:
    uint8_t dev_id;
    uint8_t inst_num;
    uint8_t *inst_id;
    uint8_t param_wr_num;
    uint8_t *param_wr_addr;
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
    bool Set_Param_Val(const std::string param_name, void *param_p);
    bool Set_Param_Array(const std::string param_name, uint8_t *param_p, uint8_t length, uint8_t dev_id = 0);
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


#endif // RDSQR_CLIENT_H
