/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*\
 
  PsWriteMSI.cxx
 
  Date      Release   Name  Ver.   Comments
  --------- -------   ----  ----   --------
  09-Dec-99           MYA          Created
  13-Dec-99           MYA          Call GetOwnBinScripts
  15-Dec-99 I-03-24   JJE   $$1    Group Submission
  16-Dec-99           MYA          Use defaultstate and MsiVisible
  17-Dec-99           MAZ          Add AddIconEntry()
  20-Dec-99           MYA          Add WriteIniFileTable, WriteShortcutTable
  21-Dec-99           MAZ          Check if xtop.ico exists before loading it
  22-Dec-99 I-03-24+  JJE   $$2    Group Submission
  27-Dec-99           MAZ          Add PsGetShortFName()
  03-Jan-99           MAZ          Fix shortcut table entries
  04-Jan-00           MYA          don't write some tables w/o proe component
  04-Jan-00 I-03-25+  JJE   $$3    Group Submission
  06-Jan-00           MAZ          Add funcs to support HKCU shortcut 
  11-Jan-00           MYA          Change extension.ptc to extension.csv
  11-Jan-00           MAZ          Support proe advertising (SPR 806518)
  12-Jan-00 I-03-26+  JJE   $$4    Group Submission
  13-Jan-00           MYA          change return of WriteDatabase
  14-Jan-00           MAZ          Add INSTALLDIRComp and customActions
  17-Jan-00 I-03-26+  JJE   $$5    Group Submission  
  19-Jan-00           MAZ          Add more OLE regkeys
  24-Jan-00           MAZ          Create extensions from .1 to .299
  26-Jan-00           MAZ          Fix win2k search engine bug using EXT_GRP_CNT
  26-Jan-00 I-03-26+  JJE   $$6    Group Submission
  31-Jan-00           MAZ          Use unique progId descriptions
  31-Jan-00           JJE          Pad with spaces instead of numbers
  01-Feb-00           MAZ          Fix 809165 (removing advertised installs) 
  02-Feb-00           MAZ          set PTC_EXT_COUNT to 0 by default
  02-Feb-00 I-03-26+  JJE          Group Submission
  09-Feb-00           JJE          Change icon name to proe.ico, rm mydocs
  09-Feb-00 I-03-26+  JJE          Group Submission
  11-Feb-00 I-03-27+  MAZ          Use new SetName() and add better debug
  14-Feb-00           MAZ          Provide better debug info for ext table
  14-Feb-00 J-01-02   TWH   $$7    Group Submission
  01-Mar-00           MAZ          Support langs by using ExpandMsiCdSectionList()
  17-Mar-00 I-03-28+  JJE   $$8    Group Submission
  23-Mar-00           TWH/MAZ      Fix langs bug and use XNew
  28-Mar-00 I-03-28+  TWH   $$9    Group Submission
  31-Mar-00           MAZ          Create Lang subfeatures per package
  10-Apr-00 I-03-28+  JJE   $$10   Group Submission
  12-Apr-00           MAZ          Fix SecureCustomProperties value
  13-Apr-00 I-03-28+  JJE          Group Submission
  10-May-00           MAZ          Use "PersonalFolder" for shortcut startup dir
  15-May-00 I-03-28+  JJE   $$11   Group Submission
  31-Aug-00           MAZ          Modify PsGetMsiVer() for rel 2000i3
  18-Sep-00 J-01-18   TWH   $$12   Group Submission
  25-Sep-00           MAZ          Fix FindComponentName and FindDirectoryName
  02-Oct-00 J-01-19   TWH   $$13   Group Submission
  02-Jan-01           MAZ          Add ProductName arg to ups_always action
  08-Jan-01 J-01-25   TWH   $$14   Group Submission
  30-Jan-01           MAZ          Fix OLE reg & iniFile (SPR 825105, 828990)
  02-Feb-01           MAZ          Provide two $PATH options, sys and user
  21-Feb-01           MAZ          Set the shortcut WkDir value 
  21-Feb-01 J-01-27+  TWH   $$15   Group Submission
  30-Mar-01 I-01-30+  MAZ   $$16   Fix Registry table key counter
  09-Apr-01 J-01-32+  MAZ   $$17   Add STARTIN_DIR as Wkdir w/o []
  01-May-01 J-01-32+  MAZ   $$18   Use new AddDirectory prototype
  18-Jun-01 J-03-02   MAZ   $$19   Add CustomAction ChangeProp2
  07-Sep-01 J-03-07   MAZ   $$20   SPR 898283: Support AllUsers
  21-Sep-01 J-03-09   jas   $$21   Removed WINDOWS_95 macro
  02-Oct-01 J-01-37+  MAZ          Change ApplicationUsers to APPUSERS
  30-Oct-01 J-03-11   MAZ   $$22   Add SIMPLIFY & TYPEREG (+ Update from J01)
  06-Dec-01 J-03-14   MAZ   $$23   Support -ae (AEFLAG)
  31-Dec-02 J-03-40   TWH   $$24   Add PTC_USE_MSI
  28-Nov-05 K-03-37   TWH   $$25   Remove GALAXY ref.
  26-Jun-06 L-01-11   KSV   $$26   Types changed to avoid conflicts in UI
  12-Jul-06 L-01-12   ksi   $$27   Unicode compliant changes
  15-Nov-22 Q-10-36   Ahmad $$28   some strings scrambled
  
\*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#include <btkcstdio.h>
#include <Ps.h>
#include <btkscale31.h>

#if OPER_SYS == WINDOWS_32
#ifdef PTC_USE_MSI

#define PTC_EXT_COUNT 0  // don't let msi create any numeric extensions 
#define EXT_GRP_CNT 30	// 30 extensions per progId
#define UPGRADECODE "{5DB1743D-B71F-11D3-ABBD-00C04F60462B}"

int PsGetExtensions(int *count, StringXArray *Ext);
void PsGetMsiVer(btkString & value);

int PsWriteMSI::WriteToDatabase()
{
// write date to MSI database;

	PsInfo << "write to database" << endl << endm;
	
	MSIHANDLE hDB;
 
	// try to open database
 	int ret = MsiOpenDatabase((cStringT)PsCd::MSIDB(), MSIDBOPEN_TRANSACT, &hDB);

	ret = MSIOpenDatabaseError(ret,(cStringT)PsCd::MSIDB());
	
	if (ret)
	{
		MSIHANDLE hViewF, hRecord;
 		int ret1 = MsiDatabaseOpenView(hDB, TEXT("Select Directory, Directory_Parent, DefaultDir FROM Directory"), &hViewF);
		btkString message = "INSTALLDIR";

		MsiViewExecute(hViewF,0);

		hRecord = MsiCreateRecord(3);
		MsiRecordSetString(hRecord, 1, "INSTALLDIR");
		MsiRecordSetString(hRecord, 2, "ProgramFilesFolder");

		// always use the default lp name (proe2000i2) 
		// regardless of where we actually install
		// reconstruct the path as if we are installing in proe2000i2
		btkString shortName(*(GetCurrentInstallable()->GetLpDirName()));
		shortName += *(GetPsCD()->GetDateCode()->GetDatecodeVersion());
		btkFSEntry msilp(GetCurrentInstallable()->GetLoadpoint()->GetLoadpoint());
		msilp = msilp.GetHead();
		msilp /= shortName;
		PsGetShortFName(msilp, shortName);
		MsiRecordSetString(hRecord, 3, shortName);
     
		ret1 = MsiViewModify(hViewF, MSIMODIFY_INSERT, hRecord);
		MSIViewModifyError(ret1, (cStringT)message);
		MsiViewClose(hViewF);
		
		// write directories to directory table
		for (int j=0; j<Directories.GetSize(); j++)
		{
			if (!Directories[j]->WriteDatabase(hDB))
			{
				PsWarning << "Directories[" << j << "] failed" << endl << endm;
				ret = FALSE;
			}
		}
		// write features to feature table
		for (int i=0; i<Features.GetSize(); i++)
		{
	  		if (!(Features[i]->WriteDatabase(hDB)))
			{
				ret = FALSE;
			}
		}
		
		if (bincomponent != "")
		{
			if (!WriteEnvTable(hDB))
			{
				PsWarning << "WriteEnvTable failed" << endl << endm;
				ret = FALSE;
			}
		}
		if (proe_component.GetComponentName() != "")
		{
			if (!WriteExtTable(hDB))
			{
				PsWarning << "WriteExtTable failed" << endl << endm;
				ret = FALSE;
			}
			if (!WriteRegTable(hDB))
			{
				PsWarning << "WriteRegTable failed" << endl << endm;
				ret = FALSE;
			}
			if (!WriteHKCUKey(hDB))
			{
				PsWarning << "WriteHKCUKey failed" << endl << endm;
				ret = FALSE;
			}
			if (!WriteShortcutTable(hDB))
			{
				PsWarning << "WriteShortcutTable failed" << endl << endm;
				ret = FALSE;
			}
			if (!WriteIniFileTable(hDB))
			{
				PsWarning << "WriteIniFileTable failed" << endl << endm;
				ret = FALSE;
			}
			if (!WriteCustomActions(hDB))
			{
				PsWarning << "WriteCustomActions failed" << endl << endm;
				ret = FALSE;
			}
			if (!AddIconEntry(hDB))
			{
				PsWarning << "AddIconEntry failed" << endl << endm;
				ret = FALSE;
			}
		}
		if (!WritePropertyTable(hDB))
		{
			PsWarning << "WritePropertyTable failed" << endl << endm;
			ret = FALSE;
		}
		if (!WriteUpgradeTable(hDB))
		{
			PsWarning << "WriteUpgradeTable failed" << endl << endm;
			ret = FALSE;
		}
		if (!WriteINSTALLDIRComp(hDB))			
		{
			PsWarning << "WriteINSTALLDIRComp failed" << endl << endm;
			ret = FALSE;
		}
	}
	
	MsiDatabaseCommit(hDB);

	return ret;
}

