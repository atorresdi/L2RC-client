#ifndef L2RC_CONSTANTS_H
#define L2RC_CONSTANTS_H

#include <string>

#define SUPPORTED_DEVS_NUM      1
#define DYNAMIXEL_PARAM_NUM     1

/* Device ids */
#define DXL_AX_ID           1

/* Control characters */
const uint8_t ACK = 0x06;
const uint8_t NAK = 0x15;
const uint8_t SYN = 0x16;
const uint8_t SOH = 0x01;

/* Commands */
const uint8_t START = 0x73;
const uint8_t TOKEN = 't';

/* Message types */
const uint8_t PROT_CTRL_CH = 0;
const uint8_t PROT_CMD = 1;
const uint8_t PROT_PKG = 2;

/* Max consecutive sent bytes */
const uint8_t MAX_CONSEC_SENT_BYTES = 16;
const uint8_t MAX_CONSEC_RX_BYTES = 32;

/* supported devices num */
const uint8_t supp_dev_num = SUPPORTED_DEVS_NUM;

/* device names */
const std::string dev_names[SUPPORTED_DEVS_NUM] = { "DXL_AX" };

/* device ids */
const uint8_t dev_ids[SUPPORTED_DEVS_NUM] = { DXL_AX_ID };

/* Dynamixel AX series parameters address */
const uint8_t dxl_ax_goal_position_addr = 30;
const uint8_t dxl_ax_param_num = DYNAMIXEL_PARAM_NUM;
const std::string dxl_ax_param_name[DYNAMIXEL_PARAM_NUM] = { "goal_position" };
const uint8_t dxl_ax_param_addr[DYNAMIXEL_PARAM_NUM] = { dxl_ax_goal_position_addr };
const uint8_t dxl_ax_param_size[DYNAMIXEL_PARAM_NUM] = { 2 };

/* Alphabet lowercase and underscore */
const char alphabet_lc[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '_'};

/* Robot Device Driver client constants */
const uint8_t F_RDD_TOKEN = 0x01;

/* Robot configuration parameters field nums */
const uint8_t P_RDD_PERIOD              = 0;
const uint8_t P_RDD_DEV_NUM             = 1;
const uint8_t P_RDD_DEV_ID              = 2;
const uint8_t P_RDD_INST_NUM            = 3;
const uint8_t P_RDD_INST_ID             = 4;
const uint8_t P_RDD_PARAM_WR_NUM        = 5;
const uint8_t P_RDD_PARAM_WR_ADDR       = 6;
const uint8_t P_RDD_PARAM_RD_NUM        = 7;
const uint8_t P_RDD_PARAM_RD_ADDR       = 8;
const uint8_t P_RDD_PARAM_RD_PER        = 9;

/* Protocol packages types */
const uint8_t PRO_CONFIG_PKG = 0;
const uint8_t PRO_INSTR_PKG = 1;
const uint8_t PRO_ERR_PKG = 2;

#endif // L2RC_CONSTANTS_H
