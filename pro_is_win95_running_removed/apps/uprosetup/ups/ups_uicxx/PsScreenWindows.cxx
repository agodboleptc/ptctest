/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*\

  PsScreenWindows.cxx

  Pro/SETUP functions

  Date      Release   Name  Ver.   Comments
  --------- -------   ----  ----   --------
  04-Mar-03 K-01-06   TWH   $$1    Created from LpcfgWindows
  26-Jun-03 K-01-10   Chris $$2    Removed char ** casts from relmem
  06-Nov-03 K-01-18   MAZ   $$3    SPR 1059473: fix _ShortCutFolder updates
  14-Jul-06 L-01-12   ksi   $$4    Unicode changes
  23-Sep-08 L-03-18   TWH   $$5    Add mShortCheckQuickLaunch and properties
                                   to hide UI elements
  08-Oct-08 L-03-18+  TWH   $$6    Fix hide UI elements
  19-Feb-09 L-03-28   TWH   $$7    ExpandEnvVar for StartInIP
  09-Mar-10 L-05-18   TWH   $$8    Add StartInWarning
  04-Mar-11 L-05-42   TWH   $$9    Improve ShortcutFolder support
  11-May-11 L-05-47   TWH   $$10   Fix initialization of checkboxes

\*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#include <Ps.h>

ScreenWindows::ScreenWindows() :
	PsLayout("ScreenWindows"),
	PsScreen("ScreenWindows"),
	LocLabel("LocationLab"),
	mShortCheckDesktop("ScDesktop"),
	mShortCheckStartMenu("ScStartMenu"),
	mShortCheckProgramFolder("ScProgramFolder"),
	mShortCheckQuickLaunch("ScQuickLaunch"),
	FolderLabel("FolderLab"),
	_ShortCutFolder("ShortcutFolder"),
	StartInLabel("StartInDirLab"),
	StartInWarning("StartinDirWarning"),
	_StartInIP("StartInDir"),
	mStartInBrowse("StartInDirBrowse"),
	PathChoice("PathChoice"),
	TA("TextArea"),
	EnvLayout("PathLayout")
{
	AddComponent(LocLabel);
	AddComponent(mShortCheckDesktop);
	AddComponent(mShortCheckStartMenu);
	AddComponent(mShortCheckProgramFolder);
	AddComponent(mShortCheckQuickLaunch);
	AddComponent(FolderLabel);
	AddComponent(_ShortCutFolder);
	AddComponent(StartInLabel);
	AddComponent(StartInWarning);
	AddComponent(_StartInIP);
	AddComponent(mStartInBrowse);
	AddComponent(PathChoice);
	AddComponent(TA);
	AddComponent(EnvLayout);
	Desktop_Type_exist = FALSE;
	StartMenu_Type_exist = FALSE;
	ProgGrp_Type_exist = FALSE;
	No_shortcut_Type_exist = FALSE;
	setDefaults = TRUE;
	CanWriteAdminKeys = TRUE;
}

ScreenWindows::~ScreenWindows()
{
}

int ScreenWindows::InitFunc()
{
# if OPER_SYS == WINDOWS_32
	btkString shortcutsPath;

	// populate the OptionMenu inside InitFunc() not in PreFunc

	ScreenName = TranslateWcharMsg(SN_Lpcfg_Windows,
		"Windows Preferences");
	// it is NT
	// if NT try to put the shortcuts for all users if you have permissions
	shortcutsPath = psRegkeyGetStringFromValue("", (mach_root + key_path), "Common Programs");
	// check if we have write access to this dir
	if( !psOSAccessWrite(shortcutsPath))
		shortcutsPath = psRegkeyGetStringFromValue("", (user_root + key_path), "Programs");

	// for the path settings->
	// check if the user is admin? if not disable radio group
	if (!psRegkeyCanWriteAdminKeys(NULL))
	{
		CanWriteAdminKeys = FALSE; // save this value for later use

		// check the user settings radio button
		PathChoice.Select("userpath");
		PathChoice.SetSensitive(FALSE);
	}

	btkFSEntry path(shortcutsPath);
	_ShortCutFolder.Clear();
	if (! path.IsDirectory())
	{
		// can't continue since path to shortcuts dir is not valid
		PsWarning << "shortcutsPath failed: " << path << endl << endm;
		return 0;
	}
	// now go and get the sub dirs which are the prog grps in the start menu
	btkFSList DirList(path);
	btkString tmp;
	int DirSize = DirList.GetSize();
	PsWarning << "DirList.GetSize() = " << DirSize << endl << endm;
	for (int i=0; i < DirSize; i++)
	{
		btkFSEntry entity = DirList[i];
		if (entity.IsDirectory())
		{
			tmp = (cStringT)entity.GetTail();
			_ShortCutFolder.Append(tmp, tmp);
		}
	}
	DirSize = _ShortCutFolder.GetSize();
	if (DirSize == 0)
		return 0;
	_ShortCutFolder.Refresh();
	
#endif	  // end of #if WINDOWS
	return 0;
}