// We want two PATH entries in this table, one for Admin and for user
// Admin sets PATH system wide and non-admin sets it to current user
int PsWriteMSI::WriteEnvTable(MSIHANDLE hDB)
{
 	MSIHANDLE hViewE, hViewC, hViewCF, hViewFC, hRecord;
	int retVal = TRUE, ret, i;
	const int items = 2;
	btkString btkID;
	const char *pathComps[2] = {"PTCPathSys", "PTCPathUsr"};
	// Fix SPR 900669, check for AllUsers value
	const char *condition[2] = {"APPUSERS = \"AllUsers\" AND AdminUser", 
		"APPUSERS <> \"AllUsers\" OR NOT AdminUser"};
	const char *pathValue[2] = {"*=-PATH", "=-PATH"};

	// Now the component table -> Add two components
	ret = MsiDatabaseOpenView(hDB,
		TEXT("Select Component, ComponentId, Directory_, Attributes, Condition, KeyPath FROM Component"), 
		&hViewC);
	if (ret != ERROR_SUCCESS) retVal = FALSE;
	MSIViewOpenError(ret, "Comp table (Path): ");
	MsiViewExecute(hViewC, 0);

	for (i=0; i < items; i++)
	{
		hRecord = MsiCreateRecord(6);
		MsiRecordSetString(hRecord, 1, pathComps[i]);
		PsGetNewGUID(btkID);
		MsiRecordSetString(hRecord, 2, (cStringT)btkID);
		MsiRecordSetString(hRecord, 3, "INSTALLDIR");
		MsiRecordSetInteger(hRecord, 4, 0);
		MsiRecordSetString(hRecord, 5, condition[i]); // condition entry
		MsiRecordSetString(hRecord, 6, "");  // leave it empty
		ret = MsiViewModify(hViewC, MSIMODIFY_INSERT, hRecord);
		if (ret != ERROR_SUCCESS) retVal = FALSE;
		MSIViewModifyError(ret, 
			btkString("Comp table " + btkString(pathComps[i]) + ": "));
	}
	MsiViewClose(hViewC);

	// Add these two entries to the createFolders table to avoid
	// validation errors!!!
	ret = MsiDatabaseOpenView(hDB,
		TEXT("Select Directory_, Component_ FROM CreateFolder"), &hViewCF);
	if (ret != ERROR_SUCCESS) retVal = FALSE;
	MSIViewOpenError(ret, "CreateFolder table: ");
	MsiViewExecute(hViewCF, 0);

	for (i=0; i < items; i++)
	{
		hRecord = MsiCreateRecord(2);
		MsiRecordSetString(hRecord, 1, "INSTALLDIR");
		MsiRecordSetString(hRecord, 2, pathComps[i]);	
		ret = MsiViewModify(hViewCF, MSIMODIFY_INSERT, hRecord);
		if (ret != ERROR_SUCCESS) retVal = FALSE;
		MSIViewModifyError(ret,
			btkString("CreateFolder table " + btkString(pathComps[i]) + ": "));
	}
	MsiViewClose(hViewCF);

	// Then populate the FeatureComponents table
	ret = MsiDatabaseOpenView(hDB,
		TEXT("Select Feature_,Component_ FROM FeatureComponents"),
		&hViewFC);
	if (ret != ERROR_SUCCESS) retVal = FALSE;
	MSIViewOpenError(ret, "FeatComp table (PATH): ");
	MsiViewExecute(hViewFC,0);

	for (i=0; i < items; i++)
	{
		hRecord = MsiCreateRecord(2);
		MsiRecordSetString(hRecord, 1, "proe_base");
		MsiRecordSetString(hRecord, 2, pathComps[i]);
		ret = MsiViewModify(hViewFC, MSIMODIFY_INSERT, hRecord);
		if (ret != ERROR_SUCCESS) retVal = FALSE;
		btkString msg("FeatComp table (PATH): " + btkString(pathComps[i]));
		MSIViewModifyError(ret, msg);
	}
	MsiViewClose(hViewFC);

	ret = MsiDatabaseOpenView(hDB,
		TEXT("SELECT Environment, Name, Value, Component_ FROM Environment"), 
		&hViewE);
	if (ret != ERROR_SUCCESS) retVal = FALSE;
	btkString message = "Environment table ";
	MSIViewOpenError(ret, message); 
	MsiViewExecute(hViewE,0);
 
	for (i=0; i < items; i++)
	{
		hRecord = MsiCreateRecord(4);
	 
		// write value without "*" -> SPR 807303 (NOT REALLY)
		// we will actually add * for Admin and not for User
		MsiRecordSetString(hRecord, 1, pathComps[i]);
		MsiRecordSetString(hRecord, 2, pathValue[i]);
		MsiRecordSetString(hRecord, 3, "[~];[INSTALLDIR]bin");
		MsiRecordSetString(hRecord, 4, pathComps[i]);
		ret = MsiViewModify(hViewE, MSIMODIFY_INSERT, hRecord);
		if (ret != ERROR_SUCCESS) retVal = FALSE;
		message += pathComps[i];
		MSIViewModifyError(ret, message);		 
	}
	MsiViewClose(hViewE);
 
	return retVal;  
}

