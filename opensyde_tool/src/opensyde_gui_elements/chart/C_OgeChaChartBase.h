//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base implementation for Qt chart (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHABASE_H
#define C_OGECHABASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QChart>
#include <QGestureEvent>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChaChartBase :
   public QtCharts::QChart
{
public:
   C_OgeChaChartBase(QGraphicsItem * const opc_Parent = NULL);

   void SetDrawingActive(const bool oq_Active);

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget = NULL) override;

private:
   bool mq_DrawingActive;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
