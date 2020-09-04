//#define _DEBUG_

#include "dockybase.h"

#include <proto/keymap.h>
#include <proto/iffparse.h>
#include <workbench/icon.h>

#include <stdlib.h> // used for malloc() & free()

#include "prefs.h"
#include "KeyMapSwitcher_rev.h"

// the class pointer
extern Class *ListBrowserClass, *StringClass, *ButtonClass, *LayoutClass,
             *LabelClass, *WindowClass;

#define CXIE_HOTKEY 1
extern struct MsgPort *cxBrokerMP;
extern CxObj *cxBroker, *filter, *sender, *translate;

#define TOTALCOLUMNS 2
Object *pwin, *imageP;
struct Window *pwindow;
struct List listbrowser_list;
extern char keymap1Flag[1024];

#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
//#define CATCOMP_CODE
#include "include/keymapswitcher_strings.h"
extern struct LocaleInfo li;

void PrefsWindow(struct DockyData *);
int32 beginCommand(char *);
void SaveToolType(struct DockyData *, STRPTR, STRPTR);


BOOL PrefsWindow_NP(struct DockyIFace *Self, struct DockyData *dd)
{
 if( (dd->PrefsProc = IDOS->CreateNewProcTags(
                             NP_Entry, OpenPrefsWindow,
                             NP_Name, LIBNAME"_prefs",
                             NP_Child, TRUE,
                             NP_EntryData, Self,
                             //NP_NotifyOnDeathSigTask, dd->attn.amidockTask,
                             //NP_NotifyOnDeathSignalBit, dd->attn.attentionSignalBit,
                             #ifdef _DEBUG_
                             NP_Output, IDOS->Open("CON:20/20/600/150/KMS_prefs",MODE_OLDFILE),
                             #endif
//                             NP_FinalCode, IExec->CloseLibrary,
//                             NP_FinalData, processDockyBase,
                            TAG_DONE)) )
 {
#ifdef _DEBUG_
IDOS->Printf("NewProcess CREATED (PREFS=0x%lx)\n",dd->PrefsProc);
#endif
  return TRUE;
 }
#ifdef _DEBUG_
IDOS->Printf("NewProcess FAILED (PREFS=0x%lx)\n",dd->PrefsProc);
#endif
 return FALSE;
}