int PsWriteMSI::WriteCustomActions(MSIHANDLE hDB)
{
	// the two tables: CustomAction and  
	const int actions_count = 5;	
	MSIHANDLE hViewC, hViewI, hRecord;  
	const char *action[]= {"ups_always", "ups_postcfg", "msi_uninst", 
					"ChangeProp1", "ChangeProp2"};
	const int type[] = {18, 18, 18, 51, 51}; 
	const char *source[]= {"PTCptcsetup.exe", "PTCptcsetup.exe", 
					"PTCpsuninst.exe", "STARTIN_DIR", "STARTIN_DIR"};
	const char *target[]= {
				"-msi \"[INSTALLDIR]\" \"[ProductName]\" [APPUSERS] [TYPEREG]", 
					"-msiutil [APPUSERS] [SIMPLIFY] [AEFLAG]", 
					"-msi", "%SystemRoot%\\Temp", "PersonalFolder"};
	// call the uninstaller only when removing proe_base while psuninst exists
	const char *condition[]= {"(&proe_base=3)", "(&proe_base=3) AND UPS_POST=\"y\"", 
					"(&proe_base=2) AND (?PTCPSUNINST.EXE=3)", 
					"((ALLUSERS=1) OR (ALLUSERS=2))",
					"(NOT (ALLUSERS=1)) AND (NOT (ALLUSERS=2))"};
	// ChangeProperty seq is > InstallInitialize
	int seq[]= {6750, 6760, 2450, 1520, 1525};
	int retVal = TRUE;
        int ret = MsiDatabaseOpenView(hDB,
                        TEXT("SELECT * FROM CustomAction"),
                        &hViewC);
        MSIViewOpenError(ret,"CustomAction: ");
	if (ret != ERROR_SUCCESS)
		retVal = FALSE; 

        MsiViewExecute(hViewC,0);
	for (int i = 0; i < actions_count; i++)
	{
		hRecord = MsiCreateRecord(actions_count);
		MsiRecordSetString(hRecord, 1, action[i]);  // Action
		MsiRecordSetInteger(hRecord, 2, type[i]); // Type
		MsiRecordSetString(hRecord, 3, source[i]); //Source
		MsiRecordSetString(hRecord, 4, target[i]); // target
		ret = MsiViewModify(hViewC, MSIMODIFY_INSERT, hRecord);
		if (ret != ERROR_SUCCESS)
			retVal = FALSE;	

		MSIViewModifyError(ret, "CustomAction: ");
	}
	MsiViewClose(hViewC);
 
	ret = MsiDatabaseOpenView(hDB,
		TEXT("SELECT * FROM InstallExecuteSequence"),
		&hViewI);
	MSIViewOpenError(ret,"InstallExeSeq: ");
	if (ret != ERROR_SUCCESS)
		retVal = FALSE; 

	MsiViewExecute(hViewI,0);
	btkString versionNT("VersionNT AND ");
	for (i = 0; i < actions_count; i++)
	{
		btkString cond(versionNT);
		cond += condition[i];
		hRecord = MsiCreateRecord(actions_count);
		MsiRecordSetString(hRecord, 1, action[i]);  // Action
		MsiRecordSetString(hRecord, 2, cond); // condition 
		MsiRecordSetInteger(hRecord, 3, seq[i]); // sequence  
		ret = MsiViewModify(hViewI, MSIMODIFY_INSERT, hRecord);
		if (ret != ERROR_SUCCESS)
			retVal = FALSE; 
		MSIViewModifyError(ret, "InstallExeSeq: ");
	} 
	MsiViewClose(hViewI);

	return retVal;
}

int PsWriteMSI::WritePropertyTable(MSIHANDLE hDB)
{
	MSIHANDLE hViewP, hRecord;
	int ret, retVal = TRUE;

	StringXArray names, values;
	btkString value; 
	
	// Add ProductName
	char dcode_buf[32];
	names += XNew btkString("ProductName");
	btk_sprintf(dcode_buf, "%ld", (long int)((GetPsCD()->GetDateCode())->Getdcode()));
	value = btkString(GetCurrentInstallable()->GetName());
	value += " ";
	value += *((GetPsCD()->GetDateCode())->GetDatecodeVersion());	
	value += " [";
	value += btkString(dcode_buf);
	value +=  "]";
	values += XNew btkString(value);

	// Add ProductCode
	names += XNew btkString("ProductCode");
	PsGetNewGUID(value);	// Get a new GUID every time we run	
	values += XNew btkString(value);

	// Add ProductVersion, it is of the form aa.bb.cccc
	// aa is the major version (ie 23), bb is the minor and cccc is the build
	// aa < 0xFF, bb < 0xFF, and cccc < 0xFFFF   (0xFF = 255, 0xFFFF = 65535)
	//
	names += XNew btkString("ProductVersion");
	PsGetMsiVer(value);
	values += XNew btkString(value);

	// Add SecureCustomProperties
	names += XNew btkString("SecureCustomProperties");
	value = "INSTALLDIR;UPS_POST;TYPEREG;PSUPGRADE;PTCLANGS;SIMPLIFY;AEFLAG";
	values += XNew btkString(value);

	// Add UpgradeCode, this guy is a constant always! (I think!)
	names += XNew btkString("UpgradeCode");
	values += XNew btkString(UPGRADECODE);

	// Add Manufacturer,
	names += XNew btkString("Manufacturer");
	values += XNew btkString("PTC");

	// Add global property UPS_POST
	names += XNew btkString("UPS_POST");
	values += XNew btkString("n");

	// Add global property TYPEREG 
	names += XNew btkString("TYPEREG");
	values += XNew btkString("n");

	// Add global property SIMPLIFY
	names += XNew btkString("SIMPLIFY");
	values += XNew btkString("n");

	// Add global property AEFLAG (n by default)
	names += XNew btkString("AEFLAG");
	values += XNew btkString("n");

	// Add global property PTCLANGS
	names += XNew btkString("PTCLANGS");
	values += XNew btkString("_"); 

	// Add global property STARTIN_DIR with default PersonalFolder
	names += XNew btkString("STARTIN_DIR");
	values += XNew btkString("PersonalFolder");

	ret = MsiDatabaseOpenView(hDB, TEXT("SELECT * FROM Property"), &hViewP);
	if (!MSIViewOpenError(ret, " Property Table "))
	{
		retVal = FALSE;
	}
	MsiViewExecute(hViewP, 0);

	for (int i=0; i < names.GetSize(); i++)
	{
		btkString msg("Property " + names[i]);

		hRecord = MsiCreateRecord(2);
		MsiRecordSetString(hRecord, 1, names[i]);  // property name 
		MsiRecordSetString(hRecord, 2, values[i]); // property value
		ret = MsiViewModify(hViewP, MSIMODIFY_INSERT, hRecord);
		if (!MSIViewModifyError(ret, msg))
		{
			retVal = FALSE;
		}
	}
	MsiViewClose(hViewP);

	return retVal;
}

int PsWriteMSI::WriteUpgradeTable(MSIHANDLE hDB)
{
	MSIHANDLE hViewU, hRecord;
	int ret, retVal = TRUE;
	btkString ver_val;

	ret = MsiDatabaseOpenView(hDB, TEXT("SELECT * FROM Upgrade"), &hViewU);
	if (retVal = MSIViewOpenError(ret, " Upgrade Table "))
	{
		MsiViewExecute(hViewU, 0);

		hRecord = MsiCreateRecord(7);
		MsiRecordSetString(hRecord, 1, UPGRADECODE); // UpgradeCode
		MsiRecordSetString(hRecord, 2, ""); // VersionMin
		PsGetMsiVer(ver_val);
		MsiRecordSetString(hRecord, 3, (cStringT) ver_val); // VersionMax
		MsiRecordSetString(hRecord, 4, ""); // language; empty for all langs
		MsiRecordSetInteger(hRecord, 5, 517); // attribs; see msi help for explanation	
		MsiRecordSetString(hRecord, 6, "[]"); // Remove; [] for don't rm
		MsiRecordSetString(hRecord, 7, "PSUPGRADE"); // ActionProperty

		ret = MsiViewModify(hViewU, MSIMODIFY_INSERT, hRecord);
		retVal = MSIViewModifyError(ret, "Upgrade Table");
		MsiViewClose(hViewU);
	}

	return retVal;
}

