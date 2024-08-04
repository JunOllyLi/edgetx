#ifndef _MCP_PINS_FOR_BOARD_H_
#define _MCP_PINS_FOR_BOARD_H_

#define MCP23017_DIR_REG 0x03FFFFFF
#define MCP23017_PULLUP  0x03FFFFFF

#define MCP_PWR_SW_DET (1 << 25)  // G1B1

// output pins
#define MCP_PWR_EN (8 * 3 + 6)  // G1B6
#define MCP_5V_EN  (8 * 3 + 2)  // G1B2
#define MCP_ELRS_5V_EN  (8 * 3 + 3)  // G1B3
#define MCP_ELRS_BOOT   (8 * 3 + 4)  // G1B4
#define MCP_4IN1_5V_EN  (8 * 3 + 5)  // G1B3
#define MCP_EXT_5V_EN   (8 * 3 + 7)  // G1B7

#endif // _MCP_PINS_FOR_BOARD_H_