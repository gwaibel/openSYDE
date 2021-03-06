//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Align undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOALIGNCOMMAND_H
#define C_SEBUNOALIGNCOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.h"
#include "C_SebUtil.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoAlignCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoAlignCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                        const stw_types::uint64 & oru64_GuidelineItemID, const E_Alignment & ore_Alignment,
                        QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoAlignCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;

private:
   void m_Align(const stw_types::uint64 & oru64_GuidelineItemID, const E_Alignment & ore_Alignment);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
