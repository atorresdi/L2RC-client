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
const uint8_t dxl_ax_param_num = DYNAMIXEL_PARAM_NUM;
const std::string dxl_ax_param_name[DYNAMIXEL_PARAM_NUM] = { "goal_position" };
const uint8_t dxl_ax_param_addr[DYNAMIXEL_PARAM_NUM] = { 30 };
const uint8_t dxl_ax_param_size[DYNAMIXEL_PARAM_NUM] = { 2 };

/* Alphabet lowercase and underscore */
const char alphabet_lc[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '_'};

#endif // L2RC_CONSTANTS_H
