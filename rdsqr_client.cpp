#include "iostream"
#include <cstdlib>
#include "rdsqr_client.h"
#include "l2rc_constants.h"

using namespace std;

/* class: Global_Parameters ------------------------------------ */

Global_Parameters::Global_Parameters() : dev_num(0) { }

/* ------------------------------------ class: Global_Parameters */

/* class: Device ------------------------------------ */

Device::Device() : inst_num(0), inst_id(0), param_wr_num(0), param_wr_addr(0), param_rd_num(0), param_rd_addr(0), param_rd_per(0) { }

Device::~Device()
{
    if (inst_id)
        delete[] inst_id;

    if (param_wr_addr)
        delete[] param_wr_addr;

    if (param_rd_addr)
        delete[] param_rd_addr;

    if (param_rd_per)
        delete[] param_rd_per;
}

/* ------------------------------------ class: Device */

/* class: Robot_Configuration ------------------------------------ */

Robot_Configuration::Robot_Configuration() : device(0) { }

Robot_Configuration::~Robot_Configuration()
{
    if (device)
        delete[] device;

    if (config_file.is_open())
    {
        config_file.close();
        cout << "file 'robot_config' closed" << endl;
    }

}

bool Robot_Configuration::Open_Config_File()
{
    config_file.open("./robot_config");

    if (!config_file.is_open())
    {
        cerr << "Err: file 'robot_config' could not be opened" << endl;
        return false;
    }

    cout << "file 'robot_config' opened" << endl;

    return true;
}

bool Robot_Configuration::Set_Param_Val(const std::string param_name, void *param_p)
{
    string cf_line;
    string param_value_str;
    size_t line_pos;

    /* take text line */
    while ( (!cf_line.size()) || (cf_line.find_first_not_of(" \t") == string::npos) )
        getline(config_file, cf_line);              cout << "cf_line " << cf_line << endl;
    if (config_file.eof())
    {
        cerr << "Err: parameter '" << param_name << "' could not be found" << endl;
        return false;
    };
    /* search for the parameter name in the text line */
    line_pos = cf_line.find(param_name);
    if (line_pos == string::npos)
    {
        cerr << "Err: parameter '" << param_name << "' could not be found" << endl;
        return false;
    };
    /* search for the equal sign in the text line */
    line_pos = cf_line.find_first_of("=");
    if (line_pos == string::npos)
    {
        cerr << "Err: invalid sintax, '=' not found after parameter '" << param_name << "'" << endl;
        return false;
    };
    /* take the parameter value from the text line */
    param_value_str.erase();
    while (line_pos != string::npos)
    {
        line_pos = cf_line.find_first_of("0123456789", line_pos + 1);

        if (line_pos != string::npos)
            param_value_str += cf_line[line_pos];
    };                                                              cout << "param_value_str " << param_value_str << endl;
    if (!param_name.compare("PERIOD") || !param_name.compare("REPEAT"))
        *((uint16_t *)param_p) = atoi(param_value_str.data());
    else
        *((uint8_t *)param_p) = atoi(param_value_str.data());

    cout << "*param_p " << *((uint16_t *)param_p) << "\n" << endl;

    return true;
}