int ScreenWindows::SkipScreen()
{
#if OPER_SYS == WINDOWS_32
	return (PsScreen::SkipScreen());
#else
	return (TRUE);
#endif
}

int ScreenWindows::PreFunc(ScreenDir Dir)
{
	btkString text;
	wchar_t *gen_text;
	int tf = TRUE;

	text = (cStringT)*(GetCurrentInstallable()->GetTag());
	if (text == "student")
	{
		gen_text = ConvertWcharMsg(ST_Lpcfg_Windows_SE,
		"Enter Windows Configuration information");
	}
	else
	{
		gen_text = ConvertWcharMsg(ST_Lpcfg_Windows,
		"Enter Windows Configuration information");
	}
	text = gen_text;
	TA.SetText(text);
	relmem(&gen_text);
# if OPER_SYS == WINDOWS_32

	if (GetCurrentInstallable()->Prop.GetInt("ScreenWindows.HideEnv") == 1)
		tf = FALSE;
	else
		tf = TRUE;
	EnvLayout.SetVisible(tf);

	if (GetCurrentInstallable()->Prop.GetInt("ScreenWindows.HideStartDir") == 1)
		tf = FALSE;
	else
		tf = TRUE;
	StartInLabel.SetVisible(tf);
	StartInWarning.SetVisible(tf);
	_StartInIP.SetVisible(tf);
	mStartInBrowse.SetVisible(tf);

	if (GetCurrentInstallable()->Prop.GetInt("ScreenWindows.HideProgFolder") == 1)
		tf = FALSE;
	else
		tf = TRUE;
	FolderLabel.SetVisible(tf);
	_ShortCutFolder.SetVisible(tf);
	mShortCheckProgramFolder.SetVisible(tf);

	if (GetCurrentInstallable()->Prop.GetInt("ScreenWindows.ShowQuickLaunch") == 1)
		tf = TRUE;
	else
		tf = FALSE;
	mShortCheckQuickLaunch.SetVisible(tf);

	btkFSEntry instlog_path("");
	PsGetInstLogPath(instlog_path);
	btkString install_logfile(instlog_path);

	if (install_logfile !=  stored_logfile)
	{
		stored_logfile = install_logfile;  // update the stored value
		btkString newProgGrp;

		if (GetCurrentInstallable()->Prop("ShortcutFolder") != btkString(""))
		{
			newProgGrp = (cStringT)GetCurrentInstallable()->Prop("ShortcutFolder");
			newProgGrp.Substitute("/", "\\");
			newProgGrp.Substitute("*", " ");
			newProgGrp.Substitute("|", " ");
		}
		else
		{
			newProgGrp = (cStringT)GetCurrentInstallable()->GetName();

			// Pro/Engineer will become "Pro Engineer"
			newProgGrp.Substitute("/", " ");
			newProgGrp.Substitute("*", " ");
			newProgGrp.Substitute("|", " ");

			// SPR 948721: use backslashes in prg folders
			newProgGrp = "PTC\\" + newProgGrp;
		}
		
		// Now we will read the inst_log_file if it exists.
		// we will do this under the static var  so we only read the file once
		// per product installation

		StringXArray actionsDataArray;
		bool retFlag = PsGetUninstActionsByType(PUN_SHORTCUT_TYPE,
					&install_logfile, &actionsDataArray);
		if ((!retFlag) || (actionsDataArray.GetSize() == 0))
		{
			// if PsGetUninstActionsByType() fails or
			// GetSize is zero, use the default
			//
			PsWarning << "ERROR or no SHORTCUT_TYPE found" << endl << endm;
			set_wintab_defaults();
		}	  
		else 
		{
			// old prog grps entries were found so we use those in the selection
			btkString shortcut_type;

			for (int ii=0; ii < actionsDataArray.GetSize(); ii++)
			{
				// select from the list one at time
				// in I-02 we used the following keywords refering to shortcut_types
				// No_shortcut_Type = 5, Desktop = 1, start_menu = 2, prog_grp 3&4
				shortcut_type = actionsDataArray[ii];
				if (shortcut_type == No_shortcut_Type || shortcut_type == "5")
				{
					No_shortcut_Type_exist = TRUE;
					setDefaults = FALSE;
					// no point to continue here so break the for-loop
					break;
				}
				if (shortcut_type == Desktop_Type || shortcut_type == "1")
				{
					setDefaults = FALSE;
					Desktop_Type_exist = TRUE;
				}
				else if (shortcut_type == StartMenu_Type || shortcut_type == "2")
				{
					setDefaults = FALSE;
					StartMenu_Type_exist = TRUE;
				}
				else if (shortcut_type == ProgGrp_Type ||
					shortcut_type == "3" || shortcut_type == "4")
				{
					setDefaults = FALSE;
					ProgGrp_Type_exist = TRUE;
				}
			}

			if (setDefaults)
			{
				// call set_wintab_defaults() if we couldn't determine previous changes
				//
				PsWarning << "Calling LpcfgWin::set_wintab_defaults to setup defaults"
					<< endl << endm;
				set_wintab_defaults();
			}
			else
			{
				// now we are done with the for-loop, so we set the tab accordingly
				if (No_shortcut_Type_exist)
				{
					// uncheck all check boxes
					mShortCheckDesktop.Check(FALSE);
					mShortCheckStartMenu.Check(FALSE);
					mShortCheckProgramFolder.Check(FALSE);
					_ShortCutFolder.SetSensitive(FALSE);
					
					// reset the other Types just in case
					Desktop_Type_exist = FALSE;
					StartMenu_Type_exist = FALSE;
					ProgGrp_Type_exist = FALSE;
				}
				if (Desktop_Type_exist)
					mShortCheckDesktop.Check(TRUE);
				else
					mShortCheckDesktop.Check(FALSE);

				if (StartMenu_Type_exist)
					mShortCheckStartMenu.Check(TRUE);
				else
					mShortCheckStartMenu.Check(FALSE);
	
				if (ProgGrp_Type_exist)
				{
					mShortCheckProgramFolder.Check(TRUE);
					_ShortCutFolder.SetSensitive(TRUE);

					// a prog grp was created last time so we have to get its name
					// this will store just the prog_grp name
					//
					StringXArray prog_grp_name;
					// PUN_SHORTCUT_LOC stores the name of the prog_grp
					//	  not the whole path
					retFlag = PsGetUninstActionsByType(PUN_SHORTCUT_LOC,
						&install_logfile, &prog_grp_name);
					if ((!retFlag) || (prog_grp_name.GetSize() == 0))
					{
						// Error Reading the file or no entries are recorded!
						// if so default the program grp selection to newProgGrp
						text = (cStringT)newProgGrp;
					}
					else
					{   // only one entry is expected here
						text = (cStringT)prog_grp_name[0];
						// display saved_prog_grp in the prog_grp optionMenu
						// even if it is not actually in the list,
					}

					// SPR 1059473: fix _ShortCutFolder updates
					if (_ShortCutFolder.Find(text) == -1)
					{
						PsWarning << "Append to _ShortCutFolder: " << text <<
							endl << endm;
						_ShortCutFolder.Append(text, text);
						_ShortCutFolder.Refresh();
					}
					_ShortCutFolder.Select(text);
				}
				else
				{   // uncheck the check box and disable the option menu
					mShortCheckProgramFolder.Check(FALSE);
					_ShortCutFolder.SetSensitive(FALSE);
				}
			}
		}  
		// It is time to setup the starting up dir values
		StringXArray startup_array;
		retFlag = PsGetUninstActionsByType(PUN_STARTUP_DIR,
				&install_logfile,&startup_array);
		btkString startupdir;
		if ((!retFlag) || (startup_array.GetSize() == 0))
		{
			// Error Reading the file or no entries are recorded! if so
			// default the input field to PersonalFolder
			PsGetPersonalFolder(startupdir);
		}
		else
		{
			// set input to startup_array[0]
			//	  since only one entry is expected in the file
			startupdir = startup_array[0];
		}
		_StartInIP.SetText(startupdir);

		// Set the status of the StartInDir* comoponents
		set_StartInDir_status();

		// Last thing to initialize is the path settings selection section
		StringXArray path_array;
		retFlag = PsGetUninstActionsByType(PUN_PATH_SETTING,
				&install_logfile, &path_array);
		if ((retFlag) && (path_array.GetSize() > 0))
		{
			// set to user level settings if previously set as USER
			// one entry of PUN_PATH_SETTING is expected
			if (path_array[0] == "USER")
			{
				PathChoice.Select("userpath");
				// check the user settings radio button
			}
		}
	}

#endif // end-of if (OPER_SYS == WINDOWS_32)
	return 0;
}

