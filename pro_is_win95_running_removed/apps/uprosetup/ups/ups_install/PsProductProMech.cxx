/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*\
 
  PsProductProMech.cxx
 
  Pro/SETUP functions
 
  Date      Release   Name  Ver.   Comments
  --------- -------   ----  ----   --------
  20-Apr-99           MYA         Created
  11-May-99 I-03-09   JJE   $$1    Group Submission
  18-May-99           TWH          Move in alot of logic into PostFunc
  20-May-99           TWH          externalize action header
  25-May-99           TWH          Chg PostFunc proto;Add Error msg
  07-Jun-99 I-03-10   JJE   $$2    Group Submission
  30-Sep-99           MAZ          Fix WINDOWS32 & WINDOWS95 macros 
  04-Oct-99 I-03-17   JJE   $$3    Group Submission
  13-Oct-99           MAZ          Call parent postfunc in the PostFunc()
  22-Oct-99 I-03-18+  JJE   $$4    Group Submission
  22-Nov-99           MAZ          Fix bug in WriteEnvVals() (SPR 20027986)
  23-Nov-99 I-03-22   JJE   $$5    Group Submission
  15-Dec-99           JJE          Fix WriteLicenseFile
  15-Dec-99           MYA          Don't call MechCreateLinks
  21-Dec-99           JJE          Add GetFallbackMachType
  22-Dec-99           JJE          Create the proe folder if needed
  22-Dec-99 I-03-24+  JJE   $$6    Group Submission
  06-Jan-99           MYA          Add SetDefault
  10-Jan-99           MYA          Initialize static MechInputs and MechPrefix
  10-Jan-99           JJE          Fix nt build - cast in plpf call
  10-Jan-99           JJE          Fix writing of several files
  12-Jan-00 I-03-26+  JJE   $$7    Group Submission
  01-Feb-00           JJE          Add call to CreateProHelpConfigFile
  02-Feb-00 I-03-26+  JJE          Group Submission
  04-Feb-00           MYA          Write aekey to aekey.dat
  07-Feb-00 I-03-26+  JJE   $$8    Group Submission
  18-Feb-00           MYA          Comment out CreateProHelp
  24-Feb-00           MYA/TWH      AeMode SetLM_LICENSE_FILE
  24-Feb-00 I-03-27+  TWH   $$9    Group Submission
  03-Mar-00           MYA/TWH      AeMode call ConvertDBAdd
  08-Mar-00 I-03-27+  JJE   $$10   Group Submission
  04-Apr-00           MYA          Get rid of windows_95 \"
  10-Apr-00           TWH          Change to PsSystemCall
  10-Apr-00 I-03-28+  JJE   $$11   Group Submission
  09-Jun-00           TWH          Add ScreenProductPostFunc
  10-Jul-00 I-03-30+  JJE   $$12   Group Submission
  02-Nov-00           TWH          NULL terminate ui_inquire 847968
  03-Nov-00 J-01-20+  JJE   $$13   Group Submission
  10-Nov-00 J-01-20+  JJE   $$14   Add new UG license type
  01-Dec-00           MAZ          Call PsProduct::ScreenProdPostFunc
  12-Dec-00 J-01-23+  TWH   $$15   Group Submission
  13-Feb-01 J-01-27   TWH   $$16   Remove MECINTERFACE (859870)
  09-Mar-01 J-01-29   TWH   $$17   read LM_LICENSE_FILE info on update
  04-Apr-01 J-01-30+  TWH   $$18   Fix ScreenProdPostFunc
  20-Apr-01 J-01-32+  TWH   $$19   fix typo #17
  26-Apr-01 J-01-32+  TWH   $$20   Fix compiler warning
  12-Sep-01 J-03-07   TWH   $$21   chg to PTC_D_LICENSE_FILE
  24-Sep-01 J-03-08   TWH   $$22   Modify ScreenProdPostFunc proto
  21-Sep-01 J-03-09   jas   $$23   Removed WINDOWS_95 macro
  29-Oct-01 J-03-09   TWH   $$24   fix ScreenProdPostFunc 862849
  08-Apr-02 J-03-23   TWH   $$25   New AE key mechanism
  11-Jun-02 J-03-27   ALG   $$26   SPR 952386: Remove Integrated Mode config
  25-Jul-02 J-03-30   JJE   $$27   Rm UG license type
  07-Aug-02 J-03-29+  JJE   $$28   Ref PRO_VERSION_STRING in call to PutAEMess
  18-Oct-02 J-03-36   MAZ   $$29   SPR 983305: Fix AE ptcstatus script
  13-Dec-02 J-03-39+  JJE   $$30   SPR 993163: Fix value passed to SetLM_LICENSE_FILE
  21-Jan-03 J-03-41   MAZ   $$31   SPR 997818: use FlexSep instead of Sep
  06-Feb-03 J-03-41+  JJE   $$32   Add new mech lic types MECBASIC
  17-Mar-03 K-01-03   MAZ   $$33   SPR 1002855: reread triad servers from bin
  29-Apr-03 K-01-06   TWH   $$34   Use PS instead of "ps"
  26-Jun-03 K-01-10   Chris $$35   Removed char ** casts from relmem
  20-Aug-03 K-01-13   MAZ   $$36   SPR 1039043: check for uninst key access
  07-Oct-03 K-01-16   TWH   $$37   1047326: Mechprefixes no underscore
  03-Nov-03 K-01-17   TWH   $$38   Determine Lp from in session prev proe install
  16-May-05 K-03-27   MAZ   $$39   SPR 1147562: rm mech MOTION features
  11-Jan-05 K-03-39+  TWH   $$40   eliminate buffer; avoid buffer overflow
  22-Mar-06 L-01-05   JJE   $$41   Can't call ui_inquire from install thread
  08-May-06 L-01-08   JJE   $$42   Remove some old ifdef'd stuff
  14-Jul-06 L-01-12   ksi   $$43   Unicode changes
  24-May-10 L-05-23   TWH   $$44   Enhance Setup Project
  04-Nov-11 P-10-12   TWH   $$45   replace _ps_strtowsdup

  NOTE - This file is not used anymore, mech install now part of proe