int PsWriteMSI::WriteShortcutTable(MSIHANDLE hDB)
{
	MSIHANDLE hViewS, hViewD, hRecord;
	int ret = MsiDatabaseOpenView(
			hDB, 
			TEXT("SELECT Directory, Directory_Parent, DefaultDir  FROM Directory"), 
			&hViewD);
	btkString message = "Directory table ";
	MSIViewOpenError(ret, message); 
	MsiViewExecute(hViewD,0);

	btkString prodname(GetCurrentInstallable()->GetName());
	prodname.Substitute("/", " ");
	btkString shortname(prodname);
	shortname.Substitute(" ", "");
	shortname = shortname(0,5);
	shortname = shortname.ToUpper();
	shortname += "~1|";
	shortname += prodname;
 
	hRecord = MsiCreateRecord(3);
	 
	MsiRecordSetString(hRecord, 1, "PTCShortcutDir"); 
	MsiRecordSetString(hRecord, 2, "ProgramMenuFolder");  
	MsiRecordSetString(hRecord, 3, shortname);
	
	ret = MsiViewModify(hViewD, MSIMODIFY_INSERT, hRecord);
	MSIViewModifyError(ret, message);
	MsiViewClose(hViewD);

	ret = MsiDatabaseOpenView(
			hDB, 
			TEXT("SELECT Shortcut, Directory_, Name, Component_, Target, Arguments, Description, Icon_, IconIndex, ShowCmd, WkDir FROM Shortcut"), 
			&hViewS);

	message = "Shortcut table ";
	MSIViewOpenError(ret, message); 
	MsiViewExecute(hViewS,0);

	btkString proe_start = proe_component.GetComponentName();
	// get rid of starting "PTC" & ending ".bat"
	proe_start = proe_start(3, proe_start.Pos(".bat")-1);
	btkString proe_name(proe_start);
	proe_name = proe_name(0,5);
	proe_name = proe_name.ToUpper();
	proe_name += "~1|";
	proe_name += proe_start;

 	hRecord = MsiCreateRecord(11);
	 
	MsiRecordSetString(hRecord, 1, proe_start);//shortcut
	MsiRecordSetString(hRecord, 2, "PTCShortcutDir"); //directory
	MsiRecordSetString(hRecord, 3, proe_name); //name
	MsiRecordSetString(hRecord, 4, proe_component.GetComponentName()); //component

	// use proe_base as target for the sake of advertisement
	MsiRecordSetString(hRecord, 5, "proe_base");
	MsiRecordSetString(hRecord, 6, ""); // Arguments
	MsiRecordSetString(hRecord, 7, "Pro/ENGINEER startup command");
	MsiRecordSetString(hRecord, 8, "proe.ico"); // Icon_
	MsiRecordSetInteger(hRecord, 9, 0);//IconIndex
	MsiRecordSetInteger(hRecord, 10, 0); //ShowCmd
	// The WkDir (or startup dir) is the PersonalFolder
	// Actually, the WkDir will be Personal or Temp depening on ALLUSERS value
	// you should add STARTIN_DIR w/o []
	MsiRecordSetString(hRecord, 11, "STARTIN_DIR");

	ret = MsiViewModify(hViewS, MSIMODIFY_INSERT, hRecord);
	MSIViewModifyError(ret, message);		 
 	MsiViewClose(hViewS);

	if (ret == ERROR_SUCCESS)
		return TRUE;
	else return FALSE;  
	
}

int PsWriteMSI::WriteIniFileTable(MSIHANDLE hDB)
{
	const char *Key[]= {"PRO_DIRECTORY", "PRO_E_EXECUTABLE",
					"PRO_COMM_MSG_EXE", "PRO_LATE_CONNECT", "LANG",
					"PROOBJ_LANG_DLL", "PROOBJ_START_DIRECTORY", 
					"PROE_START_DIRECTORY", "WAIT_FRPROE", "PROOBJ_HELPFILE"};
	const char *Value[]= {"[INSTALLDIR]", "[INSTALLDIR]bin\\",
					"[INSTALLDIR]i486_nt\\obj\\pro_comm_msg.exe", "YES", "C",
					"[INSTALLDIR]i486_nt\\obj\\prooleus.dll", "[PersonalFolder]",
					"[PersonalFolder]", "600", "[INSTALLDIR]text\\usascii\\proole.hlp"};

	btkString proe_start = proe_component.GetComponentName();
	 
	if (proe_start == "")
	{
		PsWarning << "WriteIniFileTable returned empty GetComponentName" << endl << endm;
		return 0;
	}
	else
	{
		// get rid of starting "PTC"   --> SPR 825105
		proe_start = proe_start(3, proe_start.GetLength() -1);
	}

	MSIHANDLE hViewI, hRecord;

 
	int ret = MsiDatabaseOpenView(
			hDB, 
			TEXT("SELECT * FROM IniFile"), 
			&hViewI);

	btkString message = "IniFile ";
	MSIViewOpenError(ret, message); 

	btkString key = "PRO_E_EXECUTABLE";
 	for (int i=0; i< 11; i++)
	{
		hRecord = MsiCreateRecord(8);
	 
		MsiRecordSetString(hRecord, 1, Key[i]);
	 	MsiRecordSetString(hRecord, 2, "proobj.ini");
	 	MsiRecordSetString(hRecord, 3, "INSTALLDIR");
	 	MsiRecordSetString(hRecord, 4, "PRO_E_SECTION");
		MsiRecordSetString(hRecord, 5, Key[i]);
		if (Key[i] != key)
			MsiRecordSetString(hRecord, 6, Value[i]);
		else
		{
			btkString value = Value[i];
			value += proe_start;
			MsiRecordSetString(hRecord, 6, value);
		}

		MsiRecordSetInteger(hRecord, 7, 0);
		MsiRecordSetString(hRecord, 8, proe_component.GetComponentName());

		ret = MsiViewModify(hViewI, MSIMODIFY_INSERT, hRecord);

		message = Key[i];
		message += "=";
		message += Value[i];
		MSIViewModifyError(ret, message);		 
	}
 
	MsiViewClose(hViewI);
	if (ret == ERROR_SUCCESS)
		return TRUE;
	else return FALSE;  
}

int PsWriteMSI::WriteExtTable(MSIHANDLE hDB)
{
	MSIHANDLE hViewE, hViewP, hViewV, hRecord;
 	StringXArray Ext;
	int count = 0, size, progIdCount, i, j;
	char buf[128];
	StringXArray progIds;

	// continue if the file doesn't exist, we will only
	// create version numbers from 1 to 999
	// PsGetExtensions will get actual string extensions from csv file
 	if (!PsGetExtensions(&count, &Ext))
	{
		PsWarning <<"File doesn't exist: extension.csv!" 
			<< endl << endm;
	}

	btkString alt_count;
	if(btkGetEnv( BTK_UNSCRAMBLE_31_S("PTC_ALT_EXT_COUNT"), alt_count))
	{
		size = atoi((cStringT)(alt_count)) + count; 
	}
	else
	{
		// if the env var is not set, use the default
		size = count + PTC_EXT_COUNT;
	}
	
	// the rumor is we can not have more than 64 extensions per progId
	progIdCount = (size/EXT_GRP_CNT);
	if ((size % EXT_GRP_CNT) > 0)
	{
		progIdCount++;
	}

	// Write ProgId table

	int ret = MsiDatabaseOpenView(
			hDB, 
			TEXT("SELECT ProgId, Description, Icon_, IconIndex FROM ProgId"), 
			&hViewP);
	btkString message = "ProgId table ";
	MSIViewOpenError(ret, message); 
	MsiViewExecute(hViewP,0);

	for (i=0; i < progIdCount; i++)
	{
		hRecord = MsiCreateRecord(4);
		btk_sprintf(buf, "proeFile%d", i+1);
		// store the progIds to be used later
		progIds += XNew btkString(buf); 
		MsiRecordSetString(hRecord, 1, buf);
		btk_sprintf(buf, "Pro/ENGINEER File");
		for( j=0; j<i; j++ )
			strcat( buf, " " );
		MsiRecordSetString(hRecord, 2, buf);
		MsiRecordSetString(hRecord, 3, "proe.ico");
		MsiRecordSetString(hRecord, 4, 0);

		ret = MsiViewModify(hViewP, MSIMODIFY_INSERT, hRecord);
		MSIViewModifyError(ret, message);
	}
	MsiViewClose(hViewP);

	// Write Extention table

	ret = MsiDatabaseOpenView(
			hDB, 
			TEXT("SELECT Extension, Component_, ProgId_, Feature_ FROM Extension"), 
			&hViewE);

	message = "Extension table ";
	MSIViewOpenError(ret, message); 

	MsiViewExecute(hViewE,0);

	btkString message1 = "Verb table";
	
	ret = MsiDatabaseOpenView(
			hDB, 
			TEXT("SELECT Extension_, Verb, Sequence FROM Verb"), 
			&hViewV);

	MSIViewOpenError(ret, message); 

	MsiViewExecute(hViewE,0);

	// create extensions for .1 to .PTC_EXT_COUNT
	int idx = 0;
	for (i=0; i<size ; i++)
	{
		message = "Ext Table: ";
		message += Ext[i];
	 	hRecord = MsiCreateRecord(4);
		if (i<count)
			MsiRecordSetString(hRecord, 1, Ext[i]);//extension
		else
		{
			btk_sprintf(buf, "%d", i-count+1);
			MsiRecordSetString(hRecord, 1, buf);
		}
		MsiRecordSetString(hRecord, 2, "PTCPROEMSG.EXE"); //component

		if (i && (i % EXT_GRP_CNT)==0)		
		{
			idx++;		// use the next progId
		}
		MsiRecordSetString(hRecord, 3, progIds[idx]); // prog_id
		MsiRecordSetString(hRecord, 4, "proe_base"); //feature
	 
		ret = MsiViewModify(hViewE, MSIMODIFY_INSERT, hRecord);
		MSIViewModifyError(ret, message);

		message1 = "Verb Table: ";
		message1 += Ext[i];
		hRecord = MsiCreateRecord(3);
		if (i<count)
			MsiRecordSetString(hRecord, 1, Ext[i]);//extension
		else
		{
			btk_sprintf(buf, "%d", i-count+1);
			MsiRecordSetString(hRecord, 1, buf);
		}
		MsiRecordSetString(hRecord, 2, "Open");
		MsiRecordSetInteger(hRecord, 3, 0);
		
		ret = MsiViewModify(hViewV, MSIMODIFY_INSERT, hRecord);
		MSIViewModifyError(ret, message1);
	}

 	MsiViewClose(hViewE);
	MsiViewClose(hViewV);

	if (ret == ERROR_SUCCESS)
		return TRUE;
	return FALSE;  
}