bool Robot_Configuration::Set_Param_Array(const std::string param_name, uint8_t *param_p, uint8_t length, uint8_t dev_id)
{
    string cf_line;
    string param_value_str;
    size_t line_pos;

    /* take text line */
    while ( (!cf_line.size()) || (cf_line.find_first_not_of(" \t") == string::npos) )
        getline(config_file, cf_line);              cout << "cf_line " << cf_line << endl;
    if (config_file.eof())
    {
        cerr << "Err: parameter '" << param_name << "' could not be found" << endl;
        return false;
    };
    /* search for the parameter name in the text line */
    line_pos = cf_line.find(param_name);
    if (line_pos == string::npos)
    {
        cerr << "Err: parameter '" << param_name << "' could not be found" << endl;
        return false;
    };
    /* search for the equal sign in the text line */
    line_pos = cf_line.find_first_of("=");
    if (line_pos == string::npos)
    {
        cerr << "Err: invalid sintax, '=' not found after parameter '" << param_name << "'" << endl;
        return false;
    };
    /* search for the left bracket sign in the text line */
    line_pos = cf_line.find_first_of("{");
    if (line_pos == string::npos)
    {
        cerr << "Err: invalid sintax, '{' not found after '='" << endl;
        return false;
    };
    if ( !param_name.compare("INSTANCE_ID") || !param_name.compare("PARAM_RD_PER") )
    {
        /* take the parameter values from the text line */
        for (int n = 0; n < length; n++)
        {
            size_t value_start_pos;
            size_t value_end_pos;

            value_start_pos = cf_line.find_first_of("0123456789", line_pos + 1);
            value_end_pos = cf_line.find_first_not_of("0123456789", value_start_pos + 1);

            if ( (value_start_pos == string::npos) || (value_end_pos == string::npos))
            {
                cerr << "Err: invalid sintax, array value not found" << endl;
                return false;
            };

            line_pos = value_end_pos + 1;

            param_value_str = cf_line.substr(value_start_pos, value_end_pos - value_start_pos);
            param_p[n] = atoi(param_value_str.data());              cout << "param_p[n] " << (int)(param_p[n]) << endl;
        };               cout << endl;
    }
    else
    {
        for (int n = 0; n < length; n++)
        {
            int8_t param_addr;
            size_t value_start_pos;
            size_t value_end_pos;


            value_start_pos = cf_line.find_first_of(alphabet_lc, line_pos + 1);
            value_end_pos = cf_line.find_first_not_of(alphabet_lc, value_start_pos + 1);

            if ( (value_start_pos == string::npos) || (value_end_pos == string::npos))
            {
                cerr << "Err: invalid sintax, array value not found" << endl;
                return false;
            };

            line_pos = value_end_pos + 1;

            param_value_str = cf_line.substr(value_start_pos, value_end_pos - value_start_pos);         cout << "param_value_str " << param_value_str << endl;

            param_addr = Get_Param_Addr(dev_id, param_value_str);
            if (param_addr < 0)
                return false;
            param_p[n] = param_addr;        cout << "param_p[n] " << (int)param_p[n] << endl;
        };
    };

    return true;
}

int8_t Robot_Configuration::Get_Dev_Id(std::string dev_name)
{
    for (uint8_t n = 0; n < supp_dev_num; n++)
    {
        if (!dev_name.compare(dev_names[n]))
            return dev_ids[n];
    };

    cerr << "Err: device type '" << dev_name << "' not supported" << endl;
    return -1;
}

int8_t Robot_Configuration::Get_Param_Addr(uint8_t dev_id, std::string param_name)
{
    if (dev_id == DXL_AX_ID)
    {
        for (uint8_t n = 0; n < dxl_ax_param_num; n++)
        {
            if (!param_name.compare(dxl_ax_param_name[n]))
                return dxl_ax_param_addr[n];
        };
    };

    cerr << "Err: parameter id '" << param_name << "' not supported" << endl;
    return -1;
}

bool Robot_Configuration::Set_Dev_Id(uint8_t *dev_id_p)
{
    int8_t dev_id;
    string cf_line;
    string dev_name;
    size_t dev_name_start;
    size_t dev_name_end;

    /* take text line */
    while ( (!cf_line.size()) || (cf_line.find_first_not_of(" \t") == string::npos) )
        getline(config_file, cf_line);
    if (config_file.eof())
    {
        cerr << "Err: invalid sintax, device name not found" << endl;
        return false;
    };
    /* search for the device name in the text line */
    dev_name_start = cf_line.find_first_not_of(" \t");
    dev_name_end = cf_line.find_first_of(" {", dev_name_start);
    if ( (dev_name_start == string::npos) || (dev_name_end == string::npos))
    {
        cerr << "Err: invalid sintax, device name not found" << endl;
        return false;
    };
    dev_name = cf_line.substr(dev_name_start, dev_name_end - dev_name_start);       cout << "dev_name " << dev_name << endl;

    dev_id = Get_Dev_Id(dev_name);
    if (dev_id < 0)
        return false;
    *dev_id_p = dev_id;           cout << "*dev_id_p " << (int)(*dev_id_p) << endl;

    return true;
}

bool Robot_Configuration::Find_Char(char ch)
{
    string cf_line;

    while ( ((!cf_line.size()) || (cf_line.find_first_not_of(" \t") == string::npos)) && (!config_file.eof()) )
        getline(config_file, cf_line);
    if (cf_line.find_first_of(ch) == string::npos)
        return false;

    return true;
}

