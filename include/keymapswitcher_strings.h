#ifndef KEYMAPSWITCHER_STRINGS_H
#define KEYMAPSWITCHER_STRINGS_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_CODE
#ifndef CATCOMP_BLOCK
#define CATCOMP_ARRAY
#endif
#endif

#ifdef CATCOMP_ARRAY
#ifndef CATCOMP_NUMBERS
#define CATCOMP_NUMBERS
#endif
#ifndef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif
#endif

#ifdef CATCOMP_BLOCK
#ifndef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_DOCKY_MODE 1
#define MSG_DOCKY_POPUP 2
#define MSG_DOCKY_WINDOW 3
#define MSG_DOCKY_SETTINGS 4
#define MSG_OK_GAD 5
#define MSG_ERROR_NO_INPUTPREFS 6
#define MSG_ERROR_SCANNING 7
#define MSG_ERROR_NOKEYMAP2 8
#define MSG_ERROR_KEYMAP 9
#define MSG_ERROR_CANTCHANGE 10
#define MSG_PREFS_WINDOW 11
#define MSG_PREFS_TITLE_KEYMAP 12
#define MSG_PREFS_TITLE_DESC 13
#define MSG_PREFS_GROUP1 14
#define MSG_PREFS_GROUP2 15
#define MSG_PREFS_BTN_SAVE 16
#define MSG_PREFS_BTN_USE 17
#define MSG_PREFS_BTN_CANCEL 18
#define MSG_COMMODITY_DESC 19
#define MSG_COMMODITY_HK1 20
#define MSG_COMMODITY_HK1_INVALID 21
#define MSG_PREFS_GROUP3 22
#define MSG_PREFS_INPUTPREFS 23
#define MSG_PREFS_NOTE 24
#define MSG_KM2_USING 25
#define MSG_CMDLINE_CHANGING 1000
#define MSG_CMDLINE_NEWKEYMAP 1001
#define MSG_CMDLINE_FAILED 1002

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_DOCKY_MODE_STR "Mode"
#define MSG_DOCKY_POPUP_STR "Menu"
#define MSG_DOCKY_WINDOW_STR "Window"
#define MSG_DOCKY_SETTINGS_STR "Settings..."
#define MSG_OK_GAD_STR "OK"
#define MSG_ERROR_NO_INPUTPREFS_STR "'input.prefs' not found, using default keymap."
#define MSG_ERROR_SCANNING_STR "Error scanning 'KEYMAPS:'"
#define MSG_ERROR_NOKEYMAP2_STR "KEYMAP2 tooltype not defined!"
#define MSG_ERROR_KEYMAP_STR "Keymap '%s' doesn't exists!"
#define MSG_ERROR_CANTCHANGE_STR "Failed to set '%s' keymap!"
#define MSG_PREFS_WINDOW_STR "KeymapSwitcher: Settings"
#define MSG_PREFS_TITLE_KEYMAP_STR " Keymap"
#define MSG_PREFS_TITLE_DESC_STR " Description"
#define MSG_PREFS_GROUP1_STR "Main Keymap (Input Prefs.)"
#define MSG_PREFS_GROUP2_STR "Secondary Keymap (KEYMAP2)"
#define MSG_PREFS_BTN_SAVE_STR "_Save"
#define MSG_PREFS_BTN_USE_STR "_Use"
#define MSG_PREFS_BTN_CANCEL_STR "_Cancel"
#define MSG_COMMODITY_DESC_STR "Swap keymap in an easy way"
#define MSG_COMMODITY_HK1_STR "Hotkey:"
#define MSG_COMMODITY_HK1_INVALID_STR "Invalid hotkey."
#define MSG_PREFS_GROUP3_STR "Swap Keymaps"
#define MSG_PREFS_INPUTPREFS_STR "_Modify..."
#define MSG_PREFS_NOTE_STR "(re-run KMS to reflect this change) \0"
#define MSG_KM2_USING_STR "KEYMAP2 set to"
#define MSG_CMDLINE_CHANGING_STR "Changing keymap to '%s'...\n"
#define MSG_CMDLINE_NEWKEYMAP_STR "New keymap: (%s) %s | %s\n"
#define MSG_CMDLINE_FAILED_STR "Failed to set '%s' keymap!\n"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG         cca_ID;
    CONST_STRPTR cca_Str;
};

