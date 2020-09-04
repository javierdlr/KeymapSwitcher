#ifndef DOCKY_BASE_H
#define DOCKY_BASE_H
#ifndef DOCKY_DATA_H
#define DOCKY_DATA_H

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <proto/commodities.h>
#include <libraries/docky.h>
#include <interfaces/docky.h>
#include <prefs/prefhdr.h>
#include <prefs/input.h>
#define ALL_REACTION_CLASSES
#include <reaction/reaction.h>
#include <proto/listbrowser.h>
#include <proto/popupmenu.h>

//#define DOCKYF_IMAGEFLAG (1 << 0)
#define OBJ(x) Objects[x]
#define GAD(x) (struct Gadget *)Objects[x]

// flag images Width and Height
#define IMG_W 25
#define IMG_H 18

struct DockyData {
 struct DockyObjectNr self;
 struct DockySize size;
 struct RastPort *rp;
 struct DockyAttention attn;
 struct Screen *scr;
 struct Window *win;
 struct Process *PrefsProc, *CxProc;
 struct FontISO {
                 //struct TextFont *tf;
                 struct TTextAttr tta;
                 struct TagItem ti[2];
                } fiso[3];
 char filename[1024], keymapSel[64], KeymapName[3][64],
      hotkey1[32],
      filepath[1024];
 int32 km_item, km_pos;
 uint32 dockyFlags;
 BOOL alt_menunames;
 float scaleflag;
 //APTR pMutex;
};

struct DockyBase {
 struct Library libBase;
 APTR SegList;
};

extern uint32              VARARGS68K _docky_Obtain(struct DockyIFace *);
extern uint32              VARARGS68K _docky_Release(struct DockyIFace *);
extern void                VARARGS68K _docky_Expunge(struct DockyIFace *);
extern struct DockyIFace * VARARGS68K _docky_Clone(struct DockyIFace *);
extern BOOL                VARARGS68K _docky_DockyGet(struct DockyIFace *, uint32 msgType, uint32 *msgData);
extern BOOL                VARARGS68K _docky_DockySet(struct DockyIFace *, uint32 msgType, uint32 msgData);
extern BOOL                VARARGS68K _docky_DockyProcess(struct DockyIFace *, uint32 turnCount, uint32 *msgType, uint32 *msgData, BOOL *anotherTurn);

enum // windows
{
 WID_MAIN = 0,
 WID_PREFS,
 WID_LAST
};

enum
{
 // preferences objects
 OBJ_LISTBROWSER = 0,
 OBJ_INPUTPREFS,
 OBJ_QUIT,
 OBJ_USE,
 OBJ_SAVE,
 OBJ_HK1,
 OBJ_USEKM2,
 // ChooseKeymapMenu objects
 GID_BITMAP1,
 GID_BITMAP2,
 GID_BITMAP3,
 // ChooseKeymapWin objects/options
 OBJ_MAIN,
 OBJ_BTN1,
 OBJ_BTN2,
 OBJ_BTN3,
 GID_KM1,
 GID_KM2,
 GID_KM3,
 LAST_NUM
};

enum
{
 // ChooseKeymapMenu options
 MENU_OPT1 = 0,
 MENU_OPT2,
 MENU_OPT3,
 MENU_PREFS
};

#endif
#endif

