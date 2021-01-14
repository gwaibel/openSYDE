//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box which should avoid a zero value (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXFACTOR_H
#define C_OGESPXFACTOR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OgeSpxDoubleToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxFactor :
   public C_OgeSpxDoubleToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxFactor(QWidget * const opc_Parent = NULL);
   virtual void stepBy(const stw_types::sintn osn_Steps) override;
   virtual QValidator::State validate(QString & orc_Text, stw_types::sintn & orsn_Pos) const override;

   static const stw_types::float64 mhf64_FACTOR_MIN;

protected:
   virtual StepEnabled stepEnabled(void) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
