//#define DDEBUG

#include "dockybase.h"

#include <proto/datatypes.h>
#include <proto/graphics.h>
#include <graphics/composite.h>

#include "KeyMapSwitcher_rev.h"

#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
//#define CATCOMP_CODE
#include "include/keymapswitcher_strings.h"
extern struct LocaleInfo li;
char textbuf[150]; // for localized messages

extern struct ExecIFace *IExec;
extern struct UtilityIFace *IUtility;
extern struct IntuitionIFace *IIntuition;
extern struct IconIFace *IIcon;
extern struct DataTypesIFace *IDataTypes;
extern struct DOSIFace *IDOS;
extern struct GraphicsIFace *IGraphics;
extern struct CommoditiesIFace *ICommodities;
extern struct WorkbenchIFace *IWorkbench;
extern struct ListBrowserIFace *IListBrowser;
//extern struct DiskfontIFace *IDiskfont;

// the class pointer
extern Class *PopupMenuClass;

extern struct TagItem dockyTags[];
extern Object *pwin, *win2;
extern struct Window *pwindow;
extern CxObj *cxBroker;

#ifdef NEWPROC
extern int32 OpenPrefsWindow(CONST_STRPTR args, LONG length, struct ExecBase *sysbase);
#else
extern int32 OpenPrefsWindow(struct DockyData *dd);
#endif
extern BOOL PrefsWindow_NP(struct DockyIFace *Self, struct DockyData *dd);
extern void OpenDockWindow(struct DockyData *dd);
extern void OpenDockMenu(struct DockyIFace *Self);
extern void SetKeyMapFlag(char *keymapname, char *keymapflag);
extern BOOL ChangeKeymap(struct DockyData *dd, STRPTR keymap);
extern void ReadInputPrefsKeymap(STRPTR keymap);
extern void DoMessage(char *message, char reqtype, struct DockyData *dd);
extern BOOL OpenCX_NP(struct DockyIFace *Self, struct DockyData *dd);

void SignalDocky(struct DockyData *dd, uint32 flags);

BOOL DockySClick, WinMode, KM3;
char keymap0Flag[1024] = "", keymap1Flag[1024] = "", keymap2Flag[1024] = "";
Object *picObject = NULL;


uint32 _docky_Obtain(struct DockyIFace *Self)
{
#ifdef DDEBUG
IExec->DebugPrintF("[%s]docky_Obtain\n",LIBNAME);
#endif
 Self->Data.RefCount++;
 return Self->Data.RefCount;
}


void LoadCharsetfonts(struct DockyData *dd)
{
 struct TextAttr *font = dd->scr->Font;

 dd->fiso[1].tta.tta_Name  = font->ta_Name;
 dd->fiso[1].tta.tta_YSize = font->ta_YSize;
 dd->fiso[1].tta.tta_Style = font->ta_Style | FSF_TAGGED;
 dd->fiso[1].tta.tta_Flags = font->ta_Flags;
 dd->fiso[1].tta.tta_Tags  = dd->fiso[1].ti;
 dd->fiso[1].ti[0].ti_Tag  = TA_CharSet;
 dd->fiso[1].ti[0].ti_Data = 5; // 5=ISO_8859-2
 dd->fiso[1].ti[1].ti_Tag  = TAG_DONE;
// dd->fiso[1].tf = IDiskfont->OpenDiskFont((struct TextAttr *)&dd->fiso[1].tta);
// if(!dd->fiso[1].tf) IDOS->Printf("Can't open font in ISO-8859-2 charset\n");

 dd->fiso[2].tta = dd->fiso[1].tta;
 dd->fiso[2].tta.tta_Tags  = dd->fiso[2].ti;
 dd->fiso[2].ti[0].ti_Tag  = TA_CharSet;
 dd->fiso[2].ti[0].ti_Data = 8; // 8=ISO_8859-5
 dd->fiso[2].ti[1].ti_Tag  = TAG_DONE;
// dd->fiso[2].tf = IDiskfont->OpenDiskFont((struct TextAttr *)&dd->fiso[2].tta);
// if(!dd->fiso[2].tf) IDOS->Printf("Can't open font in ISO-8859-5 charset\n");

 dd->fiso[0].tta = dd->fiso[1].tta;
 dd->fiso[0].tta.tta_Tags  = dd->fiso[0].ti;
 dd->fiso[0].ti[0].ti_Tag  = TA_CharSet;
 dd->fiso[0].ti[0].ti_Data = 111; // 111=ISO_8859_15
 dd->fiso[0].ti[1].ti_Tag  = TAG_DONE;
// dd->fiso[0].tf = IDiskfont->OpenDiskFont((struct TextAttr *)&dd->fiso[0].tta);
// if(!dd->fiso[0].tf) IDOS->Printf("Can't open font in ISO-8859-15 charset\n");
}


