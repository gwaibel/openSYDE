//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image in system definition (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISDIMAGEGROUP_H
#define C_GISDIMAGEGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiImageGroup.h"
#include "C_PuiSdDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSdImageGroup :
   public C_GiBiImageGroup,
   public stw_opensyde_gui_logic::C_PuiSdDataElement
{
public:
   C_GiSdImageGroup(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                    const QString & orc_ImagePath, QGraphicsItem * const opc_Parent = NULL);
   C_GiSdImageGroup(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                    const stw_types::float64 of64_Width, const stw_types::float64 of64_Height,
                    const QPixmap & orc_Image, QGraphicsItem * const opc_Parent = NULL);

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
