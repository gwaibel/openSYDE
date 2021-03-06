//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Provides functions to use STW flashloader protocol services to interact with flashloader server nodes

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCFLASHPROTOCOLSTWFLASHLOADER_H
#define C_OSCFLASHPROTOCOLSTWFLASHLOADER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CXFLFlashWrite.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCFlashProtocolStwFlashloader :
   public stw_diag_lib::C_XFLFlashWrite
{
public:
   typedef stw_types::sint32 (* PR_ReportProgress)(void * const opv_Instance, const stw_types::uint8 ou8_Progress,
                                                   const stw_scl::C_SCLString & orc_Text);

   PR_ReportProgress pr_ReportProgress;
   void * pv_ReportProgressInstance;

   C_OSCFlashProtocolStwFlashloader(const PR_ReportProgress opr_Progress, void * const opv_Instance);
   virtual ~C_OSCFlashProtocolStwFlashloader(void);

protected:
   //reporting functions from STW Flashloader protocol driver
   ///Report progress in 1/1000; return a value != C_NO_ERR to signal the flashing procedure to abort.
   ///This is just for informational purposes and not necessarily exactly linear to the actual progress.
   //lint -e{8001,8011}  //name of function dictated by base class
   virtual stw_types::sint32 TRG_ReportProgress(const stw_types::uint16 ou16_Progress1_1000,
                                                const stw_scl::C_SCLString & orc_AdditionalText);
   ///report status as text; can be used for logging what's going on
   //lint -e{8001,8011}  //name of function dictated by base class
   virtual void TRG_ReportStatus(const stw_scl::C_SCLString & orc_Text, const stw_types::uint8 ou8_Type);

private:
   //Avoid call
   C_OSCFlashProtocolStwFlashloader(const C_OSCFlashProtocolStwFlashloader &);
   C_OSCFlashProtocolStwFlashloader & operator =(const C_OSCFlashProtocolStwFlashloader &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
