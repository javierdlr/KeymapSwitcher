//#define IDEBUG

#include "dockybase.h"

#include <exec/exec.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <exec/types.h>
//#include <stdlib.h>

#include "KeyMapSwitcher_rev.h"

#include <proto/locale.h>
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#define CATCOMP_CODE
struct Library *LocaleBase = NULL;
struct LocaleIFace *ILocale = NULL;
struct LocaleInfo li;
#include "include/keymapswitcher_strings.h"
//struct Catalog *catalog = (struct Catalog *)NULL;

// the class library base
struct ClassLibrary *PopupMenuBase = NULL, *ListBrowserBase = NULL, *StringBase = NULL,
                    *ButtonBase = NULL, *BitMapBase = NULL, *LayoutBase = NULL,
                    *LabelBase = NULL, *WindowBase = NULL;
// the class pointer
Class *PopupMenuClass, *ListBrowserClass, *StringClass, *ButtonClass,
      *BitMapClass, *LayoutClass, *LabelClass, *WindowClass;

volatile char __attribute__((unused)) verstag[] = VERSTAG;

BOOL openLibs(void);
void closeLibs(void);

struct Library *DOSBase = NULL;
struct Library *UtilityBase = NULL;
struct Library *IconBase = NULL;
struct Library *IntuitionBase = NULL;
struct Library *IFFParseBase = NULL;
struct Library *KeymapBase = NULL;
struct Library *DataTypesBase = NULL;
struct Library *GfxBase = NULL;
struct Library *CommoditiesBase = NULL;
//struct Library *DiskfontBase = NULL;

struct ExecIFace *IExec = NULL;
struct DOSIFace *IDOS = NULL;
struct UtilityIFace *IUtility = NULL;
struct IconIFace *IIcon = NULL;
struct IntuitionIFace *IIntuition = NULL;
struct IFFParseIFace *IIFFParse = NULL;
struct KeymapIFace *IKeymap = NULL;
struct DataTypesIFace *IDataTypes = NULL;
struct GraphicsIFace *IGraphics = NULL;
struct CommoditiesIFace *ICommodities = NULL;
//struct DiskfontIFace *IDiskfont = NULL;

struct Library *NewlibBase = NULL;
struct Interface *INewlib = NULL;

//struct Library *ApplicationBase = NULL;
//struct PrefsObjectsIFace *IPrefsObjects = NULL;

struct ListBrowserIFace *IListBrowser = NULL;

/*
 * The system (and compiler) rely on a symbol named _start which marks
 * the beginning of execution of an ELF file. To prevent others from 
 * executing this library, and to keep the compiler/linker happy, we
 * define an empty _start symbol here.
 *
 * On the classic system (pre-AmigaOS4) this was usually done by
 * moveq #0,d0
 * rts
 */
int32 _start(STRPTR args, LONG length, struct ExecBase *sysbase)
{
 /* If you feel like it, open DOS and print something to the user */
 return RETURN_OK;
}


/* Open the library */
struct Library *libOpen(struct LibraryManagerInterface *Self, ULONG version)
{
struct Library *libBase = (struct Library *)Self->Data.LibBase; 

 if(version > VERSION)
  return NULL;

 // Add any specific open code here,
 // Return 0 before incrementing OpenCnt to fail opening.
 if( ((struct Library *)libBase)->lib_OpenCnt == 2 ) // KMS already running?
 {
#ifdef IDEBUG
IExec->DebugPrintF("[%s]libOpen: docky already running!\n",LIBNAME);
#endif
  IDOS->Printf("%s already running!\n",LIBNAME);
  return NULL;
 }

 // Add up the open count
 ((struct Library *)libBase)->lib_OpenCnt++;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]libOpen %ld\n",LIBNAME,((struct Library *)libBase)->lib_OpenCnt);
#endif
 return libBase;
}


/* Close the library */
APTR libClose(struct LibraryManagerInterface *Self)
{
 struct Library *libBase = (struct Library *)Self->Data.LibBase;
 // Make sure to undo what open did

 // Make the close count
 ((struct Library *)libBase)->lib_OpenCnt--;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]libClose %ld\n",LIBNAME,((struct Library *)libBase)->lib_OpenCnt);
#endif
 return 0;
}


