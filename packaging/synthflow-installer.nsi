; SynthFlow Programming Language Installer
; NSIS Script

!define PRODUCT_NAME "SynthFlow"
!define PRODUCT_VERSION "0.0.25"
!define PRODUCT_PUBLISHER "SynthFlow Team"
!define PRODUCT_WEB_SITE "https://github.com/WEE-TECHNOLOGY-SOLUTION/SynthFlow"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\synthflow.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "..\releases\synthflow-${PRODUCT_VERSION}-installer.exe"
InstallDir "$PROGRAMFILES\SynthFlow"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\dist\synthflow-windows\bin\synthflow.exe"
  File "..\dist\synthflow-windows\bin\synthflow.bat"
  File "..\dist\synthflow-windows\install.bat"
  File "..\dist\synthflow-windows\uninstall.bat"
  File "..\dist\synthflow-windows\README.txt"
  File "..\dist\synthflow-windows\README.md"
  File "..\dist\synthflow-windows\LICENSE"
  File "..\dist\synthflow-windows\VERSION"
  
  SetOutPath "$INSTDIR\examples"
  File "..\dist\synthflow-windows\examples\hello.synth"
  File "..\dist\synthflow-windows\examples\fibonacci.sf"
  
  SetOutPath "$INSTDIR\docs"
  File "..\dist\synthflow-windows\docs\accessibility.md"
  File "..\dist\synthflow-windows\docs\ai.md"
  File "..\dist\synthflow-windows\docs\api-reference.md"
  File "..\dist\synthflow-windows\docs\architecture.md"
  File "..\dist\synthflow-windows\docs\ast.md"
  File "..\dist\synthflow-windows\docs\best-practices.md"
  File "..\dist\synthflow-windows\docs\break-continue-feature.md"
  File "..\dist\synthflow-windows\docs\build-system.md"
  File "..\dist\synthflow-windows\docs\case-studies.md"
  File "..\dist\synthflow-windows\docs\cli-reference.md"
  File "..\dist\synthflow-windows\docs\code-generation.md"
  File "..\dist\synthflow-windows\docs\code-of-conduct.md"
  File "..\dist\synthflow-windows\docs\codegen.md"
  File "..\dist\synthflow-windows\docs\community.md"
  File "..\dist\synthflow-windows\docs\compatibility.md"
  File "..\dist\synthflow-windows\docs\compiler-architecture.md"
  File "..\dist\synthflow-windows\docs\contributing.md"
  File "..\dist\synthflow-windows\docs\design-principles.md"
  File "..\dist\synthflow-windows\docs\development.md"
  File "..\dist\synthflow-windows\docs\education.md"
  File "..\dist\synthflow-windows\docs\faq.md"
  File "..\dist\synthflow-windows\docs\for-loop-feature.md"
  File "..\dist\synthflow-windows\docs\future-roadmap.md"
  File "..\dist\synthflow-windows\docs\glossary.md"
  File "..\dist\synthflow-windows\docs\governance.md"
  File "..\dist\synthflow-windows\docs\ide.md"
  File "..\dist\synthflow-windows\docs\internationalization.md"
  File "..\dist\synthflow-windows\docs\lexer.md"
  File "..\dist\synthflow-windows\docs\lsp.md"
  File "..\dist\synthflow-windows\docs\migration.md"
  File "..\dist\synthflow-windows\docs\parser.md"
  File "..\dist\synthflow-windows\docs\performance.md"
  File "..\dist\synthflow-windows\docs\privacy.md"
  File "..\dist\synthflow-windows\docs\project-overview.md"
  File "..\dist\synthflow-windows\docs\release-process.md"
  File "..\dist\synthflow-windows\docs\research.md"
  File "..\dist\synthflow-windows\docs\roadmap-detailed.md"
  File "..\dist\synthflow-windows\docs\roadmap.md"
  File "..\dist\synthflow-windows\docs\runtime.md"
  File "..\dist\synthflow-windows\docs\security.md"
  File "..\dist\synthflow-windows\docs\semantic-analyzer.md"
  File "..\dist\synthflow-windows\docs\semantic.md"
  File "..\dist\synthflow-windows\docs\stdlib.md"
  File "..\dist\synthflow-windows\docs\style-guide.md"
  File "..\dist\synthflow-windows\docs\sustainability.md"
  File "..\dist\synthflow-windows\docs\SYNTHFLOW_USAGE_GUIDE.md"
  File "..\dist\synthflow-windows\docs\SYNTHFLOW_WINDOWS_INSTALLATION.md"
  File "..\dist\synthflow-windows\docs\terms.md"
  File "..\dist\synthflow-windows\docs\testing.md"
  File "..\dist\synthflow-windows\docs\troubleshooting.md"
  File "..\dist\synthflow-windows\docs\while-loop-feature.md"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\SynthFlow\SynthFlow.lnk" "$INSTDIR\bin\synthflow.exe"
  CreateShortCut "$SMPROGRAMS\SynthFlow\SynthFlow Batch.lnk" "$INSTDIR\bin\synthflow.bat"
  CreateShortCut "$SMPROGRAMS\SynthFlow\Examples.lnk" "$INSTDIR\examples"
  CreateShortCut "$SMPROGRAMS\SynthFlow\Documentation.lnk" "$INSTDIR\docs"
  CreateShortCut "$SMPROGRAMS\SynthFlow\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\SynthFlow\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\bin\synthflow.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\bin\synthflow.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  
  ; Add to PATH
  ; EnVar::SetHKLM
  ; EnVar::AddValue "PATH" "$INSTDIR\bin"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Main SynthFlow programming language files"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

/******************************
 * Uninstaller Section        *
 ******************************/

Section "Uninstall"
  ; Remove from PATH
  ; EnVar::SetHKLM
  ; EnVar::DeleteValue "PATH" "$INSTDIR\bin"
  
  ; Remove registry keys
  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  
  ; Remove files and directories
  Delete "$INSTDIR\bin\synthflow.exe"
  Delete "$INSTDIR\bin\synthflow.bat"
  Delete "$INSTDIR\install.bat"
  Delete "$INSTDIR\uninstall.bat"
  Delete "$INSTDIR\README.txt"
  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\LICENSE"
  Delete "$INSTDIR\VERSION"
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  
  ; Remove examples
  Delete "$INSTDIR\examples\hello.synth"
  Delete "$INSTDIR\examples\fibonacci.sf"
  RMDir "$INSTDIR\examples"
  
  ; Remove docs
  Delete "$INSTDIR\docs\*.md"
  RMDir "$INSTDIR\docs"
  
  ; Remove shortcuts
  Delete "$SMPROGRAMS\SynthFlow\Uninstall.lnk"
  Delete "$SMPROGRAMS\SynthFlow\Website.lnk"
  Delete "$SMPROGRAMS\SynthFlow\Documentation.lnk"
  Delete "$SMPROGRAMS\SynthFlow\Examples.lnk"
  Delete "$SMPROGRAMS\SynthFlow\SynthFlow Batch.lnk"
  Delete "$SMPROGRAMS\SynthFlow\SynthFlow.lnk"
  
  ; Remove directories
  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR"
  
  ; Remove start menu directory
  RMDir "$SMPROGRAMS\SynthFlow"
SectionEnd