/*
 * This file is part of the Bus Pirate project
 * (http://code.google.com/p/the-bus-pirate/).
 *
 * Written and maintained by the Bus Pirate project.
 *
 * To the extent possible under law, the project has
 * waived all copyright and related or neighboring rights to Bus Pirate. This
 * work is published from United States.
 *
 * For details see: http://creativecommons.org/publicdomain/zero/1.0/.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "configuration.h"

#include "base.h"
#include "bus_pirate_core.h"

#ifdef BP_ENABLE_1WIRE_SUPPORT
#include "1wire.h"
#endif /* BP_ENABLE_1WIRE_SUPPORT */

#ifdef BP_ENABLE_UART_SUPPORT
#include "uart.h"
#endif /* BP_ENABLE_UART_SUPPORT */

#ifdef BP_ENABLE_I2C_SUPPORT
#include "i2c.h"
#endif /* BP_ENABLE_I2C_SUPPORT */

#ifdef BP_ENABLE_SPI_SUPPORT
#include "spi.h"
#endif /* BP_ENABLE_SPI_SUPPORT */

#ifdef BP_ENABLE_JTAG_SUPPORT
#include "jtag.h"
#endif /* BP_ENABLE_JTAG_SUPPORT */

#ifdef BP_ENABLE_RAW_2WIRE_SUPPORT
#include "raw2wire.h"
#endif /* BP_ENABLE_RAW_2WIRE_SUPPORT */

#ifdef BP_ENABLE_RAW_3WIRE_SUPPORT
#include "raw3wire.h"
#endif /* BP_ENABLE_RAW_3WIRE_SUPPORT */

#ifdef BP_ENABLE_PC_AT_KEYBOARD_SUPPORT
#include "pc_at_keyboard.h"
#endif /* BP_ENABLE_PC_AT_KEYBOARD_SUPPORT */

#ifdef BP_ENABLE_HD44780_SUPPORT
#include "hd44780.h"
#endif /* BP_ENABLE_HD44780_SUPPORT */

#ifdef BP_ENABLE_PIC_SUPPORT
#include "pic.h"
#endif /* BP_ENABLE_PIC_SUPPORT */

#ifdef BP_ENABLE_DIO_SUPPORT
#include "dio.h"
#endif /* BP_ENABLE_DIO_SUPPORT */

/**
 * Predefined "print settings" callback for protocols that do not have any
 * settings to begin with.
 *
 * Given the circumstances, all this function does is to print a CR+LF character
 * pair to the serial port.
 */
static void empty_print_settings_implementation(void);

/**
 * Prints the state of the pins for the high impedance mode.
 */
static void hiz_print_pins_state(void);

/**
 * Resets the current mode configuration to 8-bits data.
 */
static void reset_mode_to_8_bits(void);

/**
 * Predefined callback for running a protocol macro when there are none.
 *
 * All it does is print an appropriate error message to the serial port, and
 * flag an error condition.
 *
 * @param[in] value the macro to run, currently unused.
 */
static void null_macro_callback(uint16_t value);

/**
 * Predefined callback for running an operation without any expected I/O.
 *
 * All it does is print an appropriate error message to the serial port, and
 * flag an error condition.
 */
static void null_operation_callback(void);

/**
 * Predefined callback for sending data when no I/O is available.
 *
 * All it does is print an appropriate error message to the serial port, and
 * flag an error condition.
 *
 * @param[in] data the data to be sent, currently ignored.
 *
 * @return 0x0100, indicating an error condition?
 */
static uint16_t null_send_callback(uint16_t data);

/**
 * Predefined callback for reading data when no I/O is available.
 *
 * All it does is print an appropriate error message to the serial port, and
 * flag an error condition.
 *
 * @return dummy data, 0 in this case.
 */
static uint16_t null_data_read_callback(void);

/**
 * Predefined callback for reading a bit when no I/O is available.
 *
 * All it does is print an appropriate error message to the serial port, and
 * flag an error condition.
 *
 * @return dummy bit, OFF in this case.
 */
static bool null_bit_read_callback(void);

extern bus_pirate_configuration_t bus_pirate_configuration;
extern mode_configuration_t mode_configuration;
extern bool command_error;

