/*
 * BITOPS.h
 *
 *  Created on: 2013/4/17
 *      Author: 300212
 */

#ifndef BITOPS_H_
#define BITOPS_H_

#include <stdint.h>

/*
 * _REG_    : register data
 * _SIZE_   : bit size in register
 * _POS_    : bit position(offset) in register
 * _VAL_    : bit value to write at specific position in register address
 */

/**< Calculate the bit mask **/
#define HX_BIT_MASK(_SIZE_, _POS_) ((uint32_t)(((_SIZE_)==32)?0xFFFFFFFF:((0x1 << (_SIZE_)) - 1)) << (_POS_))

/**< Clear the specific bit value **/
#define HX_BIT_CLEAR(_REG_, _SIZE_, _POS_) ((_REG_) = ((_REG_) & ~((uint32_t)HX_BIT_MASK(_SIZE_, _POS_))))

/**< Clear the specific MASK BIT **/
#define HX_BIT_MCLEAR(_REG_, _MASK_) ((_REG_) = ((_REG_) & ~(_MASK_)))

/**< Get the specific bit value **/
#define HX_BIT_GET(_REG_, _SIZE_, _POS_) (uint32_t)(((_REG_) & (uint32_t) HX_BIT_MASK(_SIZE_, _POS_)) >> (_POS_))

/**< Get the specific MASK bit value **/
#define HX_BIT_MGET(_REG_, _MASK_) ((_REG_) = ((_REG_) & (_MASK_)) >> (_POD_))

/**< Set the specific bit value **/
#define HX_BIT_SET(_REG_, _SIZE_, _POS_, _VAL_) ((_REG_) = ((uint32_t) (_REG_) & (uint32_t)~(HX_BIT_MASK(_SIZE_, _POS_))) | \
                                        ((uint32_t) ((uint32_t) (_VAL_) << (uint32_t)(_POS_)) & HX_BIT_MASK(_SIZE_, _POS_)))

/**< Set the specific MASK bit value **/
#define HX_BIT_MSET(_REG_, _MASK_) ((_REG_) = ((_REG_) | (_MASK_)))

/**< Generate register value according bit value **/
#define HX_BIT_GEN(_SIZE_, _POS_, _VAL_) ((uint32_t)(((uint32_t)(_VAL_) << (uint32_t)(_POS_)) & ((uint32_t)HX_BIT_MASK(_SIZE_, _POS_))))

#endif /* BITOPS_H_ */