/* Expunge the library */
APTR libExpunge(struct LibraryManagerInterface *Self)
{// If your library cannot be expunged, return 0
 APTR result = (APTR)0;
 struct Library *libBase = (struct Library *)Self->Data.LibBase;
 struct DockyBase *DockyLibBase = (struct DockyBase *)libBase;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]libExpunge\n",LIBNAME);
#endif
 if(libBase->lib_OpenCnt == 0)
 {
  result = (APTR)DockyLibBase->SegList;
  // Undo what the init code did
  closeLibs();

  IExec->Remove( (struct Node *)libBase );
  IExec->DeleteLibrary( (struct Library *)libBase );
 }
 else
 {
  result = (APTR)0;
  libBase->lib_Flags |= LIBF_DELEXP;
 }

return result;
}


/* The ROMTAG Init Function */
struct Library *libInit(struct Library *libBase, APTR seglist, struct Interface *exec)
{
 struct DockyBase *DockyLibBase = (struct DockyBase *)libBase;

 IExec = (struct ExecIFace *)exec;

 libBase->lib_Node.ln_Type = NT_LIBRARY;
 libBase->lib_Node.ln_Pri  = 0;
 libBase->lib_Node.ln_Name = LIBNAME;
 libBase->lib_Flags        = LIBF_SUMUSED|LIBF_CHANGED;
 libBase->lib_Version      = VERSION;
 libBase->lib_Revision     = REVISION;
 libBase->lib_IdString     = VSTRING;

 DockyLibBase->SegList = seglist;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]libInit\n",LIBNAME);
#endif
 if( openLibs() )
  return libBase;

 closeLibs();
 return NULL;
}


BOOL openLibs(void)
{
 NewlibBase = IExec->OpenLibrary("newlib.library", 52);
 if(!NewlibBase) return FALSE;
 INewlib = (struct Interface *)IExec->GetInterface(NewlibBase, "main", 1, NULL);
 if(!INewlib) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(NEWLIB)\n",LIBNAME);
#endif
/*
 ApplicationBase = IExec->OpenLibrary("application.library", 52);
 if(!ApplicationBase) return FALSE;
 IPrefsObjects = (struct PrefsObjectsIFace *)IExec->GetInterface(ApplicationBase, "prefsobjects", 2, NULL);
 if(!IPrefsObjects) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(APPLIB/PREFSOBJ)\n",LIBNAME);
#endif
*/
 DOSBase = IExec->OpenLibrary("dos.library", 52);
 if(!DOSBase) return FALSE;
 IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
 if(!IDOS) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(DOS)\n",LIBNAME);
#endif

 UtilityBase = IExec->OpenLibrary("utility.library", 52);
 if(!UtilityBase) return FALSE;
 IUtility = (struct UtilityIFace *)IExec->GetInterface(UtilityBase, "main", 1, NULL);
 if(!IUtility) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(UTILITY)\n",LIBNAME);
#endif

 IconBase = IExec->OpenLibrary("icon.library", 52);
 if(!IconBase) return FALSE;
 IIcon = (struct IconIFace *)IExec->GetInterface(IconBase, "main", 1, NULL);
 if(!IIcon) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(ICON)\n",LIBNAME);
#endif

 IntuitionBase = IExec->OpenLibrary("intuition.library", 52);
 if(!IntuitionBase) return FALSE;
 IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionBase, "main", 1, NULL);
 if(!IIntuition) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(INTUITION)\n",LIBNAME);
#endif

 IFFParseBase = IExec->OpenLibrary("iffparse.library", 52);
 if(!IFFParseBase) return FALSE;
 IIFFParse = (struct IFFParseIFace *)IExec->GetInterface(IFFParseBase, "main", 1, NULL);
 if(!IIFFParse) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(IFFPARSE)\n",LIBNAME);
#endif

 KeymapBase = IExec->OpenLibrary("keymap.library", 52);
 if(!KeymapBase) return FALSE;
 IKeymap = (struct KeymapIFace *)IExec->GetInterface(KeymapBase, "main", 1, NULL);
 if(!IKeymap) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(KEYMAP)\n",LIBNAME);
#endif

 DataTypesBase = IExec->OpenLibrary("datatypes.library", 52);
 if(!DataTypesBase) return FALSE;
 IDataTypes = (struct DataTypesIFace *)IExec->GetInterface(DataTypesBase, "main", 1, NULL);
 if(!IDataTypes) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(DATATYPES)\n",LIBNAME);
#endif

 GfxBase = IExec->OpenLibrary("graphics.library", 54);
 if(!GfxBase) return FALSE;
 IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
 if(!IGraphics) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(GRAPHICS)\n",LIBNAME);
#endif

 CommoditiesBase = IExec->OpenLibrary("commodities.library", 52);
 if(!CommoditiesBase) return FALSE;
 ICommodities = (struct CommoditiesIFace *)IExec->GetInterface(CommoditiesBase, "main", 1, NULL);
 if(!ICommodities) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(COMMODITIES)\n",LIBNAME);
#endif
/*
 DiskfontBase = IExec->OpenLibrary("diskfont.library", 52);
 if(!DiskfontBase) return FALSE;
 IDiskfont = (struct DiskfontIFace *)IExec->GetInterface(DiskfontBase, "main", 1, NULL);
 if(!IDiskfont) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(DISKFONT)\n",LIBNAME);
#endif
*/
 ButtonBase = IIntuition->OpenClass("gadgets/button.gadget", 52, &ButtonClass);
 LayoutBase = IIntuition->OpenClass("gadgets/layout.gadget", 52, &LayoutClass);
 StringBase = IIntuition->OpenClass("gadgets/string.gadget", 52, &StringClass);
 BitMapBase = IIntuition->OpenClass("images/bitmap.image", 52, &BitMapClass);
 LabelBase = IIntuition->OpenClass("images/label.image", 52, &LabelClass);
 PopupMenuBase = IIntuition->OpenClass("popupmenu.class", 52, &PopupMenuClass);
 WindowBase = IIntuition->OpenClass("window.class", 52, &WindowClass);
 if(!ButtonBase||!LayoutBase||!BitMapBase||!StringBase||!LabelBase|| \
    !PopupMenuBase||!WindowBase)
  return FALSE;

 ListBrowserBase = IIntuition->OpenClass("gadgets/listbrowser.gadget", 52, &ListBrowserClass);
 if(!ListBrowserBase) return FALSE;
 IListBrowser = (struct ListBrowserIFace *)IExec->GetInterface( (struct Library *)ListBrowserBase, "main", 1, NULL );
 if(!IListBrowser) return FALSE;
#ifdef IDEBUG
IExec->DebugPrintF("[%s]openLibs(LISTBROWSER)\n",LIBNAME);
#endif

 li.li_Catalog = NULL;
 if( (LocaleBase = IExec->OpenLibrary("locale.library", 52)) &&
     (ILocale = (struct LocaleIFace *)IExec->GetInterface(LocaleBase, "main", 1, NULL)) )
 {
  li.li_ILocale = ILocale;
  li.li_Catalog = ILocale->OpenCatalog(NULL, "keymapswitcher_docky.catalog",
                                       OC_BuiltInLanguage, "english",
                                       OC_PreferExternal, TRUE,
                                      TAG_END);
 }
 else
  IDOS->PutErrStr("Failed to use catalog system. Using built-in strings.\n");

 return TRUE;
}


