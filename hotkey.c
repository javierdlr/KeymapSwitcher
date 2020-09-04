//#define HDEBUG

#include "dockybase.h"

#include "KeyMapSwitcher_rev.h"

#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
//#define CATCOMP_CODE
#include "include/keymapswitcher_strings.h"
extern struct LocaleInfo li;
//extern char textbuf[150]; // for localized messages

extern struct ExecIFace *IExec;
extern struct DOSIFace *IDOS;
extern struct UtilityIFace *IUtility;
extern struct CommoditiesIFace *ICommodities;

extern void DoMessage(char *message, char reqtype, struct DockyData *dd);
extern void LoadImageFlag(struct DockyData *dd);
extern BOOL ChangeKeymap(struct DockyData *dd, STRPTR keymap);
extern void SignalDocky(struct DockyData *dd, uint32 flags);

BOOL ProcessMsg(struct DockyData *dd);
int32 OpenCX(CONST_STRPTR args, LONG length, struct ExecBase *sysbase);

extern BOOL KM3;

#define CXIE_HOTKEY 1L
struct MsgPort *cxBrokerMP = NULL;
CxObj *cxBroker = NULL,
      *filter = NULL, // for hotkey1
      *sender = NULL,
      *translate = NULL;
struct NewBroker cxNewbroker = { NB_VERSION,
                                 "KeymapSwitcher",
                                 LIBNAME,//VERS,
                                 NULL, // Description (set/localized later)
                                 NBU_UNIQUE|NBU_NOTIFY,
                                 COF_CONSTANT, // don't exit/end from within Exchange
                                 0, 0, 0 };
ULONG cxsigflag = 0;


BOOL OpenCX_NP(struct DockyIFace *Self, struct DockyData *dd)
{
 struct DockyBase *processDockyBase;
 // Open the library for the use of the process
 // so it doesn't get expunged while the process is starting
 if( (processDockyBase=(struct DockyBase *)IExec->OpenLibrary(LIBNAME, VERSION)) )
 {
  Self->Obtain();

  if( (dd->CxProc=IDOS->CreateNewProcTags(
                         NP_Entry, OpenCX,
                         NP_Name, LIBNAME,
                         NP_Child, TRUE,
                         NP_EntryData, Self,
                         NP_StackSize, 16384,
                         NP_ProgramDir, IDOS->Lock(dd->filepath, SHARED_LOCK), // for Exchange VERSION & WBINFO
                         //NP_NotifyOnDeathSigTask, dd->attn.amidockTask,
                         //NP_NotifyOnDeathSignalBit, dd->attn.attentionSignalBit,
                         #ifdef HDEBUG
                         NP_Output, IDOS->Open("CON:20/20/600/150/KMS_cx",MODE_OLDFILE),
                         #endif
                         NP_FinalCode, IExec->CloseLibrary,
                         NP_FinalData, processDockyBase,
                        TAG_DONE)) )
  {
#ifdef HDEBUG
IDOS->Printf("NewProcess CREATED (CX=0x%lx)\n",dd->CxProc);
#endif
   return TRUE;
  }

  Self->Release();
  IExec->CloseLibrary( (struct Library *)processDockyBase );
 }
#ifdef HDEBUG
IDOS->Printf("NewProcess FAILED (CX=0x%lx)\n",dd->CxProc);
#endif
 return FALSE;
}


