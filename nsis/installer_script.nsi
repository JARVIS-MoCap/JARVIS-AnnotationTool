;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
  !include x64.nsh


;--------------------------------
;General

  ;Properly display all languages
  Unicode true

  ;Define name of the product
  !define PRODUCT "JARVIS AnnotationTool"

  ;Define optional URL that will be opened after the installation was successful
  !define AFTER_INSTALLATION_URL "https://github.com/AnonymerNiklasistanonym/NsiWindowsInstallerExamples"


  ;Define the main name of the installer
  Name "${PRODUCT}"

  ;Define the directory where the installer should be saved
  OutFile "${PRODUCT}_Installer.exe"


  ;Define the default installation folder (Windows ProgramFiles example)
  InstallDir "$PROGRAMFILES64\JARVIS\${PRODUCT}"

  ;Define optional a directory for program files that change (Windows AppData example)
  !define INSTDIR_DATA "$APPDATA\${PRODUCT}"


  ;Request rights if you want to install the program to program files
  RequestExecutionLevel admin

  ;Properly display all languages
  Unicode true

  ;Show 'console' in installer and uninstaller
  ShowInstDetails "show"
  ShowUninstDetails "show"

  ;Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\${PRODUCT}" ""


;--------------------------------
;Interface Settings

  ;Show warning if user wants to abort
  !define MUI_ABORTWARNING

  ;Show all languages, despite user's codepage
  !define MUI_LANGDLL_ALLLANGUAGES

  ;Use optional a custom icon:
  !define MUI_ICON "example_icon_installer.ico" # for the Installer
  !define MUI_UNICON "example_icon_uninstaller.ico" # for the later created UnInstaller

  ;Use optional a custom picture for the 'Welcome' and 'Finish' page:
  !define MUI_HEADERIMAGE_RIGHT
  !define MUI_WELCOMEFINISHPAGE_BITMAP "example_picture_installer.bmp"  # for the Installer
  !define MUI_UNWELCOMEFINISHPAGE_BITMAP "example_picture_uninstaller.bmp"  # for the later created UnInstaller

  ;Optional no descripton for all components
  !define MUI_COMPONENTSPAGE_NODESC


;--------------------------------
;Pages

  ;For the installer
  !insertmacro MUI_PAGE_WELCOME # simply remove this and other pages if you don't want it
  !insertmacro MUI_PAGE_LICENSE "..\LICENSE" # link to an ANSI encoded license file
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  ;For the uninstaller
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH


;--------------------------------
;Languages

  ;At start will be searched if the current system language is in this list,
  ;if not the first language in this list will be chosen as language
  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "SpanishInternational"
  !insertmacro MUI_LANGUAGE "SimpChinese"
  !insertmacro MUI_LANGUAGE "TradChinese"
  !insertmacro MUI_LANGUAGE "Japanese"
  !insertmacro MUI_LANGUAGE "Korean"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "Dutch"
  !insertmacro MUI_LANGUAGE "Danish"
  !insertmacro MUI_LANGUAGE "Swedish"
  !insertmacro MUI_LANGUAGE "Norwegian"
  !insertmacro MUI_LANGUAGE "NorwegianNynorsk"
  !insertmacro MUI_LANGUAGE "Finnish"
  !insertmacro MUI_LANGUAGE "Greek"
  !insertmacro MUI_LANGUAGE "Russian"
  !insertmacro MUI_LANGUAGE "Portuguese"
  !insertmacro MUI_LANGUAGE "PortugueseBR"
  !insertmacro MUI_LANGUAGE "Polish"
  !insertmacro MUI_LANGUAGE "Ukrainian"
  !insertmacro MUI_LANGUAGE "Czech"
  !insertmacro MUI_LANGUAGE "Slovak"
  !insertmacro MUI_LANGUAGE "Croatian"
  !insertmacro MUI_LANGUAGE "Bulgarian"
  !insertmacro MUI_LANGUAGE "Hungarian"
  !insertmacro MUI_LANGUAGE "Thai"
  !insertmacro MUI_LANGUAGE "Romanian"
  !insertmacro MUI_LANGUAGE "Latvian"
  !insertmacro MUI_LANGUAGE "Macedonian"
  !insertmacro MUI_LANGUAGE "Estonian"
  !insertmacro MUI_LANGUAGE "Turkish"
  !insertmacro MUI_LANGUAGE "Lithuanian"
  !insertmacro MUI_LANGUAGE "Slovenian"
  !insertmacro MUI_LANGUAGE "Serbian"
  !insertmacro MUI_LANGUAGE "SerbianLatin"
  !insertmacro MUI_LANGUAGE "Arabic"
  !insertmacro MUI_LANGUAGE "Farsi"
  !insertmacro MUI_LANGUAGE "Hebrew"
  !insertmacro MUI_LANGUAGE "Indonesian"
  !insertmacro MUI_LANGUAGE "Mongolian"
  !insertmacro MUI_LANGUAGE "Luxembourgish"
  !insertmacro MUI_LANGUAGE "Albanian"
  !insertmacro MUI_LANGUAGE "Breton"
  !insertmacro MUI_LANGUAGE "Belarusian"
  !insertmacro MUI_LANGUAGE "Icelandic"
  !insertmacro MUI_LANGUAGE "Malay"
  !insertmacro MUI_LANGUAGE "Bosnian"
  !insertmacro MUI_LANGUAGE "Kurdish"
  !insertmacro MUI_LANGUAGE "Irish"
  !insertmacro MUI_LANGUAGE "Uzbek"
  !insertmacro MUI_LANGUAGE "Galician"
  !insertmacro MUI_LANGUAGE "Afrikaans"
  !insertmacro MUI_LANGUAGE "Catalan"
  !insertmacro MUI_LANGUAGE "Esperanto"
  !insertmacro MUI_LANGUAGE "Asturian"
  !insertmacro MUI_LANGUAGE "Basque"
  !insertmacro MUI_LANGUAGE "Pashto"
  !insertmacro MUI_LANGUAGE "Georgian"
  !insertmacro MUI_LANGUAGE "Vietnamese"
  !insertmacro MUI_LANGUAGE "Welsh"
  !insertmacro MUI_LANGUAGE "Armenian"
  !insertmacro MUI_LANGUAGE "Corsican"


