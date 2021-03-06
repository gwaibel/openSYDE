//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN bus (header)

   CAN bus (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILICANBUS_H
#define C_GILICANBUS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsItem>

#include "C_GiLiBus.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiCANBus :
   public C_GiLiBus
{
public:
   C_GiLiCANBus(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                C_GiTextElementBus * const opc_TextElementName, const bool oq_DoErrorCheck,
                const std::vector<QPointF> * const opc_Points = NULL, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiLiCANBus();

   virtual stw_types::sintn type() const override;
   virtual bool OpenStyleDialog(void) override;
   virtual stw_opensyde_core::C_OSCSystemBus::E_Type GetType() const override;

private:
   //Avoid call
   C_GiLiCANBus(const C_GiLiCANBus &);
   C_GiLiCANBus & operator =(const C_GiLiCANBus &); //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