int PsWriteMSI::WriteRegTable(MSIHANDLE hDB)
{
	MSIHANDLE hViewR, hRecord;
	char buf[10];
	int reg_counter = 0, ret1, ret2;
  	const char * RegKey[3] = {"{53006553-645C-11CF-9C66-02608CAAE0C8}", 
					"{53006554-645C-11CF-9C66-02608CAAE0C8}", 
					"{53006555-645C-11CF-9C66-02608CAAE0C8}"};

	const char * RegKey2[3] = {"ProPart.Document", "ProAsm.Document", 
					"ProDrw.Document"};

	// Add InprocHandler32 key -> SPR (825105, 828990) 
	const char * KeyTail[11] = {"", "\\Verb\\0", "\\Insertable",  
				"\\AuxUserType\\2", "\\AuxUserType\\3",
				"\\MiscStatus", "\\ProgID", "\\procHandler32",
				"\\LocalServer32", "\\DefaultIcon", "\\InprocHandler32"};

	const char * KeyTail2[5] = {"", "\\Insertable", 
				"\\protocol\\StdFileEditing\\verb\\0", 
				"\\protocol\\StdFileEditing\\server", "\\CLSID"};

	const char * Val[11] = {"PTC ", "&Edit,0,2", "", "PTC ", "PTC OLE Server", "32",
					"", "ole32.dll", "", "", "ole32.dll"};

	const char * Val2[8] = {"PTC ", "", "&Edit", "", ""}; 

	const char * va[3]= {"Part", "Assembly", "Drawing"};

	btkString objdir = "[INSTALLDIR]";
	objdir += psOSGetMctype();
	objdir += "\\OBJ\\PROOBJ.EXE";

  	int ret = MsiDatabaseOpenView(
			hDB, 
			TEXT("SELECT * FROM Registry"), 
			&hViewR);
	btkString message = "Registry table ";
	MSIViewOpenError(ret, message); 
	MsiViewExecute(hViewR,0);

	btkString Reg, Key, Value;
  
 	for (int i=0; i<3; i++)
	{
		for (int j=0; j<11; j++)
		{
	 		hRecord = MsiCreateRecord(6);
			btk_sprintf(buf, "%d", reg_counter++);
			Reg = btkString("registry") + buf;
			MsiRecordSetString(hRecord, 1, Reg); 
	 		MsiRecordSetInteger(hRecord, 2, 0);  
			Key = btkString("CLSID\\") + RegKey[i];
			Key += KeyTail[j];
			MsiRecordSetString(hRecord, 3, Key); 
			MsiRecordSetString(hRecord, 4, ""); 
			Value = Val[j];
			if (j==0) 
				Value += va[i] + btkString(" Document");
			else if (j==3)
				Value += va[i];
			else if (j==6)
				Value = RegKey2[i];
			else if (j==8)
				Value = objdir;
			else if (j==9)
			{	
				btk_sprintf(buf, "%d", i+1);
				Value = objdir + btkString(",") + buf;
			}
		 
			MsiRecordSetString(hRecord, 5, Value);
			MsiRecordSetString(hRecord, 6, proe_component.GetComponentName());
		 	ret1 = MsiViewModify(hViewR, MSIMODIFY_INSERT, hRecord);
			MSIViewModifyError(ret1, btkString(message + Reg + " "));
		}
	 }

	for (i=0; i<3; i++)
	{
		for (int j=0; j<5; j++)
		{
			hRecord = MsiCreateRecord(6);
			btk_sprintf(buf, "%d", reg_counter++);
			Reg = btkString("registry") + buf;
			MsiRecordSetString(hRecord, 1, Reg);
			MsiRecordSetInteger(hRecord, 2, 0);
			Key = RegKey2[i];
			Key += KeyTail2[j];
			MsiRecordSetString(hRecord, 3, Key);
			MsiRecordSetString(hRecord, 4, "");
			Value = Val2[j];
			if (j==0)
				Value += va[i] + btkString(" Document");
			else if (j==3)
				Value = objdir;
			else if (j==4)
				Value = RegKey[i];
			MsiRecordSetString(hRecord, 5, Value);
			MsiRecordSetString(hRecord, 6, proe_component.GetComponentName());
			ret2 = MsiViewModify(hViewR, MSIMODIFY_INSERT, hRecord);
			MSIViewModifyError(ret2, btkString(message + Reg + " "));
		}
	}

 	MsiViewClose(hViewR);
 
	if ((ret == ERROR_SUCCESS) && (ret1 == ret) && (ret2 == ret))
		return TRUE;
	return FALSE;  
}

int PsWriteMSI::WriteHKCUKey(MSIHANDLE hDB)
{
	// turn this off for now! we might not use it afterall
#if 0
	MSIHANDLE hViewR, hRecord;
	int ret = MsiDatabaseOpenView(
			hDB,
			TEXT("SELECT * FROM Registry"),
			&hViewR);
	btkString message = "HKCU key: ";
	MSIViewOpenError(ret, message);
	MsiViewExecute(hViewR,0);

	hRecord = MsiCreateRecord(6);
	MsiRecordSetString(hRecord, 1, "PTCUserRegKey");
	MsiRecordSetInteger(hRecord, 2, 1); // 1 for HK_CURRENT_USER
	MsiRecordSetString(hRecord, 3, "Software\\PTC\\MSI");
	MsiRecordSetString(hRecord, 4, "");
	MsiRecordSetString(hRecord, 5, "1");
	MsiRecordSetString(hRecord, 6, "PTCCurrentUser");
	ret = MsiViewModify(hViewR, MSIMODIFY_INSERT, hRecord);
	MSIViewModifyError(ret, message);
	MsiViewClose(hViewR);

	if (ret == ERROR_SUCCESS)
		return TRUE;
	return FALSE; 
#endif
	return TRUE;
}

