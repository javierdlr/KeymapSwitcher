#ifndef DOCKY_WIN_H
#define DOCKY_WIN_H

#include <classes/window.h>
#include <gadgets/slider.h>
#include <gadgets/layout.h>
#include <gadgets/button.h>
#include <images/bitmap.h>
#include <classes/requester.h>

#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
//#define CATCOMP_CODE
#include "include/keymapswitcher_strings.h"
extern struct LocaleInfo li;
extern char textbuf[150]; // for localized messages

extern struct UtilityIFace *IUtility;
extern struct DOSIFace *IDOS;

void OpenDockMenu(struct DockyIFace *Self);

void OpenDockWindow(struct DockyData *dd);
void FreeDockWindow(void);

BOOL ProcessDockWindow(struct DockyData *dd);
BOOL ChangeKeymap(struct DockyData *dd, STRPTR keymap);

#endif
