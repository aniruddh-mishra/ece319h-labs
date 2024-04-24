/*!
 * @defgroup IR
 * @brief GPIO output to IR LED
 <table>
<caption id="IRpins">GPIO pins on the MSPM0G3507</caption>
<tr><th>Pin <th>Description
<tr><td>PA8 <td>GPIO output to IR LED, to other microcontroller IR sensor, PA22 UART2 Rx
</table>
 * @{*/
/**
 * @file      IRxmt.h
 * @brief     Initialize GPIO output to IR LED
 * @details   IR modulated at 38kHz initialization, data sent at 2000 bps baud,
 * 1 start, 8 data bits, 1 stop, no parity.<br>

 * @version   ECE319K v1.0
 * @author    your name
 * @copyright lab 8H
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      December 30, 2023
 <table>
<caption id="IRpins2">GPIO pins on the MSPM0G3507</caption>
<tr><th>Pin <th>Description
<tr><td>PA8 <td>GPIO output to IR LED, to other microcontroller IR sensor, PA22 UART2 Rx
</table>

</table>
  ******************************************************************************/
#ifndef __IRxmt_H__
#define __IRxmt_H__

/**
 * Initialize GPIO on PA8.<br>
 * PA8 UART1 Tx to other microcontroller PA9 Rx<br>
 * no synchronization on transmit, no receiving<br>
 * Baud rate=2375 evenly divides into 38,000<br>
 * 38 kHz is 26.315us period<br>
 * when pulsing, 38 kHz wave is on for 13.158us, off for 13.158us
 * @param none
 * @return none
 * @brief  Initialize IR transmission
*/
void IRxmt_Init(void);

/**
 * baud rate = 2375 bps<br>
 * bit time = 1/2375 = 421.05us<br>
 * 16 pulses per bit (receiver needs at least 10 pulses to decode IR signal)<br>
 * each pulse is 421.06us/16 = 26.315us<br>
 * negative logic: 38KHz IR pulses occur with bit=0<br>
 * if bit=0 the PA8 pulses 16 times at on for 13.158us, off for 13.158us<br>
 * if bit=1 the PA8 pulses 16 times at off for 13.158us, off for 13.158us
 * @param bit 0 or 1 to send
 * @return none
 * @brief  send one bit on IR
 * @note Normally, this function would be private, but it
 * is exposed to assign in debugging
 * */
void IRxmt_SendBit(int bit);

/**
 * Output 8-bit to IR transmission<br>
 * Uses blind synchronization<br>
 * This function uses 100% of CPU time while transmitting,
 * and should not be interrupted.
 * @param data is an 8-bit ASCII character to be transferred
 * @return none
 * @brief output character to UART1
 */
void IRxmt_OutChar(char data);


#endif // __IRxmt_H__
/** @}*/