int32 OpenCX(CONST_STRPTR args, LONG length, struct ExecBase *sysbase)
{
 struct ExecIFace *iexec = (struct ExecIFace *)sysbase->MainInterface;
 struct Process *me = (struct Process *)iexec->FindTask(NULL);
 struct DockyIFace *Self = (struct DockyIFace *)me->pr_EntryData;
 struct DockyData *dd = (struct DockyData *)((uint32)Self - Self->Data.NegativeSize);
 CxMsg *msg;

 if( (cxBrokerMP=IExec->AllocSysObjectTags(ASOT_PORT, ASOPORT_Name,LIBNAME"_port", TAG_DONE)) != NULL)
 {
  cxNewbroker.nb_Descr = GetString(&li, MSG_COMMODITY_DESC);
  cxNewbroker.nb_Port = cxBrokerMP;
  cxsigflag = 1L << cxBrokerMP->mp_SigBit;
  if( (cxBroker=ICommodities->CxBroker(&cxNewbroker, NULL)) )
  {
   filter = CxFilter(dd->hotkey1); // hotkey to swap keymaps
   if( ICommodities->CxObjError(filter) )
   {// Invalid hotkey -> set default hotkey to swap keymaps
    DoMessage( (STRPTR)GetString(&li, MSG_COMMODITY_HK1_INVALID), REQIMAGE_ERROR, dd );
    IUtility->Strlcpy( dd->hotkey1, "ALT ESC", sizeof(dd->hotkey1) );
    filter = CxFilter(dd->hotkey1);
   }
   ICommodities->AttachCxObj(cxBroker, filter);
   sender = CxSender(cxBrokerMP, CXIE_HOTKEY);
   ICommodities->AttachCxObj(filter, sender);
   if( (translate=CxTranslate(NULL)) )
    ICommodities->AttachCxObj(filter, translate);

   ICommodities->ActivateCxObj(cxBroker, TRUE);

   while( ProcessMsg(dd) );

   ICommodities->DeleteCxObjAll(cxBroker);
   // Empty the port of all CxMsgs
   while( (msg=(CxMsg *)IExec->GetMsg(cxBrokerMP)) )
    IExec->ReplyMsg( (struct Message *)msg );
  }

  IExec->FreeSysObject(ASOT_PORT, cxBrokerMP);
 }
#ifdef HDEBUG
IDOS->Delay(150);
#endif
 Self->Release();
 dd->CxProc = NULL;
//IExec->DebugPrintF("[%s]OpenCX cx=0x%lX\n",LIBNAME,dd->CxProc);
 return( (int32)iexec );
}


BOOL ProcessMsg(struct DockyData *dd)
{
 CxMsg *msg;
 uint32 sigrcvd, msgid, msgtype;
 BOOL CXcontinue = TRUE;

 sigrcvd = IExec->Wait(SIGBREAKF_CTRL_C|cxsigflag);

 if( (msg=(CxMsg *)IExec->GetMsg(cxBrokerMP)) )
 {
  msgid = ICommodities->CxMsgID(msg);
  msgtype = ICommodities->CxMsgType(msg);
  IExec->ReplyMsg( (struct Message *)msg );

  switch(msgtype)
  {
   case CXM_IEVENT:
#ifdef HDEBUG
IDOS->Printf("A CXM_EVENT, ");
#endif
    switch(msgid)
    {
     case CXIE_HOTKEY: // we got the message from the sender CxObject
#ifdef HDEBUG
IDOS->Printf("You hit the HOTKEY!\n");
#endif
      if( dd->km_pos==3 || (dd->km_pos==2 && !KM3) )
       dd->km_pos = 0;

      if( ChangeKeymap(dd, dd->KeymapName[dd->km_pos]) )
      {
       IUtility->Strlcpy( dd->keymapSel, dd->KeymapName[dd->km_pos], sizeof(dd->keymapSel) );
       dd->km_pos++; // points to next possible KeymapName[]
       LoadImageFlag(dd); // (re)load new image/flag after user choosed new
       SignalDocky(dd, 1); // need to 'RedrawNow'
      }
     break;
     default:
#ifdef HDEBUG
IDOS->Printf("???? msgid\n");
#endif
     break;
    }
   break;

   case CXM_COMMAND:
#ifdef HDEBUG
IDOS->Printf("CXM_COMMAND: ");
#endif
     switch(msgid)
     {
      case CXCMD_DISABLE:
#ifdef HDEBUG
IDOS->Printf("CXCMD_DISABLE\n");
#endif
       ICommodities->ActivateCxObj(cxBroker, FALSE);
      break;
      case CXCMD_ENABLE:
#ifdef HDEBUG
IDOS->Printf("CXCMD_ENABLE\n");
#endif
       ICommodities->ActivateCxObj(cxBroker, TRUE);
      break;
      case CXCMD_KILL:
#ifdef HDEBUG
IDOS->Printf("CXCMD_KILL\n");
#endif
       CXcontinue = FALSE;
      break;
      case CXCMD_UNIQUE:
#ifdef HDEBUG
IDOS->Printf("CXCMD_UNIQUE\n");
#endif
       CXcontinue = FALSE;
      break;
      default:
#ifdef HDEBUG
IDOS->Printf("???? msgid\n");
#endif
      break;
     }
   break;

   default:
#ifdef HDEBUG
IDOS->Printf("???? msgtype\n");
#endif
   break;
  }
 }

 if(sigrcvd & SIGBREAKF_CTRL_C)
 {
  CXcontinue = FALSE;
#ifdef HDEBUG
IDOS->Printf("CTRL+C signal break\n");
#endif
 }

 return(CXcontinue);
}