\*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#include <Ps.h>

static int GetTagsList(StringXArray * tags);
const char * PsProductProMech::MechInputs[] = {
								"MEC_BASIC_UI",
								"MEC_BASIC_ENG",
								"STRUCT_UI",
								"STRUCT_ENGINE",
								"STRUCT_THERMAL_UI",
								"STRUCT_THERMAL_ENGINE" };
const char *PsProductProMech::MechPrefixes[] = {
								"MECBASICUI",
								"MECBASICENG",
								"MECSTRUCUI",
								"MECSTRUCENG",
								"MECTHERMUI",
								"MECTHERMENG" };
int PsProductProMech::MechCount = 6;

void PsProductProMech::Initialize( )
{ 
	lic_count = 0;
	AddInstAction( PS_NO_ASSOC_SCREEN,
		TranslateWcharMsg(IA_Mech_postfunc,
		"Pro/MECHANICA Configuration"),
		PS_Product_Postfunc_ACTION, (ui_action_func_t) NULL );
	if (PsCd::AeMode())
	{
		PsPackage *pkg = GetPkg("promech_pkg");
		pkg->DelLpcfgScreen("LpcfgMechfeat");
	}
}

int PsProductProMech::PreFunc (void ** data = NULL)
{
	PsWarning << "<PsProductProMech::PreFunc>" << endl << endm;
	// SPR 983305: Fix AE ptcstatus script
	if (PsCd::AeMode())
	{
		btkFSEntry path;
		btkString licpath;

#if OPER_SYS == WINDOWS_32
		path = "%PRODIR%";
#else
		path = "$prodir";
#endif

		path /= "text/licensing/ae_license.dat";
		licpath = (cStringT)path;
		ConvertDBAdd("__LM_LICENSE_FILE__", (cStringT)licpath);
	}
	return TRUE;
}

