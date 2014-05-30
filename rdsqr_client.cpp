#include "iostream"
#include <cstdlib>
#include "rdsqr_client.h"
#include "l2rc_constants.h"

using namespace std;

/* static variables */
static uint8_t param_wr_data[256] = {0};

/* class: Global_Parameters ------------------------------------ */

Global_Parameters::Global_Parameters() : dev_num(0) { }

/* ------------------------------------ class: Global_Parameters */

/* class: Device ------------------------------------ */

Device::Device() : inst_num(0), inst_id(0), param_wr_num(0), param_wr_addr(0), param_wr_size(0), param_wr_file(0), param_rd_num(0), param_rd_addr(0), param_rd_per(0) { }

Device::~Device()
{
    if (inst_id)
        delete[] inst_id;

    if (param_wr_addr)
        delete[] param_wr_addr;

    if (param_wr_size)
        delete[] param_wr_size;

    if (param_wr_file)
        delete[] param_wr_file;

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

int8_t Robot_Configuration::Get_Param_Size(uint8_t dev_id, uint8_t param_addr)
{
    if (dev_id == DXL_AX_ID)
    {
        for (uint8_t n = 0; n < dxl_ax_param_num; n++)
        {
            if (param_addr == dxl_ax_param_addr[n])
                return dxl_ax_param_size[n];
        };
    };

    cerr << "Err: parameter address '" << (int)param_addr << "' not supported" << endl;
    return -1;
}


bool Robot_Configuration::Set_Param_Val(const std::string param_name, void *param_p)
{
    string cf_line;
    string param_value_str;         /* stores the parameter value in ASCII */
    size_t line_pos;

    /* take text line */
    while ( (!cf_line.size()) || (cf_line.find_first_not_of(" \t") == string::npos) )
        getline(config_file, cf_line);
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
    };
    if (!param_name.compare("PERIOD") || !param_name.compare("ITERATION_NUM"))
        *((uint16_t *)param_p) = atoi(param_value_str.data());                  /* ASCII to integer */
    else
        *((uint8_t *)param_p) = atoi(param_value_str.data());                   /* ASCII to integer */

    return true;
}

bool Robot_Configuration::Set_Param_Array(const std::string param_name, uint8_t *param_p, uint8_t length, uint8_t dev_id, std::ifstream  *param_wr_file_p, uint8_t *param_wr_size_p)
{
    string cf_line;
    string param_value_str;         /* stores the parameter value in ASCII */
    size_t line_pos;

    /* take text line */
    while ( (!cf_line.size()) || (cf_line.find_first_not_of(" \t") == string::npos) )
        getline(config_file, cf_line);
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
            param_p[n] = atoi(param_value_str.data());      /* ASCII to integer */
        };
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

            param_value_str = cf_line.substr(value_start_pos, value_end_pos - value_start_pos);

            param_addr = Get_Param_Addr(dev_id, param_value_str);
            if (param_addr < 0)
                return false;
            param_p[n] = param_addr;

            /* open param wr files */
            if ( !param_name.compare("PARAM_WR_ID") )
            {
                int8_t param_wr_size = Get_Param_Size( dev_id, param_addr);

                if (param_wr_size < 0)
                    return false;

                param_wr_size_p[n] = param_wr_size;
                param_wr_file_p[n].open(("./" + param_value_str).data());

                if (!param_wr_file_p[n].is_open())
                {
                    cerr << "Err: file '" << param_value_str << "' could not be opened" << endl;
                    return false;
                }

                cout << "file '" << param_value_str << "' opened" << endl;
            };
        };
    };

    return true;
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
    dev_name = cf_line.substr(dev_name_start, dev_name_end - dev_name_start);

    dev_id = Get_Dev_Id(dev_name);
    if (dev_id < 0)
        return false;
    *dev_id_p = dev_id;

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

