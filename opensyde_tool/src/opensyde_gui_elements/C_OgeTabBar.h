//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETABBAR_H
#define C_OGETABBAR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTabBar>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTabBar :
   public QTabBar
{
public:
   C_OgeTabBar(QWidget * const opc_Parent = NULL);

protected:
   virtual QSize tabSizeHint(const stw_types::sintn osn_Index) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