// FIX SPR 805084
int PsWriteMSI::WriteINSTALLDIRComp(MSIHANDLE hDB)
{
	btkFSEntry msiFile(GetCurrentInstallable()->GetLoadpoint()->GetLoadpoint());
	msiFile /= "msi.dat";
	btkOFileStream OFS;
	if( OFS.Create(msiFile) )
	{
		OFS << "This file is need for windows installer."<< endl;
		OFS << "Don't delete it!" << endl;
		OFS.Close();
	}
	
	// File, Component, FeatureComponent and WiseSourcePath tables
        MSIHANDLE hViewF, hViewC, hViewFC, hViewW, hRecord;

	// START with the file table
	int ret = MsiDatabaseOpenView(hDB,
		TEXT("Select File, Component_, FileName, FileSize, Version, Language, Attributes, Sequence FROM File"),
		&hViewF);
	MSIViewOpenError(ret, "File table (msi.dat): ");
	MsiViewExecute(hViewF,0);
	hRecord = MsiCreateRecord(8);
	MsiRecordSetString(hRecord, 1, "PTCmsi.dat");
	MsiRecordSetString(hRecord, 2, "INSTALLDIR");
	MsiRecordSetString(hRecord, 3, "msi.dat|msi.dat");
	MsiRecordSetInteger(hRecord, 4, 3000);
	MsiRecordSetString(hRecord, 5, "1.0");
	MsiRecordSetString(hRecord, 6, "");
	MsiRecordSetInteger(hRecord, 7, 0); // attribs
	MsiRecordSetInteger(hRecord, 8, 0); // Sequence
	ret = MsiViewModify(hViewF, MSIMODIFY_INSERT, hRecord);
	MSIViewModifyError(ret, "File table (msi.dat): ");
	MsiViewClose(hViewF);

	// Then fix the WiseSOurce table
	ret = MsiDatabaseOpenView( hDB,
		TEXT("Select File_, SourcePath, Date, Attributes FROM WiseSourcePath"), 
		&hViewW);
	MSIViewOpenError(ret, "WiseSrc table (msi.dat): ");
	MsiViewExecute(hViewW,0);
	hRecord = MsiCreateRecord(4);
	MsiRecordSetString(hRecord, 1, "PTCmsi.dat");
	MsiRecordSetString(hRecord, 2, (cStringT)msiFile);
	MsiRecordSetInteger(hRecord, 3, 938789666);
	MsiRecordSetInteger(hRecord, 4, 0);
	ret = MsiViewModify(hViewW, MSIMODIFY_INSERT, hRecord);
	MSIViewModifyError(ret, "WiseSrc table (msi.dat): ");
	MsiViewClose(hViewW);

	// Now the component table
	ret = MsiDatabaseOpenView(hDB, 
		TEXT("Select Component, ComponentId, Directory_, Attributes, Condition, KeyPath FROM Component"), &hViewC);
	MSIViewOpenError(ret, "Comp table (msi.dat): ");
	MsiViewExecute(hViewC,0);
	hRecord = MsiCreateRecord(6);
	MsiRecordSetString(hRecord, 1, "INSTALLDIR");
	btkString btkID;
	PsGetNewGUID(btkID);
	MsiRecordSetString(hRecord, 2, (cStringT)btkID);
	MsiRecordSetString(hRecord, 3, "INSTALLDIR");
	MsiRecordSetInteger(hRecord, 4, 0);
	MsiRecordSetString(hRecord, 5, "1");
	MsiRecordSetString(hRecord, 6, "PTCmsi.dat");
	ret = MsiViewModify(hViewC, MSIMODIFY_INSERT, hRecord);
	MSIViewModifyError(ret, "Comp table (msi.dat): ");
	MsiViewClose(hViewC);
	
	// LAST populate the FeatureComponents table
	ret = MsiDatabaseOpenView(hDB,
		TEXT("Select Feature_,Component_ FROM FeatureComponents"), 
		&hViewFC);
	MSIViewOpenError(ret, "FeatComp table (msi.dat): ");
	MsiViewExecute(hViewFC,0);

	for (int i=0; i<Features.GetSize(); i++)
	{
		hRecord = MsiCreateRecord(2);
		MsiRecordSetString(hRecord, 1, Features[i]->GetFeatureName());
		MsiRecordSetString(hRecord, 2, "INSTALLDIR");
		ret = MsiViewModify(hViewFC, MSIMODIFY_INSERT, hRecord);
		btkString msg("FeatComp table (msi.dat): " + Features[i]->GetFeatureName());
		MSIViewModifyError(ret, msg);

		for (int j=0; j< Features[i]->GetSubFeatures()->GetSize(); j++)
		{
			hRecord = MsiCreateRecord(2);
			MsiRecordSetString(hRecord, 1, 
				(*Features[i]->GetSubFeatures())[j]->GetFeatureName());
			MsiRecordSetString(hRecord, 2, "INSTALLDIR");
			ret = MsiViewModify(hViewFC, MSIMODIFY_INSERT, hRecord);
			msg = btkString("Sub FeatComp table (msi.dat): " + 
				(*Features[i]->GetSubFeatures())[j]->GetFeatureName());
			MSIViewModifyError(ret, msg);
		}
	}
	MsiViewClose(hViewFC);

	if (ret == ERROR_SUCCESS)
		return TRUE;

	return FALSE;  

}

void PsWriteMSI::AddFeature(MSIFeature * feature)
{
	Features += feature;
}

MSIFeature * PsWriteMSI::GetFeature(const char * name)
{
	for (int i=0; i< Features.GetSize(); i++)
	{
		if (Features[i]->GetFeatureName() == name)
			return (Features[i]);
	}
	return NULL;
}

int PsWriteMSI::GetFeatures()
{
	PsProduct * I = GetCurrentInstallable();
	PsPackageXArray * pkgList = I->GetPkgList();
	PsPackageXArray * subpkgList;
	StringXArray * secList;
	PsCmdTypeXArray * coms;
	btkFSEntry path;
	const btkString bindir = "bin";
	const btkString version = *((GetPsCD()->GetDateCode())->GetDatecodeVersion());
	int k;

	for (int i=0; i< pkgList->GetSize(); i++)
	{
		if ((*pkgList)[i]->IsInstalled())
		{
			// just declare as a ptr, don't call "new" here
			StringXArray *binscripts;
			StringXArray commands;
			MSIFeature * pkgFeature;

			// ptcutil and uninstall will not have their own features
			if (btkString((*pkgList)[i]->GetTag()) == "ptcutil" ||
				btkString((*pkgList)[i]->GetTag()) == "uninstall")
			{
				pkgFeature = GetFeature("proe_base");	
				// check if it is already created
				if (pkgFeature == NULL)
				{
					pkgFeature = XNew MSIFeature();
					pkgFeature->SetFeatureName("proe_base");
					pkgFeature->SetFeatureDesc("Pro/ENGINEER");
					pkgFeature->SetDisplay(Gbl_Display);
					Gbl_Display += 2;
					AddFeature(pkgFeature);
				}
			}
			else
			{
				pkgFeature = GetFeature((*pkgList)[i]->GetTag());
				if (pkgFeature == NULL)
				{
					pkgFeature = XNew MSIFeature();
					pkgFeature->SetFeatureName((*pkgList)[i]->GetTag());
					pkgFeature->SetFeatureDesc(
						BTKCHARP(btkString((*pkgList)[i]->GetName())));

					if ((*pkgList)[i]->MsiVisible())
					{
						pkgFeature->SetDisplay(Gbl_Display);
						Gbl_Display += 2;
					}
					else
					{
						pkgFeature->SetDisplay(0);
					}

					if (!(*pkgList)[i]->GetDefaultState())
					{
						pkgFeature->SetLevel(5);
					}

					AddFeature(pkgFeature);
				}
			}
			
			// Add components from cdsections to pkgFeature
			secList = (*pkgList)[i]->GetOwnCdSections();
			if (secList->GetSize() > 0)
			{
				// The main tree will only have usascii files
				ExpandMsiCdSectionList(secList, I, "usascii");
				pkgFeature->AddSectionComponent(secList, this);
				// make sure you delete secList when done with it
				delete secList;

				// Now add the languages as subfeatures
				//
				secList = (*pkgList)[i]->GetOwnCdSections();
				AddPtcLangFeatures(pkgFeature, secList);
				// make sure you delete secList when done with it
				delete secList;
			}

			// Add bin scrips component to pkgFeature
			binscripts = (*pkgList)[i]->GetOwnBinScripts();	 
			if (binscripts->GetSize() > 0)
			{
	 			for (k=0; k<binscripts->GetSize(); k++)
				{
					path= bindir;
					path /= (cStringT)(*binscripts)[k];
					path += ".bat";
				 	(*binscripts)[k] = (cStringT)path;
				 }
				pkgFeature->AddBinComponent(binscripts, this);
			}
			// delete allocated memory here too
			delete binscripts;
			
			// Add command components to pkgFeature
			coms = (*pkgList)[i]->GetCmdList();
			if (coms->GetSize() > 0)
			{
				for (k=0; k<coms->GetSize(); k++)
				{
					path = bindir;
					if (*((*coms)[k]->GetFullName()) != "")
						path /= *((*coms)[k]->GetFullName());
					else if (*((*coms)[k]->GetDefName()) != "")
						path /= *((*coms)[k]->GetDefName()) + version;
					else
						path /= *((*coms)[k]->GetTag()) + version;
					path += ".bat";
					commands += (cStringT)path;
				 }
				pkgFeature->AddCommandComponent(&commands, this);
			}

			// sth for subpkg
			subpkgList = (*pkgList)[i]->GetSubPkgList();
			for (int j=0; j< subpkgList->GetSize(); j++)
			{
				if ((*subpkgList)[j]->IsInstalled())
				{
					StringXArray *binscripts1;
					StringXArray commands1;
					MSIFeature * subpkgFeature = XNew MSIFeature();
					subpkgFeature->SetFeatureName((*subpkgList)[j]->GetTag());
					subpkgFeature->SetFeatureDesc(BTKCHARP(btkString((*subpkgList)[j]->GetName())));
					subpkgFeature->SetParent((*pkgList)[i]->GetTag());
					if ((*subpkgList)[j]->MsiVisible())
					{
						subpkgFeature->SetDisplay(Gbl_Display);
						Gbl_Display += 2;
					}
					else
					{
						subpkgFeature->SetDisplay(0);
					}
					if (!(*subpkgList)[j]->GetDefaultState())
					{
						subpkgFeature->SetLevel(5);
					}
					pkgFeature->AddSubFeature(subpkgFeature);	
					
					// Add components from cdsections to pkgFeature
					secList = (*subpkgList)[j]->GetOwnCdSections();
					if (secList->GetSize() > 0)
					{
						// the main features will only have usascii
						ExpandMsiCdSectionList(secList, I, "usascii");
						subpkgFeature->AddSectionComponent(secList, this);
						delete secList;

						// Now add the languages as subfeatures
						//
						secList = (*subpkgList)[j]->GetOwnCdSections(); 
						AddPtcLangFeatures(subpkgFeature, secList);
						// make sure you delete secList when done with it
						delete secList;
					}
					
					// Add bin scripts component to subpkgFeature
					binscripts1 = (*subpkgList)[j]->GetOwnBinScripts();
					if (binscripts1->GetSize()>0)
					{
	 		 			for (k=0; k< binscripts1->GetSize(); k++)
						{
							path = (cStringT)bindir;
							path /= (*binscripts1)[k];
							path += ".bat";
							(*binscripts1)[k] = (cStringT)path;
						}
						subpkgFeature->AddBinComponent(binscripts1, this);
					}
					delete binscripts1;
				 
					// Add command components for subpkgFeature
					coms = (*subpkgList)[j]->GetCmdList();
					if (coms->GetSize()>0)
					{
	 		 			for (k=0; k< coms->GetSize(); k++)
						{
							path = (cStringT)bindir;
							if (*((*coms)[k]->GetFullName()) != "")
								path /= *((*coms)[k]->GetFullName());
							else if (*((*coms)[k]->GetDefName()) != "")
								path /= *((*coms)[k]->GetDefName()) + version;
							else
								path /= *((*coms)[k]->GetTag()) + version;
						 	path += ".bat";
							commands1 += (cStringT)path;
						}
						subpkgFeature->AddCommandComponent(&commands1, this);
					}
				}
			}
		}
	}
	AddInstallLogComponent();
	AddPtcInfComponent();
	AddPtcUserComp();

 	return 0;
}