void LoadImageFlag(struct DockyData *dd)
{
 char keymapflag[64];
#ifdef DDEBUG
IDOS->Printf("keymapSel='%s' Loading flag image...\n",dd->keymapSel);
#endif
 if(IUtility->Stricmp(dd->KeymapName[2], dd->keymapSel) == 0) // KEYMAP3
  IUtility->Strlcpy( keymapflag, keymap2Flag, sizeof(keymapflag) );
 else
  if(IUtility->Stricmp(dd->KeymapName[1], dd->keymapSel) == 0) // KEYMAP2
   IUtility->Strlcpy( keymapflag, keymap1Flag, sizeof(keymapflag) );
  else
   IUtility->Strlcpy( keymapflag, keymap0Flag, sizeof(keymapflag) ); // KEYMAP1

 if(picObject)
 {
  IDataTypes->DisposeDTObject(picObject);
  picObject = NULL;
 }

 picObject = IDataTypes->NewDTObject(keymapflag, PDTA_Screen,dd->scr,
                                     PDTA_DestMode,PMODE_V43, DTA_GroupID,GID_PICTURE,
                                    TAG_DONE);
#ifdef DDEBUG
IDOS->Printf("[%s]picObject=0x%08lx - keymapflag '%s' loaded\n",LIBNAME,picObject,keymapflag);
#endif
}


void DockyRender(struct DockyData *dd)
{
 struct BitMap *tempBM = NULL;
 APTR lock, *ba;
 uint32 *bpr;

 tempBM = IGraphics->AllocBitMapTags(IMG_W, IMG_H, 32, BMATags_PixelFormat,PIXF_A8R8G8B8, TAG_END);
#ifdef DDEBUG
IDOS->Printf("tempBM=0x%08lx\n",tempBM);
#endif
 if( (lock=IGraphics->LockBitMapTags(tempBM, LBM_BytesPerRow,&bpr, LBM_BaseAddress,&ba, TAG_END)) )
 {// to pass the Alpha info from the DT object to the memory (RenderInfo) of the bitmap
  IIntuition->IDoMethod(picObject, PDTM_READPIXELARRAY, ba,
                                   PBPAFMT_ARGB, bpr,
                                   0, 0, IMG_W, IMG_H);
#ifdef DDEBUG
IDOS->Printf("[0x%08lx]Memory=0x%08lx   BytesPerRow=%ld\n",lock,ba,bpr);
#endif
  IGraphics->UnlockBitMap(lock);
 }

/*
 uint32 *bpr2 = (uint32 *)IGraphics->GetBitMapAttr(tempBM, BMA_BYTESPERROW);
 APTR *ba2 = (APTR *)IGraphics->GetBitMapAttr(tempBM, BMA_BASEADDRESS);
//#ifdef DDEBUG
IDOS->Printf("            Memory=0x%08lx   BytesPerRow=%ld\n",ba2,bpr2);
//#endif
*/

 IGraphics->CompositeTags(COMPOSITE_Src_Over_Dest, tempBM, dd->rp->BitMap,
                          COMPTAG_SrcWidth,IMG_W, COMPTAG_SrcHeight,IMG_H,
                          COMPTAG_ScaleX, COMP_FLOAT_TO_FIX(dd->scaleflag),
                          COMPTAG_ScaleY, COMP_FLOAT_TO_FIX(dd->scaleflag),
                          COMPTAG_Flags , COMPFLAG_SrcFilter,
                         TAG_DONE);

 IGraphics->FreeBitMap(tempBM);
 //tempBM = NULL;
}