int ScreenWindows::PostFunc(ScreenDir Dir)
{
	int retval = TRUE;
#if (OPER_SYS == WINDOWS_32)
	// if all is fine populate the win_tab info class with info
	LpcfgWinInfo *tabInfo= XNew LpcfgWinInfo();
	btkString tmp;
	int check_flag;

	tabInfo->SetDesktopOn(mShortCheckDesktop.IsChecked());
	tabInfo->SetStartMenuOn(mShortCheckStartMenu.IsChecked());
	check_flag = mShortCheckProgramFolder.IsChecked();
	tabInfo->SetProgGrpOn(check_flag);

	if (check_flag)
	{
		_ShortCutFolder.GetText(tmp);
		if (tmp.IsEmpty()) // *******  if it is an empty string error
			PsError << "ERROR: invalid selection! " << endl << endm;
		else
			tabInfo->SetProgGrp(tmp);
	}

	// then it is NT, now check if admin
	if (CanWriteAdminKeys)
	{
		PsWarning << "psRegkeyCanWriteAdminKeys returned TRUE"
			<< endl << endm;
		// check the radio buttons and set the class
		// member data with the info
		PathChoice.GetSelected(tmp);
		if (tmp == "userpath")
			check_flag =1;
		else
			check_flag =0;
	}
	else
		check_flag = 1; // if not admin, set the path for current user only
	tabInfo->SetUserpathOn(check_flag);

	// populate the StartupDir value in the info class
	_StartInIP.GetText(tmp);
	
	if (!PsExpandEnvVar(tmp))
	{
		PsError << "ERROR: invalid selection! " << endl << endm;
	}

	// SPR 939346: prompt to create start-in dir
	if (PsScreen::PostFunc(Dir))
	{
		tabInfo->SetStartupDir(tmp);
	}
	else
		retval = FALSE;

	tabInfo->Print();
	// now store this class inside the PsProduct class
	PsProduct *I = GetCurrentInstallable();
	I->SetWinTabInfo(tabInfo);
#endif // end-of if (OPER_SYS == WINDOWS_32)
	return(retval);
}

