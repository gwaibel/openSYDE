//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      STW specific standard defines and types

   Standard typedefs in compliance with STW C++ Coding Rules.
   here: for 32bit compilation under Windows.

   Suitable for applications compiled with:
   - Embarcadero C++ Builder
   - MinGW
   - MS Visual C++

   \copyright   Copyright 2007 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef STWTYPESH
#define STWTYPESH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <stdint.h>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
#ifdef __cplusplus
namespace stw_types
{
#endif
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

typedef uint8_t  uint8;      ///< data type  8bit unsigned
typedef int8_t   sint8;      ///< data type  8bit signed
typedef uint16_t uint16;     ///< data type 16bit unsigned
typedef int16_t  sint16;     ///< data type 16bit signed
typedef uint32_t uint32;     ///< data type 32bit unsigned
typedef int32_t  sint32;     ///< data type 32bit signed
typedef uint64_t uint64;     ///< data type 64bit unsigned
typedef int64_t  sint64;      ///< data type 64bit signed
typedef float float32;             ///< data type IEEE 32bit float
typedef double float64;            ///< data type IEEE 64bit float
typedef long double float80;       ///< data type IEEE 80bit float

// native data types
typedef unsigned int uintn; ///< data type native unsigned int
typedef signed int sintn;   ///< data type native signed int
typedef char charn;         ///< data type native char

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif
