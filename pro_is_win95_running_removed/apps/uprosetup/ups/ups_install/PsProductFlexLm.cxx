/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*\
 
  PsProductFlexLm.cxx
 
  Pro/SETUP functions
 
  Date      Release   Name  Ver.   Comments
  --------- -------   ----  ----   --------
  10-Dec-98           MYA          Created
  16-Feb-99 I-03-02   JJE   $$1    Group submission
  12-Mar-99           JJE          AddInstAction
  18-Mar-99           JJE          Use XNew
  18-Mar-99 I-03-05   JJE   $$2    Group Submission
  26-Mar-99           TWH          Use debug streams
  29-Mar-99 I-03-06   JJE   $$3    Group Submission
  30-Mar-99           MAZ          Add CopyFlexDLLFiles() 
  12-Apr-99 I-03-07   JJE   $$4    Group Submission
  30-Apr-99           TWH          Add TempFile,AddInstActions,
                                   Add PreCheck,Add more Dll logic
  04-May-99           MAZ          turn off windows32 section for now
  07-May-99           TWH          Add IsServerRunning,StartFlexServer
  11-May-99 I-03-09   JJE   $$5    Group Submission
  17-May-99           TWH          Move license save;message_dialogs can't be used
                                   inside Inst_action thread;Add FixServerDaemonLines
  18-May-99           TWH          Modify destructor
  20-May-99           TWH          externalize action headers,InstError chgs
  20-May-99           TWH          Cleanup build warnings
  25-May-99           TWH          Chg PostFunc prototype
  07-Jun-99 I-03-10   JJE   $$6    Group Submission
  30-Jul-99           MAZ          Add a call to PsGetInstLogPath()
  16-Aug-99           MYA          Add PsSetPerm
  17-Aug-99 I-03-12+  JJE   $$7    Group Submission
  20-Aug-99           MAZ          Call PsRegkeyStoreValueFromString()
  24-Aug-99 I-03-13   JJE   $$8    Group Submission
  20-Sep-99           MAZ          Complete server startup logic
  27-Sep-99 I-03-16   JJE   $$9    Group Submission
  29-Sep-99           MAZ          Use plp_GetNextFeatureLineInfo()
  30-Sep-99           MAZ          Fix WINDOWS95 and WINDOWS32 macros
  01-Oct-99           MAZ          Fix IsServerRunning and StartFlexServer 
  04-Oct-99 I-03-17   JJE   $$10   Group Submission
  13-Oct-99           MAZ          Call parent postfunc in the PostFunc()
  15-Oct-99           MAZ          Log LMGRD_ARGS in instlog
  20-Oct-99           MAZ          ptcd_path should point to the obj dir 
  22-Oct-99 I-03-18+  JJE   $$11   Group Submission
  11-Nov-99           JJE          Shutdown lic server on update
  12-Nov-99           JJE          Use PsSystemCall
  15-Nov-99           JJE          Don't add dll action if not needed
  16-Nov-99 I-03-21   JJE   $$12   Group Submission
  19-Nov-99 I-03-21+  TWH   $$13   ptcd_path includes ptcd and path to ptc.opt
  29-Nov-99 I-03-21+  TWH   $$14   Quote ptcd_path when necessary
  22-Dec-99           MAZ          Always call FlexDLLs_ACTION (SPR 802158)
  22-Dec-99 I-03-24+  JJE   $$15   Group Submission
  11-Jan-00           TWH          Changes for Flex 7.0
  12-Jan-00           JJE          Services message should be warning
  12-Jan-00 I-03-26+  JJE   $$16   Group Submission
  27-Jan-00           TWH/MAZ      Fix flexlm.cpl removal; flex 7 startup
  27-Jan-00           MAZ          Fix SPR 808696 (win98 flex startup) 
  02-Feb-00 I-03-26+  JJE   $$17   Group Submission
  17-Feb-00           MYA          For AE install or PDM  cd, uncheck Flex
  24-Feb-00 I-03-27+  TWH   $$18   Group Submission
  08-Mar-00           MAZ          Fix SPR 813530
  15-Mar-00           TWH          Don't overwrite ptc.opt
  17-Mar-00 I-03-28+  JJE   $$19   Group Submission
  24-Mar-00 J-01-04+  TWH   $$20   Comment out reference to ScreenFlexFW
  18-May-00           TWH          Fix missing "+" in ptc.opt
  31-May-00 I-03-28+  JJE   $$21   Group Submission
  11-Jul-00           TWH          Fix _HOSTNAME_ substitution for triad
  10-Jul-00 I-03-30+  JJE   $$22   Group Submission
  31-Jul-00           MAZ          Use the new EzNet Area
  10-Aug-00           MAZ          Use Prop.Set instead of Prop.Add
  23-Aug-00 J-01-15   TWH   $$23   Group Submission
  13-Sep-00           TWH          Add ::HasCountedFeature
  13-Sep-00 J-01-18   TWH   $$24   Group Submission
  02-Oct-00 J-01-19   TWH   $$25   fix above; 843382
  09-Oct-00           TWH          fix daemon line 842464
  18-Oct-00 J-01-20+  TWH   $$26   Group Submission
  29-Nov-00           TWH          Add triad 1st startup message
  30-Nov-00 J-01-22   JJE   $$27   Group Submission
  20-Dec-00           MAZ          NT/Win StartOnReboot (SPR 847835, 842666)
  08-Jan-01 J-01-25   TWH   $$28   Group Submission
  14-Mar-01 J-01-29   TWH   $$29   Add ::HasServerDaemonLines
  28-Mar-01 J-01-30+  ALG   $$30   Remove PsSetPerm on Windows (SPR 824322)
  09-Apr-01           TWH          set PossibleArchs to mc_type
  09-Apr-01 J-01-31   TWH   $$31   Use PS_MESSAGE_*
  03-Jul-01 J-01-35   TWH   $$32   Fix triad failure message 822174
  21-Sep-01 J-03-09   jas   $$33   Removed WINDOWS_95 macro
  09-Jan-02 J-03-17   TWH   $$34   Fix IsServerRunning hostname strcmp
  19-Feb-02 J-03-19   ALG   $$35   Fix IsServerRunning some more (Empty lm_str)
  05-Mar-02           ALG          SPR 927737 UNIX - Don't stop svr for new inst
  05-Mar-02 J-03-20   ALG   $$36   Don't "Backup License File" for new inst
  11-Mar-02 J-03-21   ALG   $$37   SPR 922490 Win32 - Check for existing server
  25-Apr-02 J-03-24   MAZ   $$38   relmem allocated memory
  16-Sep-02 J-03-34   ALG   $$39   SPR 976478: Open write perm on TempFile
  08-Oct-02 J-03-35   JJE   $$40   Make sure the license file has the right cpuid
  15-Nov-02 J-03-38   MAZ   $$41   SPR 989040: use new PreCheck admin err msg
  05-Dec-02 J-03-39   JJE   $$42   Return false if we throw Flex_another_srv_running
  17-Dec-02 J-03-39   JJE   $$43   SPR 993967: Don't fail if lic file is already trans
  16-Jan-03 J-03-41   MAZ   $$44   SPR 998329: Fix SERVER line checking
  18-Mar-03 K-01-03   MAZ   $$45   SPR 1005103: Fix StartFlexServer return code
  02-Jun-03 K-01-07+  TWH   $$46   Fix Servershutdown possible hang
  26-Jun-03 K-01-10   Chris $$47   Removed char ** casts from relmem
  07-Oct-03 K-01-16   TWH   $$48   Chg startserver error msg
  15-Mar-04 K-01-25   TWH   $$49   Chg FixServerDaemonLines proto
  07-Nov-05 K-03-35   TWH   $$50   Add IsServerActivelyBorrowingLicenses
  14-Jul-06 L-01-12   ksi   $$51   Unicode changes
  16-Oct-07 L-01-40   TWH   $$52   Add cmdlineparams to reg entries for Vista
  19-Feb-09 L-03-28   TWH   $$53   Support multi hostid list
  03-Jun-09 L-03-33   TWH   $$54   Vista use ip_addr
  20-Jul-09 L-05-04   TWH   $$55   comment out #54 
  24-May-10 L-05-23   TWH   $$56   Enhance Setup Project
  19-Jul-10 L-05-27   TWH   $$57   update for flexadmin
  23-Jul-10 L-05-27+  TWH   $$58   Fix missing define of servicename
  11-Oct-10 L-05-33   TWH   $$59   Limit FlexAdmin Success/Webstart to New install
  17-Jan-11 L-05-40   TWH   $$60   Rebranding
  11-Mar-11 L-05-42   TWH          Admin tweak for multiproduct cdimage
  14-Mar-11 L-05-42   TWH   $$61   Delete __LOADPOINT__ in PostInstFunc
  28-Mar-11 L-05-44   TWH   $$62   HostidListGet add second argument
  04-Nov-11 P-10-12   TWH   $$63   Replace psOSGetHostname
                                   Hostname made uppercase before write to license files on disk
  09-Jul-18 P-60-09   KSV   $$64   Updated for IPv6

\*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#include <Ps.h>
#include <xarray.h> // for removal of flexcpl
#define FLEXSVCNAME "FLEXlm server for PTC"

static btkProcess *flexProcess = NULL;
const float _6dot1_flexver = 6.1;
const float _7dot0_flexver = 7.0;

PsProductFlexLm::~PsProductFlexLm()
{
	if (TempFile.IsFile())
		TempFile.Erase();
	// FW also has a TempFile so call its' destructor
#if 0
	ScreenFlexFW *S = (ScreenFlexFW *)ScreenByTag("ScreenFlexFW");
	delete S;
#endif
}

void PsProductFlexLm::Initialize()
{
	btkString mc_arch;
	StringXArray archs;

	// for AE install and pdm cd, uncheck Flex on ScreenWelcome
	if (PsCd::AeMode() || GetPsCD()->GetInf().Product  == "propdm")
		Prop.Set("Installed", FALSE);

	AddInstAction( PS_NO_ASSOC_SCREEN,
		TranslateWcharMsg(IA_licmgrcfg,
		"License Management Configuration"),
	        PS_Product_Postfunc_ACTION,
	        (ui_action_func_t) NULL );

	mc_arch = psOSGetMctype();
	archs += XNew btkString(mc_arch);
	Prop.Set("PossibleArchs", archs);

#if OPER_SYS == WINDOWS_32
	// populate the StringXArray FlexDLLs for windows
	if (GetVersion() < _7dot0_flexver)
		FlexDLLs += XNew btkString("flexlm.cpl");

	float flex_ver = PsCd::FlexlmVersion();
	PsWarning << "current flexver: " << flex_ver << endl << endm;
	PsWarning << "default flexver: " << _6dot1_flexver << endl << endm;

	if (flex_ver < _6dot1_flexver) 
	{
		// the lmgr dll files are only for flex < 6.1
		PsWarning 	<< "flex_ver < 6.1 -> Adding lmgr*.dll files" 
					<< endl << endm;
		FlexDLLs += XNew btkString("lmgr325a.dll");
		FlexDLLs += XNew btkString("lmgr325c.dll");
	}
	if (FlexDLLs.GetSize() > 0)
	{
		AddInstAction( PS_NO_ASSOC_SCREEN,
			TranslateWcharMsg(IA_flexdlls,"Copy Flexlm system files"),
			PS_Copy_FlexDLLs_ACTION, (ui_action_func_t) CopyFlexDLLFiles);
	}
#endif

	AddInstAction( PS_NO_ASSOC_SCREEN, 
		TranslateWcharMsg(IA_flexstartserver,"Startup License Server"),
		PS_Start_Server_ACTION, (ui_action_func_t) StartFlexServer);	
}

// Execute this function after all other install stuff
// is finished.
//

int PsProductFlexLm::PreFunc(void **data)
{
#if OPER_SYS == WINDOWS_32
	btkFSEntry file;
	btkString arch;

	PsGetVar(PS_PRO_MACHINE_TYPE, &arch);
	file = GetLoadpoint()->GetLoadpoint();
	file /= (cStringT)arch;
	file /= "obj/flexlm.cpl";

	if (file.Exists() && (GetVersion() >= _7dot0_flexver))
	{
		AddInstAction( PS_NO_ASSOC_SCREEN,
			TranslateWcharMsg(IA_flexrmcpl,"Remove FLEXlm Control Panel"),
			PS_Modify_InstProps_ACTION, (ui_action_func_t) RmFlexlmCplFile);
	}
#endif
	ConvertDBAdd("__LMGRD_ARGS__", GetLmgrdParameters());
	ConvertDBAdd("__HOSTNAME1__", Prop("ScreenProdFlex.Triad1_Host").ToUpper());
	ConvertDBAdd("__HOSTNAME2__", Prop("ScreenProdFlex.Triad2_Host").ToUpper());
	ConvertDBAdd("__HOSTNAME3__", Prop("ScreenProdFlex.Triad3_Host").ToUpper());
	return (TRUE);
}


// Check for admin/root priv 
//
int PsProductFlexLm::PreCheck(void **data)
{
	PsWarning << "Flex_precheck called"<<endm;
	if (! PsCd::PtcutilMode())
	{
		btkString plat;
		PsGetVar(PS_PRO_MACHINE_TYPE, &plat);
		if (! PsSectionExists(plat))
		{
			PsInfo << "Missing Platform section... failing FlexLm::PreCheck"
				<< endl << endm;
			wchar_t *title, *msg;
			title = TranslateWcharMsg(Ps_Error,"Error");
			msg = TranslateWcharMsg(Flex_missing_plat,
			"Error: Platform software is not on this CDROM", BTKCHARP(plat));
			ui_message_dialog(PS_MESSAGE_ERROR, title, msg,
				UI_LEFT,UI_MESSAGE_OK,UI_MESSAGE_OK);
			relmem(&msg);
			relmem(&title);
			return (FALSE);
		}
	}
#if OPER_SYS == WINDOWS_32
	if ( (!psRegkeyCanWriteKeys(NULL)) || (! psNTServHavePermission()) )
	{
		wchar_t *title, *msg;
		btkString prod(GetName());
		title = TranslateWcharMsg(Ps_Error, "Error");
		// SPR 989040: use new PreCheck admin err msg 
		msg = TranslateWcharMsg(Prod_PreCheck_fail_perm,
			"The installation of FlexLM requires administrative priviledges. Please log in as an administrator to start the installation.", BTKCHARP(prod));

		ui_message_dialog(PS_MESSAGE_ERROR, title, msg, UI_CENTER,
			UI_MESSAGE_OK, UI_MESSAGE_OK);

		// relmem allocated memory 
		relmem( &msg);
		relmem( &title);
		return (FALSE);
	}
#endif
	return( TRUE );
}

int PsProductFlexLm::ScreenProdPostFunc(bool precheck, int *return_this_result)
{
	PsInfo << "PsProductFlexLm::ScreenProdPostFunc() started..."
	       << endl << endm;

	// Call the generic product function first
	if (PsProduct::ScreenProdPostFunc(precheck, return_this_result) == FALSE)
	{
		return(FALSE);
	}

	bool stopServer;

#if OPER_SYS == UNIX
	// SPR 927737: On UNIX, don't try to stop server during a NEW installation
	if (LP_IS_NEW(GetLoadpoint()->GetInstallAction()))
		stopServer = FALSE;
	else
		stopServer = TRUE;
#else
	// SPR 922490: On Windows, check for existing license server
	const btkString keyName("HKEY_LOCAL_MACHINE\\SOFTWARE\\FLEXlm License Manager\\FLEXlm server for PTC");
	const btkString keyValue("Lmgrd");
	btkString keyString(psRegkeyGetStringFromValue("", keyName, keyValue));
	if (keyString.IsEmpty())
	{
		stopServer = FALSE;
	}
	else
	{
		stopServer = TRUE;
	}
#endif

	// Remove it in case it was added before
	PsInfo << "\tRemoving InstAction: PS_Stop_Server_ACTION" << endl << endm;
	ClearInstActionsByType(PS_Stop_Server_ACTION);
	if (stopServer)
	{
		PsInfo << "\tAdding InstAction: PS_Stop_Server_ACTION" << endl << endm;
		AddInstAction(PS_NO_ASSOC_SCREEN,
			TranslateWcharMsg(IA_FlexShutdown, "Shutdown License Server"),
			PS_Stop_Server_ACTION, (ui_action_func_t)StopFlexServer);
	}

	// This is the same for ALL platforms
	PsInfo << "\tRemoving InstAction: PS_Backup_Files_ACTION" << endl << endm;
	ClearInstActionsByType(PS_Backup_Files_ACTION);
	if (! LP_IS_NEW(GetLoadpoint()->GetInstallAction()))
	{
		PsInfo << "\tAdding InstAction: PS_Backup_Files_ACTION" << endl << endm;
		AddInstAction(PS_NO_ASSOC_SCREEN,
			TranslateWcharMsg(IA_flexbaklic, "Backup License File"),
			PS_Backup_Files_ACTION, (ui_action_func_t)BackupFlexFile);
	}

	// This doesn't really matter
	if (return_this_result)
		*return_this_result = TRUE;

	PsInfo << "PsProductFlexLm::ScreenProdPostFunc() done" << endl << endm;
	return(TRUE);
}

void PsProductFlexLm::WriteOptFile(const btkFSEntry &lp)
{
	btkFSEntry Opt = (cStringT)lp;
	btkFSEntry Rpt = (cStringT)lp;
	Opt /= "licensing/ptc.opt";
	Rpt /= "licensing/ptcreport.log";
	if (Opt.IsLinkToFile())
		return;
	// write log file
	btkOFileStream OFS;
	if( OFS.Create(Opt) )
	{
		OFS << "REPORTLOG +\"";
		OFS << Rpt << "\"" << endl;
		OFS << "TIMEOUTALL 7200" << endl;
		OFS.Close();
	}
}

bool PsProductFlexLm::HasServerDaemonLines(const btkFSEntry &in)
{
	// check for Server & Daemon lines
	btkIFileStream IFS;
	bool status = FALSE;
	if (IFS.Open(in))
	{
		btkString OneLine;
		bool s, d;
		s = FALSE;
		d = FALSE;
		while(IFS.Read(OneLine))
		{
			if (OneLine.Match("SERVER *"))
				s = TRUE;
			else if (OneLine.Match("DAEMON *"))
				d = TRUE;
			if (s && d)
				break;
		}
		IFS.Close();
		if (s && d)
			status = TRUE;
	}
	return (status);
}

bool PsProductFlexLm::FixServerDaemonLines(bool silent)
{
	// check Server and Daemon lines of "TempFile"
	// and replace __HOSTNAME__ & __PTCD_PATH__
	btkIFileStream IFS;
	bool didsub = FALSE;
	btkString MyHostname, hostmatch, hostmatch_upper;
	StringXArray Cpuids;
	int i, max, found;

	char *p_macAddrList;
	if (HostidListGet(&p_macAddrList, NULL) == 1)
	{
		hostmatch = p_macAddrList;
		relmem (&p_macAddrList);
		for (i = 0, max = hostmatch.GetNWords(",");
			i < max; i++)
		{
			hostmatch_upper = "*";
			hostmatch_upper += (cStringT)hostmatch.GetWord(i, ",");
			hostmatch_upper += "*";
			Cpuids += (cStringT)hostmatch_upper.ToUpper();
		}
	}

#if OPER_SYS == WINDOWS_32
	
#if 0
	if (IsVista())
	{
		//IPv6
		//Vista to Vista comms may need workaround of ipv4 address in 
		//the license file and the socket command on the client side.
		btkGetCurHostName(hostmatch);
		if (!btkNetHostNameToAddrString(MyHostname, hostmatch))
			btkGetCurHostName(MyHostname);
	}
	else
#endif
	{
		btkGetCurHostName(MyHostname);
	}
#else
	btkGetCurHostName(MyHostname);
#endif

	if (IFS.Open(TempFile))
	{
		btkOFileStream OFS;
		btkFSEntry mynewfile;
		mynewfile.MakeTmpName("ptci");
		btkString OneLine, PTC_HARDWARE_ID, PTC_SERVER_HOST, partial;
		int server_line_count = 0;

		if ( OFS.Create(mynewfile) )
		{
			i = 0, max = Cpuids.GetSize();
			while(IFS.Read(OneLine))
			{
				btkString OneLineUpper(OneLine.ToUpper());

				i = 0;
				if (OneLine.Match("SERVER*"))
				{
					server_line_count++;
					// don't substitute... rewrite line entirely.
					// start by grabbing the PTCHOSTID= info
					//
					partial = OneLineUpper.GetWord(1, "PTC_HOSTID=", STR_F_USE_SEPS_AS_STRING);
					PTC_HARDWARE_ID = partial.GetWord(0, " ");
					PTC_SERVER_HOST = OneLineUpper.GetWord(0, " PTC_HOSTID=", STR_F_USE_SEPS_AS_STRING);
					PTC_SERVER_HOST.Substitute("SERVER ", "");
					for (i = 0, found = 0; i < max; i++)
					{
						if (OneLineUpper.Match(Cpuids[i]))
						{
							OneLine = "SERVER " + MyHostname.ToUpper() + btkString(" PTC_HOSTID=") + PTC_HARDWARE_ID + btkString(" 7788");
							found = 1;
							didsub = TRUE;
							break;
						}
					}
					if ( OneLine.Match("SERVER*__HOSTNAME1__*") )
					{
						if (found)
						{
							ConvertDBAdd("__HOSTNAME1__", MyHostname.ToUpper());
							Prop.Set("ScreenProdFlex.Triad1_Host", MyHostname);
						}
						else
							OneLine = "SERVER __HOSTNAME1__ PTC_HOSTID=" + PTC_HARDWARE_ID + btkString(" 7788");
						Prop.Set("ScreenProdFlex.Triad1_HW_ID", PTC_HARDWARE_ID);
					}
					else if ( OneLine.Match("SERVER*__HOSTNAME2__*") )
					{
						if (found)
						{
							ConvertDBAdd("__HOSTNAME2__", MyHostname.ToUpper());
							Prop.Set("ScreenProdFlex.Triad2_Host", MyHostname);
						}
						else
							OneLine = "SERVER __HOSTNAME2__ PTC_HOSTID=" + PTC_HARDWARE_ID + btkString(" 7788");
						Prop.Set("ScreenProdFlex.Triad2_HW_ID", PTC_HARDWARE_ID);
					}
					else if ( OneLine.Match("SERVER*__HOSTNAME3__*") )
					{
						if (found)
						{
							ConvertDBAdd("__HOSTNAME3__", MyHostname.ToUpper());
							Prop.Set("ScreenProdFlex.Triad3_Host", MyHostname);
						}
						else
							OneLine = "SERVER __HOSTNAME3__ PTC_HOSTID=" + PTC_HARDWARE_ID + btkString(" 7788");
						Prop.Set("ScreenProdFlex.Triad3_HW_ID", PTC_HARDWARE_ID);
					}
					else
					{
						if (server_line_count == 1 &&
							Prop("ScreenProdFlex.Triad1_Host").IsEmpty() &&
							!PTC_SERVER_HOST.StripWhiteSpaces().StartsWith("__HOSTNAME"))
						{
							Prop.Set("ScreenProdFlex.Triad1_Host", PTC_SERVER_HOST.StripWhiteSpaces());
							ConvertDBAdd("__HOSTNAME1__", PTC_SERVER_HOST.StripWhiteSpaces().ToUpper());
							OneLine = "SERVER " + Prop("ScreenProdFlex.Triad1_Host").ToUpper() + btkString(" PTC_HOSTID=") + PTC_HARDWARE_ID + btkString(" 7788");
						}
						else if (server_line_count == 2 &&
							Prop("ScreenProdFlex.Triad2_Host").IsEmpty() &&
							!PTC_SERVER_HOST.StripWhiteSpaces().StartsWith("__HOSTNAME"))
						{
							Prop.Set("ScreenProdFlex.Triad2_Host", PTC_SERVER_HOST.StripWhiteSpaces());
							ConvertDBAdd("__HOSTNAME2__", PTC_SERVER_HOST.StripWhiteSpaces().ToUpper());
							OneLine = "SERVER " + Prop("ScreenProdFlex.Triad2_Host").ToUpper() + btkString(" PTC_HOSTID=") + PTC_HARDWARE_ID + btkString(" 7788");
						}
						else if (server_line_count == 3 &&
							Prop("ScreenProdFlex.Triad3_Host").IsEmpty() &&
							!PTC_SERVER_HOST.StripWhiteSpaces().StartsWith("__HOSTNAME"))
						{
							Prop.Set("ScreenProdFlex.Triad3_Host", PTC_SERVER_HOST.StripWhiteSpaces());
							ConvertDBAdd("__HOSTNAME3__", PTC_SERVER_HOST.StripWhiteSpaces().ToUpper());
							OneLine = "SERVER " + Prop("ScreenProdFlex.Triad3_Host").ToUpper() + btkString(" PTC_HOSTID=") + PTC_HARDWARE_ID + btkString(" 7788");
						}
					}
				}
				if (OneLine.Match("DAEMON*__PTCD_PATH__*"))
				{
					// ptcd_path should point to the obj dir 
					btkFSEntry ptcd_path(GetLoadpoint()->GetLoadpoint());
					btkFSEntry opt_path = (cStringT)ptcd_path;
					ptcd_path /= btkString(psOSGetMctype());
					ptcd_path /= "obj/ptc_d";
					opt_path  /= "licensing/ptc.opt";
#if OPER_SYS == WINDOWS_32
					ptcd_path += ".exe";
					btkString result;
					result = QuoteFSEntry(ptcd_path);
					result += " ";
					result += QuoteFSEntry(opt_path);
					OneLine.Substitute("__PTCD_PATH__", (cStringT)result);
#else
					ptcd_path += " ";
					ptcd_path += (cStringT)opt_path;
					OneLine.Substitute("__PTCD_PATH__", (cStringT)ptcd_path);
#endif
				}
				OFS << OneLine << endl;
			}
			OFS.Close();
			IFS.Close();

			if( ! didsub && !silent)
			{
				wchar_t *title, *msg;
				char *cpuid = NULL;

				title = TranslateWcharMsg(Ps_Error, "Error");
				HostidListGet(&cpuid, NULL);
				msg = TranslateWcharMsg(Flex_missing_my_SERVER,
				      "License file does not match this host.",cpuid);
				relmem(&cpuid);
				ui_message_dialog(PS_MESSAGE_ERROR,title,msg,
				       UI_LEFT,UI_MESSAGE_OK,UI_MESSAGE_OK);
				relmem(&title);
				relmem(&msg);
				PsInfo << "No SERVER line for this CPU." << endl << endm;
			}

			if (didsub && !silent)
			{
				// SPR 976478: Open write permissions on TempFile
				//             before trying to over-write it
				PsSetPerm(TempFile, strtol("644", NULL, 8), false);
				PsInfo << "FixServerDaemonLines: Set permissions on TempFile"
				       << endl << endm;

				bool copyStatus = mynewfile.CopyTo(TempFile);
				PsInfo << "FixServerDaemonLines: Over-write TempFile: ";
				if (copyStatus)
					PsInfo << "Success";
				else
					PsInfo << "Failed";
				PsInfo << endl << endm;
			}
		}
		else
			IFS.Close();

		mynewfile.Erase();
	}

	return (didsub);
}

bool PsProductFlexLm::HasCountedFeature()
{
	btkFSEntry license = GetTempFile();
	btkString lic = (cStringT)license;
	btkString lm_str;
	char *plplist, *endp;
	int max, count, idx;

	SetLM_LICENSE_FILE(&lic);
	plp_GetNextFeatureLineInfo("garbage_reset");
	plplist = plp_GetFeatureListByProdPrefix("");
	if (plplist == NULL) return FALSE;
	lm_str = plplist;
	max = lm_str.GetNWords(" ");
	idx = 0;
	while (max > idx)
	{
		btkString feat_name(lm_str.GetWord(idx, " "));
		idx++;
		endp = NULL;
		count = (int)strtol(BTKCHARP(feat_name),&endp, 10);
		if (strcmp(endp,"")==0)
			continue; // was an int
		plp_GetNextFeatureLineInfo(feat_name);
		count = atoi(plp_GetFeatureLineSeatCount());
		if (count)
			return TRUE;
	}
	return FALSE;
}

bool PsProductFlexLm::IsServerRunning()
{
	PsInfo << "PsProductFlexLm::IsServerRunning() started..." << endl << endm;

	btkFSEntry license = this->GetLoadpoint()->GetLoadpoint();
	license /= "licensing/license.dat";
	btkString lic = (cStringT)license;
	btkString lm_str, host;
	char *plplist;
	char **names, **ports;
	int max, count;

	PsInfo << "\tSet LM_LICENSE_FILE: " << lic << endl << endm;
	SetLM_LICENSE_FILE(&lic);
	plp_GetNextFeatureLineInfo("garbage_reset");

	plplist = plp_GetFeatureListByProdPrefix("");
	if (plplist == NULL) return(FALSE);
	lm_str = plplist;
	PsInfo << "\tlm_str = " << lm_str << endl << endm;
	max = lm_str.GetNWords(" ");

	if (max > 0)
	{
		btkString feat_name(lm_str.GetWord(0, " "));
		PsInfo << "\tfeat_name = " << feat_name << endl << endm;
		PsFlexFeature *F = XNew PsFlexFeature(feat_name);
		F->GetServerName(&count, &names);
		F->GetServerPort(&count, &ports);

		// Empty lm_str so that it's non-empty after the loop ONLY if
		// a server was found on the local machine
		lm_str.SetEmpty();

		for (int i = 0; i < count; i++)
		{
			host = names[i];
			host += ".";
			int pos = host.Pos(".");
			btkString tmpstr(host(0,pos -1).ToUpper());
			PsInfo << "\thost = " << tmpstr << endl << endm;
			// EzGetHostname returns full address like hestia.ptc.com
			// treat the return value of EzGetHostname like names[i]
			btkString hostname(EzGetHostname());
			hostname += ".";
			pos  = hostname.Pos(".");
			hostname = hostname(0,pos -1);
			PsInfo << "\tthis machine = " << hostname.ToUpper() << endl << endm;
			if (tmpstr == hostname.ToUpper())
			{
				lm_str = ports[i];
				lm_str += "@";
				lm_str += names[i];
			}
			if (i == 2) Triad = TRUE;
		}
		if (lm_str.GetLength() < 1)
		{
			PsInfo << "\tReturning FALSE" << endl << endm;
			return(FALSE);
		}
		SetLM_LICENSE_FILE(&lm_str);
		PsInfo << "\tSet LM_LICENSE_FILE: " << lm_str << endl << endm;
		plplist = plp_GetFeatureListByProdPrefix("");
		if (plplist == NULL)
		{
			PsInfo << "\tReturning FALSE" << endl << endm;
			return(FALSE);
		}
		lm_str = plplist;
		if (lm_str.GetNWords(" ") > 0)
		{
			PsInfo << "\tReturning TRUE" << endl << endm;
			return(TRUE);
		}
	}
	PsInfo << "\tReturning FALSE" << endl << endm;
	return(FALSE);
}

#if OPER_SYS == WINDOWS_32
int PsProductFlexLm::IsServiceInstalled()
{
	return (psNTServDoesExist(FLEXSVCNAME));
}

// This checks if the SERVICE STATE != STOPPED
// not if we can connect and talk to it.
int PsProductFlexLm::IsServiceRunning()
{
	return (psNTServIsRunning(FLEXSVCNAME));
}
#endif

bool PsProductFlexLm::IsServerActivelyBorrowingLicenses()
{
	if (IsServerRunning()) // also sets license file value properly
	{
		int count = 0;
		if (plp_GetBorrowCount(&count) == 0)
		{
			if (count) return TRUE;
		}
	}
	return (FALSE);
}

//
// PsProductFlexAdmin
//
#if OPER_SYS == WINDOWS_32
void PsProductFlexAdmin::Initialize()
{
	btkString mc_arch;
	StringXArray archs;

	AddInstAction( PS_NO_ASSOC_SCREEN,
		TranslateWcharMsg(IA_licmgrcfg,
		"License Management Configuration"),
	        PS_Product_Postfunc_ACTION,
	        (ui_action_func_t) NULL );

	mc_arch = psOSGetMctype();
	archs += XNew btkString(mc_arch);
	Prop.Set("PossibleArchs", archs);

	AddInstAction( PS_NO_ASSOC_SCREEN, 
		TranslateWcharMsg(IA_flexstartserver,"Startup License Server"),
		PS_Start_Server_ACTION, (ui_action_func_t) StartFlexAdminServer);	
}

// Check for admin/root priv 
//
int PsProductFlexAdmin::PreCheck(void **data)
{
	wchar_t *title = NULL;
	wchar_t *msg = NULL;
	bool lmgrd_running = FALSE;

	PsWarning << "FlexAdmin_precheck called"<<endm;

	// does check for machine type section
	// does check for psRegkeyCanWriteKeys
	// does check for psNTServHavePermission
	//
	if (! PsProductFlexLm::PreCheck(data))
	{
		return FALSE;
	}
	if (IsServiceRunning())
	{
		PsWarning << "PsProductFlexAdmin found existing FLEXAdmin service running" << endl << endm;

		title = TranslateWcharMsg(IA_FlexShutdown, "Shutdown License Server");
		msg = TranslateWcharMsg(FLEXnetRunningOKShutdown, "Shutdown ??");

		int btn = ui_message_dialog(PS_MESSAGE_QUESTION, title, msg, UI_CENTER,
			UI_MESSAGE_CONFIRM | UI_MESSAGE_CANCEL, UI_MESSAGE_CANCEL);
		
		relmem( &msg); msg = NULL;
		relmem( &title); title = NULL;

		if (btn == UI_MESSAGE_CONFIRM)
			StopService();
		else
			return (FALSE);
	}
	if (PsProductFlexLm::IsServiceRunning())
		lmgrd_running = TRUE;
	
	const btkString keyName("HKEY_LOCAL_MACHINE\\SOFTWARE\\FLEXlm License Manager\\FLEXlm server for PTC");
	btkString keyValue;

	keyValue = EzRegkeyGetStringFromValue(NULL, keyName, "Lmgrd");
	if (!keyValue.IsEmpty())
	{
		PsWarning << "PsProductFlexAdmin found existing FLEXnet Lmgrd service installed" << endl << endm;
		OldLicenseDat = EzRegkeyGetStringFromValue(NULL, keyName, "License");
		OldPtcOptFile = OldLicenseDat.GetHead();
		OldPtcOptFile /= "ptc.opt";

		// now make a backup...
		//
		btkFSEntry bak_file;
		if (OldLicenseDat.IsFile())
		{
			btkString bak_name;

			bak_file.MakeTmpName("bak");
			bak_name = "license.dat." + bak_file.GetTail();
			bak_file = bak_file.GetHead();
			bak_file /= bak_name;
			if (OldLicenseDat.CopyTo(bak_file))
			{
				OldLicenseDat = (cStringT)bak_file;
				// also make this the input file...
				Prop.Set("ScreenProdFlex.InputPanel1", (cStringT)OldLicenseDat);
				// edit file to replace DAEMON line with default placeholders
				ReplaceDaemonLine(OldLicenseDat);
			}
			else
			{
				OldLicenseDat = "";
			}
				
		}
		if (OldPtcOptFile.IsFile())
		{
			bak_file.MakeTmpName("ptc.opt.");
			if (OldPtcOptFile.CopyTo(bak_file))
			{
				OldPtcOptFile = (cStringT)bak_file;
			}
			else
			{
				OldPtcOptFile = "";
			}

		}

		if (lmgrd_running)
		{
			// see if they want to just stop the lmgrd service or uninstall
			//
			int btn;

			PsWarning << "PsProductFlexAdmin found existing FLEXnet Lmgrd service running" 
				<< endl << endm;
			PsInfo << "Request shutdown before uninstall." << endl << endm;

			title = TranslateWcharMsg(IA_FlexShutdown, "Shutdown License Server");
			msg = TranslateWcharMsg(FLEXnetRunningDoShutdown, "Shutdown of FLEXnet Required.");

			btn = ui_message_dialog(PS_MESSAGE_QUESTION, title, msg, UI_CENTER,
				UI_MESSAGE_YES | UI_MESSAGE_NO, UI_MESSAGE_NO);
		
			relmem( &msg); msg = NULL;
			relmem( &title); title = NULL;

			// if they cancel - ie lmgrd stays up; we can't continue the install.
			//
			if (btn == UI_MESSAGE_NO)
				return (FALSE);
			psNTServStop(FLEXSVCNAME);
		}

		// request permissions to uninstall
		//
		btkFSEntry foobar, psuninst, uninstlog;
		btkString substr, old_lp, Cmd;
		
		foobar = EzRegkeyGetStringFromValue(NULL, keyName, "Lmgrd");
		uninstlog = foobar.GetHead().GetHead().GetHead();
		old_lp = (cStringT)uninstlog;
		uninstlog /= "uninstall";
		psuninst = foobar.GetHead();
		substr = (cStringT)psuninst;
		substr.Substitute(old_lp, (cStringT)uninstlog, 1);
		psuninst = (cStringT)substr;
		psuninst /= "psuninst.exe";
		uninstlog /= "instlog.txt";

		if (! psuninst.IsFile() || !uninstlog.IsFile() )
		{
			// can't find what we need; tell user to do it.
			title = TranslateWcharMsg(IA_FLEXLmgrdRemove, "Remove License Server");
			msg = TranslateWcharMsg(FLEXnetInstalledDoUninstall, "Uninstall FLEXnet Required.");

			ui_message_dialog(PS_MESSAGE_ERROR, title, msg, UI_CENTER,
				UI_MESSAGE_OK, UI_MESSAGE_OK);
		
			relmem( &msg); msg = NULL;
			relmem( &title); title = NULL;

			return (FALSE);
		}
		if (! lmgrd_running )
		{
			PsWarning << "Request from user permissions to initiate uninstall" << endl << endm;

			title = TranslateWcharMsg(IA_FLEXLmgrdRemove, "Remove License Server");
			msg = TranslateWcharMsg(FLEXnetInstalledStartUninstall, "Uninstall FLEXnet Required.");

			ui_message_dialog(PS_MESSAGE_WARNING, title, msg, UI_CENTER,
				UI_MESSAGE_OK, UI_MESSAGE_OK);
		
			relmem( &msg); msg = NULL;
			relmem( &title); title = NULL;
		}
		Cmd = "\"";
		Cmd += (cStringT)psuninst;
		Cmd += "\" \"";
		Cmd += (cStringT)uninstlog;
		Cmd += "\"";

		PsSystemCall(Cmd);

		keyValue = EzRegkeyGetStringFromValue(NULL, keyName, "Lmgrd");
		if (!keyValue.IsEmpty())
		{
			// not done.. remind user to do it and stop.
			title = TranslateWcharMsg(IA_FLEXLmgrdRemove, "Remove License Server");
			msg = TranslateWcharMsg(FLEXnetInstalledDoUninstall, "Uninstall FLEXnet Required.");

			ui_message_dialog(PS_MESSAGE_ERROR, title, msg, UI_CENTER,
				UI_MESSAGE_OK, UI_MESSAGE_OK);
		
			relmem( &msg); msg = NULL;
			relmem( &title); title = NULL;

			return (FALSE);
		}
	}

	return( TRUE );
}

void PsProductFlexAdmin::ReplaceDaemonLine(btkFSEntry &file)
{
	btkFSEntry mynewfile;
	btkIFileStream IFS;
	bool didsub = FALSE;

	if (IFS.Open(file))
	{
		btkOFileStream OFS;
		btkString OneLine;

		mynewfile.MakeTmpName("ptci");
		if (OFS.Create(mynewfile))
		{
			while (IFS.Read(OneLine))
			{
				if (OneLine.Match("DAEMON*"))
				{
					OFS << "DAEMON ptc_d __PTCD_PATH__" << endl;
					didsub = TRUE;
				}
				else
				{
					OFS << OneLine << endl;
				}
			}
			OFS.Close();
		}
		IFS.Close();
	}
	else
		return;
	if (didsub)
	{
		mynewfile.CopyTo(file);
	}
	mynewfile.Erase();
}

int PsProductFlexAdmin::ScreenProdPostFunc(bool precheck, int *return_this_result)
{
	wchar_t *title = NULL;
	wchar_t *msg = NULL;

	PsInfo << "PsProductFlexAdmin::ScreenProdPostFunc() started..."
	       << endl << endm;

	// Call the generic product function first
	if (PsProduct::ScreenProdPostFunc(precheck, return_this_result) == FALSE)
	{
		return(FALSE);
	}

	// check for ADMIN License server running...
	bool stopServer = FALSE;
	if (IsServiceRunning())
	{
		PsWarning << "PsProductFlexAdmin found existing FLEXAdmin service running" << endl << endm;

		title = TranslateWcharMsg(IA_FlexShutdown, "Shutdown License Server");
		msg = TranslateWcharMsg(FLEXnetRunningOKShutdown, "Shutdown ??");
		stopServer = TRUE;

		int btn = ui_message_dialog(PS_MESSAGE_QUESTION, title, msg, UI_CENTER,
			UI_MESSAGE_CONFIRM | UI_MESSAGE_CANCEL, UI_MESSAGE_CANCEL);
		
		relmem( &msg); msg = NULL;
		relmem( &title); title = NULL;

		if (btn == UI_MESSAGE_CONFIRM)
			StopService();
		else
			return (FALSE);
	}

	// Remove it in case it was added before
	PsInfo << "\tRemoving InstAction: PS_Stop_Server_ACTION" << endl << endm;
	ClearInstActionsByType(PS_Stop_Server_ACTION);
	if (stopServer)
	{
		PsInfo << "\tAdding InstAction: PS_Stop_Server_ACTION" << endl << endm;
		AddInstAction(PS_NO_ASSOC_SCREEN,
			TranslateWcharMsg(IA_FlexShutdown, "Shutdown License Server"),
			PS_Stop_Server_ACTION, (ui_action_func_t)StopFlexAdminServer);
	}

	// This is the same for ALL platforms
	PsInfo << "\tRemoving InstAction: PS_Backup_Files_ACTION" << endl << endm;
	ClearInstActionsByType(PS_Backup_Files_ACTION);
	if (! LP_IS_NEW(GetLoadpoint()->GetInstallAction()))
	{
		PsInfo << "\tAdding InstAction: PS_Backup_Files_ACTION" << endl << endm;
		AddInstAction(PS_NO_ASSOC_SCREEN,
			TranslateWcharMsg(IA_flexbaklic, "Backup License File"),
			PS_Backup_Files_ACTION, (ui_action_func_t)BackupFlexAdminFile);
	}

	// This doesn't really matter
	if (return_this_result)
		*return_this_result = TRUE;

	PsInfo << "PsProductFlexLm::ScreenProdPostFunc() done" << endl << endm;
	return(TRUE);
}

int PsProductFlexAdmin::PreFunc(void **data)
{
	btkFSEntry lp = this->GetLoadpoint()->GetLoadpoint();
	lp /= "logs";
	lp.CreateDir();

	ConvertDBAdd("__LMGRD_ARGS__", GetLmgrdParameters());
	ConvertDBAdd("__HOSTNAME1__", Prop("ScreenProdFlex.Triad1_Host").ToUpper());
	ConvertDBAdd("__HOSTNAME2__", Prop("ScreenProdFlex.Triad2_Host").ToUpper());
	ConvertDBAdd("__HOSTNAME3__", Prop("ScreenProdFlex.Triad3_Host").ToUpper());

	return (TRUE);
}

int PsProductFlexAdmin::PostInstFunc( PsInstallResult res)
{
	wchar_t *title = NULL;
	wchar_t *msg = NULL;

	if (LP_IS_NEW(GetLoadpoint()->GetInstallAction()))
	{
		PsInstallResult res_old = res;
		if (res_old == PS_UserReadyToInstall_RESULT)
		{
			if (IsServiceRunning())
				res_old = PS_Success_RESULT;
		}
		if (res_old == PS_Success_RESULT)
		{
			msg = TranslateWcharMsg( FLEXnetComplete,
				"Install is complete");

			int btn = uiMessage(PS_MESSAGE_INFO, title, msg,
				UI_LEFT, UI_MESSAGE_YES | UI_MESSAGE_NO , UI_MESSAGE_YES);
			relmem (&msg);

			if (btn == UI_MESSAGE_YES)
				StartWebAccess();
		}
	}
	ConvertDBDelete("__LOADPOINT__");
	return (TRUE);
}


const char *PsProductFlexAdmin::GetMyCpuID(btkFSEntry &license_dat)
{
	static btkString ret_str;
	
	if (!ret_str.IsEmpty())
		return (ret_str);

	btkIFileStream IFS;
	btkString actual_server_line;
	int i, max;

	if (! IsTriad())
	{
		if (IFS.Open(license_dat))
		{
			btkString OneLine;
			while(IFS.Read(OneLine))
			{
				if (OneLine.Match("SERVER*"))
				{
					// grab HOSTID license file was generated for.
					actual_server_line = (cStringT)OneLine;
					break;
				}
			}
			IFS.Close();
		}
	}
	else
	{
		btkString hostmatch, hostmatch_upper;
		StringXArray Cpuids;
		char *p_macAddrList;
	
		if (HostidListGet(&p_macAddrList,NULL) == 1)
		{
			hostmatch = p_macAddrList;
			relmem (&p_macAddrList);
			for (i = 0, max = hostmatch.GetNWords(",");
				i < max; i++)
			{
				hostmatch_upper = "*";
				hostmatch_upper += (cStringT)hostmatch.GetWord(i, ",");
				hostmatch_upper += "*";
				Cpuids += (cStringT)hostmatch_upper.ToUpper();
			}
		}

		// open the license file and find the SERVER line that matches one of 
		// this hosts known Cpuids we can use.
		if (IFS.Open(license_dat))
		{
			btkString OneLine;
			while(IFS.Read(OneLine))
			{
				btkString OneLineUpper(OneLine.ToUpper());

				if (OneLine.Match("SERVER*"))
				{
					i = 0;
					max = Cpuids.GetSize();
					do
					{
						if (OneLineUpper.Match(Cpuids[i++]))
						{
							actual_server_line = (cStringT)OneLine;
						}
					} while (i < max && actual_server_line.IsEmpty());
					if (! actual_server_line.IsEmpty())
						break;
				}
			}
			IFS.Close();
		}
	}
	if (! actual_server_line.IsEmpty())
	{
		max = actual_server_line.GetNWords(" ");
		for (i = 0; i < max; i++)
		{
			ret_str = (cStringT)actual_server_line.GetWord(i, " ");
			if (ret_str.Match("PTC_HOSTID=*"))
			{
				ret_str.Substitute("PTC_HOSTID=", "");
				return (ret_str);
			}
		}
		ret_str.SetEmpty(); // should not happen
	}
	return (ret_str);
}


void PsProductFlexAdmin::StartWebAccess()
{
	btkString URL;

	//URL = "http://localhost:8080/dashboard?vendor=ptc_d&licenseTab=&selected=";
	URL = "http://localhost:8080/dashboard?licenseTab=floating&vendor=ptc_d&admin=systeminfo&selected=";
	EzShowURL(URL, "FLEXnet");
}


#undef FLEXSVCNAME

#endif