int PsProductProMech::ScreenProdPostFunc(bool precheck, int *return_this_result)
{
	if (LP_IS_UPDATE(GetLoadpoint()->GetInstallAction()))
	{
		// read user-defined data out of non-binscripts
		btkFSEntry file;
		btkString buf;
		btkIFileStream IFS;
		FileXArray Scripts;

		// the PsProduct version is used to update the UI from last install
		PsProduct::ScreenProdPostFunc(precheck, return_this_result);

        file = (cStringT)GetLoadpoint()->GetLoadpoint();
#if OPER_SYS == WINDOWS_32
        file /= "install/nt/mech_lmlicfile.bat";
#else
        file /= "install/unix/mech_lmlicfile.csh";
#endif
		if (file.Exists() && file.IsFile() && psIsPlainFile((cStringT)file))
		{
			btkString script, var, val;

			script = file;
			Scripts += script;
			var = "PTC_D_LICENSE_FILE";
			script = file.GetTail();
			Scripts.FindEnv(&script, &var, &val, 2);
			if (val.GetLength() == 0)
			{
				var = "LM_LICENSE_FILE";
				Scripts.FindEnv(&script, &var, &val, 2);
			}
			if (val.GetLength() > 0)
			{
				PsLicServerXArray * MyLicServerList = GetLicServerList();
                btkString srv;

                MyLicServerList->Clear();

				int i = 0;
				char *instr = strdup(val);
				char **outstr = NULL;

				// SPR 1002855: reread triad servers from startup script
				plp_licenseStringToLicenseList( instr, &outstr );
				PsWarning << "Called plp_licenseStringToLicenseList for value: [" 
					<< val << "]" << endl << endm;

				while( outstr != NULL && outstr[i] != NULL )
				{
					srv = outstr[i];
					// plp_licenseStringToLicenseList() uses new[] to allocate
					// memory for the list and its entries, so we must use
					// delete[] to free them
					delete[] outstr[i];

					PsLicServer *licsrv = XNew PsLicServer;
					licsrv->SetSrvInfo(&srv);
					*MyLicServerList += licsrv;
					i++;
				}
				delete[] outstr;
			}	
		}
	}
	return (TRUE);
}


int PsProductProMech::PreCheck (void ** data = NULL)
{
	PsWarning <<"Promech_precheck called"<<endm;
	int ret = TRUE;
#if OPER_SYS == WINDOWS_32
	// SPR 1039043: check for uninst key access
	ret = PsCanRegisterUninstaller();
#endif
	// locate proe product and if installed w/ interopt
	// for Mech LP then retrieve this value.
	//
	PsCd *cd_ptr = GetPsCD();
	if (cd_ptr)
	{
		PsProduct *prod_ptr = cd_ptr->GetProduct("proe");
		if (prod_ptr)
		{
			int i, max;

			if ((max = prod_ptr->GetInterOptList()->GetSize()) > 0)
			{
				PsInterOpt *other_product;
				btkString env, val;
				for (i = 0; i < max; i++)
				{
					other_product  =  (*(prod_ptr->GetInterOptList()))[i];
					if (other_product->GetLPEnv(env))
					{
						if (env == "MECH_LP")
						{
							if (other_product->GetLoadpoint(val))
							{
								if (!val.IsEmpty())
								{
									PsInstall *CurLP = XNew PsInstall(BTKCHARP(val));
									SetLoadpoint(CurLP);
								}
							}
						}
					}
				}
			}
		}
	}
	return ret;
}

int PsProductProMech::CreateAECommands()
{
	// call PsProduct::PromptAeMsgPassword
	PsProduct::PromptAeMsgPassword();
	return (TRUE);
}