bus_pirate_protocol_t protos[MAXPROTO] = {{/* protocol_start */
                                           null_operation_callback,
                                           /* protocol_start_with_read */
                                           null_operation_callback,
                                           /* protocol_stop */
                                           null_operation_callback,
                                           /* protocol_stop_from_read */
                                           null_operation_callback,
                                           /* protocol_send */
                                           null_send_callback,
                                           /* protocol_read */
                                           null_data_read_callback,
                                           /* protocol_clock_high */
                                           null_operation_callback,
                                           /* protocol_clock_low */
                                           null_operation_callback,
                                           /* protocol_data_high */
                                           null_operation_callback,
                                           /* protocol_data_low */
                                           null_operation_callback,
                                           /* protocol_data_state */
                                           null_data_read_callback,
                                           /* protocol_clock_pulse */
                                           null_operation_callback,
                                           /* protocol_read_bit */
                                           null_bit_read_callback,
                                           /* protocol_periodic_update */
                                           null_bit_read_callback,
                                           /* protocol_run_macro */
                                           null_macro_callback,
                                           /* protocol_setup */
                                           reset_mode_to_8_bits,
                                           /* protocol_get_ready */
                                           reset_mode_to_8_bits,
                                           /* protocol_cleanup */
                                           reset_mode_to_8_bits,
                                           /* protocol_print_pins_state */
                                           hiz_print_pins_state,
                                           /* protocol_print_settings */
                                           empty_print_settings_implementation,
                                           /* name */
                                           "HiZ"}

#ifdef BP_ENABLE_1WIRE_SUPPORT
                                          ,
                                          {/* protocol_start */
                                           onewire_reset,
                                           /* protocol_start_with_read */
                                           onewire_reset,
                                           /* protocol_stop */
                                           null_operation_callback,
                                           /* protocol_stop_from_read */
                                           null_operation_callback,
                                           /* protocol_send */
                                           onewire_write,
                                           /* protocol_read */
                                           onewire_read,
                                           /* protocol_clock_high */
                                           null_operation_callback,
                                           /* protocol_clock_low */
                                           null_operation_callback,
                                           /* protocol_data_high */
                                           onewire_data_high,
                                           /* protocol_data_low */
                                           onewire_data_low,
                                           /* protocol_data_state */
                                           onewire_data_state,
                                           /* protocol_clock_pulse */
                                           onewire_clock_pulse,
                                           /* protocol_read_bit */
                                           onewire_read_bit,
                                           /* protocol_periodic_update */
                                           null_bit_read_callback,
                                           /* protocol_run_macro */
                                           onewire_run_macro,
                                           /* protocol_setup */
                                           onewire_setup,
                                           /* protocol_get_ready */
                                           onewire_setup,
                                           /* protocol_cleanup */
                                           reset_mode_to_8_bits,
                                           /* protocol_print_pins_state */
                                           onewire_pins_state,
                                           /* protocol_print_settings */
                                           empty_print_settings_implementation,
                                           /* name */
                                           "1-WIRE"}
#endif /* BP_ENABLE_1WIRE_SUPPORT */

#ifdef BP_ENABLE_UART_SUPPORT
                                          ,
                                          {UARTstart,
                                           UARTstart,
                                           UARTstop,
                                           UARTstop,
                                           UARTwrite,
                                           UARTread,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_data_read_callback,
                                           null_operation_callback,
                                           null_bit_read_callback,
                                           UARTperiodic,
                                           UARTmacro,
                                           UARTsetup,
                                           UARTsetup_exc,
                                           UARTcleanup,
                                           UARTpins,
                                           UARTsettings,
                                           "UART"}
#endif /* BP_ENABLE_UART_SUPPORT */

#ifdef BP_ENABLE_I2C_SUPPORT
                                          ,
                                          {I2Cstart,
                                           I2Cstart,
                                           I2Cstop,
                                           I2Cstop,
                                           I2Cwrite,
                                           I2Cread,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_data_read_callback,
                                           null_operation_callback,
                                           null_bit_read_callback,
                                           null_bit_read_callback,
                                           I2Cmacro,
                                           I2Csetup,
                                           I2Csetup_exc,
                                           I2Ccleanup,
                                           I2Cpins,
                                           I2Csettings,
                                           "I2C"}
#endif /* BP_ENABLE_I2C_SUPPORT */

#ifdef BP_ENABLE_SPI_SUPPORT
                                          ,
                                          {SPIstart,
                                           SPIstartr,
                                           SPIstop,
                                           SPIstop,
                                           SPIwrite,
                                           SPIread,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_data_read_callback,
                                           null_operation_callback,
                                           null_bit_read_callback,
                                           null_bit_read_callback,
                                           SPImacro,
                                           SPIsetup,
                                           SPIsetup_exc,
                                           SPIcleanup,
                                           SPIpins,
                                           SPIsettings,
                                           "SPI"}
#endif /* BP_ENABLE_SPI_SUPPORT */

#ifdef BP_ENABLE_RAW_2WIRE_SUPPORT
                                          ,
                                          {R2Wstart,     R2Wstart,
                                           R2Wstop,      R2Wstop,
                                           R2Wwrite,     R2Wread,
                                           R2Wclkh,      R2Wclkl,
                                           R2Wdath,      R2Wdatl,
                                           R2Wbitp,      R2Wclk,
                                           R2Wbitr,      null_bit_read_callback,
                                           R2Wmacro,     R2Wsetup,
                                           R2Wsetup_exc, reset_mode_to_8_bits,
                                           R2Wpins,      R2Wsettings,
                                           "2WIRE"}
#endif /* BP_ENABLE_RAW_2WIRE_SUPPORT */

#ifdef BP_ENABLE_RAW_3WIRE_SUPPORT
                                          ,
                                          {R3Wstart,
                                           R3Wstartr,
                                           R3Wstop,
                                           R3Wstop,
                                           R3Wwrite,
                                           R3Wread,
                                           R3Wclkh,
                                           R3Wclkl,
                                           R3Wdath,
                                           R3Wdatl,
                                           R3Wbitp,
                                           R3Wclk,
                                           R3Wbitr,
                                           null_bit_read_callback,
                                           null_macro_callback,
                                           R3Wsetup,
                                           R3Wsetup_exc,
                                           reset_mode_to_8_bits,
                                           R3Wpins,
                                           R3Wsettings,
                                           "3WIRE"}
#endif /* BP_ENABLE_RAW_3WIRE_SUPPORT */

#ifdef BP_ENABLE_PC_AT_KEYBOARD_SUPPORT
                                          ,
                                          {null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           KEYBwrite,
                                           KEYBread,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_data_read_callback,
                                           null_operation_callback,
                                           null_bit_read_callback,
                                           null_bit_read_callback,
                                           KEYBmacro,
                                           KEYBsetup,
                                           KEYBsetup_exc,
                                           reset_mode_to_8_bits,
                                           hiz_print_pins_state,
                                           empty_print_settings_implementation,
                                           "KEYB"}
#endif /* BP_ENABLE_PC_AT_KEYBOARD_SUPPORT */

#ifdef BP_ENABLE_HD44780_SUPPORT
                                          ,
                                          {LCDstart,
                                           LCDstart,
                                           LCDstop,
                                           LCDstop,
                                           LCDwrite,
                                           null_data_read_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_data_read_callback,
                                           null_operation_callback,
                                           null_bit_read_callback,
                                           null_bit_read_callback,
                                           LCDmacro,
                                           LCDsetup,
                                           LCDsetup_exc,
                                           spiDisable,
                                           LCDpins,
                                           empty_print_settings_implementation,
                                           "LCD"}
#endif /* BP_ENABLE_HD44780_SUPPORT */

#ifdef BP_ENABLE_PIC_SUPPORT
                                          ,
                                          {picstart,
                                           picstart,
                                           picstop,
                                           picstop,
                                           picwrite,
                                           picread,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_data_read_callback,
                                           null_operation_callback,
                                           null_bit_read_callback,
                                           null_bit_read_callback,
                                           picmacro,
                                           picinit,
                                           picinit_exc,
                                           piccleanup,
                                           picpins,
                                           empty_print_settings_implementation,
                                           "PIC"}
#endif /* BP_ENABLE_PIC_SUPPORT */

#ifdef BP_ENABLE_DIO_SUPPORT
                                          ,
                                          {null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           dio_write,
                                           dio_read,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           null_data_read_callback,
                                           null_operation_callback,
                                           null_bit_read_callback,
                                           null_bit_read_callback,
                                           null_macro_callback,
                                           null_operation_callback,
                                           null_operation_callback,
                                           reset_mode_to_8_bits,
                                           hiz_print_pins_state,
                                           empty_print_settings_implementation,
                                           "DIO"}
#endif /* BP_ENABLE_DIO_SUPPORT */
};

void null_operation_callback(void) {
  BPMSG1059;
  command_error = true;
}

uint16_t null_send_callback(uint16_t data) {
  BPMSG1059;
  command_error = true;
  return 0x100;
}

uint16_t null_data_read_callback(void) {
  BPMSG1059;
  command_error = true;
  return 0;
}

bool null_bit_read_callback(void) {
  BPMSG1059;
  command_error = true;
  return OFF;
}

void null_macro_callback(uint16_t value) {
  BPMSG1059;
  command_error = true;
}

void empty_print_settings_implementation(void) { bpBR; }

void hiz_print_pins_state(void) {
#ifdef BUSPIRATEV4
  BPMSG1258;
#else
  BPMSG1225;
#endif /* BUSPIRATEV4 */
}

void reset_mode_to_8_bits(void) {
  /* Sets the mode configuration to 8 bits. */

  mode_configuration.numbits = 8;
  mode_configuration.int16 = 0;
}