void ScreenWindows::PsLaunchStartBrowser()
{
	PsBrowseInfo *info;
	info = (PsBrowseInfo *)getmem( sizeof( PsBrowseInfo ) );

	info->Device = _ps_strdup(PS);
	info->Button = _ps_strdup((char *)mStartInBrowse.GetId());
	info->InputPanel = _ps_strdup((char *)_StartInIP.GetId());
	info->Type = PS_BROWSE_DIR;
	info->Msg = NULL;

	int status = PsBrowse(info);

	// PsBrowse() will return UI_SUCCESS if OK was clicked
	// and will return UI_ERROR if CANCEL was clicked
	//
	if (status == UI_SUCCESS)
	{
	}

	// remember to cleanup the mess when done!
	relmem ( &info->Device);
	relmem ( &info->Button);
	relmem ( &info->InputPanel);
	relmem (&info );
}

void ScreenWindows::set_StartInDir_status()
{
	int check_flag = FALSE;

	check_flag = mShortCheckDesktop.IsChecked();
	if (!check_flag)
		check_flag = mShortCheckStartMenu.IsChecked();
	if (!check_flag)
		check_flag = mShortCheckProgramFolder.IsChecked();

	_StartInIP.SetSensitive(check_flag);
	mStartInBrowse.SetSensitive(check_flag);
}

void ScreenWindows::set_ProgramFolder_status()
{
	int check_flag;

	check_flag = mShortCheckProgramFolder.IsChecked();
	_ShortCutFolder.SetSensitive(check_flag);
}

