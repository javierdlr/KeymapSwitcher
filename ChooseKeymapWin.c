//#define WDEBUG

#include "dockybase.h"

#include <proto/keymap.h>
#include "DockWindow.h"

#include "KeyMapSwitcher_rev.h"

extern struct ExecIFace *IExec;
extern struct KeymapIFace *IKeymap;
extern struct IntuitionIFace *IIntuition;

// the class pointer
extern Class *ButtonClass, *BitMapClass, *LayoutClass, *WindowClass;

extern Object *Objects[LAST_NUM];
#define OBJ(x) Objects[x]
#define GAD(x) (struct Gadget *)Objects[x]
extern char keymap0Flag[1024], keymap1Flag[1024], keymap2Flag[1024];

extern void DoMessage(char *, char, struct DockyData *);

Object *win2;
struct Window *window2;
uint32 wsignal = 0;


void OpenDockWindow(struct DockyData *dd)
{
 //extern BOOL WinBorders;
 extern BOOL KM3;
/*
 if(dd->scaleflag == 1.0)
 {
  dd->size.width += 8; // adds space image<->button_frame
  dd->size.height += 8; // adds space image<->button_frame
 }*/

 win2 = IIntuition->NewObject(WindowClass, NULL,
    //WA_ScreenTitle,  VERS" "DATE,
    WA_CustomScreen, (long)dd->scr,
    WA_Activate,     TRUE,
    WA_DepthGadget,  FALSE,
    WA_DragBar,      FALSE,
    WA_CloseGadget,  FALSE,
    WA_SizeGadget,   FALSE,
    //WA_ToolBox, TRUE,
    //WA_SmartRefresh, TRUE,
    WINDOW_NewMenu, NULL, // enabling RMB (WMHI_MENUPICK)
    //WINDOW_GadgetHelp, TRUE, // gadgethelp on/off
    //WA_Borderless, WinBorders,
    WINDOW_Position, WPOS_CENTERMOUSE,
    WINDOW_Layout, OBJ(OBJ_MAIN) = IIntuition->NewObject(LayoutClass, NULL,
     //LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
     LAYOUT_SpaceOuter, TRUE,
     LAYOUT_InnerSpacing, 6,
     LAYOUT_AddChild, OBJ(OBJ_BTN1) = IIntuition->NewObject(ButtonClass, NULL,
      GA_ID,         OBJ_BTN1,
      GA_RelVerify,  TRUE,
      GA_Underscore, ~0,
      BUTTON_BevelStyle,  BVS_THIN,
      //BUTTON_Transparent, TRUE,
      BUTTON_RenderImage, OBJ(GID_KM1) = IIntuition->NewObject(BitMapClass, NULL,
       IA_Width,    /*dd->size.width*/IMG_W + 8,
       IA_Height,   /*dd->size.height*/IMG_H + 8,
       IA_Scalable, dd->scaleflag > 1.0? TRUE:FALSE,
       BITMAP_SourceFile, keymap0Flag,
       BITMAP_Screen,     dd->scr,
       BITMAP_Masking,    TRUE,
       //BITMAP_Transparent,TRUE,
      TAG_DONE),
     TAG_DONE),
     LAYOUT_AddChild, OBJ(OBJ_BTN2) = IIntuition->NewObject(ButtonClass, NULL,
      GA_ID,         OBJ_BTN2,
      GA_RelVerify,  TRUE,
      GA_Underscore, ~0,
      BUTTON_BevelStyle,  BVS_THIN,
      //BUTTON_Transparent, TRUE,
      BUTTON_RenderImage, OBJ(GID_KM2) = IIntuition->NewObject(BitMapClass, NULL,
       IA_Width,    /*dd->size.width*/IMG_W + 8,
       IA_Height,   /*dd->size.height*/IMG_H + 8,
       IA_Scalable, dd->scaleflag > 1.0? TRUE:FALSE,
       BITMAP_SourceFile, keymap1Flag,
       BITMAP_Screen,     dd->scr,
       BITMAP_Masking,    TRUE,
       //BITMAP_Transparent,TRUE,
      TAG_DONE),
     TAG_DONE),

   TAG_DONE),
 TAG_END);

 // languages NOT valid/available
 if( !OBJ(GID_KM1) )
  IIntuition->SetAttrs(OBJ(OBJ_BTN1), GA_Text,dd->KeymapName[0], GA_Disabled,TRUE, TAG_DONE);
 if( !OBJ(GID_KM2) )
  IIntuition->SetAttrs(OBJ(OBJ_BTN2), GA_Text,dd->KeymapName[1], GA_Disabled,TRUE, TAG_DONE);

 if(KM3) // if set add KEYMAP3 button
 {
  OBJ(OBJ_BTN3) = IIntuition->NewObject(ButtonClass, NULL,
                               GA_ID,         OBJ_BTN3,
                               GA_RelVerify,  TRUE,
                               GA_Underscore, ~0,
                               BUTTON_BevelStyle,  BVS_THIN,
                               //BUTTON_Transparent, TRUE,
                               BUTTON_RenderImage, OBJ(GID_KM3) = IIntuition->NewObject(BitMapClass, NULL,
                                IA_Width,    /*dd->size.width*/IMG_W + 8,
                                IA_Height,   /*dd->size.height*/IMG_H + 8,
                                IA_Scalable, dd->scaleflag > 1.0? TRUE:FALSE,
                                BITMAP_SourceFile, keymap2Flag,
                                BITMAP_Screen,     dd->scr,
                                BITMAP_Masking,    TRUE,
                                //BITMAP_Transparent,TRUE,
                               TAG_DONE),
                              TAG_DONE);

  if( !OBJ(GID_KM3) ) // language NOT valid/available
   IIntuition->SetAttrs(OBJ(OBJ_BTN3), GA_Text,dd->KeymapName[2], GA_Disabled,TRUE, TAG_DONE);
/*
  struct lmAddChild msg;
  msg.MethodID       = LM_ADDCHILD;
  msg.lm_Window      = window2;
  msg.lm_Object      = OBJ(OBJ_BTN3);
  msg.lm_ObjectAttrs = NULL;
  IIntuition->IDoMethodA(OBJ(OBJ_MAIN), (void*)&msg);
*/
  IIntuition->IDoMethod(OBJ(OBJ_MAIN), LM_ADDCHILD, window2, OBJ(OBJ_BTN3), NULL);
 }

 if(win2)
 {
  if( (window2=(struct Window *)IIntuition->IDoMethod(win2, WM_OPEN, NULL)) )
  {
   IIntuition->GetAttr(WINDOW_SigMask, win2, &wsignal); // obtain the window wait signal mask.
   // Input Event Loop
   while( !ProcessDockWindow(dd) );

   IIntuition->IDoMethod(win2, WM_CLOSE, NULL);
  } // END if(window2[WID_MAIN])

  FreeDockWindow();
 } // END if(win[OID_MAIN])

}


