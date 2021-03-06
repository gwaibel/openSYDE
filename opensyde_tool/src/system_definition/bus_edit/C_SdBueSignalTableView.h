//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal table view (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUESIGNALTABLEVIEW_H
#define C_SDBUESIGNALTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSortFilterProxyModel>
#include "C_TblViewScroll.h"
#include "C_SdBueSignalTableModel.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"
#include "C_SdBueMessageSignalTableDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueSignalTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdBueSignalTableView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueSignalTableView(void);

   void LoadUserSettings(const std::vector<stw_types::sint32> & orc_Values);
   void SaveUserSettings(std::vector<stw_types::sint32> & orc_Values) const;
   void SetMessageSyncManager(stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void UpdateData(void);
   stw_types::sintn GetCountRows(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigSignalSelected(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                          const stw_types::uint32 & oru32_SignalIndex);

protected:
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueSignalTableView(const C_SdBueSignalTableView &);
   C_SdBueSignalTableView & operator =(const C_SdBueSignalTableView &);

   void m_InitColumns(void);

   QSortFilterProxyModel mc_SortProxyModel;
   stw_opensyde_gui_logic::C_SdBueSignalTableModel mc_Model;
   stw_opensyde_gui_logic::C_SdBueMessageSignalTableDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