#ifdef NEWPROC
int32 OpenPrefsWindow(CONST_STRPTR args, LONG length, struct ExecBase *sysbase)
#else
int32 OpenPrefsWindow(struct DockyData *dd)
#endif
{
 #ifdef NEWPROC
 struct ExecIFace *iexec = (struct ExecIFace *)sysbase->MainInterface;
 struct Process *me = (struct Process *)iexec->FindTask(NULL);
 struct DockyIFace *Self = (struct DockyIFace *)me->pr_EntryData;
 struct DockyData *dd = (struct DockyData *)((uint32)Self - Self->Data.NegativeSize);
 #endif
 int32 success = FALSE;
 APTR KM_handle;
 STRPTR KM_desc2;//, KM_desc1;
 ULONG KM_cs = 0;
 struct Node *n = NULL;

 imageP = IIntuition->NewObject(LabelClass, NULL, 
                                IA_Font,&dd->fiso[0].tta, 
                                LABEL_DisposeImage,TRUE, LABEL_Underscore,0, 
                                LABEL_Text,"American", TAG_END);

 IExec->NewList(&listbrowser_list);

 n = IListBrowser->AllocListBrowserNode(TOTALCOLUMNS,
                                        LBNA_Column,0, LBNCA_Text, "usa",
                                        LBNA_Column,1, LBNCA_Image, imageP,
                                        //LBNA_Column,2, LBNCA_Text, "American",
                                        //LBNA_Column,2, LBNCA_CopyInteger, TRUE,
                                        //               LBNCA_Integer, &KM_cs,
                                       TAG_DONE);
 IExec->AddTail(&listbrowser_list, n);

 APTR context = IDOS->ObtainDirContextTags(EX_StringNameInput, "DEVS:Keymaps",
                                           EX_DataFields, (EXF_NAME|EXF_TYPE),
                                          TAG_END);
 if(context)
 {
  struct ExamineData *dat;

  while( (dat=IDOS->ExamineDir(context)) )
  {
   if( EXD_IS_FILE(dat) )
   {
    KM_handle = IKeymap->OpenKeyMapHandle(dat->Name, NULL);
    IKeymap->ObtainKeyMapInfo(KM_handle,
                              //KEYMAPINFO_INFOTEXT_ENGLISH, &KM_desc1,
                              KEYMAPINFO_INFOTEXT_CHARSET, &KM_cs,
                              KEYMAPINFO_INFOTEXT_LOCAL,   &KM_desc2,
                              //KEYMAPINFO_CLASSIC_ONLY, &KM_classic,
                             TAG_END);

//    IDOS->Printf("KEYMAP='%s'",dat->Name);
//    if(KM_cs == 8) IDOS->Printf(" (Special charset)");
//    if(KM_classic) IDOS->Printf(" (Only Amiga Classic)");
//    IDOS->Printf("\n       %s / %s / %ld\n",KM_desc1,KM_desc2,KM_cs);
//    IDOS->Printf(" / %s / %ld\n",KM_desc2,KM_cs);

   if(KM_cs == 8) // ISO_8859-5
   {
    imageP = IIntuition->NewObject(LabelClass, NULL, 
                                   IA_Font,&dd->fiso[2].tta, 
                                   LABEL_DisposeImage,TRUE, LABEL_Underscore,0, 
                                   LABEL_Text,KM_desc2, TAG_END);

   }
   else
   {
    if(KM_cs == 5) // ISO_8859-2
    {
     imageP = IIntuition->NewObject(LabelClass, NULL, 
                                    IA_Font,&dd->fiso[1].tta, 
                                    LABEL_DisposeImage,TRUE, LABEL_Underscore,0, 
                                    LABEL_Text,KM_desc2, TAG_END);

    }
    else // ISO_8859-15
    {
     imageP = IIntuition->NewObject(LabelClass, NULL, 
                                    IA_Font,&dd->fiso[0].tta, 
                                    LABEL_DisposeImage,TRUE, LABEL_Underscore,0, 
                                    LABEL_Text,KM_desc2, TAG_END);
    }
   }

    n = IListBrowser->AllocListBrowserNode(TOTALCOLUMNS,
                                        LBNA_Column,0, LBNCA_CopyText,TRUE,
                                                       LBNCA_Text,dat->Name,
                                        LBNA_Column,1, LBNCA_Image,imageP,
                                        //LBNA_Column,2, LBNCA_CopyText, TRUE,
                                        //               LBNCA_Text, KM_desc1,
                                        //LBNA_Column,2, LBNCA_CopyInteger, TRUE,
                                        //               LBNCA_Integer, &KM_cs,
                                       TAG_DONE);

    IExec->AddTail(&listbrowser_list, n);

    IKeymap->ReleaseKeyMapInfo(KM_handle,
                               //KEYMAPINFO_INFOTEXT_ENGLISH, &KM_desc1,
                               KEYMAPINFO_INFOTEXT_LOCAL,   &KM_desc2,
                              TAG_END);

    IKeymap->CloseKeyMapHandle(KM_handle);
   }

  }
  if( ERROR_NO_MORE_ENTRIES == IDOS->IoErr() )
   success = RETURN_OK;
  else
   IDOS->PrintFault(IDOS->IoErr(), NULL);

 }
 else
  IDOS->PrintFault(IDOS->IoErr(), NULL);

 IDOS->ReleaseDirContext(context);

 ICommodities->ActivateCxObj(cxBroker, FALSE); // pause CX

 PrefsWindow(dd);

 ICommodities->ActivateCxObj(cxBroker, TRUE); // restart CX

 IListBrowser->FreeListBrowserList(&listbrowser_list);
//IDOS->Delay(100);
 #ifdef NEWPROC
 dd->PrefsProc = NULL;
//IExec->DebugPrintF("[%s]OpenPrefsWindow prefs=0x%lX\n",LIBNAME,dd->PrefsProc);
 return( (int32)iexec );
 #else
 return(success);
 #endif
}