uint32 _docky_Release(struct DockyIFace *Self)
{
 struct DockyData *dd = (struct DockyData *)((uint32)Self - Self->Data.NegativeSize);
#ifdef DDEBUG
IExec->DebugPrintF("[%s]docky_Release\n",LIBNAME);
#endif
 if(dd->PrefsProc)
  IExec->Signal( (struct Task *)dd->PrefsProc, SIGBREAKF_CTRL_C );

 if(dd->CxProc)
  IExec->Signal( (struct Task *)dd->CxProc, SIGBREAKF_CTRL_C );

 Self->Data.RefCount--;
 if( !Self->Data.RefCount && (Self->Data.Flags & IFLF_CLONED) )
 {
  if(dd->PrefsProc)
   IExec->Signal( (struct Task *)dd->PrefsProc, SIGBREAKF_CTRL_C );

  if(dd->CxProc)
   IExec->Signal( (struct Task *)dd->CxProc, SIGBREAKF_CTRL_C );

  IExec->DeleteInterface( (struct Interface *)Self );
  return 0;
 }

 return Self->Data.RefCount;
}


struct DockyIFace *_docky_Clone(struct DockyIFace *Self)
{
 struct DockyIFace *docky;
#ifdef DDEBUG
IExec->DebugPrintF("[%s]docky_Clone\n",LIBNAME);
#endif
 docky = (struct DockyIFace *)IExec->MakeInterface(Self->Data.LibBase, dockyTags);
 if(docky)
 {
  struct DockyData *dd = (struct DockyData *)((uint32)docky - docky->Data.NegativeSize);

  docky->Data.Flags |= IFLF_CLONED;

  IUtility->ClearMem( dd, sizeof(struct DockyData) );

  // Setting default values - START //
  DockySClick = FALSE; WinMode = FALSE; KM3 = FALSE;
  dd->PrefsProc = NULL; dd->CxProc = NULL;
  dd->scaleflag = 1.0; // docky image flag shows 1:1 (no scaling)
  dd->size.width  = IMG_W;  // width of flag images
  dd->size.height = IMG_H; // height of flag images
  dd->km_pos = 1; // points to next possible KeymapName[]
  dd->alt_menunames = FALSE;
  IUtility->Strlcpy( dd->hotkey1, "ALT ESC", sizeof(dd->hotkey1) );
  dd->dockyFlags = 0;
  //dd->pMutex = IExec->AllocSysObject(ASOT_MUTEX, NULL);
  // Setting default values - END //

  ReadInputPrefsKeymap(dd->KeymapName[0]);
 }

 return docky;
}