bool PsProductProMech::WriteFeatureFile(btkFSEntry &lp)
{
	btkFSEntry path = (cStringT)lp;
	path /= "/text/licensing/mech";
	path.CreateDir();
	path /= "feature.dat";

	PsVerbose << "<PsProductProMech::WriteFeatureFile> " << (cStringT)path << endl << endm;

	btkOFileStream OFS;
	if (! OFS.Create(path))
	{
		PsError << "Error: Unable to create " << (cStringT)path << endl << endm;
		return FALSE;
	}

	int i, j;

	if (lic_count == 0)
		SetDefault(&lic_count, &lic_labels);

	btkString label;
	for (i=0; i < lic_count; i++)
	{
		label = lic_labels[i];
		j = label.Pos("\t");
		OFS << label(0,j - 1) << " \"" << label(j +1, label.GetLength())
			<< "\"" << endl;
	}

	OFS.Close();
	return TRUE;
}

bool PsProductProMech::WriteLicenseFile(btkFSEntry &lp)
{
	btkFSEntry bindir = (cStringT)lp;
	btkString templatename;
	btkString destname;
	bool ret = FALSE;

	PsVerbose << "<PsProductProMech::WriteLicenseFile> " << endl << endm;

#if OPER_SYS == WINDOWS_32
	bindir /= "install/nt";
	templatename = "mech_lmlicfile";
	destname = "mech_lmlicfile";
#else
	bindir /= "install/unix";
	templatename = "mech_lmlicfile_csh";
	destname = "mech_lmlicfile.csh";
#endif

	ret = PsWriteScript(bindir, templatename, destname);

#if OPER_SYS == UNIX
	templatename = "mech_lmlicfile";
	destname = "mech_lmlicfile.sh";
	ret = PsWriteScript(bindir, templatename, destname);
#endif

	return( ret );
}

bool PsProductProMech::WriteAppmgrScript(btkFSEntry &mech)
{
	// Create pro_appmgr_home.sh in Mech loadpoint
	btkFSEntry Mechdir = (cStringT)mech;
	btkString content;

	PsVerbose << "<PsProductProMech::WriteAppmgrScript> " << endl << endm;

#if OPER_SYS == WINDOWS_32
	Mechdir /= "install/nt";
	content = "set PRO_APPMGR_HOME=";
	content += (cStringT)appmgrlpt;
	content += "\n";
#else
	Mechdir /= "install/unix";
	content = "PRO_APPMGR_HOME=";
	content += (cStringT)appmgrlpt;
	content += "\nexport PRO_APPMGR_HOME\n";
#endif
	if (! Mechdir.IsDirectory())
		Mechdir.CreateDir();
	if (Mechdir.CanWrite())
	{
#if OPER_SYS == WINDOWS_32
		Mechdir /= "pro_appmgr_home.bat";
#else
		Mechdir /= "pro_appmgr_home.sh";
#endif
		btkOFileStream OFS;
		if (! OFS.Create(Mechdir))
		{
			PsError << "Error(WriteAppmgrScript): Unable to create "
			        << (cStringT)Mechdir << endl << endm;
			return FALSE;
		}
		OFS << content;
		OFS.Close();
		return TRUE;
	}
	return FALSE;
}

void PsProductProMech::WriteEnvVals(btkFSEntry &lp)
{
	btkString loadpoint = (cStringT)lp;
    btkString archs, action;
	StringXArray tags;
    int j,max;

	btkPutEnv("PTC_PI_LOADPOINT", loadpoint);

	// These two strings are missing the seperator
	max = GetArchs()->GetSize();
    for (j=0; j < max; j++)
	{
        archs += (*(GetArchs()))[j];
		if (j < (max-1))
		{
			archs += " ";
		}
	}
    btkPutEnv("PTC_PI_ARCHLIST", (cStringT)archs);

    GetTagsList(&tags);
	max = tags.GetSize();
    for (j=0; j < max; j++)
	{
        action += tags[j];
		if (j < (max-1))
		{
			action += " ";
		}
	}
    btkPutEnv("PTC_PI_ACTION", (cStringT)action);
}