void PrefsWindow(struct DockyData *dd)
{
 struct ColumnInfo *columninfo;
 struct Node *fnode = NULL;
 APTR KM_handle1;
 STRPTR LBkeymapname, keymapdesc;
 struct DiskObject *iconify = NULL;
 struct MsgPort *wAppPort = NULL;

 // reset icon X/Y positions so it iconifies properly on Workbench
 iconify = IIcon->GetIconTags(dd->filename, ICONGETA_FailIfUnavailable,FALSE, TAG_END);
 iconify->do_CurrentX = NO_ICON_POSITION;
 iconify->do_CurrentY = NO_ICON_POSITION;

 wAppPort = IExec->AllocSysObjectTags(ASOT_PORT, TAG_DONE);

 // get keymap1 (input prefs.) description
 KM_handle1 = IKeymap->OpenKeyMapHandle(dd->KeymapName[0], NULL);
 IKeymap->ObtainKeyMapInfo(KM_handle1, KEYMAPINFO_INFOTEXT_ENGLISH,&keymapdesc, TAG_END);
 IKeymap->CloseKeyMapHandle(KM_handle1);

 columninfo = IListBrowser->AllocLBColumnInfo(TOTALCOLUMNS,
                             LBCIA_Column,0, LBCIA_Title,GetString(&li, MSG_PREFS_TITLE_KEYMAP),
                                             LBCIA_AutoSort,TRUE, LBCIA_Sortable,TRUE,
                             LBCIA_Column,1, LBCIA_Title,GetString(&li, MSG_PREFS_TITLE_DESC),
                             //LBCIA_Column,2, LBCIA_Title," English",
                             //                LBCIA_AutoSort,TRUE, LBCIA_Sortable,TRUE,
                            TAG_DONE);

 pwin = IIntuition->NewObject(WindowClass, NULL,
         WA_ScreenTitle,  VERS" "DATE,
         WA_Title,        GetString(&li, MSG_PREFS_WINDOW),
         WA_DragBar,      TRUE,
         WA_CloseGadget,  TRUE,
         WA_SizeGadget,   TRUE,
         WA_DepthGadget,  TRUE,
         WA_Activate,     TRUE,
         WA_InnerWidth,   360,
         WA_InnerHeight,  300,
         WINDOW_Position, WPOS_CENTERSCREEN,
         WINDOW_IconifyGadget, TRUE,     // iconify support
         WINDOW_AppPort,       wAppPort, // iconify support
         WINDOW_Icon,          iconify,  // iconify support
         WINDOW_Layout, IIntuition->NewObject(LayoutClass, NULL,
          LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
          LAYOUT_SpaceOuter,  TRUE,

          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
           LAYOUT_Orientation,    LAYOUT_ORIENT_VERT,
           //LAYOUT_HorizAlignment, LALIGN_CENTER,
           LAYOUT_BevelStyle,     BVS_GROUP,
           LAYOUT_Label,          GetString(&li, MSG_PREFS_GROUP1), // "Input Preferences Keymap"
           LAYOUT_LabelColumn, LJ_RIGHT,
           LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
            LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
            LAYOUT_SpaceOuter,  TRUE,
            LAYOUT_AddChild, IIntuition->NewObject(StringClass, NULL,
             GA_ReadOnly,     TRUE,
             STRINGA_TextVal, dd->KeymapName[0],
            TAG_DONE),
            CHILD_WeightedWidth, 40,
            LAYOUT_AddChild, IIntuition->NewObject(StringClass, NULL,
             GA_ReadOnly,     TRUE,
             STRINGA_TextVal, keymapdesc,
            TAG_DONE),
            CHILD_WeightedWidth, 60,
           TAG_DONE),
//LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
//LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
           LAYOUT_AddChild, OBJ(OBJ_INPUTPREFS) = IIntuition->NewObject(ButtonClass, NULL,
            GA_ID,        OBJ_INPUTPREFS,
            GA_RelVerify, TRUE,
            GA_Text,      GetString(&li, MSG_PREFS_INPUTPREFS),
           TAG_DONE),
           CHILD_WeightedWidth, 0,
           CHILD_Label, IIntuition->NewObject(LabelClass, NULL,
            LABEL_Underscore, 0, // ~0 screws russian catalog string
            LABEL_SoftStyle,  FSF_ITALIC,
            LABEL_Text,       GetString(&li, MSG_PREFS_NOTE),
            LABEL_SoftStyle,  FS_NORMAL,
            LABEL_Text,       " ",
           TAG_DONE),
/*           LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL,
            GA_Text,       GetString(&li, MSG_PREFS_NOTE),
            GA_ReadOnly,   TRUE,
            GA_Underscore, 0,
            BUTTON_BevelStyle,    BVS_NONE,
            BUTTON_Transparent,   TRUE,
            BUTTON_SoftStyle,     FSF_ITALIC,
            BUTTON_Justification, BCJ_LEFT,
           TAG_DONE),*/
//TAG_DONE),
          TAG_DONE),
          CHILD_WeightedHeight, 0,

          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
           LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
           LAYOUT_SpaceOuter,  TRUE,
           LAYOUT_BevelStyle,  BVS_GROUP,
           LAYOUT_Label,       GetString(&li, MSG_PREFS_GROUP2), // "Keymap2 (tooltype)"
           LAYOUT_AddChild, OBJ(OBJ_LISTBROWSER) = IIntuition->NewObject(ListBrowserClass, NULL,
            GA_ID,                    OBJ_LISTBROWSER,
            GA_RelVerify,             TRUE,
            LISTBROWSER_SortColumn,   0,
            LISTBROWSER_AutoFit,      TRUE,
            LISTBROWSER_Labels,       &listbrowser_list,
            LISTBROWSER_ColumnInfo,   columninfo,
            LISTBROWSER_ColumnTitles, TRUE,
            LISTBROWSER_ShowSelected, TRUE,
            //LISTBROWSER_Selected,     -1,
            LISTBROWSER_MinVisible,   10,
            LISTBROWSER_Striping,     LBS_ROWS,
           TAG_DONE),
           LAYOUT_AddChild, OBJ(OBJ_USEKM2) = IIntuition->NewObject(StringClass, NULL,
             GA_ReadOnly,     TRUE,
             STRINGA_TextVal, dd->KeymapName[1],
           TAG_DONE),
           CHILD_Label, IIntuition->NewObject(LabelClass, NULL,
            LABEL_Text, GetString(&li, MSG_KM2_USING), // "KEYMAP2=",
           TAG_DONE),
          TAG_DONE),

          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
           LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
           LAYOUT_SpaceOuter,  TRUE,
           LAYOUT_BevelStyle,  BVS_GROUP,
           LAYOUT_Label,       GetString(&li, MSG_PREFS_GROUP3), // "Swap Keymaps"
           LAYOUT_AddChild, OBJ(OBJ_HK1) = IIntuition->NewObject(StringClass, NULL,
            //GA_ReadOnly,     TRUE,
            STRINGA_TextVal,  dd->hotkey1,
            STRINGA_HookType, SHK_HOTKEY,
           TAG_DONE),
           CHILD_Label, IIntuition->NewObject(LabelClass, NULL,
            LABEL_Text, GetString(&li, MSG_COMMODITY_HK1),
           TAG_DONE),
          TAG_DONE),
          CHILD_WeightedHeight, 0,

          LAYOUT_AddChild, IIntuition->NewObject(LayoutClass, NULL,
           LAYOUT_Orientation, LAYOUT_ORIENT_HORIZ,
           LAYOUT_SpaceOuter,  TRUE,
           LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL,
            GA_Text,      GetString(&li, MSG_PREFS_BTN_SAVE),
            GA_ID,        OBJ_SAVE,
            GA_RelVerify, TRUE,
           TAG_DONE),
           LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL,
            GA_Text,      GetString(&li, MSG_PREFS_BTN_USE),
            GA_ID,        OBJ_USE,
            GA_RelVerify, TRUE,
           TAG_DONE),
           LAYOUT_AddChild, IIntuition->NewObject(ButtonClass, NULL,
            GA_Text,      GetString(&li, MSG_PREFS_BTN_CANCEL),
            GA_ID,        OBJ_QUIT,
            GA_RelVerify, TRUE,
           TAG_DONE),
          TAG_DONE),
          CHILD_WeightedHeight, 0,

         TAG_DONE),
        TAG_END);

 // mark/select KEYMAP2 in listbrowser
 fnode = IExec->GetHead(&listbrowser_list);
 while(fnode != NULL)
 {
  IListBrowser->GetListBrowserNodeAttrs(fnode, LBNA_Column,0, LBNCA_Text,&LBkeymapname, TAG_DONE);
  if(IUtility->Stricmp(LBkeymapname, dd->KeymapName[1]) == 0)
  {
//IDOS->Printf("Found in listbrowser:'%s' (node:%ld)\n",LBkeymapname,(ULONG *)fnode);
   IIntuition->SetAttrs(OBJ(OBJ_LISTBROWSER), LISTBROWSER_SelectedNode,fnode,
                        LISTBROWSER_MakeNodeVisible,fnode, TAG_DONE);
   fnode = NULL;
  }

  fnode = IExec->GetSucc(fnode);
 }

 if( (pwindow=(struct Window *)IIntuition->IDoMethod(pwin, WM_OPEN, NULL)) )
 {
  extern char keymap2Flag[1024];
  uint32 done = TRUE, wsigmask = 0;//, res_node;
  //char *res_string = NULL;
  char keymap2Sel[128]; // to check if keymap2 changed

  IUtility->Strlcpy( keymap2Sel, dd->KeymapName[1], sizeof(keymap2Sel) );

  IIntuition->GetAttr(WINDOW_SigMask, pwin, &wsigmask);

  while(done)
  {
   uint32 siggot = IExec->Wait(wsigmask|SIGBREAKF_CTRL_C);

   if(siggot & SIGBREAKF_CTRL_C)
    done = FALSE;

   if(siggot & wsigmask)
   {
    uint16 code = 0;
    uint32 result = WMHI_LASTMSG, res_value, res_node;
    char *res_string = NULL;

    if( (result=IIntuition->IDoMethod(pwin, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG )
    {
     switch(result & WMHI_CLASSMASK)
     {
      case WMHI_CLOSEWINDOW:
       done = FALSE;
      break;
      case WMHI_ICONIFY:
//IDOS->Printf("ICONIFIED\n");
       IIntuition->IDoMethod(pwin, WM_ICONIFY, NULL);
       pwindow = NULL;
      break;
      case WMHI_UNICONIFY:
//IDOS->Printf("UNICONIFIED\n");
       pwindow = (struct Window *)IIntuition->IDoMethod(pwin, WM_OPEN, NULL);
      break;
      case WMHI_RAWKEY: // move up/down in the listbrowser
//IDOS->Printf("[WMHI_RAWKEY] 0x%lx",code);
       IIntuition->GetAttrs(OBJ(OBJ_LISTBROWSER), LISTBROWSER_Selected,&res_value,
                            LISTBROWSER_TotalNodes,&res_node, TAG_DONE);
//IDOS->Printf(" (%ld/%ld)\n",res_value,res_node);
       if(code==CURSORUP && res_value!=0)
       {
//IDOS->Printf(" Up: %ld/%lu\n",res_value,res_node);
        IIntuition->RefreshSetGadgetAttrs(GAD(OBJ_LISTBROWSER), pwindow, NULL,
                                          LISTBROWSER_Selected,res_value-1,
                                          LISTBROWSER_MakeVisible,res_value-1, TAG_DONE);
        break;
       }
       if(code==CURSORDOWN && res_value!=res_node-1)
       {
//IDOS->Printf(" Down: %ld/%lu\n",res_value,res_node);
        IIntuition->RefreshSetGadgetAttrs(GAD(OBJ_LISTBROWSER), pwindow, NULL,
                                          LISTBROWSER_Selected,res_value+1,
                                          LISTBROWSER_MakeVisible,res_value+1, TAG_DONE);
        break;
       }
       if(code==RAWKEY_RETURN || code==RAWKEY_ENTER)
       {
        IIntuition->GetAttr(LISTBROWSER_SelectedNode, OBJ(OBJ_LISTBROWSER), (uint32 *)&res_node);
        IListBrowser->GetListBrowserNodeAttrs( (struct Node *)res_node, LBNA_Column,0, LBNCA_Text,&res_string, TAG_DONE );
        IIntuition->RefreshSetGadgetAttrs(GAD(OBJ_USEKM2), pwindow, NULL,
                                          STRINGA_TextVal,res_string, TAG_DONE);
        break;
       }
      break;
      case WMHI_GADGETUP:
       switch(result & WMHI_GADGETMASK)
       {
        case OBJ_USE:
        case OBJ_SAVE:
        {
         BOOL badfilter = FALSE;

         ICommodities->DeleteCxObj(filter); // remove previous hotkey
         IIntuition->GetAttrs(OBJ(OBJ_HK1), STRINGA_TextVal,&res_string, TAG_END);
         filter = CxFilter(res_string);
         if( ICommodities->CxObjError(filter) )
         {// NOT valid hotkey -> Show error and restore hotkey
          DoMessage( (STRPTR)GetString(&li, MSG_COMMODITY_HK1_INVALID), REQIMAGE_ERROR, dd );
          IIntuition->RefreshSetGadgetAttrs(GAD(OBJ_HK1), pwindow, NULL, STRINGA_TextVal,dd->hotkey1, TAG_DONE);
          filter = CxFilter(dd->hotkey1); // restore previous hotkey
          badfilter = TRUE;
         }
         ICommodities->AttachCxObj(cxBroker, filter);
         sender = CxSender(cxBrokerMP, CXIE_HOTKEY);
         ICommodities->AttachCxObj(filter, sender);
         if( (translate=CxTranslate(NULL)) )
          ICommodities->AttachCxObj(filter, translate);
         if(badfilter)
          break;
         IUtility->Strlcpy( dd->hotkey1, res_string, sizeof(dd->hotkey1) );

         IIntuition->GetAttr(LISTBROWSER_SelectedNode, OBJ(OBJ_LISTBROWSER), (uint32 *)&res_node);
         IListBrowser->GetListBrowserNodeAttrs( (struct Node *)res_node, LBNA_Column,0, LBNCA_Text,&res_string, TAG_DONE );
         IUtility->Strlcpy( dd->KeymapName[1], res_string, sizeof(dd->KeymapName[1]) );
#ifdef _DEBUG_
IDOS->Printf("USE: '%s' '%s'\n",dd->KeymapName[1],dd->hotkey1);
#endif
         if( (result & WMHI_GADGETMASK) == OBJ_SAVE )
         {
          SaveToolType(dd, "HOTKEY", dd->hotkey1);
          SaveToolType(dd, "KEYMAP2", dd->KeymapName[1]);
#ifdef _DEBUG_
IDOS->Printf("Saved KEYMAP2 and HOTKEY tooltypes.\n");
#endif
         }
         SetKeyMapFlag(dd->KeymapName[1], keymap1Flag); // update KEYMAP2 flag
         done = FALSE;
        }
        break;
        case OBJ_QUIT:
         done = FALSE;
        break;

        case OBJ_INPUTPREFS:
         beginCommand("SYS:Prefs/Input");
         done = TRUE;
        break;

        case OBJ_LISTBROWSER:
         IIntuition->GetAttr(LISTBROWSER_SelectedNode, OBJ(OBJ_LISTBROWSER), (uint32 *)&res_node);
         IListBrowser->GetListBrowserNodeAttrs( (struct Node *)res_node, LBNA_Column,0, LBNCA_Text,&res_string, TAG_DONE );
         IIntuition->RefreshSetGadgetAttrs(GAD(OBJ_USEKM2), pwindow, NULL,
                                           STRINGA_TextVal,res_string, TAG_DONE);
        break;
       }
     } // END switch
    } // END if( (result..
   } // END if(siggot..
  } // END while(done) 


 if(IUtility->Stricmp(dd->KeymapName[1], keymap2Sel) != 0) // keymap2 changed?
  {
   SetKeyMapFlag(dd->KeymapName[1], keymap2Flag);
//IDOS->Printf("Sel='%s' [1]='%s' ('%s')\n",dd->keymapSel,dd->KeymapName[1], keymap2Flag);
   if(dd->km_pos == 2) // -> KeymapName[1] (KEYMAP2) is the active one
   {
    ChangeKeymap(dd, dd->KeymapName[1]);
    IUtility->Strlcpy( dd->keymapSel, dd->KeymapName[1], sizeof(dd->keymapSel) );
    LoadImageFlag(dd); // update flag image
    SignalDocky(dd, 1); // need to 'RedrawNow'
   }
  }


 } // END if( (pwindow..

 //IIntuition->DisposeObject(imageP);
 IIntuition->DisposeObject(pwin);
 pwin = NULL;
 IListBrowser->FreeLBColumnInfo(columninfo);
 IExec->FreeSysObject(ASOT_PORT, wAppPort);
 wAppPort = NULL;
}


void ReadInputPrefsKeymap(STRPTR keymap)
{
 struct IFFHandle *iffhandle;
 struct StoredProperty *sp;
 int32 ifferror;

 IUtility->Strlcpy(keymap, "usa", 64); // default keymap (usa=usa1)

 if( (iffhandle=IIFFParse->AllocIFF()) )
 {
  // Open the file for reading
  if((iffhandle->iff_Stream=(int32)IDOS->Open("ENV:Sys/input.prefs", MODE_OLDFILE)) )
  {
   IIFFParse->InitIFFasDOS(iffhandle); // initialize the iff handle
   if( (ifferror=IIFFParse->OpenIFF(iffhandle, IFFF_READ)) == 0 )
   {
    IIFFParse->PropChunk(iffhandle, ID_PREF, ID_INPT);
    for(;;)
    {
     ifferror = IIFFParse->ParseIFF(iffhandle, IFFPARSE_STEP);

     if( (sp=IIFFParse->FindProp(iffhandle, ID_PREF, ID_INPT)) )
     {
      if(IUtility->Stricmp( ((struct InputPrefs *)sp->sp_Data)->ip_KeymapName, "usa1") == 0)
       IUtility->Strlcpy(keymap, "usa", 64); // show USA instead of USA1
      else
       IUtility->Strlcpy(keymap, ((struct InputPrefs *)sp->sp_Data)->ip_KeymapName, 64);

      break;
     }
    }

    IIFFParse->CloseIFF(iffhandle);
   }

   IDOS->Close(iffhandle->iff_Stream);
   }
   else
    IDOS->Printf( GetString(&li, MSG_ERROR_NO_INPUTPREFS) );

   IIFFParse->FreeIFF(iffhandle);
   }
#ifdef _DEBUG_
IDOS->Printf("ReadInputPrefsKeymap(): '%s'\n",keymap);
#endif
}


void DoMessage(char *message, char reqtype, struct DockyData *dd)
{
 Object *requester = IIntuition->NewObject(NULL, "requester.class",
                                  REQ_Image, reqtype,
                                  REQ_TitleText, IDOS->FilePart(dd->filename),
                                  REQ_BodyText, message,
                                  REQ_GadgetText, GetString(&li, MSG_OK_GAD),
                                 TAG_DONE);

 IIntuition->IDoMethod(requester, RM_OPENREQ, NULL, NULL, dd->scr);
 IIntuition->DisposeObject(requester);
}


#define BUFLENGTH 128
// Some workarounds are neded to show correct flag
void SetKeyMapFlag(char *keymapname, char *keymapflag)
{
 char buffer[BUFLENGTH], KM_buf[BUFLENGTH];
 int32 pos;

 *keymapflag = '\0';
 *buffer = '\0';
 *KM_buf = '\0';
 IUtility->Strlcpy(keymapflag, "LOCALE:Flags/KeyMaps/", 1024); // flags image path
 pos = IDOS->SplitName(keymapname, '_', KM_buf, 0, BUFLENGTH);
#ifdef _DEBUG_
IDOS->Printf("1)pos=%ld '%s'\n",pos,KM_buf);
#endif
// if(IUtility->Stricmp(KM_buf, "usa1") == 0) // Input prefs. using built-in American
//  IUtility->Strlcpy(KM_buf, "usa", BUFLENGTH);

 if(IUtility->Stricmp(KM_buf, "srb") == 0    // keymap SRB_ -> flag SRB_xxx
    || IUtility->Stricmp(KM_buf, "pl") == 0) // keymap PL_ -> flag PL_xxx
 {
  pos = IDOS->SplitName(keymapname, '_', buffer, pos, BUFLENGTH);
  IUtility->Strlcat(KM_buf, "_", BUFLENGTH);
  IUtility->Strlcat(KM_buf, buffer, BUFLENGTH);
 }

 if(IUtility->Stricmp(KM_buf, "rus") == 0) // keymap RUS_ -> flag RUS_xxx
 {
  pos = IDOS->SplitName(keymapname, '_', buffer, pos, BUFLENGTH);
  IUtility->Strlcat(KM_buf, "_", BUFLENGTH);
  IUtility->Strlcat(KM_buf, buffer, BUFLENGTH);

  if(IUtility->Stricmp(KM_buf, "rus_uae") == 0) // keymap RUS_UAE -> flag RUS_UAE_xxx
  {
   //IUtility->Strlcpy(KM_buf, "rus_gb", BUFLENGTH);
   pos = IDOS->SplitName(keymapname, '_', buffer, pos, BUFLENGTH);
   IUtility->Strlcat(KM_buf, "_", BUFLENGTH);
   IUtility->Strlcat(KM_buf, buffer, BUFLENGTH);
  }
  else {
   IUtility->Strlcpy(KM_buf, "rus_gb", BUFLENGTH);
  }
 }

 if(IUtility->Stricmp(KM_buf, "usa") == 0) // keymap USA_ -> flag USA
 {
  pos = IDOS->SplitName(keymapname, '_', buffer, pos, BUFLENGTH);
  // USA special cases -> flag USA_xx
  if(IUtility->Stricmp(buffer, "gr") == 0  || IUtility->Stricmp(buffer, "il") == 0)
  {
   IUtility->Strlcat(KM_buf, "_", BUFLENGTH);
   IUtility->Strlcat(KM_buf, buffer, BUFLENGTH);
  }
 }

 if(IUtility->Stricmp(KM_buf, "tha") == 0) // no THAI flag image available
  IUtility->Strlcpy(KM_buf, "usa", BUFLENGTH);
#ifdef _DEBUG_
IDOS->Printf("2)pos=%ld '%s'\n",pos,KM_buf);
#endif
 IUtility->Strlcat(keymapflag, KM_buf, 1024);
}


int32 beginCommand(char *command)
{
 return IDOS->SystemTags(command, SYS_Input,NULL, SYS_Output,NULL, SYS_Error,NULL,
                         NP_Priority,0, SYS_Asynch,TRUE, TAG_END);
}


void SaveToolType(struct DockyData *dd, STRPTR ttpName, STRPTR ttpArg)
{
 STRPTR *oldttp = NULL;
 int i;
 char newttp[1024], ttpBuf1[64], ttpBuf2[64];
 struct DiskObject *micon = NULL;
#ifdef _DEBUG_
IDOS->Printf("START SaveToolType()\n");
#endif
 IUtility->SNPrintf(newttp, sizeof(newttp), "%s=",ttpName); // "<tooltype>="
 IUtility->Strlcpy( ttpBuf1, newttp, sizeof(ttpBuf1) ); // BUF1 = "<tooltype>="
 IUtility->SNPrintf(ttpBuf2, sizeof(ttpBuf2), "(%s=",ttpName); // BUF2 = "(<tooltype>="

 micon = IIcon->GetDiskObject(dd->filename);
 if(micon)
 {
  for(i = 0; micon->do_ToolTypes[i] != NULL; i++) // parse all icon tootlypes
  {
#ifdef _DEBUG_
IDOS->Printf("%2ld'%s'\n",i,micon->do_ToolTypes[i]);
#endif
   if(!IUtility->Strnicmp( micon->do_ToolTypes[i], ttpBuf1, IUtility->Strlen(ttpBuf1) )
      || !IUtility->Strnicmp( micon->do_ToolTypes[i], ttpBuf2, IUtility->Strlen(ttpBuf2) ))
   {// found tooltype
    IUtility->Strlcat( newttp, ttpArg, sizeof(newttp) );
#ifdef _DEBUG_
IDOS->Printf("'%s' == '%s'?\n",micon->do_ToolTypes[i],newttp);
#endif
    if( IUtility->Stricmp(micon->do_ToolTypes[i], newttp) )
    {// tooltype loaded diffs. from icon's tooltype -> modify tooltype and END
     oldttp = micon->do_ToolTypes;
     micon->do_ToolTypes[i] = newttp;
     IIcon->PutDiskObject(dd->filename, micon);
     micon->do_ToolTypes = oldttp;
#ifdef _DEBUG_
IDOS->Printf("Tooltype (%s) modified.\n",newttp);
#endif
    }
    break;

   } // END if( !strncmp(micon->..
  } // END for

  if(!micon->do_ToolTypes[i])
  {// no tooltype -> create tooltype entry and END
#ifdef _DEBUG_
IDOS->Printf("Tooltype '%s' NOT found (i=%ld)\n",newttp,i);
#endif
   IUtility->Strlcat( newttp, ttpArg, sizeof(newttp) );
   oldttp = micon->do_ToolTypes;
   STRPTR *newtts = malloc( (i+2) * sizeof(STRPTR) );
   IExec->CopyMem(oldttp, newtts, i * sizeof(STRPTR) ); // clone tooltypes
   newtts[i] = newttp;
   newtts[i+1] = NULL;
   micon->do_ToolTypes = newtts;
   IIcon->PutDiskObject(dd->filename, micon);
   micon->do_ToolTypes = oldttp;
   free(newtts);
#ifdef _DEBUG_
IDOS->Printf("Tooltype (%s) created.\n",newttp);
#endif
  }

  IIcon->FreeDiskObject(micon);
 }

#ifdef _DEBUG_
IDOS->Printf("END SaveToolType()\n");
#endif
}

