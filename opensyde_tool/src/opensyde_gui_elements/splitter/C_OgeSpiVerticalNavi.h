//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Vertical splitter for navi bar (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPIVERTICALNAVI_H
#define C_OGESPIVERTICALNAVI_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OgeSpiBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpiVerticalNavi :
   public C_OgeSpiBase
{
   Q_OBJECT

public:
   C_OgeSpiVerticalNavi(QWidget * const opc_Parent = NULL);

   void LoadUserSettings(void);
   void StoreUserSettings(void) const;

protected:
   virtual QSplitterHandle * createHandle(void) override;

private:
   bool mq_Loaded;
   stw_types::sintn msn_LastNaviBarSize;

   void m_OnDoubleClick(void);
};

class C_OgeSpiVerticalNaviHandle :
   public QSplitterHandle
{
   Q_OBJECT

public:
   C_OgeSpiVerticalNaviHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDoubleClick(void);

protected:
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
