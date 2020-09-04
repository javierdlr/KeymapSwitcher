//#define MDEBUG

#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/keymap.h>
//#include <proto/graphics.h>
//#include <proto/diskfont.h>
#include <classes/popupmenu.h>
//#include <images/label.h>
//#include <graphics/rpattr.h>
#include "dockybase.h"
#include "DockWindow.h"

#include "KeyMapSwitcher_rev.h"

extern struct ExecIFace *IExec;
extern struct KeymapIFace *IKeymap;
extern struct IntuitionIFace *IIntuition;
//extern struct GraphicsIFace *IGraphics;
//extern struct DiskfontIFace *IDiskfont;

extern Object *Objects[LAST_NUM];
#define OBJ(x) Objects[x]
#define GAD(x) (struct Gadget *)Objects[x]
extern BOOL WinMode, KM3;
extern char keymap0Flag[1024], keymap1Flag[1024], keymap2Flag[1024];

extern void SetKeyMapFlag(char *, char *);
extern BOOL PrefsWindow_NP(struct DockyIFace *, struct DockyData *);
#ifdef NEWPROC
extern int32 OpenPrefsWindow(CONST_STRPTR, LONG, struct ExecBase *);
#else
extern int32 OpenPrefsWindow(struct DockyData *);
#endif

uint32 MenuHandlerFunc(struct Hook *hook, Object *o, APTR reserved);


void OpenDockMenu(struct DockyIFace *Self)
{
 struct DockyData *dd = (struct DockyData *)((uint32)Self - Self->Data.NegativeSize);
 Object *chooseMenu;
 struct Hook *MenuHandler;
 char KeymapLocalName[3][64];
 STRPTR keymapdesc;
 APTR KM_handle;
 int32 i, KM_cs[3];

 for(i = 0; KM3? i<3:i<2; i++)
 {
  if( (KM_handle=IKeymap->OpenKeyMapHandle(dd->KeymapName[i], NULL)) )
  {
   IKeymap->ObtainKeyMapInfo(KM_handle, KEYMAPINFO_INFOTEXT_LOCAL,&keymapdesc,
                             KEYMAPINFO_INFOTEXT_CHARSET,&KM_cs[i], TAG_END);

   IUtility->Strlcpy(KeymapLocalName[i], keymapdesc, 64);

   IKeymap->ReleaseKeyMapInfo(KM_handle, KEYMAPINFO_INFOTEXT_LOCAL,&keymapdesc,
                              KEYMAPINFO_INFOTEXT_CHARSET,&KM_cs[i], TAG_END);

   IKeymap->CloseKeyMapHandle(KM_handle);
  }
  else
   IUtility->Strlcpy(KeymapLocalName[i], dd->KeymapName[i], 64);
//IDOS->Printf("KEYMAP%ld='%s' / %ld\n",i+1,KeymapLocalName[i],KM_cs[i]);
 }

 MenuHandler = (struct Hook *)IExec->AllocSysObjectTags(ASOT_HOOK,
                                      ASOHOOK_Entry, MenuHandlerFunc,
                                      ASOHOOK_Data, Self,
                                     TAG_DONE);

 chooseMenu = IIntuition->NewObject(PopupMenuClass, NULL,// "popupmenu.class",
                                    PMA_MenuHandler,MenuHandler, TAG_DONE);

 Object *item1 = IIntuition->NewObject(NULL, "popupmenuitem.class",
                              PMIA_ID,    MENU_OPT1,
                              PMIA_Title, dd->alt_menunames? KeymapLocalName[0]:dd->KeymapName[0],
                              PMIA_Icon,  OBJ(GID_BITMAP1) = IIntuition->NewObject(NULL, "bitmap.image",
                               BITMAP_SourceFile, keymap0Flag,
                               BITMAP_Screen,     dd->scr,
                               BITMAP_Masking,    TRUE,
                              TAG_DONE),
                             TAG_END);

 Object *item2 = IIntuition->NewObject(NULL, "popupmenuitem.class",
                              PMIA_ID,    MENU_OPT2,
                              PMIA_Title, dd->alt_menunames? KeymapLocalName[1]:dd->KeymapName[1],
                              //PMIA_Image, IMG_desc,
                              PMIA_Icon,  OBJ(GID_BITMAP2) = IIntuition->NewObject(NULL, "bitmap.image",
                               BITMAP_SourceFile, keymap1Flag,
                               BITMAP_Screen,     dd->scr,
                               BITMAP_Masking,    TRUE,
                              TAG_DONE),
                             TAG_END);

 Object *item3 = IIntuition->NewObject(NULL, "popupmenuitem.class",
                              PMIA_ID,    MENU_OPT3,
                              PMIA_Title, dd->alt_menunames? KeymapLocalName[2]:dd->KeymapName[2],
                              PMIA_Icon,  OBJ(GID_BITMAP3) = IIntuition->NewObject(NULL, "bitmap.image",
                               BITMAP_SourceFile, keymap2Flag,
                               BITMAP_Screen,     dd->scr,
                               BITMAP_Masking,    TRUE,
                              TAG_DONE),
                             TAG_END);

 Object *itemS = IIntuition->NewObject(NULL, "popupmenuitem.class",
                              PMIA_Title, GetString(&li, MSG_DOCKY_SETTINGS),
                              PMIA_ID,    MENU_PREFS,
                             TAG_END);

 Object *itemBar = IIntuition->NewObject(NULL, "popupmenuitem.class", PMIA_Title,~0, TAG_END);

 // language2 NOT valid/available
 if( !OBJ(GID_BITMAP2) )
  IIntuition->SetAttrs(item2, PMIA_Disabled,TRUE, TAG_DONE);

 IIntuition->IDoMethod(chooseMenu, OM_ADDMEMBER, item1); // lang1
 IIntuition->IDoMethod(chooseMenu, OM_ADDMEMBER, item2); // lang2
 if(OBJ(GID_BITMAP3) && KM3)
  IIntuition->IDoMethod(chooseMenu, OM_ADDMEMBER, item3); // lang3

 if(!WinMode)
 {
  IIntuition->IDoMethod(chooseMenu, OM_ADDMEMBER, itemBar); // separator
  IIntuition->IDoMethod(chooseMenu, OM_ADDMEMBER, itemS); // Settings...
 }

 IIntuition->IDoMethod(chooseMenu, PM_OPEN, dd->win); // show PopUp Menu

 IIntuition->DisposeObject(chooseMenu);
 IExec->FreeSysObject(ASOT_HOOK, MenuHandler);
}