;--------------------------------
;Installer Section

Section "Main Component"
  SectionIn RO # Just means if in component mode this is locked

  ;Set output path to the installation directory.
  SetOutPath $INSTDIR

  ;Put the following file in the SetOutPath
  File "..\build\AnnotationTool.exe"
  File "..\libs\OpenCV\opencv_install\x64\vc17\bin\opencv_aruco460.dll"
  File "..\libs\OpenCV\opencv_install\x64\vc17\bin\opencv_calib3d460.dll"
  File "..\libs\OpenCV\opencv_install\x64\vc17\bin\opencv_core460.dll"
  File "..\libs\OpenCV\opencv_install\x64\vc17\bin\opencv_features2d460.dll"
  File "..\libs\OpenCV\opencv_install\x64\vc17\bin\opencv_flann460.dll"
  File "..\libs\OpenCV\opencv_install\x64\vc17\bin\opencv_gapi460.dll"
  File "..\libs\OpenCV\opencv_install\x64\vc17\bin\opencv_imgcodecs460.dll"
  File "..\libs\OpenCV\opencv_install\x64\vc17\bin\opencv_imgproc460.dll"
  File "..\libs\OpenCV\opencv_install\x64\vc17\bin\opencv_videoio_ffmpeg460_64.dll"
  File "..\libs\OpenCV\opencv_install\x64\vc17\bin\opencv_videoio460.dll"
  File "..\libs\Windows\dlls\libbz2-1__.dll"
  File "..\libs\Windows\dlls\libfreetype-6__.dll"
  File "..\libs\Windows\dlls\libgcc_s_dw2-1.dll"
  File "..\libs\Windows\dlls\libgcc_s_seh-1.dll"
  File "..\libs\Windows\dlls\libgraphite2__.dll"
  File "..\libs\Windows\dlls\libharfbuzz-0__.dll"
  File "..\libs\Windows\dlls\libjpeg-9__.dll"
  File "..\libs\Windows\dlls\liblzma-5__.dll"
  File "..\libs\Windows\dlls\libpng16-16__.dll"
  File "..\libs\Windows\dlls\libstdc++-6.dll"
  File "..\libs\Windows\dlls\libtiff-5__.dll"
  File "..\libs\Windows\dlls\libwebp-7__.dll"
  File "..\libs\Windows\dlls\libwebpdemux-2__.dll"
  File "..\libs\Windows\dlls\libwebpmux-3__.dll"
  File "..\libs\Windows\dlls\libwinpthread-1.dll"
  File "..\libs\Windows\dlls\zlib1__.dll"

  File /a /r "..\IconThemes"


  ;Store installation folder in registry
  WriteRegStr HKLM "Software\${PRODUCT}" "" $INSTDIR

  ;Registry information for add/remove programs
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "DisplayName" "${PRODUCT}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "UninstallString" '"$INSTDIR\${PRODUCT}_uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "NoRepair" 1

  ;Create optional start menu shortcut for uninstaller and Main component
  CreateDirectory "$SMPROGRAMS\${PRODUCT}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT}\AnnotationTool.lnk" "$INSTDIR\AnnotationTool.exe" "" "$INSTDIR\AnnotationTool.exe" 0
  CreateShortCut "$SMPROGRAMS\${PRODUCT}\Uninstall ${PRODUCT}.lnk" "$INSTDIR\${PRODUCT}_uninstall.exe" "" "$INSTDIR\${PRODUCT}_uninstall.exe" 0

  ;Create uninstaller
  WriteUninstaller "${PRODUCT}_uninstall.exe"

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;Remove all registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"
  DeleteRegKey HKLM "Software\${PRODUCT}"

  ;Delete the installation directory + all files in it
  ;Add 'RMDir /r "$INSTDIR\folder\*.*"' for every folder you have added additionaly
  RMDir /r "$INSTDIR\*.*"
  RMDir "$INSTDIR"

  ;Delete the appdata directory + files
  RMDir /r "${INSTDIR_DATA}\*.*"
  RMDir "${INSTDIR_DATA}"

  ;Delete Start Menu Shortcuts
  Delete "$SMPROGRAMS\${PRODUCT}\*.*"
  RmDir  "$SMPROGRAMS\${PRODUCT}"

SectionEnd


;--------------------------------
;After Installation Function