static void ReadDockyPrefs(struct DockyIFace *Self)
{
 struct DockyData *dd = (struct DockyData *)((uint32)Self - Self->Data.NegativeSize);
 struct DiskObject *micon;
 STRPTR ttp;
#ifdef DDEBUG
IExec->DebugPrintF("[%s]ReadDockyPrefs\n",LIBNAME);
#endif
 micon = IIcon->GetDiskObjectNew(dd->filename);
 if(micon)
 {
  ttp = IIcon->FindToolType(micon->do_ToolTypes, "KEYMAP2");
  if(ttp)
  {
   if(IUtility->Stricmp(ttp, "usa1") == 0) // usa1 = usa
    IUtility->Strlcpy( dd->KeymapName[1], "usa", sizeof(dd->KeymapName[1]) );
   else
    IUtility->Strlcpy( dd->KeymapName[1], ttp, sizeof(dd->KeymapName[1]) );
  }
  else
   DoMessage( (STRPTR)GetString(&li, MSG_ERROR_NOKEYMAP2), REQIMAGE_WARNING, dd );

  ttp = IIcon->FindToolType(micon->do_ToolTypes, "DOCKY_1CLICK");
  if(ttp)
   DockySClick = ( IIcon->MatchToolValue(ttp, "YES") | IIcon->MatchToolValue(ttp, "ON") );

  ttp = IIcon->FindToolType(micon->do_ToolTypes, "MODE");
  if(ttp)
   WinMode = IIcon->MatchToolValue(ttp, "WIN");

  ttp = IIcon->FindToolType(micon->do_ToolTypes, "KEYMAP3");
  if(ttp)
  {
   KM3 = TRUE;
   IUtility->Strlcpy( dd->KeymapName[2], ttp, sizeof(dd->KeymapName[2]) );
  }

  ttp = IIcon->FindToolType(micon->do_ToolTypes, "HOTKEY");
  if(ttp)
   IUtility->Strlcpy( dd->hotkey1, ttp, sizeof(dd->hotkey1) );

  if( IIcon->FindToolType(micon->do_ToolTypes, "ALT_MENUNAMES") )
   dd->alt_menunames = TRUE;

  if( (ttp=IIcon->FindToolType(micon->do_ToolTypes, "DOCKY_HEIGHT")) )
  {
   int32 new_h;

   IDOS->StrToLong(ttp, &new_h);

   if(new_h > 8) // DOCKY_HEIGHT > 8 pixels to apply scaling
    dd->scaleflag = new_h*1.0 / IMG_H;
//IDOS->Printf("Scale (DOCKY_HEIGHT[%ldpx] vs IMG_H[%ldpx]) %ld%%\n",new_h,IMG_H,(int32)(dd->scaleflag*100) );
   dd->size.width  = IMG_W * dd->scaleflag; // rescaled width of flag images
   dd->size.height = IMG_H * dd->scaleflag; // rescaled height of flag images
  }
 }
 IIcon->FreeDiskObject(micon);

 IUtility->Strlcpy( dd->keymapSel, dd->KeymapName[0], sizeof(dd->keymapSel) ); // KEYMAP selected as default
 // language 1: input.prefs; KeymapName[0] & keymap0Flag
 SetKeyMapFlag(dd->KeymapName[0], keymap0Flag);
#ifdef DDEBUG
IDOS->Printf("KEYMAP1='%s' FLAG1='%s' (input.prefs)\n",dd->KeymapName[0],keymap0Flag);
#endif
 // language 2: tooltype (KEYMAP2); KeymapName[1] & keymap1Flag
 SetKeyMapFlag(dd->KeymapName[1], keymap1Flag);
#ifdef DDEBUG
IDOS->Printf("KEYMAP2='%s' FLAG2='%s' (tooltype)\n",dd->KeymapName[1],keymap1Flag);
#endif
 // language 3: tooltype (KEYMAP3); KeymapName[2] & keymap2Flag
 if(KM3)
 {
  SetKeyMapFlag(dd->KeymapName[2], keymap2Flag);
#ifdef DDEBUG
IDOS->Printf("KEYMAP3='%s' FLAG3='%s' (tooltype)\n",dd->KeymapName[2],keymap2Flag);
#endif
 }

}


void _docky_Expunge(struct DockyIFace *Self)
{
 struct DockyData *dd = (struct DockyData *)((uint32)Self - Self->Data.NegativeSize);
#ifdef DDEBUG
IExec->DebugPrintF("[%s]docky_Expunge\n",LIBNAME);
#endif
 if(!Self->Data.RefCount)
 {
#ifdef DDEBUG
IDOS->Printf("Reseting to Input Prefs. keymap ('%s')\n",dd->KeymapName[0]);
#endif
  ChangeKeymap(dd, dd->KeymapName[0]); // reset to use Input Prefs. keymap
#ifdef DDEBUG
IDOS->Printf("Disposing picObject=0x%08lx\n",picObject);
#endif
  if(picObject)
   IDataTypes->DisposeDTObject(picObject);
  picObject = NULL;

//while(dd->PrefsProc || dd->CxProc);

  //IExec->FreeSysObject(ASOT_MUTEX, dd->pMutex);
  //dd->pMutex = NULL;
 }
}


