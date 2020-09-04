# KeymapSwitcher
Docky for AmigaOS 4.1, is a plugin for the AmiDock toolbar system.


-AmigaOS 4.1 FE is a must!!!! Using V54 system/gfx functions.
-Tries to make easy for the user to change from default (Input prefs.) to a
secondary keymap choosed by the user (KEYMAP2 tooltype).
-Settings window to change KEYMAP2 tooltype and HOTKEY; click on 'Save' button
to "fix" them in tooltypes.
-Commodity support; use HOTKEY to swap keymaps.



Installation:
1. Two modes/options:
  a)Shell/CLI: COPY KeymapSwitcher.docky(|.info) SYS:Utilities/Dockies/ CLONE
  b)Workbench: drag&drop KeymapSwitcher.docky icon in SYS:Utilities/Dockies/
  NOTE: Your "original" icon and tooltypes will be overwritten!!!
2. Add docky to AmiDock toolbar system.
NOTE: If you want dockies on your screen titlebar see HowTo_make_titlebar.html



Settings:
Docky settings can be changed by modifying the icon's tooltypes.
Tooltypes used by the docky:
-DOCKY_1CLICK=YES|NO, pop-ups the chooser Window/Menu on single click.
-KEYMAP2=<keymap>, secondary keymap to use/swap (from KEYMAPS:).
-MODE=WIN, use a chooser window instead of the default menu popup.
-KEYMAP3=<keymap>, third keymap to use/swap (only via tooltypes).
-HOTKEY=<sequence>, change keymap with <sequence> (default: ALT ESC).
-ALT_MENUNAMES, show 'Description' instead of 'Keymap' in chooser Menu. (WIP)
-DOCKY_HEIGHT=<n>, height in pixels of docky flag image (minimum value: 9)



Usage:
-Once you have KeymapSwitcher on AmiDock it will show as image/docky your
current/default keymap.
-Use RMB to open a popup menu with some options:
REMOVE: quits KeymapSwitcher (restores Input Prefs. keymap)
MODE  > MENU : chooser is a popup menu with 2 options (and SETTINGS...)
        WINDOW: chooser is a window with 2 buttons
SETTINGS... (only on WINDOW mode): opens KeymapSwitcher settings
-Use LMB (single/double click) to open a MENU/WINDOW chooser that shows
your main keymap (Input Prefs.), a secondary keymap (KEYMAP2 tooltype) and
in case you enabled KEYMAP3 tooltype a third keymap, click on option/button
to change keymap or RMB to close MENU/WINDOW chooser.
-Choosing a keymap will change the image/docky to the new keymap.



Notes:
CHANGE MAIN KEYMAP
-Launch Input preferences (or use 'Modify...' button in KMS settings) and
re-run KMS to reflect such change.
HOTKEYS
-Avoid usage of ALT+SHIFT+key as hotkey/combo. Example: ALT+SHIFT+k
 produces unwanted side effects. Use CTRL+SHIFT instead of ALT+SHIFT
-'K' (uppercase) is not the same as 'k' (lowercase) keystroke!!!
KINGCON (console replacement KCON)
-Seems have some key typing problems after swapping keymap.



Compiling sources:
Latest SDK & "make"



Thanks to (in no special order):
jd, kas1e, salas00, trixie, zzd10h and everyone I forget (sorry).
Everyone at os4coding and HP forum!



Translations:
Dutch & French: Joseph Duchâtelet (jd)
Greek: Anthony Iliakis (phantom)
Italian: Samir Hawamdeh (samo79)
Russian: Mikhail Malyshev
Spanish: Javier de las Rivas