void FreeDockWindow(void)
{
 IIntuition->DisposeObject( OBJ(GID_KM1) );
 IIntuition->DisposeObject( OBJ(GID_KM2) );
 IIntuition->DisposeObject( OBJ(GID_KM3) );
 OBJ(GID_KM1) = NULL;
 OBJ(GID_KM2) = NULL;
 OBJ(GID_KM3) = NULL;
 IIntuition->DisposeObject(win2);
// if(!window2)
  win2 = NULL; // V52.2: give "option" to use RMB with DOCKY_1CLICK=YES
// else
  window2 = NULL;
}


BOOL ProcessDockWindow(struct DockyData *dd)
{
 uint32 wait = IExec->Wait(wsignal);

 if(wait & wsignal)
 {
  uint32 result = WMHI_LASTMSG;
  int16 code = 0;

  while( (result = IIntuition->IDoMethod(win2, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG )
  {
#ifdef WDEBUG
IDOS->Printf("%08lx code=%ld\n",result,code);
#endif
   switch(result & WMHI_CLASSMASK)
   {
    case WMHI_MENUPICK: // clicked RMB
     window2 = NULL; // V52.2: give "option" to use RMB with DOCKY_1CLICK=YES
    case WMHI_INACTIVE: // clicked outside DockWindow
#ifdef WDEBUG
IDOS->Printf("WMHI_MENUPICK or WMHI_INACTIVE\n");
#endif
     return TRUE;
    break;

    case WMHI_GADGETUP:
#ifdef WDEBUG
IDOS->Printf("WMHI_GADGETUP (%08lx)\n",result);
#endif
     switch(result & WMHI_GADGETMASK)
      {
       case OBJ_BTN1:
        dd->km_pos = 1;
        return TRUE;
       break;
       case OBJ_BTN2:
        dd->km_pos = 2;
        return TRUE;
       break;
       case OBJ_BTN3:
        dd->km_pos = 3;
        return TRUE;
       break;
      } // END switch(result & WMHI_GADGETMASK)
    break;
   }  // END switch(result & WMHI_CLASSMASK)

  } // END while( (result..

 } // if(wait & wsignal)

 return FALSE;
}


BOOL ChangeKeymap(struct DockyData *dd, STRPTR keymap)
{
 APTR KM_handle;
 BOOL res = FALSE;
 struct KeyMapNode *kmn = NULL;
//IDOS->Printf("ChangeKeymap(%s, %s)\n",keymap,dd->keymapSel);
 if(IUtility->Stricmp(keymap, dd->keymapSel) != 0) // only if keymap changed
 {
  if( (KM_handle = IKeymap->OpenKeyMapHandle(keymap)) )
  {
   if( IKeymap->ObtainKeyMapInfo(KM_handle, KEYMAPINFO_KEYMAPNODE,&kmn, TAG_END) )
   {
#ifdef WDEBUG
IDOS->Printf("Changing keymap from '%s' to '%s'...\n",dd->keymapSel,kmn->kn_Node.ln_Name);
#endif
    IKeymap->SetKeyMapDefault( &(kmn->kn_KeyMap) );
    //IUtility->Strlcpy( dd->keymapSel, kmn->kn_Node.ln_Name, sizeof(dd->keymapSel) );
    res = TRUE;
   }
   else
   {
    IUtility->VSNPrintf(textbuf, sizeof(textbuf), GetString(&li, MSG_ERROR_CANTCHANGE),&keymap);
    DoMessage(textbuf, REQIMAGE_WARNING, dd);
   }

   IKeymap->CloseKeyMapHandle(KM_handle);
  } // END if( (KM_handle =..
  else
  {
   IUtility->VSNPrintf(textbuf, sizeof(textbuf), GetString(&li, MSG_ERROR_KEYMAP),&keymap);
   DoMessage(textbuf, REQIMAGE_ERROR, dd);
  }

 } // END if(IUtility->Stricmp..
//IDOS->Printf("ChangeKeymap=%ld\n",res);
 return(res);
}