uint32 MenuHandlerFunc(struct Hook *hook, Object *o, APTR reserved)
{
 struct DockyIFace *Self = (struct DockyIFace *)hook->h_Data;
 struct DockyData *dd = (struct DockyData *)((uint32)Self - Self->Data.NegativeSize);
 uint32 item_ID = 0;

 if( IIntuition->GetAttr(PMIA_ID, o, &item_ID) )
 {
#ifdef MDEBUG
IDOS->Printf("Choosed: ");
#endif
  switch(item_ID)
  {
   case MENU_OPT1:
#ifdef MDEBUG
IDOS->Printf("'%s'\n",dd->KeymapName[0]);
#endif
    dd->km_pos = 1;
    //ChangeKeymap(dd, dd->KeymapName[0]);
    //IUtility->Strlcpy( dd->keymapSel, dd->KeymapName[0], sizeof(dd->keymapSel) );
   break;
   case MENU_OPT2:
#ifdef MDEBUG
IDOS->Printf("'%s'\n",dd->KeymapName[1]);
#endif
    dd->km_pos = 2;
    //ChangeKeymap(dd, dd->KeymapName[1]);
    //IUtility->Strlcpy( dd->keymapSel, dd->KeymapName[1], sizeof(dd->keymapSel) );
   break;
   case MENU_OPT3:
#ifdef MDEBUG
IDOS->Printf("'%s'\n",dd->KeymapName[2]);
#endif
    dd->km_pos = 3;
    //ChangeKeymap(dd, dd->KeymapName[2]);
    //IUtility->Strlcpy( dd->keymapSel, dd->KeymapName[2], sizeof(dd->keymapSel) );
   break;
   case MENU_PREFS:
   {
#ifdef MDEBUG
IDOS->Printf("Settings...\n");
#endif
    #ifdef NEWPROC
    PrefsWindow_NP(Self, dd);
    #else
    OpenPrefsWindow(dd);
    #endif
   }
   break;
  }
 }

 return(0);
}