bool Robot_Configuration::Set_Parameters()
{
    /* global parameters set up */
    if (!Set_Param_Val("PERIOD", &glob_param.period))
        return 0;
    if (!Set_Param_Val("ITERATION_NUM", &glob_param.iteration_num))
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
            device[n].param_wr_size = new uint8_t[(device[n].param_wr_num)*(sizeof(uint8_t))];
            device[n].param_wr_file = new std::ifstream[ (device[n].param_wr_num)*(sizeof(std::ifstream)) ];
            if (!Set_Param_Array("PARAM_WR_ID", device[n].param_wr_addr, device[n].param_wr_num, device[n].dev_id, device[n].param_wr_file, device[n].param_wr_size ))
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
    cout << "ITERATION_NUM\t\t" << (int)glob_param.iteration_num << endl;
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
        if (device[n].param_wr_num)
        {
            cout << "PARAM_WR_ADDR\t\t";
            for (uint8_t i = 0; i < device[n].param_wr_num; i++)
                cout << (int)device[n].param_wr_addr[i] << " ";
            cout << endl;
        };
        cout << "PARAM_RD_NUM\t\t" << (int)device[n].param_rd_num << endl;
        if (device[n].param_rd_num)
        {
            cout << "PARAM_RD_ADDR\t\t";
            for (uint8_t i = 0; i < device[n].param_rd_num; i++)
                cout << (int)device[n].param_rd_addr[i] << " ";
            cout << endl;
            cout << "PARAM_RD_PER\t\t";
            for (uint8_t i = 0; i < device[n].param_rd_num; i++)
                cout << (int)device[n].param_rd_per[i] << " ";
            cout << endl;
        };
    };
    cout << endl;
}

/* ------------------------------------ class: Robot_Configuration */


/* class: RDsqr_Client ------------------------------------ */

RDsqr_Client::RDsqr_Client() : flags(0), time_step_count(0), iteration_count(0)
{
    flags |= F_RDD_TOKEN;
}

RDsqr_Client::~RDsqr_Client()
{
    if (log_file.is_open())
    {
        log_file.close();
        cout << "file 'robot_log' closed" << endl;
    };
}

bool RDsqr_Client::Open_Log_File()
{
    log_file.open("./robot_log");

    if (!log_file.is_open())
    {
        cerr << "Err: file 'robot_log' could not be created" << endl;
        return false;
    }

    cout << "file 'robot_log' created" << endl;

    return true;
}

bool RDsqr_Client::Set_Up()
{
    if (!robot_config.Open_Config_File())
        return false;

    if (!robot_config.Set_Parameters())
        return false;

    robot_config.Print_Configuration();


    Open_Log_File();

    return true;
}

bool RDsqr_Client::Send_Config()
{
    /* PERIOD */
    pkg.Set_Attributes(sizeof(robot_config.glob_param.period), P_RDD_PERIOD, (uint8_t *)(&robot_config.glob_param.period));
    pkg.Set_Opts_Field(PRO_CONFIG_PKG, 0, sizeof(robot_config.glob_param.period));
    pkg.Send();

    /* DEV_NUM */
    pkg.Set_Attributes(sizeof(uint8_t), P_RDD_DEV_NUM, &robot_config.glob_param.dev_num);
    pkg.Set_Opts_Field(PRO_CONFIG_PKG, 0, sizeof(uint8_t));
    pkg.Send();

    for (uint8_t n = 0; n < robot_config.glob_param.dev_num; n++)
    {
        uint8_t dev_id = robot_config.device[n].dev_id;
        pkg.Set_Opts_Field(PRO_CONFIG_PKG, dev_id, sizeof(uint8_t));                /* common options for device config package */

        /* DEV_ID */
        pkg.Set_Attributes(sizeof(uint8_t), P_RDD_DEV_ID, &robot_config.device[n].dev_id);
        pkg.Send();

        /* INST_NUM */
        pkg.Set_Attributes(sizeof(uint8_t), P_RDD_INST_NUM, &robot_config.device[n].inst_num);
        pkg.Send();

        /* INST_ID */
        pkg.Set_Attributes(robot_config.device[n].inst_num, P_RDD_INST_ID, robot_config.device[n].inst_id);
        pkg.Send();

        /* PARAM_WR_NUM */
        pkg.Set_Attributes(sizeof(uint8_t), P_RDD_PARAM_WR_NUM, &robot_config.device[n].param_wr_num);
        pkg.Send();

        if (robot_config.device[n].param_wr_num)
        {
            /* PARAM_WR_ADDR */
            pkg.Set_Attributes(robot_config.device[n].param_wr_num, P_RDD_PARAM_WR_ADDR, robot_config.device[n].param_wr_addr);
            pkg.Send();
        };

        /* PARAM_RD_NUM */
        pkg.Set_Attributes(sizeof(uint8_t), P_RDD_PARAM_RD_NUM, &robot_config.device[n].param_rd_num);
        pkg.Send();

        if (robot_config.device[n].param_rd_num)
        {
            /* PARAM_RD_ADDR */
            pkg.Set_Attributes(robot_config.device[n].param_rd_num, P_RDD_PARAM_RD_ADDR, robot_config.device[n].param_rd_addr);
            pkg.Send();

            /* PARAM_RD_PER */
            pkg.Set_Attributes(robot_config.device[n].param_rd_num, P_RDD_PARAM_RD_PER, robot_config.device[n].param_rd_per);
            pkg.Send();
        };
    };

    return true;
}

