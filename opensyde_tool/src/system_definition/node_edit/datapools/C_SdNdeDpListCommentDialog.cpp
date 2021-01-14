//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog for editing Datapool list comment.

   Popup dialog for editing Datapool list comment.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDpListCommentDialog.h"
#include "ui_C_SdNdeDpListCommentDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
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

   \param[in,out]  orc_Parent          Reference to parent
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListCommentDialog::C_SdNdeDpListCommentDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                       const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_DataPoolIndex,
                                                       const stw_types::uint32 ou32_ListIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpListCommentDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   mu32_DataPoolIndex(ou32_DataPoolIndex),
   mu32_ListIndex(ou32_ListIndex)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // start with all selected
   this->mpc_Ui->pc_TextEditComment->selectAll();

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeDpListCommentDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked,
           this, &C_SdNdeDpListCommentDialog::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListCommentDialog::~C_SdNdeDpListCommentDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListCommentDialog::InitStaticNames(void) const
{
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);

   if (pc_List != NULL)
   {
      mrc_ParentDialog.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("List %1")).arg(pc_List->c_Name.c_str()));
      this->mpc_Ui->pc_TextEditComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));
      this->mpc_Ui->pc_TextEditComment->setText(pc_List->c_Comment.c_str());
   }
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Comment Edit"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get comment from text field

   \return
   Comment
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDpListCommentDialog::GetComment(void) const
{
   return this->mpc_Ui->pc_TextEditComment->toPlainText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListCommentDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
void C_SdNdeDpListCommentDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListCommentDialog::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}
