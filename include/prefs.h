#ifndef DOCKY_PREFS_H
#define DOCKY_PREFS_H

void DoMessage(char *, char, struct DockyData *);
void ReadInputPrefsKeymap(STRPTR);
BOOL scanfiles(CONST_STRPTR, struct DockyData *);
BOOL PrefsWindowNP(struct DockyIFace *, struct DockyData *);
#ifdef NEWPROC
int32 OpenPrefsWindow(CONST_STRPTR, LONG, struct ExecBase *);
#else
int32 OpenPrefsWindow(struct DockyData *);
#endif
BOOL ProcessPrefsWindow(struct DockyData *);
void SetKeyMapFlag(char *, char *);
void SaveToolType(struct DockyData *, STRPTR, STRPTR);
void FreeWindow(void);

extern void LoadImageFlag(struct DockyData *dd);
extern BOOL ChangeKeymap(struct DockyData *dd, STRPTR keymap);
extern void SignalDocky(struct DockyData *dd, uint32 flags);

extern struct ExecIFace *IExec;
extern struct KeymapIFace *IKeymap;
extern struct DOSIFace *IDOS;
extern struct UtilityIFace *IUtility;
extern struct IntuitionIFace *IIntuition;
extern struct IFFParseIFace *IIFFParse;
extern struct ListBrowserIFace *IListBrowser;
extern struct IconIFace *IIcon;
extern struct CommoditiesIFace *ICommodities;
extern struct GraphicsIFace *IGraphics;
extern struct DiskfontIFace *IDiskfont;

Object *Objects[LAST_NUM];
#define OBJ(x) Objects[x]
#define GAD(x) (struct Gadget *)Objects[x]

#endif
