#ifndef L2RC_CONSTANTS_H
#define L2RC_CONSTANTS_H

/* Control characters */
const uint8_t ACK = 0x06;
const uint8_t NAK = 0x15;
const uint8_t SYN = 0x16;

/* Commands */
const uint8_t START = 0x73;

/* Message types */
const uint8_t PROT_CTRL_CH = 0;
const uint8_t PROT_CMD = 1;
const uint8_t PROT_PKG = 2;

#endif // L2RC_CONSTANTS_H