int PsWriteMSI::AddPtcLangFeatures(MSIFeature *pkgFeature, StringXArray *secArray)
{
	btkString featureName(pkgFeature->GetFeatureName());
	PsProduct * I = GetCurrentInstallable();
	StringXArray *SelectedLangs = I->GetLangs();
	StringXArray *secList = NULL;

	for (int i=0; i< SelectedLangs->GetSize(); i++)
	{
		// we have already taken care of usascii
		if (((*SelectedLangs)[i]) == "usascii")
		{
			continue;
		}

		btkString subFeatName(featureName + "_" + ((*SelectedLangs)[i]));
		// Add components from cdsections to pkgFeature
		// first reset and restore secList
		//
		secList = XNew StringXArray();
		*secList += (*secArray);

		// 4th arg is true so we only return sections that are lang specific
		ExpandMsiCdSectionList(secList, I, (*SelectedLangs)[i], TRUE);
		int secCount = secList->GetSize();
		if (secCount > 0)
		{
			PsWarning << "Add Langs for: " << subFeatName <<  
				" secCount: " << secCount << endl << endm;

#if 0
			for (int j=0; j < secCount; j++)
				PsWarning << subFeatName << "[" << j << "]: " <<
					(*secList)[j] << endl << endm;
#endif
 
			MSIFeature *subpkgFeature = pkgFeature->GetSubFeature(subFeatName);
			if (subpkgFeature == NULL)
			{
				subpkgFeature = XNew MSIFeature();
				subpkgFeature->SetFeatureName(BTKCHARP(subFeatName));
				subpkgFeature->SetFeatureDesc(BTKCHARP(subFeatName));
				subpkgFeature->SetParent(BTKCHARP(featureName));
				subpkgFeature->SetDisplay(0);	// we will hide you!
				subpkgFeature->SetLevel(5);	// we will disable you!
				pkgFeature->AddSubFeature(subpkgFeature);
			}
			else
			{
				PsWarning << subFeatName << " already exits" << endl << endm;
			}

			subpkgFeature->AddSectionComponent(secList, this);

			// Now for each one of these subFeatures, we have to add
			// an entry for it in the Condition table
			//
			if (subpkgFeature->GetLangID() == "")
			{
				btkString langID;
				if (((*SelectedLangs)[i]) == "chinese_cn")
				{
					langID = "cn";
				}
				else if (((*SelectedLangs)[i]) == "chinese_tw")
				{
					langID = "tw";
				}
				else
				{
					// for the rest of the languages, take the first two letters
					// french-> fr, german->ge,  and so
					langID = ((*SelectedLangs)[i])(0,1); 
				}

				subpkgFeature->SetLangID(langID);
			}
		}
		delete secList;
	}

	return(TRUE);
}

int PsWriteMSI::AddInstallLogComponent()
{
	MSIFeature * feature = GetFeature("proe_base");
	if( feature !=NULL)
	{
		MSIComponent * component = XNew MSIComponent();
		component->SetComponentName("PTCuninstall");
		btkString dirID;
		AddDirectory("uninstall", dirID);
		component->SetDirectory(dirID);
		component->SetFullPath("uninstall");
		component->SetKeyPath("PTCinstlog.txt");
		MSIFile * file = XNew MSIFile();
	 	btkFSEntry fullpath = "uninstall";
		fullpath /= "instlog.txt";
		file->SetName(fullpath);
		file->SetFileName("PTCinstlog.txt");
		file->SetFilePath(fullpath);
		file->SetComponent("PTCuninstall");
		btkFSEntryInfo fileinfo(fullpath);
		file->SetSize(3000);
		component->AddFile(file);
		feature->AddComponent(component);
	}
	return 0;
}

int PsWriteMSI::AddPtcInfComponent()
{
	MSIFeature * feature = GetFeature("proe_base");
	if( feature !=NULL)
	{
		MSIComponent * component;
		component = FindComponentPath("ptc_inst");
		if (component == NULL)
		{
			component = XNew MSIComponent();
			component->SetComponentName("PTCptc_inst");
			btkString dirID;
			AddDirectory("ptc_inst", dirID);
			component->SetDirectory(dirID);
			component->SetFullPath("ptc_inst");
			component->SetKeyPath("PTCptc.inf");
			feature->AddComponent(component);
		}
		MSIFile * file = XNew MSIFile();
	 	btkFSEntry fullpath = "ptc_inst";
		fullpath /= "ptc.inf";
		file->SetName(fullpath);
		file->SetFileName("PTCptc.inf");
		file->SetFilePath(fullpath);
		file->SetComponent(component->GetComponentName());
		btkFSEntryInfo fileinfo(file->GetFilePath());
		file->SetSize(3000);
		component->AddFile(file);
	}
	return 0;
}



int PsWriteMSI::AddPtcUserComp()
{
        // turn this off for now! we might not use it afterall
#if 0

	MSIFeature * feature = GetFeature("proe_base");
	if( feature !=NULL)
	{
		MSIComponent * component = XNew MSIComponent();
		component->SetComponentName("PTCCurrentUser");
		component->SetDirectory("INSTALLDIR");
		component->SetFullPath("");
		component->SetKeyPath("PTCUserRegKey");
		component->SetAttributes(4);
		feature->AddComponent(component);
	}
#endif 
	return 0;
}