bool RDsqr_Client::Set_Param_Wr_Data(uint8_t dev_idx, uint8_t param_wr_idx)
{
    string mf_line;
    string value_str;
    size_t value_start = 0;
    size_t value_end;
    uint8_t param_wr_data_idx = 0;

    /* take text line */
    while ( (!mf_line.size()) || (mf_line.find_first_not_of(" \t") == string::npos) )
        getline(robot_config.device[dev_idx].param_wr_file[param_wr_idx], mf_line);
    if (robot_config.device[dev_idx].param_wr_file[param_wr_idx].eof())
        cout << "Warning: EOF reached" << endl;

    for (uint8_t n = 0; n < robot_config.device[dev_idx].inst_num; n++)
    {
        /* search for the param_wr value in the text line */
        value_start = mf_line.find_first_of("-0123456789", value_start);
        value_end = mf_line.find_first_not_of("0123456789.", value_start + 1);

        if(value_end == value_start)
        {
            cerr << "Err: invalid matrix file, missing value" << endl;
            return false;
        }

        value_str = mf_line.substr(value_start, value_end - value_start);       cout << "value_str " << value_str << endl;
        value_start = value_end + 1;

        /* fill param_wr_data */
        if (robot_config.device[dev_idx].dev_id == DXL_AX_ID)
        {
            if ( robot_config.device[dev_idx].param_wr_addr[param_wr_idx] == dxl_ax_goal_position_addr)
            {
                uint16_t dxl_angle;
                float angle = atof(value_str.data());

                if ((angle < -2.617) || (angle > 2.617))
                {
                    cerr << "Err: invalid goal_position angle value, out of range" << endl;
                    return false;
                }

                dxl_angle = -195.38*angle + 511.5;                       cout << "dxl_angle " << dxl_angle << endl;

                *(uint16_t *)(&param_wr_data[param_wr_data_idx]) = dxl_angle;
                cout << "param_wr_data[param_wr_data_idx] " << (int)param_wr_data[param_wr_data_idx] << endl;
                cout << "param_wr_data[param_wr_data_idx + 1] " << (int)param_wr_data[param_wr_data_idx + 1] << endl;
                param_wr_data_idx += robot_config.device[dev_idx].param_wr_size[param_wr_idx];
            };
        };
    };

    return true;
}

bool RDsqr_Client::Send_Inst_Pkg(uint8_t dev_idx, uint8_t param_wr_idx)
{
    uint16_t pkg_length = (robot_config.device[dev_idx].inst_num)*(robot_config.device[dev_idx].param_wr_size[param_wr_idx]);
    uint8_t param_addr = robot_config.device[dev_idx].param_wr_addr[param_wr_idx];

    /* PERIOD */
    pkg.Set_Attributes(pkg_length , param_addr, param_wr_data);
    pkg.Set_Opts_Field(PRO_INSTR_PKG, robot_config.device[dev_idx].dev_id, robot_config.device[dev_idx].param_wr_size[param_wr_idx]);
    pkg.Send();

    return true;
}

/* ------------------------------------ class: RDsqr_Client */