BOOL _docky_DockyGet(struct DockyIFace *Self, uint32 msgType, uint32 *msgData)
{
 struct DockyData *dd = (struct DockyData *)((uint32)Self - Self->Data.NegativeSize);
 BOOL res = TRUE;
#ifdef DDEBUG
IExec->DebugPrintF("[%s]DockyGet\n",LIBNAME);
//IDOS->Printf("  %02ld\n",msgType);
#endif
 switch(msgType)
 {
  case DOCKYGET_Version:
   *msgData = DOCKYVERSION;
  break;

  case DOCKYGET_GetSize:
   *((struct DockySize *)msgData) = dd->size;
  break;

  case DOCKYGET_FrameDelay:
   *msgData = -1;
  break;

  case DOCKYGET_RenderMode:
   *msgData = DOCKYRENDERMODE_RPPA;
  break;

//  case DOCKYGET_AllowsIconChange:
//   *msgData = FALSE;
//  break;

  case DOCKYGET_Notifications:
   *msgData = DOCKYNOTIFY_MOUSEBUTTONS;
  break;

  case DOCKYGET_ContextMenu:
  {
   Object *contextMenu = (Object *)msgData;
#ifdef DDEBUG
IDOS->Printf("[DOCKYGET_ContextMenu]\n");
#endif
   Object *itemSM = IIntuition->NewObject(NULL, "popupmenuitem.class",
                                 PMIA_Title,   GetString(&li, MSG_DOCKY_MODE),
                                 PMIA_SubMenu, IIntuition->NewObject(PopupMenuClass, NULL,
                                  PMA_AddItem, IIntuition->NewObject(NULL, "popupmenuitem.class",
                                   PMIA_Title,   GetString(&li, MSG_DOCKY_POPUP),
                                   PMIA_ID,      MENU_OPT1,
                                   PMIA_CheckIt, TRUE,
                                   PMIA_Checked, !WinMode,
                                  TAG_END),
                                  PMA_AddItem, IIntuition->NewObject(NULL, "popupmenuitem.class",
                                   PMIA_Title,   GetString(&li, MSG_DOCKY_WINDOW),
                                   PMIA_ID,      MENU_OPT2,
                                   PMIA_CheckIt, TRUE,
                                   PMIA_Checked, WinMode,
                                  TAG_END),
                                 TAG_END),
                                TAG_END);

   Object *item = IIntuition->NewObject(NULL, "popupmenuitem.class",
                               PMIA_Title,    GetString(&li, MSG_DOCKY_SETTINGS),
                               PMIA_ID,       MENU_PREFS,
                               PMIA_Disabled, FALSE,
                              TAG_END);

//   Object *itemBar = IIntuition->NewObject(NULL, "popupmenuitem.class", PMIA_Title,~0, TAG_END);

   IIntuition->IDoMethod(contextMenu, OM_ADDMEMBER, itemSM); // SubMenu Mode/Window
//   IIntuition->IDoMethod(contextMenu, OM_ADDMEMBER, itemBar);
   if(WinMode)
    IIntuition->IDoMethod(contextMenu, OM_ADDMEMBER, item); // Settings...

   if(pwin)
   {
    IIntuition->SetAttrs(itemSM, PMIA_Disabled,TRUE, TAG_DONE);
    IIntuition->SetAttrs(item, PMIA_Disabled,TRUE, TAG_DONE);
   }
  }
  break;

//  case DOCKYGET_AllowsRename:
//   *msgData = FALSE;
//  break;

  case DOCKYGET_InvisibleProcess:
   *msgData = FALSE;
  break;

  case DOCKYGET_ShowsName:
   *msgData = TRUE;
  break;

//  case DOCKYGET_Name:
//   *msgData = (uint32)IDOS->FilePart(dd->filename);
//  break;

  case DOCKYGET_NeedsAttention:
#ifdef DDEBUG
IDOS->Printf("[DOCKYGET_NeedsAttention]\n");
#endif
   if(dd->dockyFlags)
    *msgData = TRUE;
   else
    *msgData = FALSE;
  break;

  default:
#ifdef DDEBUG
IDOS->Printf("[DOCKYGET_????] %02ld\n",msgType);
#endif
   res = FALSE;
  break;
 }

 return res;
}


