//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBTOOLBOXUTIL_H
#define C_SEBTOOLBOXUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QVBoxLayout>
#include "stwtypes.h"
#include "C_SdTopologyListWidget.h"
#include "C_OgeFrameSeparator.h"
#include "C_OgePubIconOnly.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebToolboxUtil
{
public:
   static stw_opensyde_gui::C_SdTopologyListWidget * h_AddNewList(const QString & orc_Name,
                                                                  QVBoxLayout * const opc_Layout,
                                                                  QVector<stw_opensyde_gui::C_SdTopologyListWidget *> & orc_ListWidgets, QWidget * const opc_Parent);
   static stw_opensyde_gui_elements::C_OgeFrameSeparator * h_AddNewHeading(const QString & orc_Name,
                                                                           QVBoxLayout * const opc_Layout,
                                                                           QWidget * const opc_Parent,
                                                                           const bool oq_AddSpacerBefore);
   static std::vector<stw_opensyde_gui_elements::C_OgePubIconOnly *> h_AddNewUserHeading(const QString & orc_Name,
                                                                                         QVBoxLayout * const opc_Layout,
                                                                                         QWidget * const opc_Parent);
   static void h_InitFreeElements(QListWidget * const opc_ListWidget);
   static void h_AddElementToList(QListWidget * const opc_ListWidget, const QString & orc_Text,
                                  const QString & orc_IconPath, const QString & orc_IconPathDark = "",
                                  const QString & orc_ToolTipContent = "", const QString & orc_ToolTipHeading = "");
   static void h_AddFinalSpacer(QVBoxLayout * const opc_Layout, QListWidget * const opc_ListWidget);

   static const stw_types::sintn hsn_LabelSize;
   static const stw_types::sintn hsn_HeadingSpacerSizeTop;
   static const stw_types::sintn hsn_HeadingSpacerSizeBottom;

private:
   C_SebToolboxUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
