//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data part of UI system definition

   Data part of UI system definition

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_SdBueSortHelper.h"
#include "C_PuiSdHandlerData.h"
#include "C_OSCLoggingHandler.h"
#include "C_PuiSdHandlerFiler.h"
#include "C_PuiBsElementsFiler.h"
#include "C_PuiSdHandlerFilerV2.h"
#include "C_OSCSystemDefinitionFiler.h"
#include "C_OSCSystemDefinitionFilerV2.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system definition

   Load system definition and store in information in our instance data.

   \param[in] orc_Path          Path to system definition file
   \param[in] opu16_FileVersion Optional storage for file version

   \return
   C_NO_ERR    data read and placed into instance data
   C_RD_WR     problems accessing file system (e.g. no read access to file)
   C_RANGE     specified file does not exist (when loading)
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    content of file is invalid or incomplete
   C_OVERFLOW  node in system definition references a device not part of the device definitions
   C_CHECKSUM  verify of system definition failed. Loaded ui part does not match to loaded core part
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerData::LoadFromFile(const stw_scl::C_SCLString & orc_Path, uint16 * const opu16_FileVersion)
{
   sint32 s32_Return = C_NO_ERR;

   const uint16 u16_TimerId = osc_write_log_performance_start();

   if (TGL_FileExists(orc_Path) == true)
   {
      C_OSCXMLParser c_XMLParser;
      s32_Return = c_XMLParser.LoadFromFile(orc_Path);
      if (s32_Return == C_NO_ERR)
      {
         uint16 u16_FileVersion;
         //We need to use the old format to improve loading performance in compatibility mode
         s32_Return = C_OSCSystemDefinitionFiler::h_LoadSystemDefinition(
            mc_CoreDefinition, c_XMLParser,
            C_Uti::h_GetAbsolutePathFromExe("../devices/devices.ini").toStdString().c_str(),
            orc_Path, true, &u16_FileVersion);
         if (opu16_FileVersion != NULL)
         {
            *opu16_FileVersion = u16_FileVersion;
         }
         if (s32_Return == C_NO_ERR)
         {
            if ((u16_FileVersion == 1U) || (u16_FileVersion == 2U))
            {
               //Deprecated version: reuse same XML parser
               tgl_assert(c_XMLParser.SelectRoot() == "opensyde-system-definition");
               tgl_assert(c_XMLParser.SelectNodeChild("nodes") == "nodes");

               s32_Return = C_PuiSdHandlerFilerV2::h_LoadNodes(this->mc_UINodes, c_XMLParser);

               if (s32_Return == C_NO_ERR)
               {
                  //Return
                  tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-definition");
                  //Bus
                  tgl_assert(c_XMLParser.SelectNodeChild("buses") == "buses");
                  s32_Return = C_PuiSdHandlerFilerV2::h_LoadBuses(this->mc_UIBuses, c_XMLParser);
               }
               //GUI items
               this->c_Elements.Clear();
               this->c_BusTextElements.clear();
               if (s32_Return == C_NO_ERR)
               {
                  //Return
                  tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-definition");
                  if (c_XMLParser.SelectNodeChild("gui-only") == "gui-only")
                  {
                     //bus text elements
                     if (c_XMLParser.SelectNodeChild("bus-text-elements") == "bus-text-elements")
                     {
                        s32_Return = C_PuiSdHandlerFilerV2::h_LoadBusTextElements(this->c_BusTextElements, c_XMLParser);
                        tgl_assert(c_XMLParser.SelectNodeParent() == "gui-only");
                     }
                     else
                     {
                        s32_Return = C_CONFIG;
                     }
                     if (s32_Return == C_NO_ERR)
                     {
                        //Base elements
                        s32_Return = C_PuiBsElementsFiler::h_LoadBaseElements(this->c_Elements, c_XMLParser);
                     }
                  }
                  else
                  {
                     s32_Return = C_CONFIG;
                  }
               }
            }
            else
            {
               QString c_FilePath = C_PuiSdHandlerFiler::h_GetSystemDefinitionUiFilePath(orc_Path.c_str());

               //Load separate UI files
               s32_Return = C_PuiSdHandlerFiler::h_LoadSystemDefinitionUiFile(c_FilePath, this->mc_UINodes,
                                                                              this->mc_UIBuses, this->c_BusTextElements,
                                                                              this->c_Elements);

               // Loading separate shared Datapool configuration
               if (s32_Return == C_NO_ERR)
               {
                  c_FilePath = C_PuiSdHandlerFiler::h_GetSharedDatapoolUiFilePath(orc_Path.c_str());
                  s32_Return = C_PuiSdHandlerFiler::h_LoadSharedDatapoolsFile(c_FilePath, this->mc_SharedDatapools);

                  if (s32_Return != C_NO_ERR)
                  {
                     osc_write_log_error("Loading shared Datapool configuration UI",
                                         "Could not load shared Datapool configuration UI. Error code: " +
                                         stw_scl::C_SCLString::IntToStr(s32_Return));
                  }
               }
               else
               {
                  osc_write_log_error("Loading System Definition UI",
                                      "Could not load System Definition UI. Error code: " +
                                      stw_scl::C_SCLString::IntToStr(s32_Return));
               }

               if (s32_Return == C_NO_ERR)
               {
                  //calculate the hash value and save it for comparing (only for new file version!)
                  this->mu32_CalculatedHashSystemDefinition = this->CalcHashSystemDefinition();
               }
            }
         }
         else
         {
            osc_write_log_error("Loading System Definition", "Could not load System Definition. Error code: " +
                                stw_scl::C_SCLString::IntToStr(s32_Return));
         }

         if (s32_Return == C_NO_ERR)
         {
            //Fix inconsistency problems with existing projects (notify user about changes, so do this after CRC update)
            m_FixCommInconsistencyErrors();
            m_FixAddressIssues();
            m_FixNameIssues();
         }

         //AFTER automated adaptions!
         //signal "node change"
         Q_EMIT this->SigNodesChanged();
         //signal "bus change"
         Q_EMIT this->SigBussesChanged();
      }
      else
      {
         osc_write_log_error("Loading System Definition",
                             "File \"" + orc_Path + "\" could not be opened.");
         s32_Return = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading System Definition", "File \"" + orc_Path + "\" does not exist.");
      s32_Return = C_RANGE;
   }

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = m_VerifyLoadedSystemDefintion();
   }

   osc_write_log_performance_stop(u16_TimerId, "Load system definition");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save system definition (V2 file format)

   Save UI data part of system definition to XML file.

   \param[in] orc_Path                     Path to system definition file
   \param[in] oq_UseDeprecatedFileFormatV2 Flag to enable saving using the deprecated V2 file format

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (e.g. could not erase pre-existing file before saving)
   C_COM      Bus message sorting failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerData::SaveToFile(const stw_scl::C_SCLString & orc_Path, const bool oq_UseDeprecatedFileFormatV2)
{
   sint32 s32_Return = C_NO_ERR;

   const uint16 u16_TimerId = osc_write_log_performance_start();

   //Sort first
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCSystemDefinition c_SortedOSCDefinition = this->mc_CoreDefinition;
      std::vector<C_PuiSdNode> c_SortedUiNodes = this->mc_UINodes;
      for (uint32 u32_ItNode = 0; (u32_ItNode < c_SortedUiNodes.size()) && (s32_Return == C_NO_ERR); ++u32_ItNode)
      {
         s32_Return = C_PuiSdHandlerData::mh_SortMessagesByName(c_SortedOSCDefinition.c_Nodes[u32_ItNode],
                                                                c_SortedUiNodes[u32_ItNode]);
      }
      //Save sorted structure
      if (s32_Return == C_NO_ERR)
      {
         if (TGL_FileExists(orc_Path) == true)
         {
            //erase it:
            sintn sn_Return;
            sn_Return = std::remove(orc_Path.c_str());
            if (sn_Return != 0)
            {
               osc_write_log_error("Saving System Definition",
                                   "Could not erase pre-existing file \"" + orc_Path + "\".");
               s32_Return = C_RD_WR;
            }
         }
         if (s32_Return == C_NO_ERR)
         {
            if (oq_UseDeprecatedFileFormatV2)
            {
               C_OSCXMLParser c_XMLParser;
               C_OSCSystemDefinitionFilerV2::h_SaveSystemDefinition(c_SortedOSCDefinition, c_XMLParser);
               //Reuse same XML parser for deprecated file format
               tgl_assert(c_XMLParser.SelectRoot() == "opensyde-system-definition");
               tgl_assert(c_XMLParser.SelectNodeChild("nodes") == "nodes");

               C_PuiSdHandlerFilerV2::h_SaveNodes(c_SortedUiNodes, c_XMLParser);
               tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-definition"); //back up

               //Bus
               tgl_assert(c_XMLParser.SelectNodeChild("buses") == "buses");
               C_PuiSdHandlerFilerV2::h_SaveBuses(this->mc_UIBuses, c_XMLParser);
               tgl_assert(c_XMLParser.SelectNodeParent() == "opensyde-system-definition"); //back up

               //GUI items
               c_XMLParser.CreateAndSelectNodeChild("gui-only");

               //Bus text elements
               c_XMLParser.CreateAndSelectNodeChild("bus-text-elements");
               C_PuiSdHandlerFilerV2::h_SaveBusTextElements(this->c_BusTextElements, c_XMLParser);
               tgl_assert(c_XMLParser.SelectNodeParent() == "gui-only"); //back up

               //Base elements
               C_PuiBsElementsFiler::h_SaveBaseElements(this->c_Elements, c_XMLParser);

               s32_Return = c_XMLParser.SaveToFile(orc_Path);
               if (s32_Return != C_NO_ERR)
               {
                  osc_write_log_error("Saving System Definition", "Could not write to file \"" + orc_Path + "\".");
                  s32_Return = C_RD_WR;
               }
            }
            else
            {
               s32_Return = C_OSCSystemDefinitionFiler::h_SaveSystemDefinitionFile(c_SortedOSCDefinition, orc_Path);
               if (s32_Return == C_NO_ERR)
               {
                  QString c_FilePath = C_PuiSdHandlerFiler::h_GetSystemDefinitionUiFilePath(orc_Path.c_str());
                  //New files for UI
                  s32_Return = C_PuiSdHandlerFiler::h_SaveSystemDefinitionUiFile(c_FilePath, c_SortedOSCDefinition,
                                                                                 c_SortedUiNodes, this->mc_UIBuses,
                                                                                 this->c_BusTextElements,
                                                                                 this->c_Elements);

                  // Saving shared Datapool configuration
                  if (s32_Return == C_NO_ERR)
                  {
                     c_FilePath = C_PuiSdHandlerFiler::h_GetSharedDatapoolUiFilePath(orc_Path.c_str());
                     s32_Return = C_PuiSdHandlerFiler::h_SaveSharedDatapoolsFile(c_FilePath, this->mc_SharedDatapools);

                     if (s32_Return != C_NO_ERR)
                     {
                        osc_write_log_error("Saving shared Datapool configuration UI",
                                            "Could not write to file \"" + orc_Path + "\".");
                        s32_Return = C_RD_WR;
                     }
                  }
                  else
                  {
                     osc_write_log_error("Saving System Definition UI",
                                         "Could not write to file \"" + orc_Path + "\".");
                     s32_Return = C_RD_WR;
                  }
               }
               //Only update hash in non deprecated mode
               //calculate the hash value and save it for comparing
               this->mu32_CalculatedHashSystemDefinition = this->CalcHashSystemDefinition();
            }
         }
      }
      else
      {
         s32_Return = C_COM;
      }
   }
   else
   {
      s32_Return = C_COM;
   }

   osc_write_log_performance_stop(u16_TimerId, "Save system definition");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compares the last saved hash value against the actual hash

   \return
   false    Hash has not changed
   true     Hash has changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerData::HasHashChanged(void) const
{
   const uint32 u32_NewHash = this->CalcHashSystemDefinition();
   bool q_Changed = true;

   if (u32_NewHash == this->mu32_CalculatedHashSystemDefinition)
   {
      q_Changed = false;
   }

   return q_Changed;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value of the system definition

   Start value is 0xFFFFFFFF

   \return
   Calculated hash value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdHandlerData::CalcHashSystemDefinition(void) const
{
   // init value of CRC
   uint32 u32_Hash = 0xFFFFFFFFU;
   uint32 u32_Counter;

   // calculate the hash for the core elements
   this->mc_CoreDefinition.CalcHash(u32_Hash);

   // calculate the hash for the ui elements
   this->c_Elements.CalcHash(u32_Hash);

   for (u32_Counter = 0U; u32_Counter < this->c_BusTextElements.size(); ++u32_Counter)
   {
      this->c_BusTextElements[u32_Counter].CalcHash(u32_Hash);
   }

   for (u32_Counter = 0U; u32_Counter < this->mc_UINodes.size(); ++u32_Counter)
   {
      this->mc_UINodes[u32_Counter].CalcHash(u32_Hash);
   }

   for (u32_Counter = 0U; u32_Counter < this->mc_UIBuses.size(); ++u32_Counter)
   {
      this->mc_UIBuses[u32_Counter].CalcHash(u32_Hash);
   }

   this->mc_SharedDatapools.CalcHash(u32_Hash);

   return u32_Hash;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerData::Clear(void)
{
   this->mc_UIBuses.clear();
   this->mc_UINodes.clear();
   this->mc_CoreDefinition.c_Buses.clear();
   this->mc_CoreDefinition.c_Nodes.clear();
   this->c_Elements.Clear();
   this->c_BusTextElements.clear();
   this->mc_SharedDatapools.Clear();

   //Reset hash
   this->mu32_CalculatedHashSystemDefinition = this->CalcHashSystemDefinition();
   //signal "node change"
   Q_EMIT this->SigNodesChanged();
   //signal "bus change"
   Q_EMIT this->SigBussesChanged();
   Q_EMIT this->SigSyncClear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get snapshot of all current items

   \return
   Snapshot containing all items
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdTopologyDataSnapshot C_PuiSdHandlerData::GetSnapshot(void) const
{
   C_SdTopologyDataSnapshot c_Retval;

   //Linked objects
   c_Retval.c_OSCBuses = this->mc_CoreDefinition.c_Buses;
   c_Retval.c_UIBuses = this->mc_UIBuses;
   c_Retval.c_OSCNodes = this->mc_CoreDefinition.c_Nodes;
   c_Retval.c_UINodes = this->mc_UINodes;

   //Independent objects
   this->c_Elements.ReplaceSnapshotElements(c_Retval);
   c_Retval.c_BusTextElements = this->c_BusTextElements;

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get reference to core system definition

   \return
   Core system definition
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSystemDefinition & C_PuiSdHandlerData::GetOSCSystemDefinition(void)
{
   return this->mc_CoreDefinition;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get reference to core system definition

   \return
   Core system definition
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCSystemDefinition & C_PuiSdHandlerData::GetOSCSystemDefinitionConst(void) const
{
   return this->mc_CoreDefinition;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt given string to fulfill c variable name requirements

   \param[in] orc_Input               String to process

   \return
   String fulfilling c variable name requirements
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSdHandlerData::h_AutomaticCStringAdaptation(const QString & orc_Input)
{
   QString c_Retval;

   for (sintn sn_It = 0; sn_It < orc_Input.length(); ++sn_It)
   {
      if (C_OSCUtils::h_CheckValidCName(orc_Input.at(sn_It).toLatin1()) == false)
      {
         c_Retval += "_";
      }
      else
      {
         c_Retval += orc_Input.at(sn_It);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandlerData::C_PuiSdHandlerData(QObject * const opc_Parent) :
   QObject(opc_Parent),
   mu32_CalculatedHashSystemDefinition(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do sort of messages by name for specified node

   \param[in,out] orc_OSCNode Node core part
   \param[in,out] orc_UiNode  Node UI part

   \return
   C_NO_ERR Operation success
   C_CONFIG Operation failure: configuration invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerData::mh_SortMessagesByName(C_OSCNode & orc_OSCNode, C_PuiSdNode & orc_UiNode)
{
   sint32 s32_Retval = C_NO_ERR;

   //No sync manager necessary because sorting the nodes by name internally
   // does not change how the messages are merged together

   //Protocols
   //---------
   tgl_assert(orc_UiNode.c_UICanProtocols.size() == orc_OSCNode.c_ComProtocols.size());
   if (orc_UiNode.c_UICanProtocols.size() == orc_OSCNode.c_ComProtocols.size())
   {
      for (uint32 u32_ItProtocol = 0;
           (u32_ItProtocol < orc_OSCNode.c_ComProtocols.size()) && (s32_Retval == C_NO_ERR); ++u32_ItProtocol)
      {
         C_OSCCanProtocol & rc_OSCProtocol = orc_OSCNode.c_ComProtocols[u32_ItProtocol];

         //Data pools
         //----------
         tgl_assert((rc_OSCProtocol.u32_DataPoolIndex < orc_OSCNode.c_DataPools.size()) &&
                    (rc_OSCProtocol.u32_DataPoolIndex < orc_UiNode.c_UIDataPools.size()));
         if ((rc_OSCProtocol.u32_DataPoolIndex < orc_OSCNode.c_DataPools.size()) &&
             (rc_OSCProtocol.u32_DataPoolIndex < orc_UiNode.c_UIDataPools.size()))
         {
            C_PuiSdNodeCanProtocol & rc_UiProtocol = orc_UiNode.c_UICanProtocols[u32_ItProtocol];

            //Message containers
            //------------------
            tgl_assert(rc_UiProtocol.c_ComMessages.size() == rc_OSCProtocol.c_ComMessages.size());
            if (rc_UiProtocol.c_ComMessages.size() == rc_OSCProtocol.c_ComMessages.size())
            {
               for (uint32 u32_ItContainer = 0;
                    (u32_ItContainer < rc_OSCProtocol.c_ComMessages.size()) && (s32_Retval == C_NO_ERR);
                    ++u32_ItContainer)
               {
                  C_OSCCanMessageContainer & rc_OSCContainer = rc_OSCProtocol.c_ComMessages[u32_ItContainer];
                  C_PuiSdNodeCanMessageContainer & rc_UiContainer =
                     rc_UiProtocol.c_ComMessages[u32_ItContainer];

                  //Messages
                  //--------
                  tgl_assert((rc_OSCContainer.c_RxMessages.size() == rc_UiContainer.c_RxMessages.size()) &&
                             (rc_OSCContainer.c_TxMessages.size() == rc_UiContainer.c_TxMessages.size()));
                  if ((rc_OSCContainer.c_RxMessages.size() == rc_UiContainer.c_RxMessages.size()) &&
                      (rc_OSCContainer.c_TxMessages.size() == rc_UiContainer.c_TxMessages.size()))
                  {
                     //Lists
                     //-----
                     //Container index -> interface index
                     C_OSCNodeDataPool & rc_OSCDataPool = orc_OSCNode.c_DataPools[rc_OSCProtocol.u32_DataPoolIndex];
                     const sint32 s32_TxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_OSCDataPool,
                                                                                     u32_ItContainer,
                                                                                     true);
                     const sint32 s32_RxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_OSCDataPool,
                                                                                     u32_ItContainer,
                                                                                     false);
                     C_PuiSdNodeDataPool & rc_UiDataPool = orc_UiNode.c_UIDataPools[rc_OSCProtocol.u32_DataPoolIndex];
                     tgl_assert(((((s32_TxListIndex >= 0) && (s32_RxListIndex >= 0)) &&
                                  (rc_OSCDataPool.c_Lists.size() == rc_UiDataPool.c_DataPoolLists.size())) &&
                                 (static_cast<uint32>(s32_TxListIndex) < rc_OSCDataPool.c_Lists.size())) &&
                                (static_cast<uint32>(s32_RxListIndex) < rc_OSCDataPool.c_Lists.size()));
                     if (((((s32_TxListIndex >= 0) && (s32_RxListIndex >= 0)) &&
                           (rc_OSCDataPool.c_Lists.size() == rc_UiDataPool.c_DataPoolLists.size())) &&
                          (static_cast<uint32>(s32_TxListIndex) < rc_OSCDataPool.c_Lists.size())) &&
                         (static_cast<uint32>(s32_RxListIndex) < rc_OSCDataPool.c_Lists.size()))
                     {
                        C_OSCNodeDataPoolList & rc_OSCTxList =
                           rc_OSCDataPool.c_Lists[static_cast<uint32>(s32_TxListIndex)];
                        C_PuiSdNodeDataPoolList & rc_UiTxList =
                           rc_UiDataPool.c_DataPoolLists[static_cast<uint32>(s32_TxListIndex)];
                        //Start sorting
                        //-------------
                        s32_Retval = C_SdBueSortHelper::h_SortOneMessageVector(rc_OSCContainer.c_TxMessages,
                                                                               rc_UiContainer.c_TxMessages,
                                                                               rc_OSCTxList,
                                                                               rc_UiTxList);
                        if (s32_Retval == C_NO_ERR)
                        {
                           C_OSCNodeDataPoolList & rc_OSCRxList =
                              rc_OSCDataPool.c_Lists[static_cast<uint32>(s32_RxListIndex)];
                           C_PuiSdNodeDataPoolList & rc_UiRxList =
                              rc_UiDataPool.c_DataPoolLists[static_cast<uint32>(s32_RxListIndex)];
                           //Start sorting
                           //-------------
                           s32_Retval = C_SdBueSortHelper::h_SortOneMessageVector(rc_OSCContainer.c_RxMessages,
                                                                                  rc_UiContainer.c_RxMessages,
                                                                                  rc_OSCRxList,
                                                                                  rc_UiRxList);
                        }
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }
                  }
                  else
                  {
                     s32_Retval = C_CONFIG;
                  }
                  //Trigger data element index recalculation
                  rc_OSCContainer.ReCalcDataElementIndices();
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get hash for node

   \param[in] ou32_NodeIndex Index

   \return
   Hash for bus
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdHandlerData::m_GetHashNode(const uint32 ou32_NodeIndex) const
{
   uint32 u32_Retval = 0xFFFFFFFFU;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      rc_Node.CalcHash(u32_Retval);
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get hash for bus

   \param[in] ou32_BusIndex Index

   \return
   Hash for bus
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSdHandlerData::m_GetHashBus(const uint32 ou32_BusIndex) const
{
   uint32 u32_Retval = 0xFFFFFFFFU;

   if (ou32_BusIndex < this->mc_CoreDefinition.c_Buses.size())
   {
      const C_OSCSystemBus & rc_Bus = this->mc_CoreDefinition.c_Buses[ou32_BusIndex];
      rc_Bus.CalcHash(u32_Retval);
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to fix name errors for existing projects
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerData::m_FixNameIssues(void)
{
   for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      rc_OSCNode.c_Properties.c_Name = C_PuiSdHandlerData::h_AutomaticCStringAdaptation(
         rc_OSCNode.c_Properties.c_Name.c_str()).toStdString().c_str();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to fix address errors for existing projects
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerData::m_FixAddressIssues(void)
{
   for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      rc_OSCNode.RecalculateAddress();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to fix inconsistency errors for internally created inconsistencies in existing projects
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerData::m_FixCommInconsistencyErrors(void)
{
   //Nodes
   //-----
   tgl_assert(this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size());
   if (this->mc_UINodes.size() == this->mc_CoreDefinition.c_Nodes.size())
   {
      for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size();
           ++u32_ItNode)
      {
         C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         C_PuiSdNode & rc_UiNode = this->mc_UINodes[u32_ItNode];

         //Protocols
         //---------
         tgl_assert(rc_UiNode.c_UICanProtocols.size() == rc_OSCNode.c_ComProtocols.size());
         if (rc_UiNode.c_UICanProtocols.size() == rc_OSCNode.c_ComProtocols.size())
         {
            for (uint32 u32_ItProtocol = 0;
                 u32_ItProtocol < rc_OSCNode.c_ComProtocols.size(); ++u32_ItProtocol)
            {
               uint32 u32_CANInterfaceCounter = 0;
               C_OSCCanProtocol & rc_OSCProtocol = rc_OSCNode.c_ComProtocols[u32_ItProtocol];
               C_PuiSdNodeCanProtocol & rc_UiProtocol = rc_UiNode.c_UICanProtocols[u32_ItProtocol];

               //Fix interface inconsistency (message container)
               for (uint32 u32_ItInterface = 0; u32_ItInterface < rc_OSCNode.c_Properties.c_ComInterfaces.size();
                    ++u32_ItInterface)
               {
                  const C_OSCNodeComInterfaceSettings & rc_CurInterface =
                     rc_OSCNode.c_Properties.c_ComInterfaces[u32_ItInterface];
                  if (rc_CurInterface.e_InterfaceType == C_OSCSystemBus::eCAN)
                  {
                     ++u32_CANInterfaceCounter;
                  }
               }
               if (u32_CANInterfaceCounter != rc_OSCProtocol.c_ComMessages.size())
               {
                  rc_OSCProtocol.c_ComMessages.resize(u32_CANInterfaceCounter);
                  rc_UiProtocol.c_ComMessages.resize(u32_CANInterfaceCounter);
                  osc_write_log_warning("Loading project",
                                        "The number of COMM message containers is inconsistent with the number"
                                        " of CAN interfaces in your node,"
                                        " so the number of COMM message containers was"
                                        " changed to the expected level.");
               }

               //Fix interface inconsistency (lists)
               //Data pools
               //----------
               tgl_assert((rc_OSCProtocol.u32_DataPoolIndex < rc_OSCNode.c_DataPools.size()) &&
                          (rc_OSCProtocol.u32_DataPoolIndex < rc_UiNode.c_UIDataPools.size()));
               if ((rc_OSCProtocol.u32_DataPoolIndex < rc_OSCNode.c_DataPools.size()) &&
                   (rc_OSCProtocol.u32_DataPoolIndex < rc_UiNode.c_UIDataPools.size()))
               {
                  C_OSCNodeDataPool & rc_OSCDataPool = rc_OSCNode.c_DataPools[rc_OSCProtocol.u32_DataPoolIndex];
                  C_PuiSdNodeDataPool & rc_UiDataPool = rc_UiNode.c_UIDataPools[rc_OSCProtocol.u32_DataPoolIndex];

                  //Lists
                  //-----
                  tgl_assert(rc_OSCDataPool.c_Lists.size() == rc_UiDataPool.c_DataPoolLists.size());
                  if (rc_OSCDataPool.c_Lists.size() == rc_UiDataPool.c_DataPoolLists.size())
                  {
                     //Message containers
                     //------------------
                     tgl_assert(rc_UiProtocol.c_ComMessages.size() == rc_OSCProtocol.c_ComMessages.size());
                     if (rc_UiProtocol.c_ComMessages.size() == rc_OSCProtocol.c_ComMessages.size())
                     {
                        //Inconsistency error check
                        if ((rc_OSCDataPool.c_Lists.size() % 2) == 0)
                        {
                           if (rc_OSCDataPool.c_Lists.size() < (rc_OSCProtocol.c_ComMessages.size() * 2))
                           {
                              rc_OSCProtocol.c_ComMessages.resize(rc_OSCDataPool.c_Lists.size() / 2);
                              rc_UiProtocol.c_ComMessages.resize(rc_OSCDataPool.c_Lists.size() / 2);
                              osc_write_log_warning("Loading project",
                                                    "The number of COMM Datapools are inconsistent to the number"
                                                    " of COMM Message container in your COMM protocols"
                                                    " so the number of COMM Message container were"
                                                    " reduced to the expected level.");
                           }
                        }
                        else
                        {
                           osc_write_log_warning("Loading project",
                                                 "At least one COMM Datapool has an invalid number of lists"
                                                 " (should be double the number of CAN interfaces)");
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the loaded system definition for a valid configuration

   Cross check between core and ui parts

   \retval   C_NO_ERR     System definition is valid
   \retval   C_CHECKSUM   System definition is not valid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerData::m_VerifyLoadedSystemDefintion(void) const
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mc_UINodes.size() != this->mc_CoreDefinition.c_Nodes.size())
   {
      osc_write_log_error(
         "Verifying System Definition",
         static_cast<QString>("UI part does not match core part: Number of nodes (UI %1 vs. core %2).").
         arg(this->mc_UINodes.size()).
         arg(this->mc_CoreDefinition.c_Nodes.size()).toStdString().c_str());

      s32_Return = C_CHECKSUM;
   }
   else if (this->mc_UIBuses.size() != this->mc_CoreDefinition.c_Buses.size())
   {
      osc_write_log_error(
         "Verifying System Definition",
         static_cast<QString>("UI part does not match core part: Number of buses (UI %1 vs. core %2).").
         arg(this->mc_UIBuses.size()).
         arg(this->mc_CoreDefinition.c_Buses.size()).toStdString().c_str());

      s32_Return = C_CHECKSUM;
   }
   else
   {
      uint32 u32_Nodes;

      // Check nodes
      for (u32_Nodes = 0U; u32_Nodes < this->mc_UINodes.size(); ++u32_Nodes)
      {
         const C_PuiSdNode & rc_UiNode = this->mc_UINodes[u32_Nodes];
         const C_OSCNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[u32_Nodes];

         if (rc_UiNode.c_UIDataPools.size() != rc_OscNode.c_DataPools.size())
         {
            osc_write_log_error(
               "Verifying System Definition",
               static_cast<QString>("UI part does not match core part: Number of Datapools of node %1 "
                                    "(UI %2 vs. core %3).").
               arg(rc_OscNode.c_Properties.c_Name.c_str()).
               arg(rc_UiNode.c_UIDataPools.size()).
               arg(rc_OscNode.c_DataPools.size()).toStdString().c_str());

            s32_Return = C_CHECKSUM;
         }
         else
         {
            // Check Datapools
            uint32 u32_Datapool;

            for (u32_Datapool = 0U; u32_Datapool < rc_OscNode.c_DataPools.size(); ++u32_Datapool)
            {
               const C_PuiSdNodeDataPool & rc_UiDp = rc_UiNode.c_UIDataPools[u32_Datapool];
               const C_OSCNodeDataPool & rc_OscDp = rc_OscNode.c_DataPools[u32_Datapool];

               if (rc_UiDp.c_DataPoolLists.size() != rc_OscDp.c_Lists.size())
               {
                  osc_write_log_error(
                     "Verifying System Definition",
                     static_cast<QString>("UI part does not match core part: Number of Datapool lists of "
                                          "Datapool %1::%2 (UI %3 vs. core %4).").
                     arg(rc_OscNode.c_Properties.c_Name.c_str()).
                     arg(rc_OscDp.c_Name.c_str()).
                     arg(rc_UiDp.c_DataPoolLists.size()).
                     arg(rc_OscDp.c_Lists.size()).toStdString().c_str());

                  s32_Return = C_CHECKSUM;
               }
               else
               {
                  // Check lists
                  uint32 u32_List;

                  for (u32_List = 0U; u32_List < rc_OscDp.c_Lists.size(); ++u32_List)
                  {
                     const C_PuiSdNodeDataPoolList & rc_UiList = rc_UiDp.c_DataPoolLists[u32_List];
                     const C_OSCNodeDataPoolList & rc_OscList = rc_OscDp.c_Lists[u32_List];

                     if (rc_UiList.c_DataPoolListElements.size() != rc_OscList.c_Elements.size())
                     {
                        osc_write_log_error(
                           "Verifying System Definition",
                           static_cast<QString>("UI part does not match core part: Number of Datapool"
                                                " list elements of list %1::%2::%3 (UI %4 vs. core %5).").
                           arg(rc_OscNode.c_Properties.c_Name.c_str()).
                           arg(rc_OscDp.c_Name.c_str()).
                           arg(rc_OscList.c_Name.c_str()).
                           arg(rc_UiList.c_DataPoolListElements.size()).
                           arg(rc_OscList.c_Elements.size()).toStdString().c_str());

                        s32_Return = C_CHECKSUM;
                     }
                  }
               }
            }
         }

         if (rc_UiNode.c_UICanProtocols.size() != rc_OscNode.c_ComProtocols.size())
         {
            osc_write_log_error(
               "Verifying System Definition",
               static_cast<QString>("UI part does not match core part: Number of CAN protocols of node %1 "
                                    "(UI %2 vs. core %3).").
               arg(rc_OscNode.c_Properties.c_Name.c_str()).
               arg(rc_UiNode.c_UICanProtocols.size()).
               arg(rc_OscNode.c_ComProtocols.size()).toStdString().c_str());

            s32_Return = C_CHECKSUM;
         }
         else
         {
            // Check Protocols
            uint32 u32_Protocol;

            for (u32_Protocol = 0U; u32_Protocol < rc_OscNode.c_ComProtocols.size(); ++u32_Protocol)
            {
               const C_PuiSdNodeCanProtocol & rc_UiProtocol = rc_UiNode.c_UICanProtocols[u32_Protocol];
               const C_OSCCanProtocol & rc_OscProtocol = rc_OscNode.c_ComProtocols[u32_Protocol];

               if (rc_UiProtocol.c_ComMessages.size() != rc_OscProtocol.c_ComMessages.size())
               {
                  osc_write_log_error(
                     "Verifying System Definition",
                     static_cast<QString>("UI part does not match core part: Number of message containers"
                                          " of node %1 (UI %2 vs. core %3).").
                     arg(rc_OscNode.c_Properties.c_Name.c_str()).
                     arg(rc_UiProtocol.c_ComMessages.size()).
                     arg(rc_OscProtocol.c_ComMessages.size()).toStdString().c_str());

                  s32_Return = C_CHECKSUM;
               }
               else
               {
                  uint32 u32_MsgContainer;

                  for (u32_MsgContainer = 0U; u32_MsgContainer < rc_OscProtocol.c_ComMessages.size();
                       ++u32_MsgContainer)
                  {
                     // Check Tx messages
                     if (rc_UiProtocol.c_ComMessages[u32_MsgContainer].c_TxMessages.size() !=
                         rc_OscProtocol.c_ComMessages[u32_MsgContainer].c_TxMessages.size())
                     {
                        osc_write_log_error(
                           "Verifying System Definition",
                           static_cast<QString>("UI part does not match core part: Number of Tx Messages"
                                                " of node %1.(UI %2 vs. core %3)").
                           arg(rc_OscNode.c_Properties.c_Name.c_str()).
                           arg(rc_UiProtocol.c_ComMessages[u32_MsgContainer].c_TxMessages.size()).
                           arg(rc_OscProtocol.c_ComMessages[u32_MsgContainer].c_TxMessages.size()).
                           toStdString().c_str());

                        s32_Return = C_CHECKSUM;
                     }
                     else
                     {
                        uint32 u32_Msg;

                        for (u32_Msg = 0U;
                             u32_Msg < rc_OscProtocol.c_ComMessages[u32_MsgContainer].c_TxMessages.size();
                             ++u32_Msg)
                        {
                           const C_PuiSdNodeCanMessage & rc_UiMsg =
                              rc_UiProtocol.c_ComMessages[u32_MsgContainer].c_TxMessages[u32_Msg];
                           const C_OSCCanMessage & rc_OscMsg =
                              rc_OscProtocol.c_ComMessages[u32_MsgContainer].c_TxMessages[u32_Msg];

                           // Check signals
                           if (rc_UiMsg.c_Signals.size() != rc_OscMsg.c_Signals.size())
                           {
                              osc_write_log_error(
                                 "Verifying System Definition",
                                 static_cast<QString>("UI part does not match core part: Number of Tx Signals"
                                                      " of node %1 in CAN message %2 (UI %3 vs. core %4).").
                                 arg(rc_OscNode.c_Properties.c_Name.c_str()).
                                 arg(rc_OscMsg.c_Name.c_str()).
                                 arg(rc_UiMsg.c_Signals.size()).
                                 arg(rc_OscMsg.c_Signals.size()).toStdString().c_str());

                              s32_Return = C_CHECKSUM;
                           }
                        }
                     }

                     // Check Rx messages
                     if (rc_UiProtocol.c_ComMessages[u32_MsgContainer].c_RxMessages.size() !=
                         rc_OscProtocol.c_ComMessages[u32_MsgContainer].c_RxMessages.size())
                     {
                        osc_write_log_error(
                           "Verifying System Definition",
                           static_cast<QString>("UI part does not match core part: Number of Rx Messages"
                                                " of node %1 (UI %2 vs. core %3).").
                           arg(rc_OscNode.c_Properties.c_Name.c_str()).
                           arg(rc_UiProtocol.c_ComMessages[u32_MsgContainer].c_RxMessages.size()).
                           arg(rc_OscProtocol.c_ComMessages[u32_MsgContainer].c_RxMessages.size()).
                           toStdString().c_str());

                        s32_Return = C_CHECKSUM;
                     }
                     else
                     {
                        uint32 u32_Msg;

                        for (u32_Msg = 0U;
                             u32_Msg < rc_OscProtocol.c_ComMessages[u32_MsgContainer].c_RxMessages.size();
                             ++u32_Msg)
                        {
                           const C_PuiSdNodeCanMessage & rc_UiMsg =
                              rc_UiProtocol.c_ComMessages[u32_MsgContainer].c_RxMessages[u32_Msg];
                           const C_OSCCanMessage & rc_OscMsg =
                              rc_OscProtocol.c_ComMessages[u32_MsgContainer].c_RxMessages[u32_Msg];

                           // Check signals
                           if (rc_UiMsg.c_Signals.size() != rc_OscMsg.c_Signals.size())
                           {
                              osc_write_log_error(
                                 "Verifying System Definition",
                                 static_cast<QString>("UI part does not match core part: Number of Rx Signals"
                                                      " of node %1 in CAN message %2 (UI %3 vs. core %4).").
                                 arg(rc_OscNode.c_Properties.c_Name.c_str()).
                                 arg(rc_OscMsg.c_Name.c_str()).
                                 arg(rc_UiMsg.c_Signals.size()).
                                 arg(rc_OscMsg.c_Signals.size()).toStdString().c_str());

                              s32_Return = C_CHECKSUM;
                           }
                        }
                     }
                  }
               }
            }
         }
      }

      // Nothing to check for buses
   }

   return s32_Return;
}