void closeLibs(void)
{
 if(ILocale)
 {
  ILocale->CloseCatalog(li.li_Catalog);
  IExec->DropInterface( (struct Interface *)ILocale );
 }
 IExec->CloseLibrary( (struct Library *)LocaleBase );
/*
 IExec->DropInterface( (struct Interface *)IDiskfont );
 IExec->CloseLibrary(DiskfontBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(DISKFONT)\n",LIBNAME);
#endif
*/
 IIntuition->CloseClass(ButtonBase);
 IIntuition->CloseClass(BitMapBase);
 IIntuition->CloseClass(LayoutBase);
 IIntuition->CloseClass(StringBase);
 IIntuition->CloseClass(LabelBase);
 IIntuition->CloseClass(PopupMenuBase);
 IIntuition->CloseClass(WindowBase);

 IExec->DropInterface( (struct Interface *)IListBrowser );
 IIntuition->CloseClass(ListBrowserBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(LISTBROWSER)\n",LIBNAME);
#endif

 IExec->DropInterface( (struct Interface *)ICommodities );
 IExec->CloseLibrary(CommoditiesBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(COMMODITIES)\n",LIBNAME);
#endif

 IExec->DropInterface( (struct Interface *)IGraphics );
 IExec->CloseLibrary(GfxBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(GRAPHICS)\n",LIBNAME);
#endif

 IExec->DropInterface( (struct Interface *)IDataTypes );
 IExec->CloseLibrary(DataTypesBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(DATATYPES)\n",LIBNAME);
#endif

 IExec->DropInterface( (struct Interface *)IKeymap );
 IExec->CloseLibrary(KeymapBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(KEYMAP)\n",LIBNAME);
#endif

 IExec->DropInterface( (struct Interface *)IIFFParse );
 IExec->CloseLibrary(IFFParseBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(IFFPARSE)\n",LIBNAME);
#endif

 IExec->DropInterface( (struct Interface *)IIntuition );
 IExec->CloseLibrary(IntuitionBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(INTUITION)\n",LIBNAME);
#endif

 IExec->DropInterface( (struct Interface *)IIcon );
 IExec->CloseLibrary(IconBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(ICON)\n",LIBNAME);
#endif

 IExec->DropInterface( (struct Interface *)IUtility );
 IExec->CloseLibrary(UtilityBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(UTILITY)\n",LIBNAME);
#endif

 IExec->DropInterface( (struct Interface *)IDOS );
 IExec->CloseLibrary(DOSBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(DOS)\n",LIBNAME);
#endif

 IExec->DropInterface( (struct Interface *)INewlib );
 IExec->CloseLibrary(NewlibBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(NEWLIB)\n",LIBNAME);
#endif
/*
 IExec->DropInterface( (struct Interface *)IPrefsObjects );
 IExec->CloseLibrary(ApplicationBase);
#ifdef IDEBUG
IExec->DebugPrintF("[%s]closeLibs(APPLIB/PREFSOBJ)\n",LIBNAME);
#endif
*/
}


/* ------------------- Manager Interface ------------------------ */
/* These are generic. Replace if you need more fancy stuff */
static LONG _manager_Obtain(struct LibraryManagerInterface *Self)
{
 Self->Data.RefCount++;
 return Self->Data.RefCount;
}


static ULONG _manager_Release(struct LibraryManagerInterface *Self)
{
 Self->Data.RefCount--;
 return Self->Data.RefCount;
}


/* Manager interface vectors */
STATIC CONST APTR lib_manager_vectors[] =
{
 (void *)_manager_Obtain,
 (void *)_manager_Release,
 (void *)0,
 (void *)0,
 (void *)libOpen,
 (void *)libClose,
 (void *)libExpunge,
 (void *)0,
 (void *)-1,
};


/* "__library" interface tag list */
static struct TagItem lib_managerTags[] =
{
 { MIT_Name,        (ULONG)"__library"         },
 { MIT_VectorTable, (ULONG)lib_manager_vectors },
 { MIT_Version,     1                          },
 { TAG_END,         0                          }
};


/* ------------------- Library Interface(s) ------------------------ */
static void *docky_vectors[] = {
 (void *)_docky_Obtain,
 (void *)_docky_Release,
 (void *)_docky_Expunge,
 (void *)_docky_Clone,
 (void *)_docky_DockyGet,
 (void *)_docky_DockySet,
 (void *)_docky_DockyProcess,
 (void *)-1
};

/* Uncomment this line (and see below) if your library has a 68k jump table */
//extern ULONG VecTable68K[];
struct TagItem dockyTags[] =
{
 { MIT_Name,        (uint32)"docky"                    },
 { MIT_VectorTable, (uint32)docky_vectors              },
 { MIT_DataSize,    (uint32)(sizeof(struct DockyData)) },
 { MIT_Flags,       IFLF_PRIVATE                       },
 { MIT_Version,     1                                  },
 { TAG_END,         0                                  }
};

static uint32 libInterfaces[] =
{
 (uint32)lib_managerTags,
 (uint32)dockyTags,
 (uint32)0
};

struct TagItem libCreateTags[] =
{
 { CLT_DataSize,   (uint32)(sizeof(struct DockyBase)) },
 { CLT_InitFunc,   (uint32)libInit                    },
 { CLT_Interfaces, (uint32)libInterfaces              },
 { TAG_DONE,       0                                  }
};


/* ------------------- ROM Tag ------------------------ */
struct Resident USED lib_res =
{
 RTC_MATCHWORD,
 &lib_res,
 &lib_res+1,
 RTF_NATIVE|RTF_AUTOINIT, // Add RTF_COLDSTART if you want to be resident
 VERSION,
 NT_LIBRARY,              // Make this NT_DEVICE if needed
 0,                       // PRI, usually not needed unless you're resident
 LIBNAME,
 VSTRING,
 libCreateTags
};