BOOL _docky_DockySet(struct DockyIFace *Self, uint32 msgType, uint32 msgData)
{
 struct DockyData *dd = (struct DockyData *)((uint32)Self - Self->Data.NegativeSize);
 BOOL res = TRUE;
#ifdef DDEBUG
IExec->DebugPrintF("[%s]DockySet\n",LIBNAME);
//IDOS->Printf("%04ld\n",msgType);
#endif
 switch(msgType)
 {
  case DOCKYSET_FileName:
   IUtility->Strlcpy( dd->filename, (STRPTR)msgData, sizeof(dd->filename) );
   IUtility->Strlcpy(dd->filepath, dd->filename, IUtility->Strlen(dd->filename)-19); // 19 = Strlen("keymapswitcher.docky") - 1
#ifdef DDEBUG
IDOS->Printf("filename:'%s'\n",dd->filename);
IDOS->Printf("filepath:'%s'\n",dd->filepath);
#endif
   ReadDockyPrefs(Self);
   if(dd->CxProc)
    IExec->Signal( (struct Task *)dd->CxProc, SIGBREAKF_CTRL_C );

   OpenCX_NP(Self, dd); // create commodity process (aka enable hotkey usage)
  break;

  case DOCKYSET_Screen:
   dd->scr = (struct Screen *)msgData;
   LoadCharsetfonts(dd);
   LoadImageFlag(dd);
#ifdef DDEBUG
IDOS->Printf("[DOCKYSET_Screen]\n");
#endif
 break;

  case DOCKYSET_Window:
   dd->win = (struct Window *)msgData;
#ifdef DDEBUG
IDOS->Printf("[DOCKYSET_Window]\n");
#endif
  break;

  case DOCKYSET_RenderDestination:
  {
   struct DockyRenderDestination *drd = (struct DockyRenderDestination *)msgData;
#ifdef DDEBUG
IDOS->Printf("[DOCKYSET_RenderDestination] ");
#endif
   dd->size = drd->renderSize;

   switch(drd->renderMode)
   {
    case DOCKYRENDERMODE_RPPA:
#ifdef DDEBUG
IDOS->Printf("DOCKYRENDERMODE_RPPA\n");
#endif
     dd->rp = drd->render.RP;
    break;
    default:
#ifdef DDEBUG
IDOS->Printf("default\n");
#endif
     dd->rp = NULL;
     res = FALSE;
    break;
   }

  }
  break;

  case DOCKYSET_RedrawNow:
#ifdef DDEBUG
IDOS->Printf("[DOCKYSET_RedrawNow]\n");
#endif
//   LoadImageFlag(dd); <- DON'T! Freezes system if drag an icon over docky
   dd->dockyFlags = 0;
   DockyRender(dd);
  break;

  case DOCKYSET_DockTypeChange:
  {
   struct DockyDockType *ddt = (struct DockyDockType *)msgData;
#ifdef DDEBUG
IDOS->Printf("[%s]DOCKYSET_DockTypeChange\n",LIBNAME);
#endif
   if(dd->self.dockNr==ddt->dockNr && ddt->dockType!=AMIDOCK_DockType_Icons)
    res = FALSE;
  }
  break;

  case DOCKYSET_SelectChange:
#ifdef DDEBUG
IDOS->Printf("[DOCKYSET_SelectChange] 0x%08lx (0x%08lx)\n",win2,msgData);
#endif
  break;

  case DOCKYSET_SingleClick:
#ifdef DDEBUG
IDOS->Printf("[DOCKYSET_SingleClick] 0x%08lx (0x%08lx)\n",win2,msgData);
#endif
   if(!DockySClick || win2) // V52.2: '..|| win2)' ChooserWindow mode just closed,
   {                        // give "option" to use RMB with DOCKY_1CLICK=YES
//    win2 = NULL;
    break;
   }
   msgData = 0; // V52.2: see 6 lines below 'if(msgData..'
  case DOCKYSET_DoubleClick:
  {
#ifdef DDEBUG
IDOS->Printf("[DOCKYSET_DoubleClick] 0x%08lx (0x%08lx)\n",win2,msgData);
#endif
   if(msgData && DockySClick) // v52.2: msgData=0 -> from _SingleClick and DOCKY_1CLICK=YES
    break;

   if(pwin) // PrefsWindow already open -> to front & activate
   {
    IIntuition->WindowToFront(pwindow);
    IIntuition->ActivateWindow(pwindow);
    break;
   }

   ICommodities->ActivateCxObj(cxBroker, FALSE); // pause CX

   if(WinMode)
    OpenDockWindow(dd); // chooser is a WINDOW
   else
    OpenDockMenu(Self); // chooser is a MENU

   ICommodities->ActivateCxObj(cxBroker, TRUE); // restart CX

   if( ChangeKeymap(dd, dd->KeymapName[dd->km_pos-1]) )
   {
    IUtility->Strlcpy( dd->keymapSel, dd->KeymapName[dd->km_pos-1], sizeof(dd->keymapSel) );
    LoadImageFlag(dd); // (re)load new image/flag after user choosed new
    SignalDocky(dd, 1); // need to 'RedrawNow'
   }
  break;
  }

  case DOCKYSET_ContextMenuResult:
  {
   Object *o = (Object *)msgData;
#ifdef DDEBUG
IDOS->Printf("[DOCKYSET_ContextMenuResult]\n");
#endif
   if(o)
   {
    IIntuition->GetAttr(PMIA_ID, o, (uint32 *)&dd->km_item);
#ifdef DDEBUG
IDOS->Printf("KeyMapSwitcher: item=%ld\n",dd->km_item);
#endif
    switch(dd->km_item)
    {
     case MENU_OPT1:
      WinMode = FALSE; // PopUp mode
     break;
     case MENU_OPT2:
      WinMode = TRUE; // Window mode
     break;
     case MENU_PREFS:
     {
      #ifdef NEWPROC
      PrefsWindow_NP(Self, dd);
      #else
      OpenPrefsWindow(dd);
      #endif
     }
     break;
    } // END switch(dd-km->item)

   } // END if(o)..
  }
  break;

/*  case DOCKYSET_MouseIsOverDocky: // V52.2
IDOS->Printf("[%s]MouseIsOverDocky 0x%08lx\n",LIBNAME,msgData);
  break;*/

  case DOCKYSET_DockyAttention:
  {
   dd->attn = *(struct DockyAttention *)msgData;
#ifdef DDEBUG
IDOS->Printf("[DOCKYSET_DockyAttention] 0x%08lx %d\n",dd->attn.amidockTask,dd->attn.attentionSignalBit);
#endif
  }
  break;

  default:
#ifdef DDEBUG
IDOS->Printf("[DOCKYSET_????] %04ld\n",msgType);
#endif
   res = FALSE;
  break;
 }

 return res;
}


BOOL _docky_DockyProcess(struct DockyIFace *Self, uint32 turnCount, uint32 *msgType,
                         uint32 *msgData, BOOL *anotherTurn)
{
 //struct DockyData *dd = (struct DockyData *)((uint32)Self - Self->Data.NegativeSize);
#ifdef DDEBUG
IExec->DebugPrintF("[%s]docky_DockyProcess\n",LIBNAME);
IDOS->Printf("DockyProcess: turnCount=%ld msgType=%ld msgData=%ld anotherTurn=%ld\n",\
              turnCount,*msgType,*msgData,*anotherTurn);
#endif

 return TRUE;
}


void SignalDocky(struct DockyData *dd, uint32 flags)
{
#ifdef DDEBUG
IDOS->Printf("[SignalDocky]\n");
#endif
 dd->dockyFlags = flags;
 IExec->Signal( (struct Task *)dd->attn.amidockTask, 1L << dd->attn.attentionSignalBit);
}