static int GetTagsList(StringXArray * tags)
{
	PsProduct *I = GetCurrentInstallable();
	PsPackageXArray * packages = I->GetPkgList();	
	for (int i=0; i<packages->GetSize(); i++)
	{
		if ((*packages)[i]->IsInstalled())
		{
			*tags += (*packages)[i]->GetTag();
			PsPackageXArray * subpkgs = (*packages)[i]->GetSubPkgList();
			for (int j=0; j< subpkgs->GetSize(); j++)
				if ((*subpkgs)[j]->IsInstalled())
					*tags += (*subpkgs)[j]->GetTag();
		}
	}
	return 0;
}

bool PsProductProMech::CheckErrMsgs(btkString &ErrFiles)
{
	btkString contents;
	int max = ErrFiles.GetNWords(" ");
	btkFSEntry ErrorFile;
	for (int i = 0; i < max; i++)
	{
		ErrorFile = (cStringT)ErrFiles.GetWord(i, " ");
		if ( ErrorFile.IsFile())
		{
			btkIFileStream IFS;
			btkString buf;
			if ( IFS.Open(ErrorFile))
			{
				while( IFS.Read(buf))
				{
					contents += buf;
				}
				contents += "\n";
				IFS.Close();
			}
		}
	}
	if (contents != "")
		return FALSE;
	return TRUE;
}

void PsProductProMech::MechCreateLinks(btkFSEntry &lp)
{
	btkString link_path = *GetLinkPath();
	if (link_path != "")
	{
		PsCommandXArray *commands = GetLoadpoint()->GetBinCommandArray();
		btkString name;
		int max = commands->GetSize();
		for (int i=0; i < max; i++)
		{
			name = *((*commands)[i]->GetName());
			btkFSEntry fpath(lp); 
			fpath /= "bin"; fpath /= name; 
			btkFSEntry lpath(link_path);
			lpath /= name;
			btkFSEntry oldpath = lpath ;
			oldpath += ".old";
			if (oldpath.IsSymbolicLink())
				oldpath.Erase(); 
			if (!oldpath.IsFile() && lpath.IsFile())
				lpath.MoveTo(oldpath);
			if (!lpath.IsFile())
				lpath.LinkTo(fpath);
		}
	}
}

int PsProductProMech::WritePtcInf()
{
    btkIFileStream IFS;
    btkOFileStream OFS;
    btkFSEntry Inf;
    btkFSEntry MechInf;
    btkString buf;
    int ret = FALSE;

    if( PsProduct::WritePtcInf() )
    {
        Inf = GetCurrentInstallable()->GetLoadpoint()->GetLoadpoint();
        Inf /= "ptc_inst/ptc.inf";

        MechInf = GetPsCD()->GetCdPath();
        MechInf /= "dsrc/text/mech_vermodels.inf";

        if( OFS.Append( Inf, btkGetAsciiSerializer() ) )
        {
            if( IFS.Open( MechInf, btkGetAsciiSerializer() ) )
            {
                while (! IFS.IsEof())
                {
					IFS.Read(buf);
                    OFS << buf << endl;
                }
                IFS.Close();
                ret = TRUE;
            }
            OFS.Close();
        }
    }
    return(ret);
}

bool PsProductProMech::GetFallbackMachType( const btkString &Mach, btkString &Fallback )
{
	bool ret = FALSE;

	if( Mach == "hp8k" )
	{
		Fallback = "hp700";
		ret = TRUE;
	}

	return(ret);
}

int PsProductProMech::SetDefault(int * count, wchar_t *** plabels)
{
	wchar_t ** labels;
	char * plplist;
	btkString b_label;
	int i;

	labels = (wchar_t **)getmem(sizeof(wchar_t*) * MechCount);
	for (i=0; i<MechCount; i++ )
	{
		b_label = BTKCHARP(MechInputs[i]);
		b_label += "\t";
		plplist = plp_GetFeatureListByProdPrefix( BTKCHARP(MechPrefixes[i]) );
		if( plplist )
		{
			if (strcmp(plplist, "")==0)
				b_label += "<No License Features>";
			else
				b_label += plplist;
		}
		else
			b_label += "<No License Features>";
		labels[i] = _ps_btktowsdup(b_label);
	}
	*plabels = labels;
	*count = MechCount;
	return 1;
}
