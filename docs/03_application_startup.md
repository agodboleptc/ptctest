# PIM Application Startup

> Traced entirely from source in `installmgr.zip`. See `docs/01_repository_inventory.md`
> §0 for corpus scope/limits. No source was modified.

## 1. Entry Points (recap, with new evidence)

No `main`/`WinMain`/`DllMain`/`ServiceMain` exists in the supplied archive (confirmed
Phase 1). New evidence from `pim/pim_src/pimUIInit.cxx` narrows the external launcher
identity further than Phase 1 could:

```cxx
// pimUIInit.cxx, uninstall_mode == true
static char pim_rm_exe[] = "pim_rm.exe";
...
// pimUIInit.cxx, uninstall_mode == false
static char pim_exe[] = "pim.exe";
```

These literal strings are passed as `argv[0]` to the external `uiInitialize()` API
(from the UI toolkit, `<uicxx.h>`, external/not included). This confirms the real
top-level executable is named **`pim.exe`** for install mode and **`pim_rm.exe`** for
uninstall mode — consistent with the `pimTop.cxx` comment history ("Uninstall.exe is
renamed to pim_rm.exe and reconfigure.exe is renamed to pim_re.exe") and the argv layout
parsed for `pim_rl.exe` in `pimFrictionlessTrialRun`. **The source for these `.exe`
projects is still not included in this archive** — only the DLL side (`pim` module) that
they load and call into.

Three DLL-exported functions in `pim/pim_src/pimTop.cxx` serve as the actual startup
logic invoked by those EXEs:

- `pimInstallerRun(int argc, const char *argv[])`
- `pimFrictionlessTrialRun(int argc, const char *argv[])`
- `pimRenewLicenseRun(int argc, const char *argv[])`

All three follow the same init skeleton before diverging. This document traces
`pimInstallerRun` in full (the main install path) and calls out where the other two
differ.

## 2. Initialization Order (shared prefix, all three Run functions)

Every `pim*Run` function begins with this exact call sequence (identical order and
identical early-return-on-failure pattern in all three, per `pimTop.cxx`):

| Step | Call | File | What it does |
|---|---|---|---|
| 1 | `btki18nSetup()` | external (`<btkscale31.h>`/`btk` i18n, not included) | Locale/i18n setup — **UNKNOWN internals** |
| 2 | `pimXmlInit()` | `pim/pim_src/pimXmlInit.cxx` | One-time (`static bool is_xml_init` guard) call to `XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize(btkLangGetPosixLocale())`, wrapped in try/catch returning `-99` on exception. Returning anything other than `1` aborts startup with `PIM_INITIALIZE_XML_ERROR` |
| 3 | `pimCommandLineArgs(argc, argv)` | `pim/pim_src/pimGeneralInit.cxx` | Parses all CLI flags into module-static state (see §3) |
| 4 | `pimGeneralInit()` | `pim/pim_src/pimGeneralInit.cxx` | One-time-guarded (`static bool is_init`): `EnableThreading()` (external), sets `PRO_MACHINE_TYPE` env var (`arm64_win64` or `x86e_win64` depending on `ARM64_WIN64` build define), computes language/text-resource search paths and registers them via `pro_search_component_set` (external, from `imp_pim_dll.h`), calls `msg_init(L"pim.ndx")`, locates and registers `license.res` (`pim_plp_SetLicenseResourceFile`), registers a help-tag callback (`pim_plp_SetGetOptionLabelFunc`), calls `pimSendNRecvInit()`. Failure (`-99` from any caught exception) aborts with `PIM_INITIALIZE_LIBRARY_ERROR` |
| 5 | `pimBrowserInit()` | `pim/pim_src/pimBrowserInit.cxx` | One-time-guarded: `InitGetCredentialsAPI()` (external, `<bs_pro_browser_security.h>`) — sets up the browser/HTTP credential subsystem used later for PTC.com communication and the `ProSiteAuthenticationPreAction` callback (`pimSecurityCallback`, defined in the same file). Failure aborts with `PIM_INITIALIZE_COMMS_ERROR` |
| 6 | `InitLog()` | `pim_core/pim_core_src/pimLog.cxx` | Picks a log directory: `PIM_LOG_ENV` environment variable if set (via `BTK_UNSCRAMBLE_31_S`-obfuscated env var name), else `pimGetMyDocuments()`. Calls `pimProcessOldLogs(dir)` (rotate/clean prior logs), sets the log base filename to `PIM_LOG_FILE_NAME`, creates a `btkLogManager` + `btkLogger` named `"LogService"`, and creates the `LOG_SERVICE` log area used by every `LG_INFO`/`LG_ERROR`/`LG_DEBUG` call in the codebase. If `PIM_DEBUG` env var is set, enables `LG_T_TRACE` level and dumps the full process environment (via `env` command through `btkPipedProcess`) into the log. No return value is checked by the caller — logging failure is not fatal to startup |
| 7 | `pimUIInit(false)` | `pim/pim_src/pimUIInit.cxx` | One-time-guarded: locates and applies a UI skin file (`LocateSkinFile`/`ui_global_modify`), builds a synthetic `argv` (`{"pim.exe", NULL}` for install mode — see §1), calls the external `uiInitialize(argc, argv, "pim", "pim")`, then `uit_SkinInitialize()`. In non-uninstall mode also calls `uitools_search_init()`. Failure aborts with `PIM_INITIALIZE_UI_ERROR` |

If any of steps 2, 4, 5, or 7 fail, the function returns immediately with the
corresponding `PIM_INITIALIZE_*_ERROR` code and none of the later steps run — there is
no partial-cleanup/rollback of earlier successful init steps visible in this code path.

## 3. `pimInstallerRun` — Full Sequence (after the shared init prefix)

```mermaid
sequenceDiagram
    participant EXE as pim.exe (external, not in archive)
    participant Top as pimTop.cxx::pimInstallerRun
    participant Xml as pimXmlFile (Xerces DOM)
    participant SI as pimSessionInfo
    participant Ent as pimEntitlement (per product)
    participant Dlg as pimInstallMgrDlg

    EXE->>Top: pimInstallerRun(argc, argv)
    Top->>Top: btki18nSetup(); pimXmlInit(); pimCommandLineArgs(); pimGeneralInit(); pimBrowserInit(); InitLog(); pimUIInit(false)
    Top->>Top: pimWindowsPathTest()
    Top->>Top: LocatePhysicalImage(pimImageLocation)
    alt physical media image found
        Top->>Xml: parse image root XML, scan <FAMILY>/<ENTITLEMENT> nodes
        Xml-->>Top: WGM family detection -> pimSetProductMode(PIM_WGM_MODE) if matched
    end
    Top->>SI: construct pimSessionInfo
    Top->>SI: SetProperty(EXERUN_PROPERTY, argv[0]); SetProperty(LICENSED_PROPERTY,"N")
    Top->>SI: FoundPreviousSessionInfoFile() (checked, load path currently disabled by `if (0)`)
    alt no physical image (network/PTC.com path)
        Top->>SI: TestCommunicationToPTC() (skipped if in reconfigure mode)
    else physical image present
        Top->>Top: skip TestCommunicationToPTC (log only)
    end
    Top->>SI: SetProperty(PLATFORM_PROPERTY, LANGUAGE_PROPERTY, MSIVERSION_PROPERTY via btkDlmGetImageVersionW("msi.dll"))
    alt physical media image
        Top->>Top: enumerate *.xml files under image, filter by image.xml reference list, trial-mode, LimitToThisList, FLEX admin/lmgrd rules
        loop for each candidate xml
            Top->>SI: AddEntitlement(file)
            SI-->>Top: pimEntitlement* created/registered on success
            Top->>Ent: SetQualityAgent(true) / SetQualityAgentRequired(...) as applicable
            opt UI language != file language
                Top->>Top: pimTranslateMgr::TranslateFrom(translation file)
            end
        end
        Top->>Top: derive product mode from family flags found (FlexOnly/Schematics/Mathcad/WGM/ARPlugin/CreoHelp/MKS)
        Top->>Ent: SetInstallMe(true/false) per mode default, for every loaded entitlement
        Top->>Xml: re-parse image XML for SHIPCODE/DATECODE/LABEL/BANNER/PLATFORM -> SessionInfo properties
        opt platform bitness mismatch detected (GetSystemWow64DirectoryA probe)
            Top->>Top: show blocking error dialog (uiMessage), pimHitError(PIM_GENERAL_XML_ERROR), return -104
        end
    else network install / reconfigure
        Top->>SI: SetProperty(MEDIA_PROPERTY,"N")
        alt reconfigure mode
            Top->>Top: detect FlexOnly via installed FLEX_LMGRD_XML_INSTALLED / FLEX_ADMIN_XML_INSTALLED under "pim/xml"
        else fresh network install
            opt no PTC.com comms and NO_PTCDOTCOM_PROPERTY set
                Top-->>EXE: return PIM_PTC_IT_COMMS_ERROR
            end
        end
        opt Mathcad mode
            Top->>SI: SetSecurity(pimMediaProductionUrl, pimPrimeAuthName, pimPrimeAuthPass)
        end
        Top->>SI: read SHIPCODE/VERSION from existing session XML node (pimPIMSETUP)
    end
    Top->>Top: pimQueryCPUIdAtIndex -> SessionInfo.SetProperty(HOSTID1_PROPERTY, ...)
    Top->>SI: Save()
    Top->>Dlg: construct pimInstallMgrDlg; SetSessionInfoFile(&SessionInfo)
    Top->>Dlg: Initialize()
    alt Initialize() succeeded
        Top->>SI: GetInterrogator()->GetKnownLicenseSources[For](...)
        loop for each known license source
            Top->>Top: triad-partner consolidation (pimGetTriadParterNames/pimTrimFlexPartner/pimGetTriadAddress)
            Top->>SI: AddLicenseSourceNew(row, entry)
            Top->>Top: detect if source is local host -> local_flex_source
        end
        Top->>SI: StartValidateThread()
        opt local flex source detected
            Top->>Top: checkForLocalFlexServerExistence(&SessionInfo, idx, source)
        end
        opt not trial/school/beta/WGM/Mathcad/FlexOnly
            Top->>Top: scan saved *_license.dat files under "PTC/Licensing" appdata for locked/valid trial licenses -> AddLicenseSource
        end
        Top->>Dlg: Refresh()
        Top->>Dlg: Display()  %% blocks until user completes/cancels the UI
        Top->>SI: Save()
    else Initialize() failed
        Top->>Top: pimHitError(PIM_INITIALIZE_UI_ERROR)
    end
    Top->>SI: for each entitlement: Wait()  %% join every per-product install thread
    opt SessionInfo.GetIsUpdateValue()
        Top->>Top: pimUpdateUtilityAppList(PIM_ADD_FOR_SVCS / PIM_ADD_FOR_QAGENT, installPath)
    end
    opt pimGetInstallPathForUtility() succeeds
        Top->>Top: pimWriteUUIDForUtility(...)
    end
    Top->>Top: LG_INFO(pimLogGoodbye)
    Top->>SI: PrepareForShutdown()
    opt last error != PIM_INITIALIZE_UI_ERROR
        Top->>Top: pimUITerminate(true)
    end
    Top->>Top: ptc_exit_run(1); btki18nCleanup()
    Top-->>EXE: return pimGetLastError()
```

## 4. Object Creation Order (summary)

1. `pimSessionInfo SessionInfo` (stack local in `pimInstallerRun`) — created once,
   before any entitlement exists.
2. `pimXmlFile` instances — created transiently to read the image root XML (twice:
   once for family/WGM detection, once for SHIPCODE/DATECODE/LABEL/PLATFORM), each
   explicitly `delete`d after use (`XNew pimXmlFile()` / `delete ImageXml`).
3. `pimEntitlement` instances — one per accepted product-definition XML file, created
   inside `SessionInfo.AddEntitlement(file)` (implementation in
   `pim_core/pim_core_src/pimSessionInfo.cxx`/`pimEntitlement.cxx`, not traced line-by-line
   in this pass) and appended to `SessionInfo`'s internal `EntitlementArray`.
4. `pimTranslateMgr` — created transiently per-entitlement only if the UI language
   differs from the entitlement XML's language.
5. `pimInstallMgrDlg Dlg` (stack local) — created once, after all entitlements are
   loaded and product mode is finalized.
6. Per-product install-step objects (`pimCopier`, `pimMSICopier`, `pimSFXCopier`,
   `pimShortcuts`, `pimRegEdit`, `pimServices`, `pimDownloader`, and ad hoc
   `pimScriptLoop`/`pimPsfLoop`) are **not** created during startup — they are created
   later, inside `pimEntitlement`'s install methods, once the user has made selections
   in `Dlg.Display()` and triggered an install. That is covered in
   `docs/04_installation_flow.md`.

## 5. Service / Thread Startup

No Windows service startup occurs in `pimInstallerRun` itself (service installation is
something PIM *performs* via `pimServiceLoop`/`ginst_install_service`, not something
PIM *is*).

Thread startup is generic and shared by every `pimLoop`-derived class
(`pimEntitlement`, `pimCopyLoop`, `pimMSILoop`, `pimSFXLoop`, `pimScriptLoop`,
`pimShortcutLoop`, `pimRegEditLoop`, `pimServiceLoop`, `pimPsfLoop`,
`pimDownloadLoop`), defined once in `pim_core/pim_core_src/pimLoop.cxx`:

```cxx
pimLoop::pimLoop(pimXmlFile *XMLContent) : thrThread(), Mutex(thrRecursive)
{
    mCancel = false; mDone = false; mPause = false; mInProgress = false;
    xmlPtr = XMLContent;
}

void pimLoop::Execute()
{
    Mutex.Lock();
    mInProgress = true; mCancel = false; mDone = false; mPause = false;
    Mutex.Unlock();
    Errors = ""; Warnings = "";
    thrThread::Execute(thrAttached);   // external btk call: spawns the OS thread,
                                        // which invokes this object's OnExecute()
}
```

`thrThread::Execute(thrAttached)` is external (`btk` toolkit, not included) — its
internals (how the OS thread is created, what `thrAttached` means precisely) are
**UNKNOWN**, but the call site confirms: every install step runs on its own thread,
guarded by a recursive mutex (`Mutex`) for the shared `mCancel`/`mDone`/`mPause`/
`mInProgress` flags and the `Errors`/`Warnings` string accumulators, and the derived
class's `OnExecute()` (e.g. `pimCopyLoop::OnExecute()`, seen in the earlier CAB→ZIP
work this session) is the actual thread body.

