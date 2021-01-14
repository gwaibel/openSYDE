//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for generate code

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "C_GtGetText.h"
#include "C_Uti.h"
#include "C_SdCodeGenerationDialog.h"
#include "ui_C_SdCodeGenerationDialog.h"

#include "C_PuiSdHandler.h"
#include "C_OSCNode.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdCodeGenerationDialog::C_SdCodeGenerationDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdCodeGenerationDialog),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdCodeGenerationDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &C_OgePubCancel::clicked, this,
           &C_SdCodeGenerationDialog::m_OnCancel);
   connect(this->mpc_Ui->pc_TreeView, &C_SdCodeGenerationView::SigSelectionChanged, this,
           &C_SdCodeGenerationDialog::m_UpdateSelection);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdCodeGenerationDialog::~C_SdCodeGenerationDialog()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationDialog::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Code Generation"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Data Blocks Selection"));
   this->mpc_Ui->pc_LabelHeading->setText(C_GtGetText::h_GetText("Data Blocks of type \"Programmable Application\""));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Continue"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks the preconditions for the import

   \param[in]  orc_NodesIndices  Nodes indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationDialog::PrepareDialog(const std::vector<uint32> & orc_NodesIndices) const
{
   if (!orc_NodesIndices.empty())
   {
      // Prepare dialog
      this->mpc_Ui->pc_TreeView->Init(orc_NodesIndices);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the checked items to code generation

   \param[out]  orc_NodeIndices         Node indices (ID)
   \param[out]  orc_AppIndicesPerNode   Vector of vectors of application indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationDialog::GetCheckedItems(std::vector<uint32> & orc_NodeIndices,
                                               std::vector<std::vector<uint32> > & orc_AppIndicesPerNode) const
{
   this->mpc_Ui->pc_TreeView->GetCheckedItems(orc_NodeIndices, orc_AppIndicesPerNode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_OkClicked();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationDialog::m_OnCancel(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update number of selected items

   \param[in] osn_SelectionCount Number of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationDialog::m_UpdateSelection(const sintn osn_SelectionCount) const
{
   if (osn_SelectionCount > 0)
   {
      this->mpc_Ui->pc_SelectionLabel->setText(static_cast<QString>(C_GtGetText::h_GetText("%1 Data Block(s) selected")).
                                               arg(osn_SelectionCount));
      this->mpc_Ui->pc_PushButtonOk->setDisabled(false);
   }
   else
   {
      this->mpc_Ui->pc_SelectionLabel->setText(static_cast<QString>(C_GtGetText::h_GetText("No Data Block(s) selected")));
      this->mpc_Ui->pc_PushButtonOk->setDisabled(true);
   }
}
