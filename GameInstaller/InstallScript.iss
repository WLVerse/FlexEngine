; TCR compliant Sample install script.
; Basically put this file in the root for what you want included in the installer,
;   then put everything into the subdirectories as listed in the installer below.
;   Remember to change the AppId
;   Thanks to Dan Weiss (dweiss@digipen.edu) for the original version.

[Setup]
; !!NOTE!!: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click "Tools" | "Create GUID" inside the Visual Studio IDE.)
AppId={{CE403582-3267-42D9-BA72-2607FC5F74C9}

; Standard app data stuff
; !!NOTE!!: Replace all instances of "GameTest" with your game name.
AppName=ChronoDrift
AppVerName=ChronoDrift Version 0.1
AppPublisher=DigiPen Institute of Technology
AppPublisherURL=http://www.digipen.edu/
AppSupportURL=http://www.digipen.edu/

; Default path to the file storage directory.
; {pf} is the default program files directory set by Windows
DefaultDirName={pf}\DigiPen\ChronoDrift

; Start menu directory
DefaultGroupName=DigiPen\ChronoDrift

; Output directory for the installer.
OutputDir=.\INSTALLER

; Setup executable installer
OutputBaseFilename=ChronoDrift_Setup

; Path to the DigiPen EULA (Needed to pass TCRs)
LicenseFile=INSTALLERFILES\DigiPen_EULA.txt

; Path to the icon for the installer (TCR check requires custom icon)
SetupIconFile=.\INSTALLERFILES\SetupIcon.ico

; Compression scheme for the installer. Check Inno Setup help files for more options.
Compression=lzma
SolidCompression=yes

; This allows the installer to run without admin privileges, which means you can't install
; in the Program Files, or change the registry. This is done to allow installation on Sandbox
; or other intermediate directory
PrivilegesRequired=none

[Dirs]
; Create directories in user/Documents folder
Name: "{userdocs}\ChronoDrift"

[UninstallDelete]
; Clean up the directory during uninstall
Type: filesandordirs; Name: "{userdocs}\ChronoDrift"

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
; Creates an installer option to allow/disallow desktop shortcut
; Checked by default
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

;
; !!NOTE!!: Your game may not require the VC or DX redistributable installers.
;	Update the [Files] and [Run] lists (below) as necessary for your game.
;

; This is the list of files that the installer should grab and install.
;
; Destination Directory Notes
;   {app} is the root directory that you will be installing to.
;   {temp} is a temporary directory that Windows sets that gets deleted after the
;      installer quits.
;   {userdocs} is the directory for My Documents/Documents on Windows XP, Vista, and 7.
;
; For more information on default paths to install to, check the "Constants" article
;   in the Inno Setup 5 documentation.
;
; I recommend placing any installers for required stuff (DirectX, PhysX, etc)
;   in the general structure below to keep things neat for you.
[Files]
; The game directory is exactly what you want your install directory in program files to look like
Source: .\GAMEDIRECTORY\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs
; Include the redistributable programs and install them to the temp directory
; Source: .\REDIST\vcredist_2008_x86.exe; DestDir: {tmp}; Flags: ignoreversion
Source: .\REDIST\VC_redist.x64.exe; DestDir: {tmp}; Flags: ignoreversion
; Source: .\REDIST\dxwebsetup.exe; DestDir: {tmp}; Flags: ignoreversion

; This is the list of shortcuts that the installer will setup for you.
; Of note, this will create the uninstaller automatically.
;
; Directory Notes
;   {group} is the start menu location that the game will install shortcuts to.
;   {commondesktop} is your Windows desktop directory.
[Icons]
Name: {group}\ChronoDrift; Filename: {app}\ChronoDrift.exe; WorkingDir: {app}; IconFilename: "{app}\Icon.ico"
Name: {group}\{cm:UninstallProgram,ChronoDrift}; Filename: {uninstallexe}
Name: {group}\{cm:UninstallProgram,ChronoDrift}; Filename: {uninstallexe};  IconFilename: "{app}\Icon.ico"
Name: {commondesktop}\ChronoDrift; Filename: {app}\ChronoDrift.exe; Tasks: desktopicon; WorkingDir: {app}; IconFilename: "{app}\Icon.ico"

; List of items to execute in the installer.
; Note that this will run all executables in their silent versions as required by the TCRs.
;
; The last item being run is the installer option to automatically launch the game after
;   the installer exits as required by the TCRs.
[Run]
;Filename: {tmp}\vc_redist.x86.exe; Parameters: /q; StatusMsg: Installing Visual C++ 2015 Redistributable...
Filename: {tmp}\VC_redist.x64.exe; Parameters: /q; StatusMsg: Installing Visual C++ 2022 Redistributable...
;Filename: {tmp}\dxwebsetup.exe; Parameters: /q; StatusMsg: Installing DirectX...
Filename: {app}\ChronoDrift.exe; Description: {cm:LaunchProgram,ChronoDrift}; Flags: nowait postinstall skipifsilent


;To be able to run and generate the GameTest_Setup.exe you need to download and install "Inno Setup" application, then open this file with the application.