## 6. Divergence: `pimFrictionlessTrialRun` and `pimRenewLicenseRun`

Both share steps 1–7 of §2 verbatim, then diverge:

- **`pimFrictionlessTrialRun`**: parses a fixed positional argument layout
  (`pim_rl.exe -optnum <opt_num> <scn> <email> <psf> [-cleanup|-trial_reg|-frictionless_reg]`),
  decodes obfuscated `scn`/`email`/`psf` values with an inline reverse-and-substitute
  scheme (unless `-cleanup` is passed, in which case they're used as-is), stores them via
  `pimSetFrictionlessSCN/Email/PSF`, constructs `pimSessionInfo` + `pimFrictionlessTrialDlg`
  instead of `pimInstallMgrDlg`, and sets `pimSetInHouseMode(1 or 2)` based on the
  `-frictionless_reg`/`-trial_reg` flag. No entitlement/image-scanning logic runs at
  all — it goes straight from session setup to `Dlg.Initialize()`/`Display()`.
- **`pimRenewLicenseRun`**: simplest of the three — session setup, `SetProperty` for
  platform/language/host ID, checks `pimIsNodeLicenseRenewMode`, then shows `rpimDlg`
  directly. No entitlement loading either.

All three converge again on the same shutdown sequence (§7).

## 7. Shutdown Sequence (all three Run functions, identical)

1. `LG_INFO(LOG_SERVICE, pimMessage(pimLogGoodbye).cString())`
2. `SessionInfo.PrepareForShutdown()` — comment: "do these so setup.exe can unload the
   dll"
3. `if (pimGetLastError() != PIM_INITIALIZE_UI_ERROR) pimUITerminate(true);` — comment:
   "this should unload any compiled_resource dll files"
4. `ptc_exit_run(1)` — external (`imp_pim_dll.h`)
5. `btki18nCleanup()` — external
6. `return pimGetLastError();`

No explicit `pimXmlTerminate()` call is made in any of the three `Run` functions in
`pimTop.cxx` — Xerces is initialized once per process and its `Terminate()` counterpart
(`pim/pim_src/pimXmlInit.cxx`) is never invoked from this file. **UNKNOWN** whether it's
called from the external launcher EXE after `pimCleanup()`/`FreeLibrary`, or not at all.

## 8. Open Questions Carried Forward

- Internals of `btki18nSetup`/`btki18nCleanup`, `EnableThreading`, `uiInitialize`,
  `InitGetCredentialsAPI`, `thrThread::Execute` — all external/`btk`, **UNKNOWN**.
  Note: this document treats them as opaque and only records the order they're called
  in — a plausible explanation for `btki18nCleanup` never pairing with a visible
  `pimXmlTerminate()` call is that the launcher EXE (not in this archive) handles it,
  but that is a hypothesis, not a traced fact, and is flagged as such rather than
  asserted.
- Exact content/schema of the image root XML (`<FAMILY>`, `<ENTITLEMENT>`, `pimSHIPCODE`,
  `pimDATECODE`, `pimLABEL`, `pimPLATFORM`, `pimBANNER_NAME`, `pimBANNER_VER` tags) —
  is covered in `docs/08_xml_configuration.md` (Phase 11).
- `pimSessionInfo::AddEntitlement` / `pimEntitlement` construction internals — belongs
  in `docs/06_entitlement_framework.md` (Phase 9) and `docs/classes/pimEntitlement.md`
  (Phase 5).

---
*Phase 3 of the requested 20-phase documentation set. Builds on Phases 1–2. No source
was modified.*