bool Robot_Configuration::Set_Parameters()
{
    /* global parameters set up */
    if (!Set_Param_Val("PERIOD", &glob_param.period))
        return 0;
    if (!Set_Param_Val("REPEAT", &glob_param.repeat))
        return 0;
    if (!Set_Param_Val("DEV_NUM", &glob_param.dev_num))
        return 0;

    /* verify the global parameters */
    if (glob_param.period < 10)
    {
        cerr << "Err: global parameter 'PERIOD' must be greater or equal than 10 ms" << endl;
        return 0;
    };
    if (!glob_param.dev_num)
    {
        cerr << "Err: global parameter 'DEV_NUM' must be greater than 0" << endl;
        return 0;
    };

    /* device parameters set up */
    device = new Device[(glob_param.dev_num)*(sizeof(Device))];

    for (uint8_t n = 0; n < glob_param.dev_num; n++)
    {
        if (!Set_Dev_Id(&device[n].dev_id))
            return 0;
        if (!Set_Param_Val("INSTANCE_NUM", &device[n].inst_num))
            return 0;
        if (device[n].inst_num <= 0)
        {
            cerr << "Err: device parameter 'INSTANCE_NUM' must be greater than 0" << endl;
            return 0;
        };

        device[n].inst_id = new uint8_t[(device[n].inst_num)*(sizeof(uint8_t))];

        if (!Set_Param_Array("INSTANCE_ID", device[n].inst_id, device[n].inst_num))
            return false;
        if (!Set_Param_Val("PARAM_WR_NUM", &device[n].param_wr_num))
            return 0;
        if (device[n].param_wr_num > 0)
        {
            device[n].param_wr_addr = new uint8_t[(device[n].param_wr_num)*(sizeof(uint8_t))];
            if (!Set_Param_Array("PARAM_WR_ID", device[n].param_wr_addr, device[n].param_wr_num, device[n].dev_id) )
                return false;
        };
        if (!Set_Param_Val("PARAM_RD_NUM", &device[n].param_rd_num))
            return 0;
        if (device[n].param_rd_num > 0)
        {
            device[n].param_rd_addr = new uint8_t[(device[n].param_rd_num)*(sizeof(uint8_t))];
            if (!Set_Param_Array("PARAM_RD_ID", device[n].param_rd_addr, device[n].param_rd_num, device[n].dev_id) )
                return false;
            device[n].param_rd_per = new uint8_t[(device[n].param_rd_num)*(sizeof(uint8_t))];
            if (!Set_Param_Array("PARAM_RD_PER", device[n].param_rd_per, device[n].param_rd_num) )
                return false;
        };
        if (!Find_Char('}'))
        {
            cerr << "Err: invalid sintax, '}' not found after device parameters" << endl;
            return false;
        };
    };

    cout << "RDsquare configured!\n" << endl;

    return true;
}

void Robot_Configuration::Print_Configuration()
{
    cout << "GLOBAL PARAMETERS" << endl;
    cout << "PERIOD\t\t\t" << (int)glob_param.period << endl;
    cout << "REPEAT\t\t\t" << (int)glob_param.repeat << endl;
    cout << "DEV_NUM\t\t\t" << (int)glob_param.dev_num << endl;
    cout << endl;
    for (uint8_t n = 0; n < (int)glob_param.dev_num; n++)
    {
        cout << "DEVICE " << (int)device[n].dev_id <<" PARAMETERS" << endl;
        cout << "INSTANCE_NUM\t\t" << (int)device[n].inst_num << endl;
        cout << "INSTANCE_ID\t\t";
        for (uint8_t i = 0; i < device[n].inst_num; i++)
            cout << (int)device[n].inst_id[i] << " ";
        cout << endl;
        cout << "PARAM_WR_NUM\t\t" << (int)device[n].param_wr_num << endl;
        cout << "PARAM_WR_ADDR\t\t";
        for (uint8_t i = 0; i < device[n].param_wr_num; i++)
            cout << (int)device[n].param_wr_addr[i] << " ";
        cout << endl;
        cout << "PARAM_RD_NUM\t\t" << (int)device[n].param_wr_num << endl;
        cout << "PARAM_RD_ADDR\t\t";
        for (uint8_t i = 0; i < device[n].param_rd_num; i++)
            cout << (int)device[n].param_rd_addr[i] << " ";
        cout << endl;
        cout << "PARAM_RD_PER\t\t";
        for (uint8_t i = 0; i < device[n].param_rd_num; i++)
            cout << (int)device[n].param_rd_per[i] << " ";
        cout << endl;
    };
    cout << endl;
}

/* ------------------------------------ class: Robot_Configuration */