MSIComponent * PsWriteMSI::FindComponentPath(const char * path)
{
	int idx;
    for (int i=0; i<Features.GetSize(); i++)
    {
		idx = Features[i]->FindPath(path);
		if (idx != -1) 
			return((*Features[i]->GetComponents())[idx]);	
		for (int j=0; j< Features[i]->GetSubFeatures()->GetSize(); j++)
	    {
			idx = (*Features[i]->GetSubFeatures())[j]->FindPath(path);
			if (idx != -1)
				return((*(*Features[i]->GetSubFeatures())[j]->GetComponents())[idx]);
		}		
    }
    return NULL;
}

int PsWriteMSI::AddComponentName(const char * name)
{
 
	FileName * filename = XNew FileName();
	filename->SetName(name);
	filename->SetMax(1);
	ComponentNames += filename;
	return 0; 
}

int PsWriteMSI::AddFileName(const char * name)
{
 
	FileName * filename = XNew FileName();
	filename->SetName(name);
	filename->SetMax(1);
	FileNames += filename;
	return 0;
}

int PsWriteMSI::AddShortName(const char * name)
{
 	FileName * filename = XNew FileName();
	filename->SetName(name);
	filename->SetMax(1);
	ShortNames += filename;
	return 0;
}

int PsWriteMSI::AddDirectoryName(const char * name)
{
	FileName * filename = XNew FileName();
	filename->SetName(name);
	filename->SetMax(1);
	DirectoryNames += filename;
	return 0;
}

// fix mem problem by not returning locally declared btkString
// add a Directory row to Directory structure
void PsWriteMSI::AddDirectory(const char *name, btkString &newname)
{
	MSIDirectory * old = FindDirectory(name);

 	if (old == NULL)
	{
	 	btkFSEntry path(name);
		btkString dirname = path.GetTail();
		MSIDirectory * Parent = XNew MSIDirectory();
		btkString parent = path.GetHead();

		if (dirname == "")		
		{
			dirname = name;
		}

		MSIDirectory * dir = XNew MSIDirectory();
		dir->SetFullName(name);
		btkString shortName("");
		btkFSEntry newPath(GetCurrentInstallable()->GetLoadpoint()->GetLoadpoint());
		newPath /= path;
		PsGetShortFName(newPath, shortName);
		dir->SetDefaultName(shortName);
		// lookup the dirname; if already there, add _"counter" to it
		FindDirectoryName(dirname);
		
		dirname.Substitute("-","_");
		dirname = "PTC" + dirname; 
		dir->SetDirName(dirname);

		if (parent != "")
		{
			Parent = FindDirectory(parent);
			//if parent exists
			if (Parent != NULL)
			{
				dir->SetParent((cStringT)(Parent->GetDirName()));
			}
			else
			{
				btkString parentID;
				AddDirectory((cStringT)parent, parentID);
				dir->SetParent(parentID);
			}
		}
		else
		{
			dir->SetParent("INSTALLDIR");
		}
		Directories += dir;
		newname = dirname;
	}
	else
	{ 
		newname = old->GetDirName();
	}
}

MSIDirectory * PsWriteMSI::FindDirectory(const char * name)
{
 	for (int i=0; i<Directories.GetSize(); i++)
	{
	 	btkString full_name(Directories[i]->GetFullName());
		if (full_name == name)
		{
			return Directories[i];
		}
	}
	return NULL;
}

void PsWriteMSI::FindComponentName(btkString & name)
{
	char buf[10];
	int max = 0;

    for (int i=0; i<ComponentNames.GetSize(); i++)
    {  
		if (ComponentNames[i]->GetName() == name)
		{
			ComponentNames[i]->AddMax();
			max = ComponentNames[i]->GetMax();
		}
			
    }
	if (max)
	{
		// since the original already exists, make a new one with _"max"
		btk_sprintf(buf, "_%d", max);
		name += buf;
	}
	else
	{
		AddComponentName(name);
	}
} 

void PsWriteMSI::FindFileName(btkString & name)
{
	btkString filename(name);
	char buf[10];
	int max = 0 ;

	for (int i=0; i<FileNames.GetSize(); i++)
	{  
		if (FileNames[i]->GetName() == name)
		{
			FileNames[i]->AddMax();
			max = FileNames[i]->GetMax();
		}		
	}
	if (max)
	{
		btk_sprintf(buf, "%d", max);
		filename = name + btkString("_") + buf;
	}
	else
		AddFileName(name);

	// set the btkString & to filename instead of returning it
	name = filename;
} 

int PsWriteMSI::FindShortName(const char * name)
{
    
    for (int i=0; i<ShortNames.GetSize(); i++)
    {  
		if (ShortNames[i]->GetName() == name)
		{
			ShortNames[i]->AddMax();
			return 	ShortNames[i]->GetMax();
		}
			
    }
    return 0;
} 

void PsWriteMSI::FindDirectoryName(btkString & name)
{
    char buf[10];
	int max = 0;

    for (int i=0; i<DirectoryNames.GetSize(); i++)
    {  
		if (DirectoryNames[i]->GetName() == name)
		{
			DirectoryNames[i]->AddMax();
			max = DirectoryNames[i]->GetMax();
		}			
    }
	if (max)
	{
		// since the original already exists, make a new one with _"max"
		btk_sprintf(buf, "_%d", max);
		name += buf;		
	}
	else
	{
		AddDirectoryName(name);
	}
}     
 
int PsWriteMSI::AddIconEntry(MSIHANDLE& hDB)
{
	MSIHANDLE hViewI,  hRecord;
	int ret;

	ret = MsiDatabaseOpenView(
		hDB, TEXT("Select Name, Data FROM Icon"), &hViewI);

	btkString message(" proe.ico ");
	ret = MSIViewOpenError(ret, (cStringT)message);
	if (!ret)
	{
		return FALSE;
	}

	MsiViewExecute(hViewI,0);
	hRecord = MsiCreateRecord(2);
	btkFSEntry ico_path(GetCurrentInstallable()->GetLoadpoint()->GetLoadpoint());
	ico_path /= "install/nt/proe.ico";

	// load file to db only if it exists!
	if (ico_path.Exists() && (hRecord != NULL))
	{
		MsiRecordSetString(hRecord, 1, "proe.ico");
		MsiRecordSetStream(hRecord, 2, (cStringT)ico_path);

		ret = MsiViewModify(hViewI, MSIMODIFY_INSERT, hRecord);
		MSIViewModifyError(ret, (cStringT)message);
	}
	else
	{
		PsWarning << ico_path << " doesn't exist or hRecord=NULL" << endl << endm;

	}

	MsiViewClose(hViewI);

	if (ret == ERROR_SUCCESS)
		return TRUE;
	else
		return FALSE;
}

int PsWriteMSI::Print()
{
	for (int i=0; i< Features.GetSize(); i++)
	{
		PsInfo <<"Feature " << i<< ":"<<  Features[i]->GetFeatureName()<<endl<<endm;
		Features[i]->Print();
	}
	return 0;
}

void PsGetShortFName(const btkFSEntry &path, btkString & sName)
{
	PsGetSysShortFName(path, sName);
	sName += "|";
	sName += path.GetTail();
}

int PsGetExtensions(int *count, StringXArray *Ext)
{
	btkIFileStream IFS;
	btkFSEntry path(GetCurrentInstallable()->GetLoadpoint()->GetLoadpoint());
	path /="ptc_inst";
	path /="extension.csv";
	btkString oneline;
	int num =0;

	if (IFS.Open(path))
	{
		while(IFS.Read(oneline))
		{
			btkString ext = oneline(0, oneline.Pos(",")-1);
			*Ext += ext;
		 	num++;
		}
		IFS.Close();
		*count = num;
		return TRUE;
	}
	return FALSE;
}

void PsGetMsiVer(btkString & value)
{
	char dcode_buf[32];
	long int dcode = (GetPsCD()->GetDateCode())->Getdcode();
	// dcode is expected to be a 7 digit number, 2000120
	int build_ver = dcode - 2000000; // so we get 120, 1120, 2120 and so on
	// I had to hard code this one!! change it for rev 23
	// I-03 was 22, J-01+ is version 23
	btk_sprintf(dcode_buf, "23.1.%d", build_ver);
	value = dcode_buf;
}
#endif /* PTC_USE_MSI */
#endif /* WINDOWS_32 */