STATIC CONST struct CatCompArrayType CatCompArray[] =
{
    {MSG_DOCKY_MODE,(CONST_STRPTR)MSG_DOCKY_MODE_STR},
    {MSG_DOCKY_POPUP,(CONST_STRPTR)MSG_DOCKY_POPUP_STR},
    {MSG_DOCKY_WINDOW,(CONST_STRPTR)MSG_DOCKY_WINDOW_STR},
    {MSG_DOCKY_SETTINGS,(CONST_STRPTR)MSG_DOCKY_SETTINGS_STR},
    {MSG_OK_GAD,(CONST_STRPTR)MSG_OK_GAD_STR},
    {MSG_ERROR_NO_INPUTPREFS,(CONST_STRPTR)MSG_ERROR_NO_INPUTPREFS_STR},
    {MSG_ERROR_SCANNING,(CONST_STRPTR)MSG_ERROR_SCANNING_STR},
    {MSG_ERROR_NOKEYMAP2,(CONST_STRPTR)MSG_ERROR_NOKEYMAP2_STR},
    {MSG_ERROR_KEYMAP,(CONST_STRPTR)MSG_ERROR_KEYMAP_STR},
    {MSG_ERROR_CANTCHANGE,(CONST_STRPTR)MSG_ERROR_CANTCHANGE_STR},
    {MSG_PREFS_WINDOW,(CONST_STRPTR)MSG_PREFS_WINDOW_STR},
    {MSG_PREFS_TITLE_KEYMAP,(CONST_STRPTR)MSG_PREFS_TITLE_KEYMAP_STR},
    {MSG_PREFS_TITLE_DESC,(CONST_STRPTR)MSG_PREFS_TITLE_DESC_STR},
    {MSG_PREFS_GROUP1,(CONST_STRPTR)MSG_PREFS_GROUP1_STR},
    {MSG_PREFS_GROUP2,(CONST_STRPTR)MSG_PREFS_GROUP2_STR},
    {MSG_PREFS_BTN_SAVE,(CONST_STRPTR)MSG_PREFS_BTN_SAVE_STR},
    {MSG_PREFS_BTN_USE,(CONST_STRPTR)MSG_PREFS_BTN_USE_STR},
    {MSG_PREFS_BTN_CANCEL,(CONST_STRPTR)MSG_PREFS_BTN_CANCEL_STR},
    {MSG_COMMODITY_DESC,(CONST_STRPTR)MSG_COMMODITY_DESC_STR},
    {MSG_COMMODITY_HK1,(CONST_STRPTR)MSG_COMMODITY_HK1_STR},
    {MSG_COMMODITY_HK1_INVALID,(CONST_STRPTR)MSG_COMMODITY_HK1_INVALID_STR},
    {MSG_PREFS_GROUP3,(CONST_STRPTR)MSG_PREFS_GROUP3_STR},
    {MSG_PREFS_INPUTPREFS,(CONST_STRPTR)MSG_PREFS_INPUTPREFS_STR},
    {MSG_PREFS_NOTE,(CONST_STRPTR)MSG_PREFS_NOTE_STR},
    {MSG_KM2_USING,(CONST_STRPTR)MSG_KM2_USING_STR},
    {MSG_CMDLINE_CHANGING,(CONST_STRPTR)MSG_CMDLINE_CHANGING_STR},
    {MSG_CMDLINE_NEWKEYMAP,(CONST_STRPTR)MSG_CMDLINE_NEWKEYMAP_STR},
    {MSG_CMDLINE_FAILED,(CONST_STRPTR)MSG_CMDLINE_FAILED_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

STATIC CONST UBYTE CatCompBlock[] =
{
    "\x00\x00\x00\x01\x00\x06"
    MSG_DOCKY_MODE_STR "\x00\x00"
    "\x00\x00\x00\x02\x00\x06"
    MSG_DOCKY_POPUP_STR "\x00\x00"
    "\x00\x00\x00\x03\x00\x08"
    MSG_DOCKY_WINDOW_STR "\x00\x00"
    "\x00\x00\x00\x04\x00\x0C"
    MSG_DOCKY_SETTINGS_STR "\x00"
    "\x00\x00\x00\x05\x00\x04"
    MSG_OK_GAD_STR "\x00\x00"
    "\x00\x00\x00\x06\x00\x30"
    MSG_ERROR_NO_INPUTPREFS_STR "\x00\x00"
    "\x00\x00\x00\x07\x00\x1A"
    MSG_ERROR_SCANNING_STR "\x00"
    "\x00\x00\x00\x08\x00\x1E"
    MSG_ERROR_NOKEYMAP2_STR "\x00"
    "\x00\x00\x00\x09\x00\x1C"
    MSG_ERROR_KEYMAP_STR "\x00"
    "\x00\x00\x00\x0A\x00\x1C"
    MSG_ERROR_CANTCHANGE_STR "\x00\x00"
    "\x00\x00\x00\x0B\x00\x1A"
    MSG_PREFS_WINDOW_STR "\x00\x00"
    "\x00\x00\x00\x0C\x00\x08"
    MSG_PREFS_TITLE_KEYMAP_STR "\x00"
    "\x00\x00\x00\x0D\x00\x0E"
    MSG_PREFS_TITLE_DESC_STR "\x00\x00"
    "\x00\x00\x00\x0E\x00\x1C"
    MSG_PREFS_GROUP1_STR "\x00\x00"
    "\x00\x00\x00\x0F\x00\x1C"
    MSG_PREFS_GROUP2_STR "\x00\x00"
    "\x00\x00\x00\x10\x00\x06"
    MSG_PREFS_BTN_SAVE_STR "\x00"
    "\x00\x00\x00\x11\x00\x06"
    MSG_PREFS_BTN_USE_STR "\x00\x00"
    "\x00\x00\x00\x12\x00\x08"
    MSG_PREFS_BTN_CANCEL_STR "\x00"
    "\x00\x00\x00\x13\x00\x1C"
    MSG_COMMODITY_DESC_STR "\x00\x00"
    "\x00\x00\x00\x14\x00\x08"
    MSG_COMMODITY_HK1_STR "\x00"
    "\x00\x00\x00\x15\x00\x10"
    MSG_COMMODITY_HK1_INVALID_STR "\x00"
    "\x00\x00\x00\x16\x00\x0E"
    MSG_PREFS_GROUP3_STR "\x00\x00"
    "\x00\x00\x00\x17\x00\x0C"
    MSG_PREFS_INPUTPREFS_STR "\x00\x00"
    "\x00\x00\x00\x18\x00\x26"
    MSG_PREFS_NOTE_STR "\x00"
    "\x00\x00\x00\x19\x00\x10"
    MSG_KM2_USING_STR "\x00\x00"
    "\x00\x00\x03\xE8\x00\x1C"
    MSG_CMDLINE_CHANGING_STR "\x00"
    "\x00\x00\x03\xE9\x00\x1A"
    MSG_CMDLINE_NEWKEYMAP_STR "\x00"
    "\x00\x00\x03\xEA\x00\x1C"
    MSG_CMDLINE_FAILED_STR "\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


#ifndef PROTO_LOCALE_H
 #ifndef __NOLIBBASE__
  #define _NLB_DEFINED_
  #define __NOLIBBASE__
 #endif
 #ifndef __NOGLOBALIFACE__
  #define _NGI_DEFINED_
  #define __NOGLOBALIFACE__
 #endif
 #include <proto/locale.h>
 #ifdef _NLB_DEFINED_
  #undef __NOLIBBASE__
  #undef _NLB_DEFINED_
 #endif
 #ifdef _NGI_DEFINED_
  #undef __NOGLOBALIFACE__
  #undef _NGI_DEFINED_
 #endif
#endif

struct LocaleInfo
{
#ifndef __amigaos4__
    struct Library     *li_LocaleBase;
#else
    struct LocaleIFace *li_ILocale;
#endif
    struct Catalog     *li_Catalog;
};


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CONST_STRPTR GetString(struct LocaleInfo *li, LONG stringNum);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#ifdef CATCOMP_CODE

CONST_STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
#ifndef __amigaos4__
    struct Library     *LocaleBase = li->li_LocaleBase;
#else
    struct LocaleIFace *ILocale    = li->li_ILocale;
#endif
    LONG         *l;
    UWORD        *w;
    CONST_STRPTR  builtIn = NULL;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum && l < (LONG *)(&CatCompBlock[sizeof(CatCompBlock)]))
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    if (*l == stringNum)
    {
        builtIn = (CONST_STRPTR)((ULONG)l + 6);
    }

#ifndef __amigaos4__
    if (LocaleBase)
    {
        return GetCatalogStr(li->li_Catalog, stringNum, builtIn);
    }
#else
    if (ILocale)
    {
#ifdef __USE_INLINE__
        return GetCatalogStr(li->li_Catalog, stringNum, builtIn);
#else
        return ILocale->GetCatalogStr(li->li_Catalog, stringNum, builtIn);
#endif
    }
#endif
    return builtIn;
}


#endif /* CATCOMP_CODE */


/****************************************************************************/


#endif /* KEYMAPSWITCHER_STRINGS_H */
