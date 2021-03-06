//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard chart item (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QApplication>
#include <QGraphicsView>

#include "stwtypes.h"
#include "stwerrors.h"
#include "gitypes.h"
#include "C_GiSvDaChartBase.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"
#include "TGLUtils.h"
#include "TGLTime.h"
#include "C_GtGetText.h"
#include "C_SyvDaPeBase.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvDaPeDataElementBrowse.h"
#include "C_SyvDaPeUpdateModeConfiguration.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32 C_GiSvDaChartBase::mhu32_MaximumDataElements = 10U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   Chart needs an offset for the x position of 0.25 to get a clear drawing in 100% view.

   \param[in]      oru32_ViewIndex        Index of system view
   \param[in]      oru32_DashboardIndex   Index of dashboard in system view
   \param[in]      ors32_DataIndex        Index of data element in dashboard in system view
   \param[in]      oru64_ID               Unique ID
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaChartBase::C_GiSvDaChartBase(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                     const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                     QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eCHART,
                         mhu32_MaximumDataElements,
                         oru64_ID, 500.0, 250.0, 600.0, 300.0, false, true, opc_Parent, QPointF(0.45, 0.0)),
   mpc_AddDataElement(NULL),
   mpc_AddSeperator(NULL),
   mpc_ConfigDataElement(NULL),
   mpc_RemoveDataElement(NULL),
   mpc_MiscSeperator(NULL)
{
   //lint -e{1938}  static const is guaranteed preinitialized before main
   mpc_ChartWidget = new C_SyvDaItChartWidget(oru32_ViewIndex, mhu32_MaximumDataElements);
   this->mpc_Widget->SetWidget(this->mpc_ChartWidget);

   connect(this->mpc_ChartWidget, &C_SyvDaItChartWidget::SigChartDataChanged,
           this, &C_GiSvDaChartBase::m_SetChangedChartData);

   //Activate child handles its own events for combo box pop up
   this->setHandlesChildEvents(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaChartBase::~C_GiSvDaChartBase(void) //lint !e1540  //no memory leak because of the parent of mpc_ChartWidget by
                                            // SetWidget and the Qt memory management
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiSvDaChartBase::type(void) const
{
   return msn_GRAPHICS_ITEM_DB_CHART;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in]  oe_Style       New style type
   \param[in]  oq_DarkMode    Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   C_GiSvDaRectBaseGroup::SetDisplayStyle(oe_Style, oq_DarkMode);

   if (this->mpc_ChartWidget != NULL)
   {
      this->mpc_ChartWidget->SetDisplayStyle(oe_Style, oq_DarkMode);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::ReInitializeSize(void)
{
   // TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::LoadData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if ((pc_Dashboard != NULL) &&
       (this->mpc_ChartWidget != NULL))
   {
      const C_PuiSvDbChart * const pc_Box = pc_Dashboard->GetChart(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         this->LoadSvBasicData(*pc_Box);

         this->mpc_ChartWidget->SetData(*pc_Box);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::UpdateData(void)
{
   if ((this->ms32_Index >= 0) &&
       (this->mpc_ChartWidget != NULL))
   {
      C_PuiSvDbChart c_Box = this->mpc_ChartWidget->GetData();

      this->UpdateSvBasicData(c_Box, true);

      tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                     this->mu32_DashboardIndex,
                                                                     static_cast<uint32>(this->ms32_Index),
                                                                     &c_Box, this->me_Type) == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::UpdateShowValue(void)
{
   if (this->mpc_ChartWidget != NULL)
   {
      uint32 u32_Counter;

      for (u32_Counter = 0U; u32_Counter < this->GetWidgetDataPoolElementCount(); ++u32_Counter)
      {
         sint32 s32_Return;
         QVector<float64> oc_Values;
         QVector<uint32> oc_Timestamps;

         // Get all values
         s32_Return = this->m_GetAllValues(u32_Counter, oc_Values, oc_Timestamps, false);

         if (s32_Return == C_NO_ERR)
         {
            C_PuiSvDbNodeDataPoolListElementId c_ElementId;

            // Get the associated id
            s32_Return = this->GetDataPoolElementIndex(u32_Counter, c_ElementId);

            if (s32_Return == C_NO_ERR)
            {
               // Update the chart
               this->mpc_ChartWidget->AddDataSerieContent(c_ElementId, oc_Values, oc_Timestamps);
            }
         }
      }

      // Update the time axis (one central call necessary as this was removed from the add data series calls)
      this->mpc_ChartWidget->UpdateTimeAxis();
      // Update the value axis (one central call necessary as this was removed from the add data series calls)
      this->mpc_ChartWidget->UpdateValueAxis();
   }

   C_GiSvDaRectBaseGroup::UpdateShowValue();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparence value configured by the actual timeout state

   \param[in]  ou32_DataElementIndex   Index of shown datapool element in widget
   \param[in]  osn_Value               Value for transparence (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::UpdateTransparence(const uint32 ou32_DataElementIndex, const sintn osn_Value)
{
   tgl_assert(this->mpc_ChartWidget != NULL);
   if (this->mpc_ChartWidget != NULL)
   {
      this->mpc_ChartWidget->UpdateTransparence(ou32_DataElementIndex, osn_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::ConnectionActiveChanged(const bool oq_Active)
{
   if (this->mpc_ChartWidget != NULL)
   {
      this->mpc_ChartWidget->ConnectionActiveChanged(oq_Active);
   }

   C_GiSvDaRectBaseGroup::ConnectionActiveChanged(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call properties for widgets

   \return true (configurable properties called)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaChartBase::CallProperties(void)
{
   if (this->mpc_ChartWidget != NULL)
   {
      uint32 u32_ConfigIndex;

      if (this->mpc_ChartWidget->GetCurrentDataSerie(u32_ConfigIndex) == true)
      {
         C_PuiSvDbChart & rc_Box = this->mpc_ChartWidget->GetData();

         if (u32_ConfigIndex < rc_Box.c_DataPoolElementsConfig.size())
         {
            C_PuiSvDbNodeDataPoolListElementId c_ElementId;
            C_PuiSvDbDataElementScaling c_Scaling;
            QString c_DisplayName;
            QGraphicsView * const pc_View = this->scene()->views().at(0);
            QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
            C_SyvDaPeBase * pc_Dialog;
            c_ElementId = rc_Box.c_DataPoolElementsConfig[u32_ConfigIndex].c_ElementId;
            c_Scaling = rc_Box.c_DataPoolElementsConfig[u32_ConfigIndex].c_ElementScaling;
            c_DisplayName = rc_Box.c_DataPoolElementsConfig[u32_ConfigIndex].c_DisplayName;

            pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Chart",
                                          c_ElementId, c_Scaling, true, this->mq_DarkMode, false, false, c_DisplayName);

            pc_Dialog->SetTheme(rc_Box.e_DisplayStyle);

            //Resize
            c_New->SetSize(QSize(800, 500));

            if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
            {
               QString c_WidgetName;
               C_PuiSvDbNodeDataElementConfig c_Tmp;

               c_Tmp.c_ElementId = c_ElementId;
               tgl_assert(c_ElementId == pc_Dialog->GetDataElementId());
               c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
               c_Tmp.c_DisplayName = pc_Dialog->GetDisplayName();
               rc_Box.c_DataPoolElementsConfig[u32_ConfigIndex] = c_Tmp;

               //Apply
               this->RemoveDataPoolElement(c_ElementId);
               this->RegisterDataPoolElement(c_Tmp.c_ElementId, c_Tmp.c_ElementScaling);

               if (c_Tmp.c_DisplayName.compare("") == 0)
               {
                  const C_OSCNodeDataPoolListElement * pc_OscElement =
                     C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
                        c_ElementId.u32_NodeIndex, c_ElementId.u32_DataPoolIndex, c_ElementId.u32_ListIndex,
                        c_ElementId.u32_ElementIndex);

                  const C_OSCNodeDataPool * pc_Datapool =
                     C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(c_ElementId.u32_NodeIndex,
                                                                     c_ElementId.u32_DataPoolIndex);

                  if (pc_OscElement != NULL)
                  {
                     if (pc_Datapool != NULL)
                     {
                        if (pc_Datapool->e_Type == C_OSCNodeDataPool::eHALC)
                        {
                           c_WidgetName = C_PuiSvHandler::h_GetShortNamespace(c_ElementId);
                        }
                        else
                        {
                           c_WidgetName = pc_OscElement->c_Name.c_str();
                        }
                     }
                  }
               }
               else
               {
                  c_WidgetName = c_Tmp.c_DisplayName;
               }

               this->mpc_ChartWidget->SetScaling(u32_ConfigIndex, c_WidgetName, c_Tmp.c_ElementScaling);

               tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(c_Tmp.c_ElementId) == C_NO_ERR);
               tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                              this->mu32_DashboardIndex,
                                                                              static_cast<uint32>(this->ms32_Index),
                                                                              &rc_Box,
                                                                              this->me_Type) == C_NO_ERR);
            }
            Q_EMIT this->SigTriggerUpdateTransmissionConfiguration();
            if (c_New != NULL)
            {
               c_New->HideOverlay();
            }
         } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
      }
   }
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to activate or deactivate drawing of performance heavy widgets

   \param[in]  oq_Active   Flag if widgets should currently be drawn
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::SetDrawingActive(const bool oq_Active)
{
   if (this->mpc_ChartWidget != NULL)
   {
      this->mpc_ChartWidget->SetDrawingActive(oq_Active);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates or deactivates all relevant context menu entries for this item

   Context menu functions:
   - Add data element
   - Remove data element

   \param[in]  opc_ContextMenuManager  Pointer to context menu manager for registration of actions
   \param[in]  oq_Active               Flag if context menu entries have to be shown or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::ConfigureContextMenu(C_SyvDaContextMenuManager * const opc_ContextMenuManager,
                                             const bool oq_Active)
{
   if ((oq_Active == true) &&
       (this->mpc_ChartWidget != NULL))
   {
      // Initial registration of the context menu
      if (mpc_AddDataElement == NULL)
      {
         mpc_AddDataElement = opc_ContextMenuManager->RegisterAction(C_GtGetText::h_GetText("Add data element(s)"));
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_AddDataElement->setVisible(false);
      }
      if (mpc_AddSeperator == NULL)
      {
         mpc_AddSeperator = opc_ContextMenuManager->RegisterSeperator();
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_AddSeperator->setVisible(false);
      }
      if (mpc_ConfigDataElement == NULL)
      {
         mpc_ConfigDataElement =
            opc_ContextMenuManager->RegisterAction(C_GtGetText::h_GetText("Selected data element properties"));
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_ConfigDataElement->setVisible(false);
      }
      if (mpc_RemoveDataElement == NULL)
      {
         mpc_RemoveDataElement =
            opc_ContextMenuManager->RegisterAction(C_GtGetText::h_GetText("Remove selected data element(s)"));
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_RemoveDataElement->setVisible(false);
      }
      if (mpc_MiscSeperator == NULL)
      {
         mpc_MiscSeperator = opc_ContextMenuManager->RegisterSeperator();
         // The action has to be set invisible initial. Only with that the function SetVisibleWithAutoHide can work.
         this->mpc_MiscSeperator->setVisible(false);
      }

      // Connect the signals
      if ((mpc_AddDataElement != NULL) &&
          (this->mpc_ChartWidget->GetCountDataSeries() < mhu32_MaximumDataElements))
      {
         opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_AddDataElement);
         opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_AddSeperator);
         connect(mpc_AddDataElement, &QAction::triggered, this, &C_GiSvDaChartBase::m_AddNewDataElement);
      }
      if ((mpc_ConfigDataElement != NULL) &&
          (this->mpc_ChartWidget->GetCountDataSeries() > 0))
      {
         opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_ConfigDataElement);
         connect(mpc_ConfigDataElement, &QAction::triggered,
                 this, &C_GiSvDaChartBase::CallProperties);
      }
      if ((mpc_RemoveDataElement != NULL) &&
          (this->mpc_ChartWidget->GetCountDataSeries() > 0))
      {
         opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_RemoveDataElement);
         connect(mpc_RemoveDataElement, &QAction::triggered,
                 this, &C_GiSvDaChartBase::m_RemoveDataElement);
      }
      if (this->mpc_ChartWidget->GetCountDataSeries() > 0)
      {
         opc_ContextMenuManager->SetVisibleWithAutoHide(this->mpc_MiscSeperator);
      }
   }
   else
   {
      // Disconnect the signals
      if (mpc_AddDataElement != NULL)
      {
         disconnect(mpc_AddDataElement, &QAction::triggered, this, &C_GiSvDaChartBase::m_AddNewDataElement);
      }
      if (mpc_ConfigDataElement != NULL)
      {
         disconnect(mpc_ConfigDataElement, &QAction::triggered, this, &C_GiSvDaChartBase::CallProperties);
      }
      if (mpc_RemoveDataElement != NULL)
      {
         disconnect(mpc_RemoveDataElement, &QAction::triggered,
                    this, &C_GiSvDaChartBase::m_RemoveDataElement);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the error icon

   Here: update table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::m_UpdateErrorIcon(void)
{
   //Don't call parent to not show any error icon
   if (this->mpc_ChartWidget != NULL)
   {
      for (uint32 u32_ItItem = 0UL; u32_ItItem < this->GetWidgetDataPoolElementCount(); ++u32_ItItem)
      {
         C_PuiSvDbNodeDataPoolListElementId c_Id;
         if (this->GetDataPoolElementIndex(u32_ItItem, c_Id) == C_NO_ERR)
         {
            if (c_Id.GetIsValid())
            {
               bool q_FoundError = false;
               QMap<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId, QString>::const_iterator c_ItError;
               QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId, stw_types::uint8>::const_iterator c_ItSigError;

               //Errors
               for (c_ItError = this->mc_CommmunicationErrors.begin(); c_ItError != this->mc_CommmunicationErrors.end();
                    ++c_ItError)
               {
                  if (c_ItError.key() == c_Id)
                  {
                     //Set error
                     this->mpc_ChartWidget->UpdateError(u32_ItItem, c_ItError.value(), true, true);

                     //Signal error
                     q_FoundError = true;
                  }
               }
               for (c_ItSigError = this->mc_InvalidDlcSignals.begin(); c_ItSigError != this->mc_InvalidDlcSignals.end();
                    ++c_ItSigError)
               {
                  if (c_ItSigError.key() == c_Id)
                  {
                     const QString c_Text = static_cast<QString>(C_GtGetText::h_GetText("%1 had invalid DLC %2.")).
                                            arg(C_PuiSdUtil::h_GetSignalNamespace(c_ItSigError.key())).
                                            arg(QString::number(c_ItSigError.value()));
                     //Set error
                     this->mpc_ChartWidget->UpdateError(u32_ItItem, c_Text, false, true);
                     //Signal error
                     q_FoundError = true;
                  }
               }
               if (q_FoundError == false)
               {
                  //Clear error
                  this->mpc_ChartWidget->UpdateError(u32_ItItem, "", false, false);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if warning icon is allowed

   \return
   True  Warning icon is allowed
   False Warning icon is not allowed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaChartBase::m_AllowWarningIcon(void) const
{
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get common tool tip content if no other item takes precedence over the tool tip

   \return
   Common tool tip content if no other item takes precedence over the tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_GiSvDaChartBase::m_GetCommonToolTipContent(void) const
{
   //No common tool tip!
   return "";
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::m_AddNewDataElement(void)
{
   QGraphicsView * const pc_View = this->scene()->views().at(0);

   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
   C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, this->mu32_ViewIndex, true,
                                                                                 false, false, true, true, false);

   //Resize
   c_New->SetSize(QSize(800, 800));

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      const std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();
      //Cursor
      QApplication::setOverrideCursor(Qt::WaitCursor);
      if (c_DataElements.size() > 0)
      {
         uint32 u32_Counter;

         for (u32_Counter = 0U; u32_Counter < c_DataElements.size(); ++u32_Counter)
         {
            if (c_DataElements[u32_Counter].GetIsValid() == true)
            {
               C_PuiSvDbDataElementScaling c_Scaling;
               const C_OSCNodeDataPoolListElement * const pc_Element =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_DataElements[u32_Counter].u32_NodeIndex,
                                                                             c_DataElements[u32_Counter].u32_DataPoolIndex,
                                                                             c_DataElements[u32_Counter].u32_ListIndex,
                                                                             c_DataElements[u32_Counter].u32_ElementIndex);
               if (pc_Element != NULL)
               {
                  // Get the original scaling configuration as initialization
                  c_Scaling.f64_Factor = pc_Element->f64_Factor;
                  c_Scaling.f64_Offset = pc_Element->f64_Offset;
                  c_Scaling.c_Unit = pc_Element->c_Unit.c_str();
               }

               if (this->mpc_ChartWidget != NULL)
               {
                  tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(
                                c_DataElements[u32_Counter]) == C_NO_ERR);
                  if (this->mpc_ChartWidget->AddNewDataSerie(c_DataElements[u32_Counter], c_Scaling) != C_NO_ERR)
                  {
                     break;
                  }

                  this->m_RegisterDataElementRail(c_DataElements[u32_Counter]);

                  // Apply to the project data
                  tgl_assert(this->m_SetChangedChartData() == C_NO_ERR);

                  this->RegisterDataPoolElement(c_DataElements[u32_Counter], c_Scaling);
               }
            }
         }
      }
      //Signal for error change
      Q_EMIT (this->SigDataElementsChanged());
      //Cursor
      QApplication::restoreOverrideCursor();
   }

   //Force update
   this->mq_InitialStyleCall = true;
   // Update the whole styling, because of new added gui elements
   this->SetDisplayStyle(this->me_Style, this->mq_DarkMode);

   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      c_New->HideOverlay();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::m_RemoveDataElement(void)
{
   if ((this->mpc_ChartWidget != NULL) &&
       (this->mpc_ChartWidget->GetCountDataSeries() > 0U))
   {
      bool q_Return;
      C_PuiSvDbNodeDataPoolListElementId c_ElementId;
      q_Return = this->mpc_ChartWidget->RemoveDataSerie(c_ElementId);

      if (q_Return == true)
      {
         const C_PuiSvData * pc_View;

         // Apply to the project data
         tgl_assert(this->m_SetChangedChartData() == C_NO_ERR);

         this->RemoveDataPoolElement(c_ElementId);

         //Remove read rail assignments as necessary
         pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

         if ((pc_View != NULL) &&
             (c_ElementId.GetIsValid() == true) &&
             (pc_View->CheckReadUsage(c_ElementId) == false))
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(this->mu32_ViewIndex,
                                                                               c_ElementId) == C_NO_ERR);
         }
         //Signal for error change
         Q_EMIT (this->SigDataElementsChanged());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaChartBase::m_RegisterDataElementRail(
   const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId) const
{
   if (orc_DataPoolElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                    orc_DataPoolElementId.u32_DataPoolIndex,
                                                                    orc_DataPoolElementId.u32_ListIndex,
                                                                    orc_DataPoolElementId.u32_ElementIndex);

      if (pc_Element != NULL)
      {
         C_PuiSvReadDataConfiguration c_Config;
         c_Config.u8_RailIndex = 1;
         if (pc_Element->GetArray() || (((pc_Element->GetType() == C_OSCNodeDataPoolContent::eFLOAT64) ||
                                         (pc_Element->GetType() == C_OSCNodeDataPoolContent::eSINT64)) ||
                                        (pc_Element->GetType() == C_OSCNodeDataPoolContent::eUINT64)))
         {
            c_Config.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
         }
         else
         {
            c_Config.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_CYCLIC;
         }
         c_Config.InitDefaultThreshold(pc_Element->c_MinValue, pc_Element->c_MaxValue);
         C_PuiSvHandler::h_GetInstance()->AddViewReadRailItem(this->mu32_ViewIndex, orc_DataPoolElementId,
                                                              c_Config);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply new chart content

   WARNING: Data element changes have to trigger RegisterDataElement

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiSvDaChartBase::m_SetChangedChartData(void)
{
   sint32 s32_Retval = C_NO_ERR;

   if ((this->ms32_Index >= 0) &&
       (this->mpc_ChartWidget != NULL))
   {
      const C_PuiSvDbChart & rc_Data = this->mpc_ChartWidget->GetData();

      tgl_assert(rc_Data.c_DataPoolElementsActive.size() ==
                 rc_Data.c_DataPoolElementsConfig.size());
      for (uint32 u32_ItEl = 0UL; u32_ItEl < rc_Data.c_DataPoolElementsConfig.size(); ++u32_ItEl)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = rc_Data.c_DataPoolElementsConfig[u32_ItEl];
         tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(rc_Config.c_ElementId) == C_NO_ERR);
      }
      s32_Retval = C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                       static_cast<uint32>(this->ms32_Index),
                                                                       &rc_Data, C_PuiSvDbDataElement::eCHART);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