void ScreenWindows::set_wintab_defaults()
{
	PsProduct *I = GetCurrentInstallable();

	if (I->Prop.GetInt(mShortCheckDesktop.GetId()) == 1)
		mShortCheckDesktop.Check(TRUE);
	else
		mShortCheckDesktop.Check(FALSE);
	if (I->Prop.GetInt(mShortCheckStartMenu.GetId()) == 1)
		mShortCheckStartMenu.Check(TRUE);
	else
		mShortCheckStartMenu.Check(FALSE);
	if (I->Prop.GetInt(mShortCheckQuickLaunch.GetId()) == 1)
		mShortCheckQuickLaunch.Check(TRUE);
	else
		mShortCheckQuickLaunch.Check(TRUE);
	if (I->Prop.GetInt(mShortCheckProgramFolder.GetId()) == 1)
	{
		mShortCheckProgramFolder.Check(TRUE);
		_ShortCutFolder.SetSensitive(TRUE);
	}
	else if (I->Prop.GetInt(mShortCheckProgramFolder.GetId()) == 0)
	{
		mShortCheckProgramFolder.Check(FALSE);
		_ShortCutFolder.SetSensitive(FALSE);
	}
	else
	{
		mShortCheckProgramFolder.Check(TRUE);
		_ShortCutFolder.SetSensitive(TRUE);
	}

	//  Add and select product name in the OptionMenu
	// the full prodName is a wchar_t and may contains forward slashes
	// (If property "ShortcutFolder" exists, use it instead of Product name)
	btkString prod_name(I->Prop("ShortcutFolder"));
	if (prod_name == "")
	{
		prod_name = I->GetName();
	}
	prod_name.Substitute("/", " ");
	prod_name.Substitute("*", " ");
	prod_name.Substitute("|", " ");

	// SPR 940286: put shortcuts in PTC/product
	if (GetCurrentInstallable()->Prop("ShortcutFolder") == btkString(""))
		prod_name = "PTC\\" + prod_name;

	if (_ShortCutFolder.Find(prod_name) == -1)
	{
		PsWarning << "Append to _ShortCutFolder: " << prod_name << endl << endm;
		_ShortCutFolder.Append(prod_name, prod_name);
		_ShortCutFolder.Refresh();
	}
	//_ShortCutFolder.SetText(prod_name);
	_ShortCutFolder.Select(prod_name);

	// set the startup_dir to the users' homedir
	btkString startupdir;
	PsGetPersonalFolder(startupdir);
	_StartInIP.SetText(startupdir);

	// set the path settings to System incase they changed since last config
	if (CanWriteAdminKeys)
		PathChoice.Select("systempath");
}

void ScreenWindows::PsGetPersonalFolder(btkString & folder)
{
	// init it to HomeDir
	btkFSEntry homedir;
	homedir.GetHomeDir();
	folder = homedir;

#if OPER_SYS == WINDOWS_32
	// Get Personal folder value from the shell!
	btkString key("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion");
	key += "\\Explorer\\Shell Folders";
	btkFSEntry startupdir(psRegkeyGetStringFromValue( NULL, key, "Personal"));
	if (startupdir.Exists())
	{
	folder = startupdir;
	}
#endif
}

bool ScreenWindows::OnPushButtonActivate(uiPushButton &pushed)
{
	if (pushed == mStartInBrowse)
	{
		PsLaunchStartBrowser();
	}
	return (TRUE);
} 

bool ScreenWindows::OnCheckButtonActivate(uiCheckButton &checked)
{
	if (checked == mShortCheckProgramFolder)
		set_ProgramFolder_status();
	set_StartInDir_status();
	return (TRUE);
}

bool ScreenWindows::OnRadioGroupSelect(uiRadioGroup &radio, cStringT name)
{
	return (TRUE);
}

bool ScreenWindows::OnOptionMenuActivate(uiOptionMenu &ip)
{
	return (TRUE);
}

LpcfgWinInfo::LpcfgWinInfo()
{
	// these are the defaults for shortcuts!!!
	DesktopOn = 0;
	StartMenuOn = 0;
	ProgGrpOn = 1;
	UserpathOn = 0;
	// convert Pro/Engineer to "Pro Engineer"
	if (GetCurrentInstallable())
	{
		// SPR 940286: put shortcuts under PTC/product
		//
		btkString prod_str(GetCurrentInstallable()->GetName());
		prod_str.Substitute("/", " ");
		// * in Info*Engine is illegal for a directory name
		prod_str.Substitute("*", " ");
		// | in Pro|CONCEPT is illegal for a directory name
		prod_str.Substitute("|", " ");

		ProgGrpName = "PTC\\";
		ProgGrpName += prod_str;
	}
	else
	{
		ProgGrpName = "PTC";
	}
	ScreenWindows::PsGetPersonalFolder(StartupDirName);
}

