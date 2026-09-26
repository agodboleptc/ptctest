# `pimSilentTestXmlIsUseable` (and its 5 sibling free functions)

**File:** `pim_core/includes/pimSilent.h` (24 lines, read in full) /
`pim_core/pim_core_src/pimSilent.cxx` (746 lines, read in full)
**Module:** `pim_core`

> **Not a class.** Like `pimGetApplicationsList`, this is a **free-function
> group** with no owning class or instance — `pimSilent.h` declares 7
> functions that together validate a silent-install user-specified product
> XML (`A`) against the matching product XML found on the install media/CD
> image (`B`), then splice the user's selections into a freshly-created
> `pimEntitlement` for that media XML. This doc covers all 7 at the same
> evidentiary depth as this set's class docs, focused on
> `pimSilentTestXmlIsUseable()` (the function requested) and its direct
> callees, since all 7 functions in this file exist solely to serve the one
> pipeline `pimSilentInstallFromXML()` drives (see
> `docs/classes/pimGetApplicationsList.md`'s own coverage of that caller).
> Sections that don't apply to a free-function group (Members, Ownership
> Model, a per-instance Lifetime) are adapted or omitted accordingly.
>
> **Enrichment (a later, dedicated pass on `pimSilentFixupPSF()`
> specifically)**: re-confirmed its call-site list is unchanged (still
> exactly the 1 call, inside `pimSilentCreateEntitlement()`, its only
> caller) and added 2 new confirmed findings by tracing its dependency on
> `pimCommandMgr::DeleteCommand()`/`CanDeleteCommand()`'s explicitly
> documented "cannot delete the last command of a type" constraint
> (`pim_core/pim_core_src/pimCommandMgr.cxx:405,454-515`): the
> `pDiUmMMY<N>` dummy placeholder this function creates to work around that
> constraint can itself become permanently stuck/undeletable in `Eb`'s final
> PSF set; and the code deletes **every** command of a colliding type, not
> just the one named command its own inline comment says it removes. See
> Risk Analysis.
>
> **Enrichment (a later, dedicated pass on `pimSilentCanMatchNeeds()`
> specifically)**: re-confirmed its call-site list is unchanged (still
> exactly the 1 call, inside `pimSilentTestXmlIsUseable()`, its only caller)
> and, by tracing its dependency on `pimPackageMgr::GetInstallPackageNames()`
> (`pim_core/pim_core_src/pimPackageMgr.cxx:611-666`), found that this
> function's package-availability check silently changes its actual meaning
> depending on 3 global, process-wide command-line mode flags
> (`pimGetBasePackMode()`/`pimGetCreoNGCRIMode()`/`pimGetAllPacksMode()`)
> that have nothing to do with the specific `A`/`B` XML pair being compared
> — directly contradicting this function's own header comment ("returns
> false if any packages marked for install in A are not known in B"), which
> only actually holds in the default, no-special-mode case. See Risk
> Analysis.
>
> **Enrichment (a later, dedicated pass on `pimSilentFixupShortcuts()`
> specifically)**: re-confirmed its call-site list is unchanged (still
> exactly the 1 call, inside `pimSilentCreateEntitlement()`, its only
> caller) and found a 2nd confirmed bug in this same function, distinct
> from the caller-side indexing bug already documented in
> `docs/classes/pimShortcutMgr.md`'s Risk Analysis: its
> `GetShortcutProgramMenu()`/`SetShortcutProgramMenu()` pair ignores the
> `Get`'s return value and calls the `Set` unconditionally, so a stale
> Program Menu group name from an earlier, unrelated shortcut in the same
> loop can be copied onto the current shortcut — confirmed by direct
> contrast with the correctly return-value-guarded
> `GetShortcutStartDir()`/`SetShortcutStartDir()` call 3 lines later in the
> same function. See Risk Analysis.
>
> **Enrichment (a later, dedicated pass on `pimSilentCreateEntitlement()`
> specifically) — CORRECTS the `pimSilentCanMatchNeeds()` finding above.**
> Re-confirmed `pimSilentCreateEntitlement()`'s call-site list is unchanged
> (still exactly the 1 call, inside `pimSilentTestXmlIsUseable()`'s caller
> at `pim/pim_src/pimTop.cxx:1535`) and found that its own package-selection
> loop (`:576-586`) calls the **identical**
> `pimPackageMgr::GetInstallPackageNames()` on the same `A` XML, under the
> same 3 global mode flags, that `pimSilentCanMatchNeeds()` uses for its
> earlier validation check on the same `A`/`B` pair (guaranteed, since
> `pimSilentCreateEntitlement()` only ever runs immediately after a
> successful `pimSilentCanMatchNeeds()` call in the same process, so the
> mode flags cannot differ between the 2 calls). This means whatever
> `pimSilentCanMatchNeeds()` validates as available in `B` is **exactly**
> what `pimSilentCreateEntitlement()` will actually attempt to select for
> install on `B` — the 2 functions are consistent with each other, not in
> conflict. **This corrects the previous framing** of `-basepack`'s effect
> as a "validation gap" and `-allpacks`' effect as "overly strict": both are
> better characterized as **confirmed to be by design** — under
> `-basepack`, neither function considers a package "wanted" unless it's
> `required="Y"`, so `pimSilentCanMatchNeeds()` correctly doesn't demand `B`
> carry non-required packages that `pimSilentCreateEntitlement()` was never
> going to try installing anyway; under `-allpacks`, both functions treat
> every declared package as wanted, consistent with that flag's own
> "install everything" intent. The one part of the original finding that
> still stands unchanged: both functions' own header comments describe a
> plain `install="Y"` check, which remains an incomplete description of the
> actual, mode-dependent behavior. See Risk Analysis.
>
> **Enrichment (a later, dedicated pass on `pimIsProductXmlMatch()`
> specifically)**: re-confirmed its call-site list is unchanged (still
> exactly the 2 calls, both inside `pimSilentTestXmlIsUseable()`/
> `pimIsVersionMatch()`, no external caller). Found a confirmed **structural
> limitation shared identically with `pimIsVersionMatch()`** (which
> duplicates the exact same control-flow shape): neither function has a
> cross-schema fallback — each only ever tests `A` and `B` under the *same*
> root-XML-schema hypothesis in one call (both `Init(..., NULL)`, i.e. a
> normal `<PRODUCT>` root, or both `Init(..., "EXTERNAL_INSTALLER")`); if
> `B`'s `Init()` fails under whichever schema succeeded for `A`, the
> function returns `false` without ever trying the other schema for `B`. A
> mixed pair — `A` and `B` using different root schemas but otherwise
> identical `<TAG>` values — is confirmed to always report "no match." See
> Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentFixupPSF()`'s already-documented `pDiUmMMY<N>` dummy-placeholder
> bug)**: traced the dummy's actual downstream consumer for the first time
> and found the bug's blast radius is larger than previously stated.
> `pimEntitlement::InstallScripts()` (`pim_core/pim_core_src/pimEntitlement.cxx:5627-5631`)
> constructs a `pimScriptLoop` directly on the **same** `pimXmlFile*`
> (`xmlPtr`) `pimSilentFixupPSF()` mutates via `Eb.GetXMLPtr()` — confirmed
> identity, not merely a similar object. `pimScriptLoop::OnInstall()`
> (`pim_core/pim_core_src/pimScriptLoop.cxx:71-127`), already a fully
> documented `pimLoop` subclass in this set, calls
> `CommandMgr.GetAllCommandNames(arr)` and — for any entitlement lacking a
> `SimulateLicenseTypes` XML property — reads `arr[0]`'s license
> identifiers via `GetCommandInfoByName()` **unconditionally, with no
> type-based selection**, to populate the `[LM_LICENSE_FILE]` template
> substitution used when generating that entitlement's install scripts. If
> the stuck `pDiUmMMY<N>` dummy (or any other command `pimSilentFixupPSF()`
> leaves in an unexpected document position) ends up at index 0, its
> already-confirmed internally-mismatched data (tagged type `lictype2` but
> populated with `licIdentifiers` for a *different* type, `lictype`) would
> drive `[LM_LICENSE_FILE]` for the **entire entitlement's** generated
> install scripts, not just for the dummy's own inert PSF entry. See Risk
> Analysis for the confirmed and unconfirmed parts of this chain.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentFixupShortcuts()`'s already-documented stale-Program-Menu-copy
> bug)**: found this bug is **confirmed reachable in normal use, not just
> hypothetically** — `<PROGRAMSMENU>` is confirmed to be an independently
> optional per-shortcut child element (a shortcut offered only via
> Desktop/Quicklaunch, not a Start Menu program group, legitimately has no
> `<PROGRAMSMENU>` node at all), evidenced by `pimShortcutLoop.cxx`'s
> identical parsing pattern treating it as one of several independently
> optional sibling children of `<SHORTCUT>`. Traced the actual downstream
> consumer for the first time and found a concrete, deterministic
> consequence — stronger than the `pimSilentFixupPSF()` finding above,
> since it does **not** depend on ambiguous document ordering:
> `pimEntitlement::InstallShortcuts()` (`pim_core/pim_core_src/pimEntitlement.cxx:5778-5787`)
> calls `pimShortcuts::GetInstance().Create(xmlPtr)` on the **same**
> `pimXmlFile*` document `pimSilentFixupShortcuts()` mutates — confirmed
> object identity. `pimShortcutLoop::OnInstall()`
> (`pim_core/pim_core_src/pimShortcutLoop.cxx:324-326,654-671`) then reads
> that same shortcut's (now possibly stale) `<PROGRAMSMENU>` text content
> and uses it **directly as a filesystem subfolder path**
> (`location /= (cStringT)programsmenu;`) to physically place the installed
> `.lnk` shortcut file. Confirmed consequences: whenever a stale, non-empty
> value from a different, earlier-processed shortcut survives, the current
> shortcut's icon is installed into **that other shortcut's Start Menu
> folder** instead of its own; whenever the stale value is empty (the first
> loop iteration), the Program-Menu placement action is silently skipped
> entirely, even if this same shortcut's boolean "create in Programs Menu"
> flag was separately set `true` — a visible, user-facing install defect,
> not merely a data-integrity concern. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimIsProductXmlMatch()`'s already-documented no-cross-schema-fallback
> structural limitation, shared identically by `pimIsVersionMatch()`)**:
> found this codebase's own OTHER 2 product-XML schema-resolution call sites
> (`pimEntitlement.cxx:634-635`, resolving a product's own `.p.xml`, tries
> `NULL`/`EXTERNAL_INSTALLER`/`HIDDEN_PRODUCT` in sequence; `:967-968`,
> resolving a `<PREREQUISITE>` reference, tries `NULL`/`EXTERNAL_INSTALLER`)
> both confirm, elsewhere in this exact codebase, that a product XML's root
> schema is treated as unpredictable and deliberately checked for by trying
> multiple schemas — direct, confirmed evidence that `pimIsProductXmlMatch()`/
> `pimIsVersionMatch()`'s single-schema-for-both-`A`-and-`B` assumption is not
> guaranteed by this codebase's own design, not merely a hypothetical
> concern. Traced the actual downstream consumer for the first time:
> `pimIsProductXmlMatch()`'s only caller, `pimSilentTestXmlIsUseable()`, is
> itself only called from `pimSilentInstallFromXML()`'s per-file loop
> (`pim/pim_src/pimTop.cxx:1532`), which simply skips (no `abort`, no
> distinct error) any product whose match fails this way — and since
> `pimSilentInstallFromXML()`'s own final return value is `pimGetLastError()`
> (`pimTop.cxx:2073`), which reports only the **last** error hit across the
> entire batch run, a mixed-schema false-negative on one product XML in a
> multi-XML batch can be silently overwritten in the process's reported exit
> code by any later, unrelated error — losing which specific product failed
> the match and why. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentCanMatchNeeds()`'s already-documented package-check
> mode-dependent finding)**: found the 3 mode flags are **not** a clean,
> mutually exclusive 3-way choice — `-allpacks`/`-basepack` **are** enforced
> mutually exclusive by the CLI parser itself (`pim/pim_src/pimGeneralInit.cxx:309-320`,
> each clears the other), but `-releaselink` (`pimGetCreoNGCRIMode()`) has
> **no such interaction with either**, so a real command line can freely
> combine `-releaselink -basepack`, silently making BasePack's definition
> win in `GetInstallPackageNames()`'s `if`/`else if` chain while
> `pimGetCreoNGCRIMode()` remains fully active for every *other* purpose
> elsewhere in the codebase — a confirmed, reachable interaction via
> ordinary command-line flags, not a hypothetical one. Traced the actual
> downstream consumer for the first time and found this mode-dependent
> logic is the literal, real-world determinant of physical install
> behavior, not merely a validation/documentation concern:
> `pimPackageMgr::SetPackageInstallState()` (`pim_core/pim_core_src/pimPackageMgr.cxx:250-336`)
> writes the mode-dependent selection directly onto the `<PACKAGE>` node's
> `install="Y"`/`"N"` attribute on `Eb`'s shared `xmlPtr`. `pimMSILoop::IsEligibleForInstall()`
> (`pim_core/pim_core_src/pimMSILoop.cxx:937-969`) reads that **same**
> attribute at real install-execution time — via `pimEntitlement::InstallMSI()`
> → `pimMSICopier::MSIInstall()`/`MSICopy_low()` constructing a `pimMSILoop`
> (already a fully documented `pimLoop` subclass) directly on this same
> `xmlPtr` — to decide whether a package's MSI feature/CDSECTION is
> **actually, physically installed**. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentCreateEntitlement()`'s already-documented `<MSI>`-node
> last-value-wins caveat, previously flagged only as a lower-confidence
> structural note)**: **upgraded to a CONFIRMED BUG** —
> `pimMSILoop::pimMSIExec()` (`pim_core/pim_core_src/pimMSILoop.cxx:154-161`,
> the actual MSI install-execution driver, already a fully documented
> `pimLoop` subclass) independently iterates **every** `<MSI>` node in the
> document via the identical `getElementsByTagName(pimMSI)` call, skipping
> only those `IsEligibleForInstall()` rejects, and executes each eligible
> node as its own, separate MSI install action — confirming multi-`<MSI>`-node
> product XML is a real, designed-for, normally-executed configuration in
> this codebase, not a hypothetical edge case as previously flagged. Traced
> the actual downstream consumer for the first time: the single
> `format`/`Cmd` value pair this function's loop stamps onto **every**
> `<MSI>` node in `B` is exactly what `pimMSILoop::pimMSIExec()` reads
> **per node** at real install-execution time —
> `attribFormat` (`pim_core/pim_core_src/pimMSILoop.cxx:596-614`) directly
> selects that node's install **UI mode** (`"full"` interactive wizard,
> `"basic"`, or silent), and the `<MSIARGUMENT>` text feeds that node's
> `msiexec.exe` command line. If `B` legitimately has 2+ MSI packages meant
> to install under different UI modes or arguments, this bug forces all of
> them to adopt whichever single mode/argument pair belonged to the
> **last** `<MSI>` node read from `A` — a confirmed, install-time
> consequential data-corruption bug that can visibly change which installer
> UI a user sees during a multi-package MSI install. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentFixupPSF()`'s already-documented collision-deletes-entire-type
> discrepancy)**: found this function's own opening "Drop" step (`:361-365`)
> already leaves **exactly 1 surviving command per originally distinct
> license type** in `B` before the collision-handling block ever runs — a
> deterministic consequence of `pimCommandMgr::CanDeleteCommand()`'s live
> "last of a type" count (`pimCommandMgr.cxx:454-515`). This means a given
> `lictype2`'s **first** collision can only ever delete that **one**
> straggler, matching the comment's singular framing — the discrepancy only
> actually manifests when **2+** `A_wants` entries in the same call
> independently collide against different `B` commands that originally
> shared the same `lictype2`, a real but narrower scenario than "any
> non-colliding command of that type" suggests. Traced an internal
> consequence, not previously examined: because each later same-type
> collision's delete sweep runs *before* its own new dummy exists, it
> deletes the *previous* collision's straggler/dummy while its own dummy
> survives — an unintentional cleanup mechanism for every dummy but the
> **last** one created per shared `lictype2`. This **narrows, without
> retracting**, the already-documented stuck-dummy finding above: the
> `pimScriptLoop` `[LM_LICENSE_FILE]` corruption risk is confirmed to apply
> to at most 1 dummy per originally-distinct license type per call, not
> every dummy this function creates. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentCreateEntitlement()`'s already-documented `<PROPERTY>` skip
> list)**: traced a distinct, confirmed downstream reason for each of the 4
> skipped names, not previously examined. **`[SHIPCODE]`**: read
> immediately after `Eb`'s creation by `pimSilentInstallFromXML()`
> (`pim/pim_src/pimTop.cxx:1556-1559`) to populate the session's own
> `SHIPCODE_PROPERTY` from `Eb`'s **own** value — must reflect which
> physical media/build `B` actually is, not whatever `A`'s request XML
> happens to declare. **`[VERSION]`**: originally populated by
> `pimEntitlement::Init()` itself (`pimEntitlement.cxx:944,1195`) directly
> from `Eb`'s own `<PRODUCT version>` attribute at initialization time —
> intrinsic to which XML file `Eb` was created from — and later read by
> `pimCustomActionsLoop` (`pim_core/pim_core_src/pimCustomActions.cxx:542`)
> for version-gated custom-action behavior and by `pimSessionInfo`
> (`pim_core/pim_core_src/pimSessionInfo.cxx:1801,2316`) for
> cross-entitlement version matching. **`[SOURCE]`**: unconditionally
> **overwritten anyway** by the same caller loop moments after
> `pimSilentCreateEntitlement()` returns
> (`pim/pim_src/pimTop.cxx:1598`, `SetProperty("[SOURCE]",
> ArgZero.GetHead())`) — its exclusion here is consistent with, but largely
> superseded by, that later explicit set. **`CustomActions`**: gates
> **many** real install/uninstall lifecycle hook points (PreUpgrade,
> PreInstall, PreMSIConfigure, PostCopy, PostInstall, PostUpgrade,
> Pre/PostReconfigure, PreUninstall, and more) via
> `pimEntitlement::OnInstallCustomActions()`/`OnUninstallCustomActions()`'s
> `xmlPtr->GetProperty("CustomActions", str)` truthy-check
> (`pimEntitlement.cxx:5151,5882,6024,6669,6753,6837,6842,7023,7159,7228,7309`),
> each constructing a `pimCustomActionsLoop` on the same `xmlPtr` — a
> **newly-discovered `pimLoop` subclass** (`pim_core/includes/pimCustomActions.h`
> + `pim_core_src/pimCustomActions.cxx`) not previously mentioned anywhere
> in this doc set. The property's mere **presence**, not even its specific
> value, determines whether `B`'s own baked-in custom-action fixups run at
> all across the entire install/uninstall lifecycle. Also verified a
> **non-issue**, not a bug: the copy loop's `name` variable is declared
> once outside the loop, so a `<PROPERTY>` node lacking a `name` attribute
> would leave `name` holding a stale value from a prior iteration when the
> skip-list check runs — but the immediately following copy loop's own
> guard (`attribName != NULL`) means **0 copies happen either way** for
> such a node, so the stale value has no observable effect, unlike the
> superficially similar (but live) bug already documented in
> `pimSilentFixupShortcuts()`'s Program Menu handling. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimIsVersionMatch()`'s shipcode comparison)**: found the check is
> **optional, not enforced** — it only runs `if (Ea.GetShipcode(MOR_A) &&
> Eb.GetShipcode(MOR_B))`, and `pimEntitlement::GetShipcode()`
> (`pimEntitlement.cxx:2066-2090`) returns `false` whenever the root
> `<PRODUCT>` node lacks *both* its `appshipcode` and `shipcode`
> attributes. Confirmed structurally reachable: `pimEntitlement::Init()`
> (`pimEntitlement.cxx:846-870`) only requires `tag`/`version` to succeed —
> **not** `shipcode`/`appshipcode` — so a fully valid, `Init()`-succeeding
> product XML can legitimately omit both, silently bypassing the gate
> rather than failing it (not confirmed against any specific real
> product/media XML in this archive, since none exists, but the mechanism
> is fully confirmed from `Init()`'s own attribute requirements). Traced
> the actual downstream effect for the first time: when skipped this way,
> `pimIsVersionMatch()` returns `true` via the **exact same path** as an
> explicit shipcode match — no log message, warning, or property records
> that the comparison didn't actually run, and the caller's own
> differentiated-error logic (`Major`/`Minor`) is never even consulted on
> this path, since it's only inspected on the rejected (`false`) path. Also
> verified 2 non-issues while tracing this: `Minor` is only set `true` when
> shipcodes are **exactly equal**, not when `B`'s is legitimately newer
> (the function's own documented common case) or when the check is
> skipped — but since `Major`/`Minor` are provably unread on every path
> where this matters, it has no effect; and `pimCompareShipcode()`'s own
> parameter names (`new_ship`/`old_ship`) don't reflect any enforced
> contract — cross-checked against its other call site
> (`pimEntitlement::GetSize()`, `pimEntitlement.cxx:2109`), confirming it's
> a generic, symmetric comparator, so this call's `A`-then-`B` argument
> order is a valid usage, not a naming-driven bug. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentCanMatchNeeds()`'s platform and language checks)**: confirmed
> `pimPlatformMgr::GetInstallPlatformNames()`/`pimLanguageMgr::GetInstallLanguageNames()`
> (`pimPlatformMgr.cxx:253-281`/`pimLanguageMgr.cxx:147-175`) really are
> free of the package check's mode-dependence — simple, unconditional
> `install="Y"` lookups, no global mode flags for either. But found they
> **lack the package check's later-confirmed consistency guarantee**:
> `pimSilentInstallFromXML()`'s post-creation code
> (`pim/pim_src/pimTop.cxx:1602-1635`, run *after* `pimSilentCreateEntitlement()`
> and hence after this validation already ran) re-derives what actually
> gets selected on `Eb` from **entirely different inputs** than `A`'s XML
> `install="Y"` markings this check validates. **Platform**:
> `MyPlats.InitPlatformState(NULL)` (`pimPlatformMgr.cxx:104-125`) derives
> the platform from `btkGetPlatform()` — the current machine's actual
> runtime OS platform — with a 2-level fallback chain
> (`i486_nt`/`x86e_win64`), entirely independent of `A`'s request or this
> check's validation. **Language**: for each of 9 hardcoded languages,
> `pimShouldWeInitLanguageID(<code>)` (`pim/pim_src/pimGeneralInit.cxx:531-539`)
> gates a `SetLanguageInstallState(<lang>, true)` call whenever that
> language appears in the `-LANG <XX>` command-line list
> (`pimGeneralInit.cxx:269-273`) **or** `pimGetAllPacksMode()` is set (in
> which case all 9 are force-selected) — independent of whether `A`'s XML
> marks that language `install="Y"` at all; `pimSilentCanMatchNeeds()`
> never receives the CLI args, so it structurally cannot validate
> `-lang`/`-allpacks`-requested languages against `B`'s actual coverage.
> Traced the downstream consequence for the first time: both `pimTop.cxx`
> call sites (`:1609-1627`, `:1630`) discard the return value of
> `SetLanguageInstallState()`/`InitPlatformState()` — both of which fail
> **silently**, with no log or error, whenever the requested platform/
> language doesn't actually exist in `B`'s media. A `-lang XX` (or
> `-allpacks`) request for a language `B` genuinely lacks is silently
> dropped with zero diagnostic anywhere in the pipeline; if the
> auto-detected platform and both fallbacks all fail to match anything in
> `B`, the entitlement silently ends up with no platform marked for
> install at all.
> **CORRECTED (found while investigating `pimSilentFixupPSF()`'s `Ea`/`Eb`
> reference-vs-copy semantics in the pass immediately below, which required
> re-reading `pimSilentCreateEntitlement()`'s full body)**: the framing
> above — that platform/language selection "lacks the package check's
> consistency guarantee" — was **incomplete**. `pimSilentCreateEntitlement()`
> itself (`:549-569`) **does** mirror `A`'s declared platform/language onto
> `B` first, via the **identical** `GetInstallPlatformNames()`/
> `GetInstallLanguageNames()` calls `pimSilentCanMatchNeeds()` uses for
> validation (`B_plat.ClearInstall()`/`B_lang.ClearInstall()` then
> `SetPlatformInstallState(A_wants[i], true, ...)`/
> `SetLanguageInstallState(A_wants[i], true)` for each of `A`'s wanted
> names) — exactly the same consistency pattern already confirmed for
> package selection. The corrected finding: this internally-consistent
> mirroring is then **undone** by `pimTop.cxx`'s separate, later,
> post-creation code, which overwrites/expands the *same* 2 properties
> using entirely different, never-validated inputs (OS auto-detection,
> `-lang`/`-allpacks`) — so the inconsistency is not that
> `pimSilentCreateEntitlement()` fails to mirror validation, but that a
> *second*, independent write happens afterward that the validation never
> anticipated. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentFixupPSF()`'s `Ea`/`Eb` reference-vs-copy semantics)**: traced
> `pimSilentCreateEntitlement()`'s own construction of `Ea`/`Eb`
> (`pimSilent.cxx:507-521,736`): `Ea` is a genuinely **ephemeral**,
> stack-allocated `pimEntitlement` — `Init()`'d fresh from `A`'s file path
> and destroyed (its `xmlPtr` explicitly `delete`d,
> `pimEntitlement.cxx:226-234`) the moment `pimSilentCreateEntitlement()`
> returns — while `Eb` is **not a copy at all**: `pimEntitlement* Eb =
> pimGetSessionInfo()->GetEntitlement(entitlement_index);` points directly
> at the real, session-owned, persistent object `AddEntitlement(B)` just
> created, and `pimSilentFixupPSF(Ea, *Eb)` mutates that **same** object by
> reference — confirming why every already-documented downstream consumer
> in this doc set (`pimScriptLoop`, `pimShortcutLoop`, `pimMSILoop`, etc.)
> genuinely reads the identical object this function wrote to, not a
> snapshot. Found `pimSilentFixupPSF()` treats its 2 symmetric,
> non-`const` reference parameters with confirmed **asymmetric roles**:
> every single `Ea`/`A_cmds` call in the function is a read-only accessor
> (`GetCommandInfoByName()`/`GetAllCommandNames()`) — never a mutator —
> while `Eb`/`B_cmds` is the sole read-write target; `Ea` could safely be
> declared `const pimEntitlement&`, a minor, confirmed API-clarity gap, not
> a functional bug. Also confirmed a real but **currently unreachable**
> structural hazard in `pimEntitlement` itself: it owns a raw `xmlPtr`
> pointer, explicitly `delete`d in its destructor, but defines **no**
> custom copy constructor or assignment operator — relying on the
> compiler-generated (shallow-copy) defaults, which would double-free
> `xmlPtr` if 2 `pimEntitlement`s ever shared it via a by-value copy. An
> exhaustive search confirms the **only 3** stack-allocated, value-type
> `pimEntitlement` instances anywhere in this entire codebase are all in
> this same file (`pimIsProductXmlMatch()`'s and `pimIsVersionMatch()`'s
> own `Ea`/`Eb`, and `pimSilentCreateEntitlement()`'s own `Ea`) — every one
> confirmed used safely (never copy-constructed or assigned from another
> `pimEntitlement`); every other entitlement in the codebase is held via
> `dsXArray<pimEntitlement*>` (`pimSessionInfo.h:96-97`) or a raw pointer,
> never by value. Also verified a non-issue: although `Ea` is destroyed
> right after this call, every value `pimSilentFixupPSF()` copies from it
> into `Eb` (`desc`/`licIdentifiers`/`features`, via
> `GetCommandInfoByName()`'s `btkString`/`StringXArray` out-parameters) is
> a genuine value copy, not a reference back into `Ea`'s own DOM — `Eb`'s
> resulting document holds no dangling reference to `Ea` after `Ea`
> disappears. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentCreateEntitlement()`'s session-index lookup)**: traced
> `entitlement_index`'s full lifecycle (`pimSilent.cxx:510-530`) against its
> only caller (`pimTop.cxx:1534-1540`). **CONFIRMED BUG**:
> `entitlement_index` is set to a valid, non-negative index at `:520` —
> immediately after `AddEntitlement(B)` succeeds — but `A`'s own
> parseability isn't checked until `:522-524`, and if `Ea.Init(A)` fails
> under both schemas, the function returns `false` at `:530` **without**
> resetting `entitlement_index`. The caller never inspects this function's
> `bool` return value at all — `k`'s sign (pre-initialized `-1`, checked via
> `if (k < 0)`) is the **sole** failure-detection channel — so this specific
> failure path is silently swallowed: `k` is left holding a real index into
> a `B` entitlement that was added to the session but never reached
> `pimSilentFixupPSF()`/`pimSilentFixupShortcuts()` (`:736,739`), and the
> caller proceeds as if creation fully succeeded. **Reachability**: not
> confirmed reachable via the codebase's own single call site —
> `pimSilentTestXmlIsUseable()` (`pimTop.cxx:1532`, called immediately
> before on the identical `A`) already requires `Ea.Init(A, NULL) ||
> Ea.Init(A, "EXTERNAL_INSTALLER")` to succeed for the same file, so `A`'s
> parseability is already established by the time this function runs — a
> confirmed but currently latent gap in the function's own error-handling
> design, not a live risk given the current call graph. Also **verified a
> related non-issue**: `pimSessionInfo::AddEntitlement()`'s own
> pre-existing-entry guard compares a filesystem path against
> `GetID()` (a product tag) and so never actually fires, confirming
> `AddEntitlement()` succeeding always means a brand-new entry was just
> appended — the index arithmetic itself (`GetEntitlementSize() - 1`) is
> otherwise sound; only its reporting back to the caller on a later failure
> is broken. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentCanMatchNeeds()`'s package check reachability)**: traced
> `A_pkg.GetInstallPackageNames(A_wants)`/`B_pkg.GetAllPackageNames(B_avail)`
> (`pimSilent.cxx:307-308`) into `pimPackageMgr`
> (`pim_core/pim_core_src/pimPackageMgr.cxx:611-666,550-575`). **CONFIRMED
> BUG**: both functions dereference `DOMNamedNodeMap::getNamedItem()`
> results (`name`, and — in `GetInstallPackageNames()` only —
> `install`/`required`/`parent`) via `->getNodeValue()` with **no** `if
> (attrib)` null-check, unlike this exact file's own
> `pimPackageMgr::GetPackageInfoByName()` (`:113-170`), which guards every
> one of the same 4 attributes before dereferencing — a direct, confirmed
> inconsistency within the same class. **Reachability, precisely
> characterized**: which attribute crashes depends on which of the 3
> global CLI mode flags is active — `install` is dereferenced
> unconditionally in **default mode and `-allpacks` mode alike** (it's the
> left operand of `||`, evaluated before `pimGetAllPacksMode()` is ever
> checked, defeating the assumption that `-allpacks` sidesteps a missing
> `install` attribute), `required` unconditionally under `-basepack`/
> `-releaselink`, and `parent` additionally under `-releaselink` when
> `required` isn't `"Y"`. `name` crashes only for a node that would
> otherwise be selected. Confirmed this is the **widest** exposure of the
> 3 sibling checks: `pimPlatformMgr`/`pimLanguageMgr`'s equivalent
> functions share the same unguarded `name`/`install` pattern, but lack
> package's extra mode-branching, so package has 4 vulnerable attributes
> to their 2. Not confirmed against a real product/media XML pair (none
> exists in this archive, and no `<PACKAGE>`-node-authoring code exists in
> this archive either, so attribute completeness in real PTC-built media
> can't be directly verified), but the mechanism and the internal
> inconsistency are fully confirmed from this codebase's own source. See
> Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically verifying
> `pimSilentFixupShortcuts()`'s already-proposed stale-Program-Menu-copy
> fix)**: traced `pimShortcutMgr::SetShortcutProgramMenu()`
> (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:169-189`) to confirm the proposed
> fix — gate the `Set` call on the `Get` call's return value, mirroring the
> adjacent `GetShortcutStartDir()`/`SetShortcutStartDir()` pattern — is
> **confirmed sufficient**: skipping the call when `Get` fails leaves `Eb`'s
> shortcut untouched, so its own original template value survives intact,
> eliminating every corruption path in the original finding. Also found a
> **new, previously unexamined limitation, shared by the fix and the
> pattern it mirrors alike**: neither `SetShortcutProgramMenu()` nor
> `SetShortcutState()` (`:426-454`, the shared primitive behind
> `SetShortcutProgramsMenuState()`'s toggle) nor `SetShortcutStartDir()`
> itself (`:213-233`) can ever *create* a missing child node — all 3 only
> mutate an *existing* one. So even with the fix applied, `A` can never
> grant a shortcut a Program-Menu (or start-directory) customization that
> `B`'s own media template never defined in the first place — a silent,
> non-crashing, by-design ceiling (confirmed consistent with
> `pimShortcutLoop::OnInstall()`'s own parsing simply skipping an absent
> node), not a defect the fix introduces or leaves unaddressed. See Risk
> Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentCreateEntitlement()`'s quality-agent flag copy)**: traced
> `Ea.IsQualityAgentEnabled(tf)`/`Eb->SetQualityAgent(tf)`
> (`pimSilent.cxx:593-599`) into `pimEntitlement::SetQualityAgent()`
> (`pimEntitlement.cxx:3225-3248`). **CONFIRMED BUG, asymmetric silent
> override**: enabling (`SetQualityAgent(true)`) always succeeds whenever
> `Eb`'s `<QUALITYAGENT>` node exists, but *disabling*
> (`SetQualityAgent(false)`) is silently **refused** — the function's own
> `bool` return stays `false` with no attribute change — whenever `Eb`'s
> own node is marked `required="Y"`, a business rule unrelated to `A`.
> `pimSilentCreateEntitlement()` discards this return value entirely, so a
> user's explicit "disable quality agent" request in `A` can be silently
> overridden by `B`'s own required-flag, with zero diagnostic anywhere.
> **Confirmed by direct contrast**: the GUI path
> (`pim_ui/pim_ui_src/pimCustomDlg.cxx:1630-1655`,
> `uiCustomTree::OnUpdate()` `:1727-1730`) calls `IsQualityAgentRequired()`
> *first* and disables ("greys out") the "QualityAgentOptIn" checkbox
> control entirely when required — structurally preventing a live user
> from ever attempting the disable this function blindly attempts.
> **Reachability**: unlike the package check, nothing in this pipeline
> pre-validates QualityAgent state — `pimSilentCanMatchNeeds()` never
> inspects it — so a plausible real scenario (an older saved request `A`
> with quality agent explicitly off, applied against a newer `B` whose
> media definition has since made it `required="Y"`) reaches this
> unguarded path directly. **Downstream consequence, traced for the first
> time**: `Eb`'s final `enable` state is read at real install-execution
> time by `pimEntitlement::OnInstall()` (`pimEntitlement.cxx:6499-6509`)
> and `OnReconfigure()` (`:7051-7060`) to write or remove a
> `"QualityAgentOptIn"` registry value under `PtcKey` — the UI's own
> "PHM"/legal-text framing confirms this is a real, user-facing
> telemetry-opt-in setting, not merely a latent XML-consistency concern.
> **Verified non-issue**: the enable direction carries no equivalent
> restriction — a request to *turn on* quality agent is never silently
> refused. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentFixupPSF()`'s `DeleteCommand()` return-value check)**: traced
> all 3 `DeleteCommand()` call sites (`pimSilent.cxx:364,391,413`) against
> `pimCommandMgr::DeleteCommand()`/`CanDeleteCommand()`/`AppendAddCommand()`
> (`pimCommandMgr.cxx:270-451,454-515`). The **Drop step** (`:364`) is a
> **verified non-issue** — its `false` return is the loop's own by-design
> terminal state, already documented by its own inline comment. The
> **collision-handling delete** (`:391`) is **confirmed, by an exhaustive
> counting proof, to always succeed** — the type's member count stays `>1`
> through every deletion in the loop because the freshly `AppendAddCommand()`'d
> dummy (`:389`) supplies the extra survivor — *conditional* on that
> `AppendAddCommand()` call itself succeeding, which is **also** never
> checked, and traced to depend on `FindPsfTempateNode(lictype2)` finding a
> matching `<PSF_TEMPLATE>` (not confirmed either way — no sample product
> XML exists in this archive). The **final cleanup loop** (`:413`) is where
> checking would matter: **CONFIRMED**, generalizing the already-documented
> stuck-dummy finding — **any** command (dummy or genuinely original) that
> is the sole survivor of its license type gets permanently, silently
> stuck whenever `A_wants` contains no command of that type at all,
> confirming `pimSilentFixupPSF()`'s own stated goal ("eliminate the names
> from `B` that are not part of `A_wants`") is structurally unachievable
> for an entire license type in that case — not a corner case. A stuck
> non-dummy leftover carries real, valid (if unrequested) license data,
> distinct in character from the dummy's internally-mismatched data, but
> can equally reach `pimScriptLoop::OnInstall()`'s unconditional
> `arr[0]` read for `[LM_LICENSE_FILE]`. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentFixupPSF()`'s `AppendAddCommand()` return-value check)**:
> traced both `AppendAddCommand()` call sites (`pimSilent.cxx:389,403`)
> against its own implementation (`pimCommandMgr.cxx:310-403`). It is
> add-or-update — an existing `name` is simply updated and always succeeds
> — but creating a genuinely **new** node depends on `FindPsfTempateNode()`
> finding a matching `<PSF_TEMPLATE>`; if none exists, nothing is created
> and the return is `false` with **zero trace anywhere**. **Site 1**
> (`:389`, the dummy) uses `lictype2`, a type read from an **existing**
> live `B` command — plausibly, though not certainly, already templated.
> **Site 2** (`:403`, the main re-add loop) — **CONFIRMED BUG, more severe
> and more clearly reachable**: when `A_wants[i]` is a name genuinely new
> to `B`, its `lictype` comes from **`A`'s own document**, entirely
> independent of `B`'s template set — if `B`'s media lacks a template for
> that license type (a plausible cross-version/cross-product mismatch),
> `A`'s entire request for that named command is **silently dropped**, not
> stuck like a dummy, simply **absent**, with no diagnostic anywhere.
> Confirmed by this function's own inline comment ("now we know that all
> the names we want to use are either not already used or match the same
> type...") to be a genuine blind spot — the author reasoned through name
> collisions but never template availability. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically verifying
> `pimSilentCreateEntitlement()`'s already-proposed `<MSI>`-node fix)**:
> the proposed fix — match `A`'s and `B`'s `<MSI>` nodes by identity (its
> own name/`PRODUCTCODE`) instead of collapsing all nodes' values into one
> pair of shared variables — is confirmed grounded in real attributes:
> traced `pimMSILoop::pimMSIExec()`'s own reads
> (`pimMSILoop.cxx:202-207`) off the identical `<MSI>` nodelist to confirm
> both `name` and `PRODUCTCODE` genuinely exist on real `<MSI>` nodes in
> this codebase. **But the fix's own "either" phrasing is under-specified,
> and the 2 keys have opposite tradeoffs**: `PRODUCTCODE` is a Windows
> Installer GUID conventionally expected to change on most new MSI
> builds — a `PRODUCTCODE`-only match would likely find **no**
> correspondence between `A`'s (older/customized) request and `B`'s
> (newly matched) media in this function's realistic use case, silently
> degrading the fix to "no customization ever survives," trading the
> confirmed corruption bug for a confirmed loss of the feature's purpose.
> `name` is the practically workable key (a stable label likely to persist
> across versions) but isn't schema-enforced unique. A robust fix needs
> both — `PRODUCTCODE` tried first, `name` as fallback — a design detail
> neither the original finding nor its proposed fix specified. **New
> asymmetry found while verifying**: the existing write loop's `format`
> handling (`:654-661`) only overwrites `B`'s attribute if it **already
> exists** (no creation fallback), while the adjacent `<MSIARGUMENT>`
> handling (`:663-673`) **does** create a missing child — any correctly-
> scoped per-node fix must consciously preserve or correct this, not just
> the node-identity matching. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically verifying
> `pimSilentCreateEntitlement()`'s `<PROPERTY>`-copy loop's proposed
> hardening fix)**: the proposed fix ("initialize the loop's `name`
> variable fresh each iteration") is **confirmed still cosmetic today** —
> traced precisely which guard does the real work: the inner for-loop's
> own `attribName != NULL` condition (`:712`) is the sole, always-present
> safety net for a nameless `<PROPERTY>` node, regardless of what the
> outer skip-list `continue` (`:710-711`) does with a stale `name`. The
> fix's real value is closing a **latent maintenance trap**: a plausible
> future refactor weakening the inner guard (mistakenly assuming the outer
> `continue` already handles nameless nodes) would silently reintroduce
> exactly the kind of stale-value corruption already confirmed live in
> `pimSilentFixupShortcuts()`'s Program Menu bug. **CONFIRMED BUG, found in
> the same pass by tracing this loop's full return-value chain — a new,
> distinct finding**: `pimXmlFile::GetProperty()`'s own return value is
> *also* discarded at both value-copy sites (`:719`, `:725`); if `Ea` ever
> has 2+ `<PROPERTY>` nodes sharing the same `name` (not schema-prevented
> in this archive), `GetPropertyNode()`'s document-order-first-match
> search could return a *different* node than the one being iterated, and
> a missing attribute there would leave `value` stale — silently written
> to `Eb` regardless. The general-attribute branch could avoid this
> entirely by reading its already-in-hand `attrib` node directly
> (`attrib->getNodeValue()`) instead of re-searching `Ea`'s whole document
> by name. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically verifying
> `pimSilentFixupShortcuts()`'s already-documented array-index fix)**: the
> proposed fix — replace `B_avail[i]` with `A_wants[i]` in the 4
> `SetShortcut*State()` calls (`:457-460`) — is **confirmed sufficient**:
> the enclosing `if (B_avail.Find(A_wants[i]) != -1)` guard already proves
> `A_wants[i]` exists in `Eb`'s document, so using it directly guarantees
> `FindShortcutNode()` identifies the same shortcut the 4 lines immediately
> below already correctly target, with no snapshot-staleness risk (`B_avail`
> is captured once, before the loop, and nothing in this function mutates
> `Eb`'s document structure). **NEW, previously unexamined limitation**: all
> 4 `SetShortcut*State()` functions — not just `SetShortcutProgramsMenuState()`,
> the only one traced in the earlier Program-Menu-fix pass — delegate to the
> same `pimShortcutMgr::SetShortcutState()` primitive (`:426-454`), which can
> return `false` for a **correctly identified** shortcut whenever that
> specific `<STARTMENU>`/`<PROGRAMSMENU>`/`<DESKTOP>`/`<QUICKLAUNCH>` child is
> absent (all 4 confirmed independently optional); none of the 4 array-index
> call sites check this return value, fix or no fix. **Verified non-issue**:
> `GetShortcutInfoByID()`'s own discarded return value (`:455`) is safe here
> — unlike the sibling Program-Menu bug, `A_wants[i]` is sourced from
> `A_shtcuts`'s own `GetAllShortcutIDs()` scan of the same document
> `FindShortcutNode()` searches, so the node is always found, and the
> function pre-initializes all 4 output booleans to `false` whenever the
> node is found (`:40-43`), before scanning children — so an absent child
> element yields a correct `false`, never a stale carryover. Found one
> adjacent, pre-existing risk while tracing this: `GetAllShortcutIDs()`
> (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:312-313`) dereferences the `id`
> attribute node without a null check, a latent crash if a `<SHORTCUT>` ever
> lacks an `id` attribute — a risk shared by both `A_wants` and `B_avail`,
> unrelated to and not introduced by the array-index fix. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically verifying
> `pimSilentCreateEntitlement()`'s already-known session-index lookup bug's
> 2 proposed fixes)**: **CONFIRMED SUFFICIENT, either one alone**: (a) the
> caller checking this function's own `bool` return value in addition to
> `k`'s sign, or (b) this function resetting `entitlement_index` to `-1` on
> its own failure path — and confirmed there is exactly **1** such path to
> fix (`:524-531`), not several, since no other early return exists between
> `:520` (where the index is set) and the final `return true;` (`:743`).
> **NEW, deeper downstream consequence, traced for the first time**: with
> the bug live, execution doesn't merely "proceed as if creation succeeded"
> in the abstract — it concretely reaches `pimTop.cxx:1777`'s
> `SessionInfo.Save()` (once the whole `AskedToInstallThisList` loop
> completes without any other entry aborting), which calls
> `pimXmlFile::DoSave()` and **persists** the corrupted session document —
> including the orphaned `<ENTITLEMENT>` node `AddEntitlement()` added for
> the half-created `B` — to `sessioninfo.xml` on disk. Confirmed both fixes
> prevent this identically: correctly detected, the failure triggers
> `abort=true; return pimGetLastError();` (`pimTop.cxx:1538-1540`), **before**
> line 1777 is ever reached, so the corrupted state is simply discarded
> when the caller's stack-local `SessionInfo` object is destroyed on
> return — traced `pimXmlFile::PreWrite()`/`PostWrite()` and
> `~pimXmlFile()` to confirm none of them auto-persist pending changes,
> so nothing short of an explicit `Save()` call (never reached once the fix
> is applied) could write this corruption to disk. **CONFIRMED BUG in the
> literal wording of the 2nd fix's own alternative phrasing**: "call
> `DropEntitlement(B)` to roll back" is confirmed **non-functional** as
> written — `pimSessionInfo::DropEntitlement(cStringT)` matches by
> `GetID()` (a product tag), exactly the same match this doc set already
> confirmed dead code in `AddEntitlement()`'s own guard, and `B` is a
> filesystem path, not a tag — so `DropEntitlement(B)` would always return
> `false` and roll back nothing. A working rollback needs
> `DropEntitlement(entitlement_index)` (the sibling `int` overload,
> matching by array position, using the value already in hand) or
> `DropEntitlement(Eb->GetID())` instead. **Verified non-issue, given the
> consequence traced above**: a full rollback isn't actually needed for
> correctness today — since the fixed detection always aborts before any
> `Save()`, simply resetting `entitlement_index` (or checking the return
> value) is fully sufficient on its own. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically verifying
> `pimSilentFixupPSF()`'s already-suggested `Ea` const-reference
> hardening)**: **CONFIRMED NOT PURELY MECHANICAL, as first proposed**.
> This function's only direct call on `Ea` — `Ea.GetXMLPtr()` (`:357`) —
> is itself declared without `const` (`pimEntitlement.h:320`), so simply
> changing the parameter to `const pimEntitlement& Ea` would **fail to
> compile** on its own. **Confirmed safe prerequisite**: `const`-qualifying
> `GetXMLPtr()` itself fixes this cleanly — its body is the single trivial
> `return xmlPtr;`, never mutating any member, and `pimCommandMgr`'s
> constructor already takes a plain, non-`const` `pimXmlFile*`, so every
> existing call site's behavior is unchanged; confirmed **zero** existing
> `const pimEntitlement` usage anywhere in this codebase today, so this
> would be a pure widening with no regression risk. **CONFIRMED SCOPE
> CEILING, the fix's real protection is narrower than it looks**: even
> with both changes applied, the `const` only blocks a (currently
> nonexistent, in this function) non-`const` `pimEntitlement`-level call
> directly on `Ea`. Read `pimCommandMgr.h` in full: **none** of its public
> methods — including the 3 read-only accessors this function actually
> calls on `A_cmds` — are declared `const`, and confirmed **zero**
> `const pimXmlFile` usage anywhere in any header either. So the hardening
> cannot, even in principle, stop `A_cmds` (built from `Ea.GetXMLPtr()`)
> from calling a mutating `pimCommandMgr` method against `Ea`'s own
> document — the actually-realistic mistake a future maintainer could
> make — with or without the fix; this codebase's classes are uniformly
> not const-correct at the member-function level throughout this whole
> call chain. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically verifying
> `pimSilentCanMatchNeeds()`'s platform/language checks' 2 proposed
> fixes)**: **FIX (a), "check the return value and log the silent
> failures," CONFIRMED SUFFICIENT ONLY FOR DIAGNOSABILITY, not for the
> outcome**: adding the checks/logging makes the failure *visible* but
> changes nothing about what actually happens — traced the zero-platform
> case one level further, into `pimPackageMgr::RefreshAFeatureNode()`
> (`pimPackageMgr.cxx:883-980`): every `<CDSECTION>`/`<MSI>` node carrying
> a `platform` attribute is set `install="N"` whenever
> `platmgr->GetPlatformInfoByName_low()` finds that platform not
> required/installed/selected — so if `InitPlatformState()`'s own
> fallback chain fails completely (no platform ever gets marked), **every
> platform-scoped feature in `B` silently becomes uninstallable**, a
> near-total install failure logging alone can only report after the
> fact, never prevent. **FIX (b), "also validate `-lang`/`-allpacks`/
> auto-detected-platform here," CONFIRMED ARCHITECTURALLY FEASIBLE for
> both, resolving the original finding's "if feasible" hedge**:
> `pimShouldWeInitLanguageID()`/`pimGetAllPacksMode()` and
> `btkGetPlatform()` are all global, parameterless accessors, already
> called this same way elsewhere in this exact file — no signature change
> needed. **NEW nuance found while verifying it**: `InitPlatformState()`'s
> own fallback (`pimPlatformMgr.cxx:104-125`) is not a simple 2-item list
> but a **mutually exclusive, conditionally-chosen single fallback** —
> `x86e_win64`/`arm64_win64` detected → try `i486_nt`; anything else → try
> `x86e_win64`; no further fallback beyond that — a validation copy that
> checked only the raw auto-detected name (skipping this exact branch)
> would produce **false-negative rejections** for A/B pairs that succeed
> today via the fallback. **NEW maintenance-hazard, not previously
> weighed**: implementing fix (b) would create a **2nd, independent copy**
> of both the 9-hardcoded-language gating rule and this platform fallback
> branch — one already live in `pimTop.cxx`'s post-creation code, a new
> one inside this validation function — with nothing enforcing the 2 stay
> in sync, trading today's "no guarantee" gap for a new "2 sources of
> truth that can silently drift" gap instead. **NEW, unresolved design
> tradeoff**: unlike the package check (where failing validation correctly
> means "don't create this entitlement"), extending validation this way
> would let a machine merely lacking the auto-detected platform (no CLI
> override) have an otherwise entirely valid single-product install
> **rejected outright**, rather than installed with only its
> platform-specific pieces silently missing, as today — a real policy
> change this codebase's own intended semantics never state either way.
> See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically verifying
> `pimSilentFixupPSF()`'s already-documented `pDiUmMMY<N>` dummy-placeholder
> bug's proposed fix)**: the proposed fix was itself conditional — "either
> `pimSilentFixupPSF()` must guarantee no dummy survives, or `pimScriptLoop`
> must select the license command by type/role rather than array position
> 0." **FIX (a) CONFIRMED ALREADY TRUE IN HALF THE CASES, STRUCTURALLY
> UNACHIEVABLE IN THE OTHER HALF without a new capability**: re-confirmed
> the dummy survives only when no other `A`-wanted entry shares the
> colliding command's old license type — when one exists, today's final
> cleanup loop already removes the dummy correctly, no fix needed. When
> none exists, `pimCommandMgr::CanDeleteCommand()`'s absolute floor
> (`count > 1`, no override anywhere in this class) makes it structurally
> **impossible** to reduce a license type to 0 live commands via any
> ordering of the existing delete-based API — "guarantee no dummy
> survives" in this remaining case requires a genuinely **new**
> `pimCommandMgr` capability: an in-place retype operation reusing
> `AppendAddCommand()`'s own template-cloning machinery
> (`FindPsfTempateNode()` + deep-cloning a `<PSF_TEMPLATE>`'s children,
> `pimCommandMgr.cxx:310-378`) against the *existing* node instead of a new
> one — never passing through a zero-count instant, so the floor check
> never blocks it. **FIX (b) CONFIRMED ARCHITECTURALLY PRECEDENTED, BUT
> CONCRETELY UNDER-SPECIFIED**: `pimScriptLoop::OnInstall()` itself,
> a few lines below the `arr[0]` lookup, already performs exactly this
> kind of role-based selection for a different purpose —
> `xmlPtr->FindNodelistAttribMatch(pimPSF, pimid, "parametric")`
> (`pimScriptLoop.cxx:135`) — confirming the mechanism is not hypothetical.
> But no canonical `id` (or `<LICTYPE>`) value representing "the
> license-bearing command" for `[LM_LICENSE_FILE]` purposes is confirmed
> to exist anywhere in this archive (no sample product/media XML or schema
> documentation defines one), so fix (b) is feasible in mechanism but not
> yet implementable as literally stated. **NEW, previously unexamined
> finding that gates BOTH fixes' urgency**: `pimScriptLoop::OnInstall()`'s
> `GetAllCommandNames(arr)` call (`:76`) passes no 2nd argument, defaulting
> `include_feature_less` to `false` (`pimCommandMgr.h:111`) — silently
> **excluding** any PSF node lacking a `<FEATURE_NAME>` child from `arr`
> entirely. Traced whether the dummy has one: its node is deep-cloned from
> `lictype2`'s own `<PSF_TEMPLATE>`, and `AppendEditCommand()` (which
> `AppendAddCommand()` delegates to for `Description`/`Features`) only ever
> *updates* an existing `<FEATURE_NAME>` child's text — it never *creates*
> one if the template lacked it. Whether the stuck dummy can even reach
> `arr[0]` at all is therefore gated by an **unconfirmed template-authoring
> detail** (does `lictype2`'s `<PSF_TEMPLATE>` define a `<FEATURE_NAME>`
> child?), independent of document order. **NEW, related design smell,
> independent of the dummy bug entirely**: `arr[0]`'s "just grab the first
> command" assumption is confirmed fragile on its own — this same function
> already relies on multiple, semantically distinct PSF commands
> coexisting (the `id="parametric"` lookup presupposes others exist), so
> even in a perfectly healthy, dummy-free document, nothing in the schema
> or code guarantees `arr[0]` is the intended "primary" license-bearing
> command. See Risk Analysis.
>
> **Enrichment (a further, dedicated pass specifically on
> `pimSilentCreateEntitlement()`'s package selection)**: traced
> `pimPackageMgr::SetPackageInstallState()`
> (`pim_core/pim_core_src/pimPackageMgr.cxx:250-336`), the function this
> loop calls to disable every `B` package `A` doesn't want. **CONFIRMED
> BUG, directly analogous to the already-documented quality-agent
> finding**: when asked to disable (`install=false`), it silently
> **refuses** — leaving `B`'s pre-existing `install` attribute untouched
> and returning `false` — whenever that package is marked `required="Y"`
> **or** already `installed="Y"` (except `"prime_converter"`, explicitly
> exempted from the 2nd check). This function's loop discards
> `SetPackageInstallState()`'s return value at both call sites (`:582,584`),
> so a required or already-installed package in `B` silently keeps
> whatever install state it already had, contrary to `A`'s explicit
> non-request, with zero diagnostic anywhere. **Confirmed by direct
> contrast with this codebase's own GUI path**, mirroring the
> quality-agent precedent exactly:
> `pim_ui/pim_ui_src/pimCustomDlg.cxx:1900-1904`'s `uiCustomTree::RefreshPkg()`
> checks the **identical** `req || installed` condition and proactively
> **disables the checkbox** (`UI_check_sensitive_Attr, false`),
> structurally preventing a live user from ever attempting the disable
> this function blindly attempts. **New nuance found in the same trace**:
> even on a refused disable, 2 hardcoded package names
> (`"prime_converter"`/`"creo_simulate"`) still have their own global mode
> flags (`pimSetPrimeInWSMode()`/`pimSetSimulateInMode()`) toggled to the
> *attempted* (refused) value regardless — a confirmed inconsistency
> between the persisted XML attribute (unchanged) and these 2 global
> flags (updated) for those 2 specific packages only. **Verified non-issue,
> confirmed wasteful but not incorrect**: this function's own
> `B_pkg.Refresh()` call (`:586`) computes `<CDSECTION>`/`<MSI>`/`<SFX>`
> install eligibility using platform/language selections mirrored from
> `A`'s static XML markings moments earlier in this same function — but
> `pimSilentInstallFromXML()`'s post-creation code
> (`pim/pim_src/pimTop.cxx`), which runs unconditionally right after this
> function returns, overwrites platform/language via auto-detection/CLI
> flags and then calls its **own**, separate `pimPackageMgr::Refresh()`
> (a freshly-constructed `MyPkg` on the same document) — confirmed to
> supersede this function's own `Refresh()` output entirely before
> anything ever reads it, since nothing in between consults
> `<CDSECTION>`/`<MSI>`/`<SFX>` state. 100% wasted computation, but
> harmless, since the final, authoritative `Refresh()` still runs
> correctly afterward. **Cross-referenced finding**: `GetAllPackageNames()`
> (`:577`) shares the exact same unguarded null-attribute-dereference
> crash risk already confirmed for `GetInstallPackageNames()` in
> `pimSilentCanMatchNeeds()`'s own package check — meaning this
> function's package-selection loop, not just that validation, is equally
> exposed. See Risk Analysis.

## Purpose

Implements the silent-install "does the user's requested product XML match,
and can it actually be satisfied by, this media's product XML" validation
and merge pipeline — the logic `pimSilentInstallFromXML()`
(`pim/pim_src/pimTop.cxx:1319-`, see `docs/classes/pimGetApplicationsList.md`)
runs once per entry in its `AskedToInstallThisList`, immediately before
creating that entry's `pimEntitlement`.

## Responsibilities

- `bool pimSilentTestXmlIsUseable(const char *file, btkFSList &KnownProductDefinitions, btkString &out)`
  (`:29-104`) — the entry point. Parses `file` (the user-specified XML, `A`)
  to check for XML syntax errors, then scans `KnownProductDefinitions`
  (the media's own XML files, `B` candidates) for one whose product tag
  matches `A`'s (via `pimIsProductXmlMatch()`), checks version compatibility
  (via `pimIsVersionMatch()`), and — if a tag match is found — checks
  package/language/platform availability (via `pimSilentCanMatchNeeds()`).
  On success, `out` is set to the matching media XML path (`B`).
- `bool pimIsProductXmlMatch(const char *A, const char *B)` (`:106-134`) —
  loads `A`/`B` as throwaway stack `pimEntitlement`s (trying `Init(A, NULL)`
  first, then `Init(A, "EXTERNAL_INSTALLER")` as a fallback for `A` only —
  see Risk Analysis) and compares their `GetTag()` values for an exact
  `strcmp()` match. **Confirmed, found in a later, dedicated pass on this
  function itself**: no cross-schema fallback exists for `B` — whichever
  schema succeeds for `A` is the only one ever tried for `B` in that call;
  a mixed-schema `A`/`B` pair is confirmed to always report "no match."
  **Confirmed, found in a further dedicated pass on this bug specifically**:
  this codebase's own other 2 product-XML resolution call sites
  (`pimEntitlement.cxx:634-635,967-968`) do try multiple schemas in sequence
  for a single file, directly evidencing that a product's root schema is not
  assumed fixed elsewhere in this codebase — and a mixed-schema
  false-negative here can have its specific cause silently lost in
  `pimSilentInstallFromXML()`'s batch-wide, last-error-wins exit code
  (`pim/pim_src/pimTop.cxx:2073`). See Risk Analysis.
- `bool pimIsVersionMatch(const char *A, const char *B, bool &Major, bool &Minor, btkString &Maj_A, btkString &MOR_A, btkString &MOR_B)`
  (`:143-215`) — after confirming the tags relate (same `Init()`/`strcmp()`
  pattern as above, and the identical no-cross-schema-fallback limitation —
  see Risk Analysis), compares major-version substrings (split on the first
  `.`) and, if both entitlements expose a shipcode, compares shipcodes via
  `pimCompareShipcode()`. Returns `true` iff `A`'s version can be satisfied
  by `B` (same or newer); sets `Major`/`Minor` to report which half of the
  comparison succeeded, for the caller's differentiated error messaging.
  **Confirmed, found in a further dedicated pass on the shipcode comparison
  specifically**: the shipcode check is **optional, not enforced** — it only
  runs if *both* `A` and `B` expose a shipcode at all, and `pimEntitlement::Init()`
  doesn't require one, so a legitimate product XML can skip this gate
  entirely, indistinguishably from an explicit pass — see Risk Analysis.
- `bool pimSilentCanMatchNeeds(const char *A, const char *B)` (`:227-346`) —
  loads `A`/`B` as full `pimXmlFile`s and checks that every platform,
  language, and package `A` requests is also available in `B`
  (`pimPlatformMgr`/`pimLanguageMgr`/`pimPackageMgr` name-list comparisons);
  logs a specific "missing platform/language/feature" message and returns
  `false` on the first unmet request. The platform/language checks are
  straightforward `install="Y"` lookups, confirmed genuinely free of the
  package check's mode-dependence — **but, found in a further dedicated
  pass on the platform/language checks specifically, they lack the package
  check's later-confirmed consistency guarantee**: unlike
  `pimSilentCreateEntitlement()`'s package selection (which calls the
  identical `GetInstallPackageNames()` under the same mode flags this
  check uses), the platform/language actually selected on `Eb` afterward
  is re-derived from **entirely different inputs** — see Risk Analysis.
  Meanwhile the package check's notion of
  "what `A` requests" (`pimPackageMgr::GetInstallPackageNames()`) is
  silently redefined by 3 unrelated global command-line mode flags.
  **Confirmed, found in a further dedicated pass on this bug specifically**:
  the 3 flags are not cleanly mutually exclusive (`-releaselink` can be
  freely combined with `-allpacks`/`-basepack`, silently subordinating
  `-releaselink`'s own package-selection effect), and whatever this check's
  mode-dependent selection resolves to flows, unbroken, all the way to
  `pimMSILoop::IsEligibleForInstall()`'s real MSI install-time gate — see
  Risk Analysis. **CONFIRMED BUG, found in a further dedicated pass on
  this check's reachability specifically**: `pimPackageMgr::GetInstallPackageNames()`/
  `GetAllPackageNames()` — the 2 functions this check's package half
  actually calls — dereference several `<PACKAGE>` node attributes without
  a null-check, unlike this exact class's own defensively-coded
  `GetPackageInfoByName()`; a `<PACKAGE>` node missing the attribute the
  active CLI mode depends on (`install` in default/`-allpacks` mode,
  `required`/`parent` under `-basepack`/`-releaselink`) crashes this
  check's package half outright, rather than merely mis-selecting — see
  Risk Analysis.
  **Confirmed, found in a further dedicated pass verifying this
  function's platform/language checks' 2 proposed fixes specifically**:
  the "log the silent failures" fix is confirmed sufficient only to make
  the outcome *diagnosable*, not to prevent it — traced the zero-platform
  case one level further downstream and confirmed it silently marks
  **every** platform-scoped `<CDSECTION>`/`<MSI>` feature `install="N"`,
  a near-total install failure logging alone cannot stop. The "also
  validate `-lang`/`-allpacks`/auto-detected-platform here" fix is
  confirmed architecturally feasible for both, but requires exactly
  replicating `InitPlatformState()`'s own non-symmetric fallback branch to
  avoid new false-negative rejections, and would create a 2nd,
  independent copy of the same selection rule that nothing keeps in sync
  with the original — see Risk Analysis.
- `bool pimSilentFixupPSF(pimEntitlement &Ea, pimEntitlement &Eb)`
  (`:353-416`) — rebuilds `Eb`'s PSF/command set (`pimCommandMgr`) to mirror
  `Ea`'s: drops `Eb`'s existing commands, adds each of `Ea`'s wanted
  commands, then deletes anything left in `Eb` that isn't one of `Ea`'s.
  Handles a name-reused-with-a-different-license-type collision via a
  temporary `pDiUmMMY<N>` placeholder — but see Risk Analysis: this
  workaround relies on `pimCommandMgr`'s "cannot delete the last command of
  a type" rule, and the dummy itself can become permanently stuck under
  that same rule, plus the collision handling deletes every command of the
  colliding type, not just the one named command its own comment describes.
  **Confirmed, found in a further dedicated pass on this specific bug**: a
  stuck dummy isn't merely cosmetic — `pimScriptLoop::OnInstall()` (the
  actual install-time PSF/script generator for this same `Eb` XML document,
  via `pimEntitlement::InstallScripts()`) can read a stuck dummy's
  mismatched license data to set `[LM_LICENSE_FILE]` for the whole
  entitlement's install scripts. **Confirmed, found in a further dedicated
  pass on the collision-deletes-entire-type discrepancy specifically**:
  this function's own opening "Drop" step already reduces `Eb` to exactly 1
  surviving command per originally distinct license type, so the
  discrepancy's real-world reach is narrower than "any non-colliding
  command" — it only bites when 2+ collisions in the same call share an
  original license type, and even then, only the *last* dummy created for
  that type is exposed to the stuck-dummy risk above (earlier dummies for
  the same type get incidentally swept away mid-loop). **Confirmed, found
  in a further dedicated pass on this function's `Ea`/`Eb`
  reference-vs-copy semantics specifically**: both parameters are declared
  as symmetric, non-`const` references, but are used with confirmed
  **asymmetric roles** — `Ea` is queried read-only throughout (only
  `GetCommandInfoByName()`/`GetAllCommandNames()`, never a mutator), while
  `Eb` is the sole mutation target; `Ea` is also a genuinely **ephemeral**
  object (destroyed the moment `pimSilentCreateEntitlement()` returns),
  while `Eb` aliases the **real, session-owned, persistent** entitlement —
  see Risk Analysis for the full characterization, a confirmed but
  currently-unreachable double-free hazard in `pimEntitlement`'s own copy
  semantics, and a verified non-issue on data surviving `Ea`'s destruction.
  **Confirmed, found in a further dedicated pass on this function's
  `DeleteCommand()` return-value check specifically**: of its 3 call
  sites, the Drop step's discarded return is a verified non-issue (its own
  by-design terminal state), the collision-handling delete is confirmed to
  always succeed given `AppendAddCommand()` itself succeeds (a precondition
  never checked either), and the final cleanup loop's discarded return
  generalizes the stuck-dummy bug: **any** sole-surviving command of a
  license type `A` doesn't want at all — not just dummies — gets
  permanently stuck, confirming this function's own goal of removing every
  name `A` doesn't want is structurally unachievable for such a type — see
  Risk Analysis. **CONFIRMED BUG, found in a further dedicated pass on
  this function's `AppendAddCommand()` return-value check specifically**: a
  more severe, more clearly reachable sibling to the `DeleteCommand()`
  finding — when the main re-add loop's `AppendAddCommand()` needs to
  create a genuinely new command for a name `A` wants that `B` never had,
  and `B`'s media has no `<PSF_TEMPLATE>` for the license type `A`
  specifies (a value sourced from `A`'s own document, unrelated to `B`'s
  template set), the command is **silently dropped with zero trace
  anywhere** — no dummy, no leftover, no diagnostic — confirmed by this
  function's own inline comment to be a genuine blind spot in its design,
  not an accepted risk — see Risk Analysis. **Confirmed, found in a
  further dedicated pass verifying the `Ea` const-reference hardening
  suggested above**: the fix is **not purely mechanical as it first
  appears** — `Ea.GetXMLPtr()`, this function's only direct call on `Ea`,
  is not itself declared `const`, so `const pimEntitlement& Ea` would fail
  to compile without also `const`-qualifying `GetXMLPtr()` (confirmed safe
  to do, but a codebase-wide, non-local change). Even applied in full, the
  hardening is confirmed to protect only this function's own top-level
  signature, not the realistic risk — `pimCommandMgr`'s entire public API
  is non-`const` throughout, so `A_cmds` itself could still be used to
  mutate `Ea`'s document with no compiler objection, before or after the
  fix — see Risk Analysis. **Confirmed, found in a further dedicated pass
  verifying the stuck-dummy bug's own proposed fix**: "guarantee no dummy
  survives" already holds today whenever `A`'s final wanted set has any
  other command of the dummy's type, and is confirmed structurally
  unachievable via the existing delete-based API otherwise, since
  `CanDeleteCommand()`'s floor has no override; a genuine fix needs a new
  in-place retype capability, not a change to this function's own logic.
  The alternative fix (select `pimScriptLoop`'s license command by
  role, not `arr[0]`) is confirmed precedented by an existing `id`-based
  lookup 2 lines later in that same function, but no canonical selector
  value for this specific purpose is confirmed to exist anywhere in this
  archive. Also found the dummy's very reachability into that array is
  gated by an unconfirmed detail of its own template's authoring — see
  Risk Analysis.
- `bool pimSilentFixupShortcuts(pimEntitlement &Ea, pimEntitlement &Eb)`
  (`:423-493`) — mirrors `Ea`'s shortcut on/off states onto `Eb`'s matching
  shortcuts (by ID) and copies over any shortcut `Ea` has that `Eb` doesn't.
  **CONFIRMED BUG, already documented from the caller side** in
  `docs/classes/pimShortcutMgr.md`'s Risk Analysis: the 4
  `SetShortcut*State()` calls in the matched-ID branch (`:457-460`) use
  `B_avail[i]` (an index into a *different* array than the one `i` actually
  indexes) instead of the correct `A_wants[i]`, so the wrong shortcut's
  location-toggle states can be overwritten whenever `A_wants`/`B_avail`'s
  document orders diverge. **CONFIRMED BUG, found in a later, dedicated pass
  on this function itself**: 2 lines later, `GetShortcutProgramMenu()`'s
  return value is silently ignored and `SetShortcutProgramMenu()` is called
  unconditionally, so a stale Program Menu group name from an earlier,
  unrelated shortcut can be copied onto the current one — confirmed by
  direct contrast with the correctly return-value-guarded
  `GetShortcutStartDir()`/`SetShortcutStartDir()` call 3 lines further down
  in the very same loop. **Confirmed, found in a further dedicated pass on
  this specific bug**: reachable in normal use (`<PROGRAMSMENU>` is
  confirmed independently optional per shortcut), and confirmed to
  physically misplace or drop the installed shortcut's Start Menu folder
  placement via `pimShortcutLoop::OnInstall()` reading this same document
  later. **Confirmed, found in a further dedicated pass verifying this
  bug's proposed fix specifically**: gating `SetShortcutProgramMenu()` on
  `GetShortcutProgramMenu()`'s return value is confirmed **sufficient** to
  eliminate the stale-copy corruption entirely, but the fix (and the
  `StartDir` pattern it mirrors) shares a separate, pre-existing structural
  ceiling — neither `Set*` primitive can ever *create* a `<PROGRAMSMENU>`
  (or `<STARTINDIR>`) child that doesn't already exist on `Eb`'s shortcut,
  so `A` can never grant a new customization point `B`'s own template never
  offered, fix or no fix — see Risk Analysis. **Confirmed, found in a
  further dedicated pass verifying the array-index bug's proposed fix
  specifically**: replacing `B_avail[i]` with `A_wants[i]` in the 4
  `SetShortcut*State()` calls is confirmed sufficient to eliminate the
  wrong-shortcut risk, but reveals that the identical create-vs-mutate-only
  ceiling already found on `SetShortcutProgramMenu()` applies uniformly to
  all 4 of these calls too, via the same shared `SetShortcutState()`
  primitive — see Risk Analysis.
- `bool pimSilentCreateEntitlement(const char *A, const char *B, int &entitlement_index)`
  (`:501-746`) — the actual merge: adds `B` to the session as a new
  `pimEntitlement` (via `pimGetSessionInfo()->AddEntitlement(B)`), then
  copies `A`'s platform/language/package selections, quality-agent flag, a
  subset of `<MSI>` node attributes, and most `<PROPERTY>` values onto it
  (skipping `[SHIPCODE]`/`[VERSION]`/`[SOURCE]`/`CustomActions`, which stay
  `B`'s own), then calls `pimSilentFixupPSF()`/`pimSilentFixupShortcuts()`.
  **Confirmed, found in a later, dedicated pass on this function itself**:
  its package-selection loop (`:576-586`) calls the same
  `pimPackageMgr::GetInstallPackageNames()` `pimSilentCanMatchNeeds()` uses
  for its own compatibility check — see Risk Analysis for why this
  corrects, rather than compounds, the earlier `pimSilentCanMatchNeeds()`
  finding. **CONFIRMED BUG, found in a further dedicated pass on this
  function's `<MSI>`-node copy loop specifically**: only the **last**
  `<MSI>` node's `format`/`<MSIARGUMENT>` values survive being read from
  `A`, and that single value pair is stamped onto **every** `<MSI>` node in
  `B` — confirmed reachable (multi-`<MSI>`-node XML is a real,
  designed-for configuration per `pimMSILoop::pimMSIExec()`'s own execution
  model) and confirmed to control that node's real install-time UI mode via
  `pimMSILoop`'s `format`-attribute read. **Confirmed, found in a further
  dedicated pass verifying this bug's proposed fix specifically**: both
  candidate matching attributes (`name`/`PRODUCTCODE`) genuinely exist on
  real `<MSI>` nodes, but the fix's own "match by either" phrasing is
  under-specified — `PRODUCTCODE` (a per-build MSI GUID) would likely
  match nothing across the version differences this function exists to
  handle, while `name` (a stable label) is the practically workable key
  but isn't schema-enforced unique; a robust fix needs both, with
  `PRODUCTCODE` tried first and `name` as fallback — neither the original
  finding nor the proposed fix specified this. **Confirmed, found in a further
  dedicated pass on the `<PROPERTY>` skip list specifically**: each of the
  4 skipped names has a distinct, confirmed downstream reason to stay `B`'s
  own, not a blanket caution — see Risk Analysis for all 4 and for a
  verified non-issue in the copy loop's own `name` variable handling.
  **Confirmed, found in a further dedicated pass verifying this loop's
  proposed hardening fix specifically**: the fix (reinitialize `name`
  each iteration) is confirmed still cosmetic today — the inner for-loop's
  own guard is the sole real protection, always — but closes a genuine
  latent maintenance trap a plausible future refactor could reopen.
  **CONFIRMED BUG, found in the same pass, by tracing this loop's full
  return-value chain**: `GetProperty()`'s own return value is *also*
  discarded at both value-copy sites, risking a stale value being written
  to `Eb` whenever `A` has 2+ `<PROPERTY>` nodes sharing the same `name` —
  a distinct issue from the `name`-staleness one, in the same loop, not
  previously examined; the general-attribute branch could avoid this
  entirely by reading its already-in-hand `attrib` node directly instead
  of re-searching by name — see Risk Analysis.
  **CONFIRMED BUG, found in a further dedicated pass on this function's
  session-index lookup specifically**: the `entitlement_index` out-parameter
  (`:520`) is set to a valid index *before* `A`'s parseability is verified
  (`:522-524`), and is never invalidated if that later check fails and the
  function returns `false` (`:530`) — silently defeating the only
  failure-detection mechanism its caller actually uses, since that caller
  never checks this function's own `bool` return value — see Risk Analysis
  for the full trace, the confirmed (but currently latent, given the
  existing call site's own guard) reachability, and a related verified
  non-issue in `AddEntitlement()`'s own pre-existing-entry check.
  **CONFIRMED BUG, found in a further dedicated pass on this function's
  quality-agent flag copy specifically**: `Eb->SetQualityAgent(tf)`
  (`:596,598`) is called unconditionally whenever `Ea.IsQualityAgentEnabled(tf)`
  succeeds, discarding `SetQualityAgent()`'s own `bool` return value —
  `SetQualityAgent(false)` silently **refuses** to disable `Eb`'s quality
  agent whenever `Eb`'s own `<QUALITYAGENT>` node is marked `required="Y"`,
  so a user's explicit opt-out request in `A` can be silently overridden,
  with the resulting state controlling a real Windows registry value
  (`"QualityAgentOptIn"`) at install time — see Risk Analysis for the full
  trace, the confirmed contrast with the GUI's own more careful handling of
  the identical business rule, and a verified non-issue in the enable
  direction, which carries no equivalent restriction.
  **Confirmed, found in a further dedicated pass verifying the
  session-index lookup bug's 2 proposed fixes specifically**: both are
  independently sufficient, and traced the currently-live consequence one
  step further than before — an undetected failure here reaches
  `pimTop.cxx:1777`'s `SessionInfo.Save()`, persisting the corrupted
  session state to disk, which either fix prevents by triggering the
  caller's existing early-abort before that line is ever reached.
  **CONFIRMED BUG in the 2nd fix's own literal wording**: "call
  `DropEntitlement(B)`" is confirmed non-functional, since `B` is a
  filesystem path and `DropEntitlement(cStringT)` matches by product tag —
  see Risk Analysis for the working alternative and why a full rollback
  turns out not to be necessary here at all.
  **CONFIRMED BUG, found in a further dedicated pass on this function's
  package-selection loop specifically, directly analogous to the
  quality-agent finding above**: `SetPackageInstallState(B_avail[i], false)`
  (`:582,584`) silently refuses whenever that `B` package is marked
  `required="Y"` or already `installed="Y"` — the identical `req ||
  installed` condition the interactive Customize dialog uses to
  proactively grey out the same checkbox — with the return value
  discarded here just as with quality agent. Also found this function's
  own `B_pkg.Refresh()` call (`:586`) is confirmed **wasted, not
  incorrect** — its computed `<CDSECTION>`/`<MSI>`/`<SFX>` install flags
  are entirely superseded by `pimTop.cxx`'s own, later `Refresh()` call
  before anything ever reads them — and that `GetAllPackageNames()`
  (`:577`) shares `GetInstallPackageNames()`'s already-confirmed unguarded
  null-attribute crash risk — see Risk Analysis.

## Dependencies

- `pimEntitlement`, `pimXmlFile`, `pimCommandMgr`, `pimShortcutMgr`,
  `pimSessionInfo` (all `pim_core` types already documented elsewhere in
  this set); `pimLanguageMgr`, `pimPlatformMgr`, `pimPackageMgr` (cited only
  via specific traced methods where relevant to this doc's findings, per
  the top-level README's Coverage Honesty Statement — no dedicated class doc
  exists for any of the 3).
- **`pimEntitlement::GetXMLPtr()`** (`pim_core/includes/pimEntitlement.h:320`,
  new citation, traced in a dedicated pass verifying `pimSilentFixupPSF()`'s
  suggested `Ea` const-reference hardening): declared without `const` —
  `pimXmlFile* GetXMLPtr() { return xmlPtr; }` — confirmed to be the sole
  blocker preventing `const pimEntitlement& Ea` from compiling as proposed,
  and confirmed safe to `const`-qualify (trivial body, no behavior change
  for any existing non-`const` call site). See Risk Analysis.
- **`pimCommandMgr`'s full public API**
  (`pim_core/includes/pimCommandMgr.h:54-118`, already a documented
  dependency — read in full in the same pass): confirmed **none** of its
  public methods, including the 3 read-only accessors this function calls
  on `A_cmds` (`GetCommandInfoByName()`/`GetAllCommandNames()`/
  `GetAllCommandNamesByType()`), are declared `const` — confirming the `Ea`
  const-reference hardening cannot, even in principle, be extended to
  block a mutating call through `A_cmds`. See Risk Analysis.
- **`pimSilentInstallFromXML()`'s post-creation code**
  (`pim/pim_src/pimTop.cxx:1602-1635`, already the confirmed caller of this
  entire free-function group — traced further in a dedicated pass on
  `pimSilentCanMatchNeeds()`'s platform/language checks specifically):
  runs *after* `pimSilentCreateEntitlement()` and re-derives `Eb`'s actual
  platform/language selection from sources this validation never sees —
  `pimPlatformMgr::InitPlatformState(NULL)` (`pim_core/pim_core_src/pimPlatformMgr.cxx:104-125`)
  from `btkGetPlatform()` (the running machine's OS), and 9 hardcoded
  `pimLanguageMgr::SetLanguageInstallState()` calls gated by
  `pimShouldWeInitLanguageID()` (`pim/pim_src/pimGeneralInit.cxx:531-539`,
  driven by the `-LANG` CLI flag list or `-allpacks`). Both discard their
  `Set*InstallState()` calls' return values, which fail silently when the
  target doesn't exist in `B`. See Risk Analysis for the confirmed
  consequence.
- **`pimSilentInstallFromXML()`'s call-site error handling**
  (`pim/pim_src/pimTop.cxx:1534-1540`, already the confirmed caller of this
  entire free-function group — traced further in a dedicated pass on
  `pimSilentCreateEntitlement()`'s session-index lookup specifically):
  `pimSilentCreateEntitlement()`'s `bool` return value is never checked;
  the caller relies solely on its `k` (`entitlement_index`) out-parameter's
  sign, pre-initialized to `-1` and checked via `if (k < 0)`. See Risk
  Analysis for the confirmed consequence when that sentinel is left
  overwritten despite an overall failure.
- **`pimSessionInfo::AddEntitlement(cStringT)`/`GetEntitlement(cStringT)`**
  (`pim_core/pim_core_src/pimSessionInfo.cxx:654-705,760-774`, already a
  documented `pim_core` dependency — traced further in the same
  session-index-lookup pass): `AddEntitlement()`'s own pre-existing-entry
  guard calls `GetEntitlement(in)` with `in` as a filesystem path, but
  `GetEntitlement(cStringT)` matches on `GetID()` (a product `<PRODUCT tag>`
  value, `pimEntitlement.cxx:874`) — see Risk Analysis for why this guard
  is confirmed to never fire, and why that makes the session-index
  lookup's own core arithmetic reliable.
- **`pimSessionInfo::DropEntitlement(cStringT)`/`DropEntitlement(int)`/`Save()`**
  (`pim_core/pim_core_src/pimSessionInfo.cxx:784-814,2220-2226`, new
  citations, traced in a dedicated pass verifying the session-index lookup
  bug's proposed fixes): `DropEntitlement(cStringT)` shares
  `AddEntitlement()`'s exact same "match a path against `GetID()`" flaw, so
  calling it with `B` (as one proposed fix literally suggests) never
  matches anything; `DropEntitlement(int)` correctly bounds-checks and
  removes by array position instead. `Save()`'s `xmlPtr->DoSave()` is the
  **only** call reachable from `pimSilentInstallFromXML()`'s silent-install
  loop that would ever persist this bug's in-memory corruption to disk —
  see Risk Analysis for the traced call path via `pimTop.cxx:1777` and why
  either proposed fix prevents ever reaching it.
- **`pimXmlFile::PreWrite()`/`PostWrite()`/`~pimXmlFile()`**
  (`pim_core/pim_core_src/pimXmlFile.cxx:146-187,216-236`, new citations,
  traced in the same pass): confirmed to be pure thread-lock bookkeeping
  and resource cleanup respectively — neither persists pending DOM changes
  to disk, confirming nothing short of an explicit `Save()`/`DoSave()` call
  could ever write this bug's corrupted `<ENTITLEMENT>` node to
  `sessioninfo.xml`.
- **`pimPackageMgr::RefreshAFeatureNode()`**
  (`pim_core/pim_core_src/pimPackageMgr.cxx:883-980`, new citation, traced
  in a dedicated pass verifying `pimSilentCanMatchNeeds()`'s
  platform/language checks' proposed fixes): not called directly by
  anything in this file, but confirmed to be the real, install-time
  consumer of whatever platform/language selection this check validates
  and `pimSilentCreateEntitlement()`/`pimTop.cxx` ultimately write — every
  `<CDSECTION>`/`<MSI>` node carrying a `platform` or `language` attribute
  is set `install="N"` here whenever that platform/language isn't
  required/selected/installed in `B`. See Risk Analysis for the confirmed
  near-total-failure consequence when no platform is ever selected.
  **Traced further in a dedicated pass on `pimSilentCreateEntitlement()`'s
  package-selection loop specifically**: this function's own `B_pkg.Refresh()`
  call (`pimSilent.cxx:586`) invokes this same method using platform/
  language values that `pimTop.cxx`'s post-creation code immediately
  overwrites afterward, before calling its **own** separate `Refresh()` —
  confirmed to make this function's own invocation's output entirely
  superseded, never read by anything in between.
- **`btkGetPlatform()`/`pimShouldWeInitLanguageID()`/`pimGetAllPacksMode()`**
  (`pim/pim_src/pimGeneralInit.cxx:531-539`, plus `btkGetPlatform()` — an
  external, undocumented-in-this-archive `btk` library function called
  identically, with no object/session context, from at least 8 other
  sites including `pimEntitlement.cxx:858,3322,5013` — new citations,
  traced in the same pass): confirmed all 3 are global, parameterless (or
  self-contained-state) accessors, requiring no signature change to call
  from `pimSilentCanMatchNeeds()` — the architectural basis for confirming
  the proposed validation-side fix is feasible. See Risk Analysis for why
  feasibility alone doesn't make the fix complete.
- **`pimScriptLoop`** (`pim_core/includes/pimScriptLoop.h` +
  `pim_core_src/pimScriptLoop.cxx`, already a fully documented `pimLoop`
  subclass — traced further in a dedicated pass on `pimSilentFixupPSF()`'s
  dummy-placeholder bug specifically): not called directly by anything in
  this file, but confirmed to consume the **same** `pimXmlFile*` document
  `pimSilentFixupPSF()` mutates (`Eb.GetXMLPtr()`), later, at real
  install-execution time, via `pimEntitlement::InstallScripts()`
  (`pim_core/pim_core_src/pimEntitlement.cxx:5627-5631`). See Risk Analysis
  for the confirmed consequence on the dummy-placeholder bug. **Traced
  further in a dedicated pass verifying the dummy-placeholder bug's own
  proposed fix**: `OnInstall()`'s `arr[0]` lookup is preceded, a few lines
  later, by `FindNodelistAttribMatch(pimPSF, pimid, "parametric")`
  (`:135`) — an existing, precedented role-based command lookup in this
  same function — and its `GetAllCommandNames(arr)` call (`:76`) passes no
  2nd argument, defaulting to excluding any PSF node lacking a
  `<FEATURE_NAME>` child from `arr` at all (`pimCommandMgr.h:111`). See
  Risk Analysis for why both are directly relevant to the proposed fix's
  feasibility and the bug's own reachability.
- **`pimCommandMgr::FindPsfTempateNode()`/`AppendEditCommand()`**
  (`pim_core/pim_core_src/pimCommandMgr.cxx:923-929,270-308`, new
  citations, traced in the same pass): `AppendAddCommand()`'s
  node-creation branch deep-clones an entire `<PSF_TEMPLATE>` node's
  children via the former; `AppendEditCommand()` (which
  `AppendAddCommand()` delegates to) only ever *updates* an existing
  `<FEATURE_NAME>` child's text, never *creates* one — confirming a
  command's `<FEATURE_NAME>` presence is fixed at creation time by its
  type's own template, not by anything `pimSilentFixupPSF()` itself
  controls. See Risk Analysis for why this bears directly on whether the
  stuck dummy can ever reach `pimScriptLoop::OnInstall()`'s `arr`.
- **`pimShortcutLoop`** (`pim_core/includes/pimShortcutLoop.h` +
  `pim_core_src/pimShortcutLoop.cxx`, already a fully documented `pimLoop`
  subclass — traced further in a dedicated pass on
  `pimSilentFixupShortcuts()`'s stale-Program-Menu-copy bug specifically):
  not called directly by anything in this file, but confirmed to consume the
  **same** `pimXmlFile*` document `pimSilentFixupShortcuts()` mutates
  (`Eb.GetXMLPtr()`), later, at real install-execution time, via
  `pimEntitlement::InstallShortcuts()`
  (`pim_core/pim_core_src/pimEntitlement.cxx:5778-5787`, which calls
  `pimShortcuts::GetInstance().Create(xmlPtr)`). See Risk Analysis for the
  confirmed consequence on the stale-Program-Menu-copy bug.
- **`pimShortcutMgr::SetShortcutProgramMenu()`/`SetShortcutState()`**
  (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:169-189,426-454`, plus
  `SetShortcutStartDir()`, `:213-233`, traced further in a dedicated pass
  verifying the stale-Program-Menu-copy bug's proposed fix): all 3
  mutators can only set a value on an **existing** child node of `Eb`'s
  shortcut — none ever creates a missing `<PROGRAMSMENU>`/`<STARTINDIR>`
  child. See Risk Analysis for the confirmed consequence this places on
  the fix's own scope.
- **`pimShortcutMgr::SetShortcutStartMenuState()`/`SetShortcutDesktopState()`/`SetShortcutQuicklaunchState()`**
  (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:426-454` via `SetShortcutState()`,
  the same shared primitive as `SetShortcutProgramsMenuState()` above —
  traced further in a dedicated pass verifying the array-index bug's
  proposed fix): confirmed to delegate to `SetShortcutState()` identically
  to `SetShortcutProgramsMenuState()`, each passing only a different
  `Match` tag; shares the identical create-vs-mutate-only ceiling. Also
  traced **`FindShortcutNode()`** (`:417-424`) and
  **`GetAllShortcutIDs()`** (`:295-320`), both new citations for this pass:
  the former is the document-wide, first-match-by-`id` search
  `SetShortcutState()` relies on (guaranteed to succeed post-fix), the
  latter is the source of both `A_wants` and `B_avail` and dereferences its
  `id`-attribute node with no null check. See Risk Analysis for the
  confirmed consequences.
- **`pimShortcutMgr::GetShortcutInfoByID()`**
  (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:29-101`, new citation, traced in
  the same array-index-fix pass): confirmed its own discarded return value
  is not reachable at this call site, and that it pre-initializes all 4
  output booleans before scanning children — a more defensive design than
  its `GetShortcutProgramMenu()` sibling. See Risk Analysis.
- **`pimCompareShipcode()`** (`pim_util/pim_util_src/pimUtil.cxx:584`,
  external to this file — traced further in a dedicated pass on
  `pimIsVersionMatch()`'s shipcode comparison specifically): a generic,
  symmetric 3-way version-string comparator with no baked-in "new"/"old"
  argument contract despite its own parameter names (`new_ship`/`old_ship`)
  — confirmed via its only other call site
  (`pimEntitlement::GetSize()`, `pimEntitlement.cxx:2109`). Only invoked
  when **both** `A` and `B` expose a shipcode (`pimEntitlement::GetShipcode()`,
  `pimEntitlement.cxx:2066-2090`) — not required by `pimEntitlement::Init()`
  (`pimEntitlement.cxx:846-870`) — see Risk Analysis for the confirmed
  consequence of the check being silently skippable.
- **`pimCommandMgr::DeleteCommand()`/`CanDeleteCommand()`**
  (`pim_core/pim_core_src/pimCommandMgr.cxx:405-451,454-515`, traced in a
  later dedicated pass on `pimSilentFixupPSF()`): explicitly documented by
  its own inline comment (`:405`, "call to delete; you can not delete the
  last command of a type") to refuse deletion (`CanDeleteCommand()` returns
  `false`, so `DeleteCommand()` is a no-op) whenever fewer than 2 commands
  in the whole entitlement XML share the target command's `<LICTYPE>` value.
  `pimSilentFixupPSF()` never checks this return value at any of its 3
  `DeleteCommand()` call sites — see Risk Analysis for the confirmed
  consequence. **Traced further still in a further dedicated pass
  specifically on this return-value check**: `CanDeleteCommand()`
  recomputes its live count on **every** call (not a fixed snapshot), which
  is what makes the Drop step's and collision-handling's own behaviors
  fully deterministic and provable, and what makes the final cleanup
  loop's stuck-leftover consequence a structural guarantee rather than a
  probabilistic risk — see Risk Analysis.
- **`pimCommandMgr::AppendAddCommand()`/`AppendEditCommand()`/`FindPsfTempateNode()`**
  (`pim_core/pim_core_src/pimCommandMgr.cxx:270-403,923-929`, traced in the
  same dedicated pass on the `DeleteCommand()` return-value check):
  `AppendAddCommand()` is add-or-update (matches an existing node by
  `name` via `FindPsfNode()` and updates it in place, never creating a
  duplicate), but creating a genuinely **new** node depends on
  `FindPsfTempateNode()` finding a `<PSF_TEMPLATE>` for the target license
  type — not confirmed always present in this archive (no product/media
  XML sample exists). Neither `AppendAddCommand()` nor `AppendEditCommand()`
  ever updates an existing node's `<LICTYPE>` child. `pimSilentFixupPSF()`
  discards `AppendAddCommand()`'s own return value at `:389` just as it
  does `DeleteCommand()`'s. See Risk Analysis for the confirmed dependency
  this creates on the collision-handling delete's own correctness.
  **Traced further still in a further dedicated pass specifically on this
  function's `AppendAddCommand()` return-value check**: the SAME
  add-or-update/template-dependent behavior applies at `:403`'s main
  re-add loop too, where it is confirmed **more severely reachable** — the
  license type passed there is sourced from `Ea`'s document, not `Eb`'s
  own existing commands, so `FindPsfTempateNode()` finding no match is a
  genuine cross-document mismatch risk, not merely a hypothetical gap. See
  Risk Analysis for the confirmed silent-drop consequence.
- **`pimPackageMgr::GetInstallPackageNames()`**
  (`pim_core/pim_core_src/pimPackageMgr.cxx:611-666`, traced in a later
  dedicated pass on `pimSilentCanMatchNeeds()`): unlike its platform/language
  siblings (`pimPlatformMgr::GetInstallPlatformNames()`,
  `pimLanguageMgr::GetInstallLanguageNames()`, both a simple `install="Y"`
  attribute check with no external dependency), this method's return value
  depends on 3 global, process-wide mode flags —
  `pimGetBasePackMode()`/`pimGetCreoNGCRIMode()`/`pimGetAllPacksMode()`
  (all `pim`-module command-line-flag-derived state, see
  `docs/classes/pimGetApplicationsList.md`) — that are unrelated to the
  specific XML file being read. See Risk Analysis for the confirmed
  consequence on `pimSilentCanMatchNeeds()`.
- **`pimPackageMgr::GetAllPackageNames()`/`GetPackageInfoByName()`**
  (`pim_core/pim_core_src/pimPackageMgr.cxx:550-575,113-170`, traced further
  in a dedicated pass on `pimSilentCanMatchNeeds()`'s package check
  reachability specifically): `GetAllPackageNames()` shares
  `GetInstallPackageNames()`'s unguarded `name`-attribute dereference;
  `GetPackageInfoByName()`, an unrelated sibling method in this same class,
  guards every one of the same 4 attributes (`name`/`install`/`required`/
  `parent`) before dereferencing — direct, confirmed evidence within this
  class's own source that these attributes are not assumed always present.
  See Risk Analysis for the confirmed crash mechanism this contrast exposes.
- **`pimEntitlement::SetQualityAgent()`/`IsQualityAgentEnabled()`/
  `IsQualityAgentRequired()`/`OnInstall()`/`OnReconfigure()`**
  (`pim_core/pim_core_src/pimEntitlement.cxx:3225-3300,6499-6509,7051-7060`,
  already-documented `pim_core` methods — traced further in a dedicated
  pass on `pimSilentCreateEntitlement()`'s quality-agent flag copy
  specifically): `SetQualityAgent(false)` silently refuses whenever `Eb`'s
  `<QUALITYAGENT>` node is `required="Y"`; `OnInstall()`/`OnReconfigure()`
  read the resulting `enable` state to write/remove a real
  `"QualityAgentOptIn"` registry value at install/reconfigure time. See
  Risk Analysis for the confirmed consequence.
- **`pimCustomDlg`/`uiCustomTree`** (`pim_ui/includes/pimCustomDlg.h` +
  `pim_ui_src/pimCustomDlg.cxx`, cited only via specific traced methods,
  per the top-level README's Coverage Honesty Statement — no dedicated
  class doc exists — traced in the same dedicated pass on the
  quality-agent flag copy): the interactive "Customize" dialog's own
  QualityAgent checkbox construction (`:1630-1655`) and toggle handler
  (`uiCustomTree::OnUpdate()`, `:1727-1730`) check `IsQualityAgentRequired()`
  *before* ever allowing a disable attempt, disabling the control entirely
  when required — a precondition check `pimSilentCreateEntitlement()` has
  no equivalent of. See Risk Analysis for the full contrast. **Traced
  further in a dedicated pass on the package-selection loop specifically**:
  `uiCustomTree::RefreshPkg()` (`:1819-1904`) checks the identical `req ||
  installed` condition and disables the package's checkbox the same way —
  confirming the same GUI-vs-silent-install asymmetry pattern applies to
  packages, not just quality agent.
- **`pimPackageMgr::SetPackageInstallState()`/`GetAllPackageNames()`**
  (`pim_core/pim_core_src/pimPackageMgr.cxx:250-336,550-575`, new
  citations, traced in the same pass): `SetPackageInstallState(name, false)`
  refuses and returns `false` unchanged whenever `required="Y"` or
  `installed="Y"` (except `"prime_converter"`) — confirmed to still toggle
  `pimSetPrimeInWSMode()`/`pimSetSimulateInMode()` for those 2 hardcoded
  names regardless of the refusal. `GetAllPackageNames()` shares
  `GetInstallPackageNames()`'s already-confirmed unguarded
  `->getNodeValue()` crash risk on a nameless `<PACKAGE>` node. See Risk
  Analysis.
- **`pimMSILoop`/`pimMSICopier`** (`pim_core/includes/pimMSILoop.h` +
  `pim_core_src/pimMSILoop.cxx`, `pim_core/includes/pimMSICopier.h` +
  `pim_core_src/pimMSICopier.cxx`; both already fully documented classes in
  this set, `pimMSILoop` a `pimLoop` subclass — traced further in a
  dedicated pass on `pimSilentCanMatchNeeds()`'s package-check
  mode-dependent finding specifically): not called directly by anything in
  this file, but confirmed to consume the **same** `<PACKAGE>` `install`
  attribute `pimPackageMgr::SetPackageInstallState()` writes (driven, in
  `pimSilentCreateEntitlement()`, by the identical mode-dependent
  `GetInstallPackageNames()` selection this finding concerns), later, at
  real install-execution time, via `pimEntitlement::InstallMSI()`
  (`pim_core/pim_core_src/pimEntitlement.cxx:5293-5330`) constructing a
  `pimMSILoop` through `pimMSICopier::MSIInstall()`/`MSICopy_low()`. See
  Risk Analysis for the confirmed consequence. **Traced further still in a
  further dedicated pass on `pimSilentCreateEntitlement()`'s `<MSI>`-node
  last-value-wins caveat**: `pimMSILoop::pimMSIExec()`
  (`pim_core/pim_core_src/pimMSILoop.cxx:154-161`) independently iterates
  and executes **every eligible** `<MSI>` node in `B`'s document as its own
  install action, each reading its **own** `format`/`<MSIARGUMENT>`
  attributes — confirming multi-`<MSI>`-node XML is a real, designed-for
  configuration this codebase's own install driver already handles per
  node, which is exactly what makes `pimSilentCreateEntitlement()`'s
  single-value-for-all-nodes copy a confirmed, reachable bug rather than a
  hypothetical one. **Traced further still in a further dedicated pass
  specifically verifying this bug's proposed fix**: `pimMSIExec()`'s own
  attribute reads (`:202-207`) confirm both `name` and `PRODUCTCODE` are
  real, live `<MSI>` node attributes in this codebase, grounding the
  proposed per-node-identity fix in fact — but confirming neither key
  alone is sufficient for this function's cross-version matching use case
  (see Risk Analysis for the full tradeoff). See Risk Analysis.
- **`pimCustomActionsLoop`** (`pim_core/includes/pimCustomActions.h` +
  `pim_core_src/pimCustomActions.cxx`, a `pimLoop` subclass — **newly
  discovered in a dedicated pass on `pimSilentCreateEntitlement()`'s
  `<PROPERTY>` skip list specifically; not previously mentioned anywhere in
  this doc set, and not otherwise documented as its own class doc, per the
  top-level README's Coverage Honesty Statement**): not called directly by
  anything in this file, but confirmed to consume the **same** `Eb.GetXMLPtr()`
  document's `CustomActions` property, whose exclusion from this function's
  copy loop is what this pass traces. `pimEntitlement::OnInstallCustomActions()`/
  `OnUninstallCustomActions()` (`pim_core/pim_core_src/pimEntitlement.cxx:5147-5157,7224-7232`
  and their many call sites across the install/uninstall lifecycle) gate
  construction of a `pimCustomActionsLoop` on this same `xmlPtr` behind a
  simple `xmlPtr->GetProperty("CustomActions", str)` truthy-check. See Risk
  Analysis for the confirmed consequence.
- **`pimEntitlement::Init()`/`pimSessionInfo`** (already-documented types in
  this set — traced further in the same dedicated pass on the
  `<PROPERTY>` skip list): `pimEntitlement::Init()`
  (`pim_core/pim_core_src/pimEntitlement.cxx:944,1195`) sets `[VERSION]`
  directly from `Eb`'s own `<PRODUCT version>` attribute at `Eb`'s own
  initialization time, and `pimSessionInfo`
  (`pim_core/pim_core_src/pimSessionInfo.cxx:1801,2316`) later reads it for
  cross-entitlement version matching; `pimSilentInstallFromXML()`
  (`pim/pim_src/pimTop.cxx:1556-1559,1598`) reads `Eb`'s own `[SHIPCODE]`
  immediately after this function returns to populate the session's
  `SHIPCODE_PROPERTY`, and unconditionally overwrites `[SOURCE]` moments
  later regardless of what this function did with it. See Risk Analysis.
- **`pimXmlFile::GetProperty()`/`SetProperty()`/`GetPropertyNode()`**
  (`pim_core/pim_core_src/pimXmlFile.cxx:684-748,755-808`, already a
  documented `pim_core` dependency — traced further in a dedicated pass
  verifying the `<PROPERTY>` copy loop's proposed hardening fix):
  `GetProperty()` finds its match via `GetPropertyNode()`, which searches
  the **entire document** for the **first** `<PROPERTY>` node matching a
  given `name`, in document order — not necessarily the node currently
  being iterated elsewhere. On no match, `GetProperty()` returns `false`
  and leaves its `value` output parameter **untouched**. See Risk Analysis
  for the confirmed stale-value risk this creates when this function's own
  copy loop discards that return value.
- **`pimHitError()`/`pimGetLastError()`** (`pim/pim_src/pimExit.cxx:16-33`,
  traced further in a dedicated pass on `pimIsProductXmlMatch()`'s
  no-cross-schema-fallback limitation): a process-wide static `Errors`
  array (`pimHitError()` appends, never removes) and a reader
  (`pimGetLastError()`) that returns only `Errors[Errors.GetSize()-1]`, the
  **last** error ever appended — not a per-XML-entry or first-error record.
  `pimSilentInstallFromXML()`'s own final return value is
  `pimGetLastError()` (`pim/pim_src/pimTop.cxx:2073`). See Risk Analysis for
  the confirmed consequence: a mixed-schema false-negative on one product
  XML in a multi-XML batch can be silently overwritten in the reported exit
  code by any later, unrelated error.

## Public APIs

| Function | Purpose |
|---|---|
| `bool pimSilentTestXmlIsUseable(const char *file, btkFSList &KnownProductDefinitions, btkString &out)` | The requested function — validates one silent-install XML entry against the media's known product definitions; sets `out` to the matching media XML on success. |
| `bool pimIsProductXmlMatch(const char *A, const char *B)` | Exact-tag-match test between 2 product XML files; no cross-schema fallback for `B`, and this codebase's own other product-XML resolution sites confirm the assumption is unsafe, plus a batch-wide last-error-wins exit code can hide which product actually failed — see Risk Analysis. |
| `bool pimIsVersionMatch(const char *A, const char *B, bool &Major, bool &Minor, btkString &Maj_A, btkString &MOR_A, btkString &MOR_B)` | Version/shipcode compatibility test between 2 already-tag-matched product XML files; shares `pimIsProductXmlMatch()`'s no-cross-schema-fallback limitation, and its shipcode comparison is confirmed optional — silently skipped, indistinguishably from an explicit pass, whenever either side lacks a shipcode attribute — see Risk Analysis. |
| `bool pimSilentCanMatchNeeds(const char *A, const char *B)` | Platform/language/package availability test between 2 already-version-matched product XML files; the package half of the check silently changes meaning under 3 global command-line modes, which are not cleanly mutually exclusive, and the resolved selection flows unbroken to `pimMSILoop`'s real MSI install-time gate; the platform/language halves are mode-free but confirmed to validate against different inputs than what `-lang`/`-allpacks`/OS auto-detection actually select afterward, with silent-failure paths for both; the package half's own `pimPackageMgr` accessors are confirmed to crash (unguarded null attribute dereference) rather than mis-select when a `<PACKAGE>` node lacks the attribute the active CLI mode depends on; the platform/language checks' 2 proposed fixes are confirmed diagnostic-only (a zero-platform outcome silently makes every platform-scoped feature uninstallable, which logging alone cannot prevent) and architecturally feasible but under-specified (a validating copy must exactly replicate the platform fallback's own non-symmetric branch, and would create a 2nd, driftable copy of the same selection rule) — see Risk Analysis. |
| `bool pimSilentFixupPSF(pimEntitlement &Ea, pimEntitlement &Eb)` | Mirrors `Ea`'s PSF/command set onto `Eb`; its collision-avoidance dummy command can itself become permanently stuck (confirmed to affect at most 1 dummy per shared license type per call), and can then corrupt the entitlement's `[LM_LICENSE_FILE]` at install time via `pimScriptLoop`; `Ea`/`Eb` are symmetric references with confirmed asymmetric roles (read-only source / sole mutation target) and lifetimes (ephemeral / persistent, session-owned); of its 3 discarded `DeleteCommand()` return values, the final cleanup loop's is confirmed to strand *any* sole-surviving command of an unwanted license type, not just dummies; of its 2 discarded `AppendAddCommand()` return values, the main re-add loop's is confirmed to silently drop an entire requested command, with zero trace, whenever `B`'s media has no matching `<PSF_TEMPLATE>` for the license type `A` specifies; the suggested `Ea` const-reference hardening is confirmed not purely mechanical (its only direct call on `Ea`, `GetXMLPtr()`, isn't itself `const`) and, even fully applied, is confirmed to protect only this function's own signature, not the realistic risk, since `pimCommandMgr`'s entire API is non-`const` throughout; the stuck-dummy bug's proposed fix is confirmed already effective whenever `A` independently wants another command of the colliding type, and confirmed structurally unachievable otherwise without a new in-place-retype capability, since `CanDeleteCommand()`'s floor has no override; its sibling fix (role-based `pimScriptLoop` selection) is confirmed precedented by an existing lookup nearby but lacks a confirmed canonical selector value, and the dummy's reachability into that array is itself gated by an unconfirmed template detail — see Risk Analysis. |
| `bool pimSilentFixupShortcuts(pimEntitlement &Ea, pimEntitlement &Eb)` | Mirrors `Ea`'s shortcut states onto `Eb`, copying over any shortcut `Eb` lacks; 2 confirmed bugs (a wrong-array index, a stale-value copy that can misplace or drop the shortcut's Start Menu folder at install time via `pimShortcutLoop`); the stale-value bug's proposed fix is confirmed sufficient, but both it and its `StartDir` model share a separate, by-design ceiling — neither can create a customization point `B`'s own template never defined; the wrong-array-index bug's own proposed fix is also confirmed sufficient, and shown to share that same ceiling across all 4 location toggles, not just Program Menu — see Risk Analysis. |
| `bool pimSilentCreateEntitlement(const char *A, const char *B, int &entitlement_index)` | Creates `B` as a new session `pimEntitlement` and merges `A`'s selections onto it; its package selection is confirmed consistent with `pimSilentCanMatchNeeds()`'s validation, its `<MSI>`-node copy loop confirmed collapses multiple, independently-configured MSI packages' UI mode/arguments into one (a proposed per-node-identity fix is confirmed grounded in real attributes, but its "name or ProductCode" phrasing is confirmed under-specified — the 2 keys have opposite version-matching tradeoffs), its `<PROPERTY>` skip list is confirmed to protect 4 distinct, real install-time-consumed values (`[SHIPCODE]`/`[VERSION]`/`[SOURCE]`/`CustomActions`), and its copy loop's `GetProperty()` calls discard their own return values, confirmed to risk a stale-value write to `Eb` if `A` ever has duplicate-named `<PROPERTY>` nodes (the proposed `name`-reinitialization hardening fix is confirmed cosmetic today but closes a genuine latent maintenance trap), its `entitlement_index` out-parameter is confirmed set before `A`'s parseability is verified, silently defeating its caller's only failure-detection check on that specific failure path (confirmed latent, not currently reachable via the codebase's single call site), and its quality-agent flag copy is confirmed to silently discard a user's disable request whenever `B`'s own quality agent is marked required, unlike the GUI's own protective check for the identical business rule; both of the session-index bug's proposed fixes are confirmed sufficient (an undetected failure is now traced to actually persist the corrupted session to disk via a later `Save()` call, which either fix prevents), and the fix's own "call `DropEntitlement(B)`" alternative is confirmed non-functional as worded (`B` is a path, not the product tag `DropEntitlement()` matches on); its package-selection loop's disable calls are confirmed to silently resist required or already-installed `B` packages too, mirroring the quality-agent bug exactly, and its own `Refresh()` call is confirmed superseded and wasted by a later, separate `Refresh()` in `pimTop.cxx` — see Risk Analysis. |

## Private Utilities

None — all 7 functions above are declared `extern`/public in `pimSilent.h`;
there is no private/internal helper specific to this group.

## Called By

- `pim/pim_src/pimTop.cxx:1532` (`pimSilentInstallFromXML()`'s per-entry
  validation loop, see `docs/classes/pimGetApplicationsList.md`) — the
  **sole confirmed call site** for `pimSilentTestXmlIsUseable()` anywhere in
  this archive. Called once per entry in `AskedToInstallThisList`
  (flag-driven + mandatory + `-allpacks` entries alike), with
  `pimSilentInstallFromXML()`'s own `files` (every `*.xml` found in the CD
  image directory) as `KnownProductDefinitions`.
- `pim/pim_src/pimTop.cxx:1535`, immediately following, guarded by the
  `if (pimSilentTestXmlIsUseable(...))` above succeeding — the **sole
  confirmed call site** for `pimSilentCreateEntitlement()`.
- `pimIsProductXmlMatch()`, `pimIsVersionMatch()`, `pimSilentCanMatchNeeds()`,
  `pimSilentFixupPSF()`, `pimSilentFixupShortcuts()` — each confirmed called
  **only from within this same file**, by
  `pimSilentTestXmlIsUseable()`/`pimSilentCreateEntitlement()` respectively;
  no external caller exists for any of these 5.

## Calls Into

- `pimEntitlement::Init()`/`GetTag()`/`GetVersion()`/`GetShipcode()`,
  `pimXmlFile::SetXml()`/`DoRead()`/`ErrorOccured()`,
  `pimPlatformMgr`/`pimLanguageMgr`/`pimPackageMgr`'s name-list accessors,
  `pimCommandMgr`'s command accessors, `pimShortcutMgr`'s shortcut
  accessors, `pimSessionInfo::AddEntitlement()`/`GetEntitlement()`.
- `pimCompareShipcode()` (external, `pim_util`).
- `pimHitError()`/`LG_ERROR`/`LG_INFO` (the shared error/logging channels
  documented in `docs/10_error_handling.md`).

## Lifetime

No file-scope state — every function here operates purely on its
parameters and freshly-constructed local objects (`pimXmlFile`,
`pimEntitlement`, the manager classes). The only object with a lifetime
crossing this file's boundary is the `pimEntitlement *Eb`
`pimSilentCreateEntitlement()` obtains from
`pimGetSessionInfo()->GetEntitlement(entitlement_index)` — owned by
`pimSessionInfo`, not by this file (consistent with `pimSessionInfo`'s
already-documented entitlement-array ownership).

## Ownership Model

Not applicable in the class sense. `pimSilentTestXmlIsUseable()` and its 5
callees are pure validation functions; `pimSilentCreateEntitlement()` is the
one function with an ownership-relevant side effect (adding an entitlement
to the shared `pimSessionInfo` singleton), documented under Lifetime above.

## Thread Safety

No lock of any kind is taken anywhere in this file. Not confirmed to be a
practical problem: the sole confirmed caller,
`pimSilentInstallFromXML()`'s per-entry loop, runs entirely on the
silent-install process's single main thread — no confirmed background-thread
caller exists in this archive, unlike `pimGetAvailable`/`pimGetMediaDetails`.

## Extension Points

- Any new silent-install validation step (e.g. a 4th "needs" category beyond
  platform/language/package) should follow `pimSilentCanMatchNeeds()`'s
  existing `A_wants`/`B_avail`/`Find()`-then-log-and-fail pattern for
  consistency with the 3 checks already there.
- `pimSilentCreateEntitlement()`'s `<MSI>`-node copy loop (`:604-684`) only
  ever keeps the **last** `<MSI>` node's `Cmd`/`format` values from `A`
  (each loop iteration unconditionally overwrites the same 2 local
  variables) and applies them to **every** `<MSI>` node in `B`.
  **Confirmed, found in a further dedicated pass on this bug specifically**:
  this is **not** merely correct-if-every-XML-has-1-node — `pimMSILoop::pimMSIExec()`
  (the real MSI install-execution driver) independently confirms
  multi-`<MSI>`-node product XML is a normal, designed-for configuration by
  iterating and executing every eligible `<MSI>` node as its own separate
  install action. See Risk Analysis for the confirmed downstream
  consequence on installer UI mode selection.

## Risk Analysis

- **CONFIRMED STRUCTURAL LIMITATION, found in a dedicated pass on
  `pimIsProductXmlMatch()` itself, shared identically by `pimIsVersionMatch()`:
  neither function has a cross-schema fallback for `B`.**
  (`pim_core/pim_core_src/pimSilent.cxx:106-134` and `:143-215`.)
  ```cpp
  bool pimIsProductXmlMatch(const char* A, const char* B)
  {
      if (A && B)
      {
          pimEntitlement Ea, Eb;
          if (Ea.Init(A, NULL))                    // A parses as a normal <PRODUCT>
          {
              if (Eb.Init(B, NULL))                 // B is ONLY ever tried the SAME way
                  if (strcmp(Ea.GetTag(), Eb.GetTag()) == 0)
                      return true;
              // <-- if Eb.Init(B, NULL) fails here, falls straight through to
              //     `return false;` -- B is never retried as EXTERNAL_INSTALLER
          }
          else if (Ea.Init(A, "EXTERNAL_INSTALLER"))  // only reached if A's NULL-schema Init failed
          {
              if (Eb.Init(B, "EXTERNAL_INSTALLER"))     // B is ONLY ever tried this way here
                  if (strcmp(Ea.GetTag(), Eb.GetTag()) == 0)
                      return true;
          }
      }
      return false;
  }
  ```
  `pimEntitlement::Init(path, root_match)` (`pim_core/pim_core_src/pimEntitlement.cxx:771-`)
  requires the XML's root element to be exactly `<PRODUCT>` when
  `root_match` is `NULL`, or exactly `<EXTERNAL_INSTALLER>` when
  `root_match` is that string — a document can only satisfy one of the two,
  never both. Confirmed consequence: whichever schema hypothesis succeeds
  for `A` is the **only** one ever tried for `B` within that same call — if
  `A` is a normal `<PRODUCT>`-rooted XML but `B` happens to be an
  `<EXTERNAL_INSTALLER>`-rooted XML (or vice versa), `Eb.Init()` fails under
  the schema chosen for `A`, and the function returns `false` **without
  ever attempting the other schema for `B`** — even if `A`'s and `B`'s
  `<TAG>` (or version/shipcode, for `pimIsVersionMatch()`) values would
  otherwise be identical. `pimIsVersionMatch()`
  (`pim_core/pim_core_src/pimSilent.cxx:143-215`) duplicates this exact
  control-flow shape verbatim for its own `Ea`/`Eb` initialization, so it
  carries the identical limitation. Both functions are called only from
  `pimSilentTestXmlIsUseable()` (see Called By), so a mixed-schema `A`/`B`
  pair reaching that function would be reported as
  `PIM_SOFTWARE_NOT_FOUND` (`pimSilent.cxx:100-101`) even if the two files
  otherwise describe the same product. Not confirmed to be reachable with
  any real product/media XML pair in this archive (no sample
  `EXTERNAL_INSTALLER`-rooted XML exists to test against a normal
  `<PRODUCT>`-rooted counterpart), but the mechanism itself is fully
  confirmed from both functions' own control flow and `pimEntitlement::Init()`'s
  documented root-tag matching contract.
  **Reachability, more precisely characterized (found in a further,
  dedicated pass on this bug specifically)**: `pimEntitlement::Init()`'s
  root-tag matching (`pim_core/pim_core_src/pimEntitlement.cxx:806-833`) is
  purely a literal-root-tag test — `Init(x, NULL)` requires the document's
  root element to be exactly `<PRODUCT>`, `Init(x, "EXTERNAL_INSTALLER")`
  requires exactly `<EXTERNAL_INSTALLER>` — confirming `pimIsProductXmlMatch()`/
  `pimIsVersionMatch()` really do reduce to 2 mutually exclusive schema
  buckets with no middle ground. Critically, this same codebase's *other* 2
  call sites that resolve a single product's own canonical XML file do
  **not** assume its schema is fixed or predictable: `pimEntitlement.cxx:634-635`
  (resolving a product's own installed `.p.xml`) tries
  `Init(file, NULL) || Init(file, "EXTERNAL_INSTALLER") || Init(file, "HIDDEN_PRODUCT")`
  — 3 schemas in sequence — and `pimEntitlement.cxx:967-968` (resolving a
  `<PREREQUISITE>` product reference) tries
  `Init(prerequisite_xml) || Init(prerequisite_xml, "EXTERNAL_INSTALLER")`.
  Both confirm that elsewhere in this exact codebase, a product XML's root
  schema is treated as an open question the code deliberately checks for by
  trying multiple schemas — `pimIsProductXmlMatch()`/`pimIsVersionMatch()`
  are the only 2 functions in this file that instead silently assume `A`
  and `B` share whichever single schema happened to match `A` first. This
  is direct, confirmed evidence — not mere speculation — that this
  codebase's own design does not guarantee a product's on-media XML (`B`)
  uses the same root schema as the user's silent-install request XML for
  that same product (`A`); the absence of a sample mixed-schema pair in
  this archive is a gap in available test data, not a reason to doubt the
  mechanism is real.
  **Downstream consequence, newly traced in this pass**:
  `pimIsProductXmlMatch()`'s only caller is `pimSilentTestXmlIsUseable()`
  (`pimSilent.cxx:54`), whose only caller in turn is
  `pimSilentInstallFromXML()`'s per-file loop (`pim/pim_src/pimTop.cxx:1532`).
  When a mixed-schema false-negative makes `pimSilentTestXmlIsUseable()`
  return `false` (falling through its whole `KnownProductDefinitions` loop
  to `j == max`, `pimHitError(PIM_SOFTWARE_NOT_FOUND)`), the `pimTop.cxx`
  caller's `if` block at `:1532` is simply skipped for that
  `AskedToInstallThisList[i]` entry — there is no `else` branch, `abort` is
  **not** set `true`, and the outer loop (`:1530`) continues straight to the
  next `i`. Traced 2 further-downstream effects of this, neither previously
  examined: (1) that product silently receives **no entitlement at all** —
  dropped from the batch with nothing beyond a log line to mark it, while
  any other products in the same batch proceed normally; and (2)
  `pimHitError()` (`pim/pim_src/pimExit.cxx:16-19`) only appends to a
  process-wide static `Errors` array, and `pimSilentInstallFromXML()`'s own
  final `return pimGetLastError();` (`pimTop.cxx:2073`) resolves to
  `Errors[Errors.GetSize()-1]` — the **last** error ever hit in the whole
  run, not specifically this one. In a multi-XML batch silent install, if
  this mixed-schema false-negative fires on entry `i` but a later entry
  `j > i` in the same run hits any different error afterward, that later
  error **overwrites** this one in the process's final reported exit code —
  the batch's diagnostics silently lose which specific product XML failed
  the schema match, and why, in favor of whatever error happened to occur
  last.
- **CONFIRMED, found in a further dedicated pass on `pimIsVersionMatch()`'s
  shipcode comparison specifically: the check is optional, not enforced,
  and its "skipped" outcome is indistinguishable from an explicit pass.**
  (`pim_core/pim_core_src/pimSilent.cxx:197-209`, depends on
  `pim_core/pim_core_src/pimEntitlement.cxx:2066-2090,846-870`.)
  ```cpp
  if (Ea.GetShipcode(MOR_A) && Eb.GetShipcode(MOR_B))   // BOTH must expose one
  {
      int res = pimCompareShipcode(MOR_A, MOR_B);
      if (res == 0)
          Minor = true; // same
      if (res == 1)
          return false; // A newer than B -- needs newer DVD source
      // res == -1 (B newer, the documented common case) or the `if` above
      // never ran at all: falls straight through with Minor still false
  }
  return true;
  ```
  `pimEntitlement::GetShipcode()` (`pimEntitlement.cxx:2066-2090`) returns
  `false` whenever the root `<PRODUCT>` node has **neither** an
  `appshipcode` **nor** a `shipcode` attribute. `pimEntitlement::Init()`
  (`pimEntitlement.cxx:846-870`) only requires the root's `tag`/`version`
  attributes to succeed — `shipcode`/`appshipcode` are **not** required —
  so a fully valid, `Init()`-succeeding product XML can legitimately omit
  both, and this `if` block simply never executes for it. Not confirmed
  against any specific real product/media XML in this archive (none
  exists), but the mechanism is fully confirmed from `Init()`'s own
  attribute requirements — this is a structural gap in what the check
  actually enforces, not a hypothetical one.
  **Downstream consequence, newly traced**: when the shipcode check is
  skipped this way, `pimIsVersionMatch()` reaches `return true;` via the
  **exact same code path** as an explicit shipcode match (`res == 0` or
  `res == -1`) — nothing distinguishes "the comparison ran and passed"
  from "the comparison never ran at all": no log message, no warning, no
  property is set to record which case occurred. `pimSilentTestXmlIsUseable()`'s
  own differentiated-error logic (`if (!pimIsVersionMatch(...)) { if
  (Major && !Minor) {...NeedNewerDVD...} if (!Major && !Minor)
  {...NeedSameVersionDVD...} }`) only inspects `Major`/`Minor` inside its
  `if (!pimIsVersionMatch(...))` branch — i.e. only on the **rejected**
  path — so it is never even consulted when the check was silently
  skipped; the caller proceeds straight to `pimSilentCanMatchNeeds()`/
  `pimSilentCreateEntitlement()` exactly as it would for an explicitly
  verified-compatible pair.
  **2 verified non-issues, checked while tracing this**: (1) `Minor` is
  only set `true` when shipcodes are **exactly equal** (`res == 0`), not
  when `B`'s is legitimately newer (`res == -1`, the function's own
  documented common case — "M020 XML definition using ... source DVD"
  from M030) or when the check is skipped entirely — but since
  `Major`/`Minor` are confirmed above to be read by the caller only on the
  rejected path, and this asymmetry only ever occurs on **accepted**
  paths, it has no observable effect. (2) `pimCompareShipcode()`'s own
  parameter names (`new_ship`/`old_ship`, `pim_util/pim_util_src/pimUtil.cxx:584`)
  don't reflect any enforced "new must be newer" contract — cross-checked
  against its only other call site
  (`pimEntitlement::GetSize()`, `pimEntitlement.cxx:2109`, which passes
  `this`'s own shipcode first and the product it's updating second,
  labeling `res == -1` "older version" without requiring a specific
  argument order) — confirming it is a generic, symmetric 3-way
  comparator with no baked-in role for either argument, so
  `pimIsVersionMatch()`'s `pimCompareShipcode(MOR_A, MOR_B)` call (`A`
  first, `B` second) is a valid, self-consistent usage, not a
  naming-driven bug.
- **CONFIRMED, found in a further dedicated pass on
  `pimSilentCanMatchNeeds()`'s platform and language checks specifically:
  both are genuinely free of the package check's mode-dependence, but lack
  its later-confirmed consistency guarantee — what actually gets selected
  on `Eb` afterward comes from entirely different inputs than what this
  check validates.** (`pim_core/pim_core_src/pimSilent.cxx:251-302`,
  depends on `pim_core/pim_core_src/pimPlatformMgr.cxx:104-125,253-281`,
  `pim_core/pim_core_src/pimLanguageMgr.cxx:41-82,147-175`,
  `pim/pim_src/pimTop.cxx:1602-1635`, `pim/pim_src/pimGeneralInit.cxx:269-273,531-539`.)
  ```cpp
  // pimSilent.cxx:253-254,280-281 -- validates A's OWN install="Y" markings
  A_plat.GetInstallPlatformNames(A_wants);   // simple install="Y" check, no mode flags
  B_plat.GetAllPlatformNames(B_avail);
  ...
  A_lang.GetInstallLanguageNames(A_wants);   // simple install="Y" check, no mode flags
  B_lang.GetAllLanguageNames(B_avail);
  ```
  Confirmed: neither `GetInstallPlatformNames()` nor
  `GetInstallLanguageNames()` has anything resembling
  `GetInstallPackageNames()`'s 3-mode branching — both are plain,
  unconditional `install=="Y"` attribute checks, so this half of the
  earlier "package check mode-dependent" finding's contrast (`docs/classes/pimSilent.md`
  above) is fully confirmed as stated.
  However, unlike the package check — where an earlier pass confirmed
  `pimSilentCreateEntitlement()`'s package-selection loop calls the
  **identical** `GetInstallPackageNames()` under the same mode flags this
  check uses, guaranteeing the 2 functions validate and select the exact
  same thing — platform and language selection has, as originally
  characterized here, **no such guarantee**.
  **CORRECTED (found while investigating `pimSilentFixupPSF()`'s `Ea`/`Eb`
  reference-vs-copy semantics, which required re-reading
  `pimSilentCreateEntitlement()`'s full body)**: that framing was
  **incomplete**. `pimSilentCreateEntitlement()` (`pimSilent.cxx:549-569`)
  **does** mirror `A`'s declared platform/language onto `B` first, via the
  identical `GetInstallPlatformNames()`/`GetInstallLanguageNames()` calls
  this check uses (`B_plat.ClearInstall()`/`B_lang.ClearInstall()` then
  `SetPlatformInstallState(A_wants[i], true, ...)`/
  `SetLanguageInstallState(A_wants[i], true)` for each of `A`'s wanted
  names) — the exact same consistency pattern already confirmed for
  package selection. The corrected gap is narrower and different in
  character: this internally-consistent mirroring is **subsequently
  undone** by a *second*, independent write —
  `pimSilentInstallFromXML()`'s post-creation code
  (`pim/pim_src/pimTop.cxx:1602-1635`, which runs *after*
  `pimSilentCreateEntitlement()` and therefore after this validation
  already ran) re-derives both from entirely different sources than `A`'s
  `install="Y"` markings:
  ```cpp
  // pimTop.cxx:1608-1627 -- LANGUAGE: driven by -LANG CLI flags / -allpacks,
  // NOT by A's <LANGUAGE install="Y"> markings
  if (pimShouldWeInitLanguageID("FR"))
      MyLang.SetLanguageInstallState("french", true);   // return value discarded
  ... // 8 more hardcoded languages, same pattern

  // pimTop.cxx:1629-1630 -- PLATFORM: driven by the CURRENT machine's own
  // OS, NOT by A's <PLATFORM install="Y"> marking
  pimPlatformMgr MyPlats(SessionInfo.GetEntitlement(k)->GetXMLPtr());
  MyPlats.InitPlatformState(NULL);   // return value discarded
  ```
  `pimGeneralInit.cxx:531-539`'s `pimShouldWeInitLanguageID(code)` returns
  `true` whenever `code` appears in the `-LANG <XX>` command-line list
  (`pimGeneralInit.cxx:269-273`) **or** `pimGetAllPacksMode()` is active —
  in which case **all 9** hardcoded languages are force-selected —
  entirely independent of whether `A`'s XML ever marked that language
  `install="Y"`. `pimPlatformMgr::InitPlatformState(NULL)`
  (`pimPlatformMgr.cxx:104-125`) derives the platform to select from
  `btkGetPlatform()` — the **current machine's own runtime OS platform** —
  falling back to `i486_nt` or `x86e_win64` if the exact detected platform
  isn't found in `B`, again entirely independent of what `A` requested or
  what this function validated. `pimSilentCanMatchNeeds()` has no access
  to the CLI args or the running machine's platform, so it structurally
  **cannot** validate what `-lang`/`-allpacks`/auto-detection will actually
  select against `B`'s real coverage — it only ever validates `A`'s own
  static XML markings.
  **Downstream consequence, newly traced**: both call sites above
  (`pimTop.cxx:1609-1627,1630`) discard the return value of
  `SetLanguageInstallState()`/`InitPlatformState()`.
  `pimLanguageMgr::SetLanguageInstallState()`
  (`pim_core/pim_core_src/pimLanguageMgr.cxx:41-82`) and
  `pimPlatformMgr::SetPlatformInstallState()`
  (`pim_core/pim_core_src/pimPlatformMgr.cxx:144-188`, called internally by
  `InitPlatformState()`) both return `false` **silently** — no log, no
  error — whenever `FindLangNode()`/`FindPlatNode()` fails to find a
  matching node in `B` at all. Confirmed consequences: a `-lang XX` (or
  `-allpacks`) request naming a language `B`'s media genuinely lacks is
  **silently dropped**, with zero diagnostic anywhere in the entire
  pipeline — neither this validation-time check (which never saw the CLI
  flag) nor the post-creation `SetLanguageInstallState()` call (whose
  failure is discarded) ever surfaces it; and if the auto-detected
  platform and **both** of `InitPlatformState()`'s fallbacks all fail to
  match anything in `B`, the resulting entitlement silently ends up with
  **no platform marked for install at all**, again completely undiagnosed.
  **FIX (a) VERIFIED SUFFICIENT ONLY FOR DIAGNOSABILITY, found in a
  further, dedicated pass verifying this bug's 2 proposed fixes**:
  checking `SetLanguageInstallState()`/`InitPlatformState()`'s return
  values and logging the failure (the `modification_impact` suggestion)
  makes the outcome **visible** — but changes **nothing** about the
  outcome itself. Traced the zero-platform case one level further,
  downstream of anything examined before: `pimPackageMgr::RefreshAFeatureNode()`
  (`pim_core/pim_core_src/pimPackageMgr.cxx:883-980`, already a documented
  dependency via `pimPackageMgr::Refresh()`) checks every
  `<CDSECTION>`/`<MSI>` node's own `platform` attribute
  (`:956-980`) against `platmgr->GetPlatformInfoByName_low()`; if that
  platform is neither required, install-selected, nor already installed
  (`!(r || i || a)`, `:967-972`) — or isn't found in `B` at all
  (`:973-978`) — the feature node is set `install="N"` and skipped. If
  `InitPlatformState()`'s own fallback fails completely, **no** platform
  is ever marked `install="Y"`, so **every** platform-scoped feature in
  `B` fails this check identically — a near-total, silent install
  failure that logging alone can only report after the fact, never
  prevent.
  **CORRECTION, found in the same pass**: `InitPlatformState()`'s
  "2-level fallback" is more precisely a **mutually exclusive,
  conditionally-chosen single fallback**
  (`pim_core/pim_core_src/pimPlatformMgr.cxx:104-125`), not both names
  tried in sequence:
  ```cpp
  if (!preferred_name) btkGetPlatform(plat);   // else plat = preferred_name
  if (!SetPlatformInstallState(plat, true, ignore_me))
  {
      if (plat == "x86e_win64" || plat == "arm64_win64")
          return SetPlatformInstallState("i486_nt", true, ignore_me);
      else
          return SetPlatformInstallState("x86e_win64", true, ignore_me);
  }
  ```
  exactly **one** fallback name is tried, chosen by which family the
  auto-detected platform belongs to — never both, and no 3rd attempt if
  that one also fails.
  **FIX (b) CONFIRMED ARCHITECTURALLY FEASIBLE for both language and
  platform, resolving the original finding's "if feasible" hedge**:
  `pimShouldWeInitLanguageID()`/`pimGetAllPacksMode()`
  (`pim/pim_src/pimGeneralInit.cxx:531-539`) and `btkGetPlatform()`
  (called identically, with no session/object context, from at least 8
  other sites across this codebase, e.g. `pimEntitlement.cxx:858,3322,5013`)
  are all global, parameterless (or self-contained-state) accessors — this
  function could call either directly, with no signature change, exactly
  as `pimSilentCanMatchNeeds()`'s own package half already reads
  `pimGetBasePackMode()`/`pimGetCreoNGCRIMode()`/`pimGetAllPacksMode()`
  this same way.
  **NEW nuance, found while verifying fix (b)'s completeness**: a
  validating copy that checks only the raw auto-detected platform name —
  without replicating the exact conditional-single-fallback branch just
  corrected above — would produce **false-negative validation
  rejections**: an A/B pair that `InitPlatformState()` would actually
  satisfy today via its 1 fallback attempt would be wrongly reported as
  incompatible.
  **NEW maintenance-hazard, not weighed by the original suggestion**:
  implementing fix (b) faithfully requires duplicating **both** the
  9-hardcoded-language gating rule and the platform fallback branch into
  this function — a **2nd, independent copy** of logic that already lives,
  unchanged, in `pimTop.cxx`'s post-creation code, with nothing in this
  codebase to keep the 2 copies in sync if either is ever edited; this
  trades today's "no consistency guarantee" gap for a structurally similar
  "2 sources of truth" gap, rather than closing it.
  **NEW, unresolved design tradeoff, found in the same pass**: unlike the
  package check (where a failed validation correctly means "don't create
  this entitlement," a request genuinely unsatisfiable by `B`), extending
  validation to auto-detected platform would mean a machine simply
  lacking that platform in `B` (no CLI override involved at all) could
  have an otherwise fully valid single-product install **rejected
  outright** by `pimSilentCanMatchNeeds()`, rather than proceeding with
  only its platform-scoped pieces silently missing, as happens today. This
  codebase states no explicit policy on which behavior is intended, so
  fix (b), unlike fix (a), is confirmed to be a genuine behavior/policy
  decision, not a mechanical hardening.
- **CONFIRMED, found in a further dedicated pass on `pimSilentFixupPSF()`'s
  `Ea`/`Eb` reference-vs-copy semantics: the 2 symmetric, non-`const`
  reference parameters have confirmed asymmetric roles and lifetimes, and
  `pimEntitlement`'s own lack of safe copy semantics is a real but
  currently unreachable structural hazard.**
  (`pim_core/pim_core_src/pimSilent.cxx:353-416`, depends on
  `pim_core/pim_core_src/pimSilent.cxx:507-521,736`,
  `pim_core/pim_core_src/pimEntitlement.cxx:203-234`,
  `pim_core/includes/pimSessionInfo.h:96-97`.)
  ```cpp
  bool pimSilentFixupPSF(pimEntitlement& Ea, pimEntitlement& Eb)   // symmetric signature
  {
      pimCommandMgr A_cmds(Ea.GetXMLPtr(), pimGetSessionInfo());   // Ea: READ-ONLY throughout
      pimCommandMgr B_cmds(Eb.GetXMLPtr(), pimGetSessionInfo());   // Eb: the sole mutation target
      ...
      A_cmds.GetCommandInfoByName(...);   // every Ea/A_cmds call is a read-only accessor
      ...
      B_cmds.DeleteCommand(...);          // every mutator (Delete/AppendAdd) targets Eb/B_cmds only
      B_cmds.AppendAddCommand(...);
  }
  ```
  Confirmed by tracing every single `A_cmds.*` call in the function body:
  `Ea` is used **purely as a read-only source** (`GetCommandInfoByName()`/
  `GetAllCommandNames()` only) — never a mutator call — while `Eb`/`B_cmds`
  is the **sole** read-write target (`DeleteCommand()`/
  `AppendAddCommand()`). `Ea` could safely be declared
  `const pimEntitlement&`; its non-`const` declaration is a minor,
  confirmed API-clarity gap, not a functional bug, since nothing actually
  mutates through it.
  Traced `pimSilentCreateEntitlement()`'s own construction of `Ea`/`Eb`
  (`:507-521,736`): `Ea` (`pimEntitlement Ea;`, a stack-allocated local) is
  a genuinely **ephemeral** object — `Init()`'d fresh from `A`'s file path
  and destroyed (its `xmlPtr` explicitly `delete`d,
  `pimEntitlement::~pimEntitlement()`, `pimEntitlement.cxx:226-234`) the
  moment `pimSilentCreateEntitlement()` returns. `Eb`
  (`pimEntitlement* Eb = pimGetSessionInfo()->GetEntitlement(entitlement_index);`)
  is **not a copy at all** — it is a pointer directly to the real,
  session-owned, persistent object `AddEntitlement(B)` just created, and
  `pimSilentFixupPSF(Ea, *Eb)` (`:736`) mutates that **same** object by
  reference. This confirms why every already-documented downstream
  consumer in this doc set (`pimScriptLoop`, `pimShortcutLoop`,
  `pimMSILoop`, etc.) genuinely reads the identical object this function
  wrote to — not a snapshot or a later-reloaded copy.
  **Reachability, precisely characterized (a real but confirmed
  *unreachable* structural hazard, checked while tracing this)**:
  `pimEntitlement` owns a raw `xmlPtr` pointer, explicitly `delete`d in its
  destructor (`pimEntitlement.cxx:226-234`), but defines **no** custom copy
  constructor or assignment operator anywhere in this file — relying
  entirely on the compiler-generated (shallow-copy) defaults. If 2
  `pimEntitlement` objects were ever to share `xmlPtr` via a by-value copy
  or assignment, **both** destructors would `delete` it — a double-free.
  An exhaustive search (`grep` for any `pimEntitlement` local/parameter
  *not* declared as a pointer or reference, across the entire archive)
  confirms the **only 3** stack-allocated, value-type `pimEntitlement`
  instances anywhere in this codebase are all in this same file —
  `pimIsProductXmlMatch()`'s and `pimIsVersionMatch()`'s own `Ea`/`Eb`
  (`:110,150`), and `pimSilentCreateEntitlement()`'s own `Ea` (`:507`) —
  and every one is confirmed used safely: default-constructed, `Init()`'d
  as a member call, **never** copy-constructed or assigned from another
  `pimEntitlement`. Every other entitlement anywhere in the codebase is
  held via `dsXArray<pimEntitlement*>`
  (`pim_core/includes/pimSessionInfo.h:96-97`) or a raw pointer, never by
  value. This makes this function's (and its siblings') choice of
  reference parameters — never a by-value `pimEntitlement` — the correct,
  load-bearing design decision that avoids ever triggering this latent
  hazard; the hazard is real (1 future by-value usage anywhere would
  crash) but confirmed not currently triggered.
  **Verified non-issue, not a bug**: although `Ea` is destroyed
  immediately after this call, every value `pimSilentFixupPSF()` copies
  from it into `Eb` (`desc`/`licIdentifiers`/`features`, via
  `GetCommandInfoByName()`'s `btkString`/`StringXArray` out-parameters) is
  a genuine value copy, not a reference back into `Ea`'s own DOM tree —
  `Eb`'s resulting document holds no dangling reference to `Ea` after `Ea`
  disappears.
  **CONFIRMED NOT PURELY MECHANICAL, found in a further, dedicated pass
  verifying the `Ea` const-reference hardening suggested just above**
  (`pim_core/includes/pimEntitlement.h:320`, `pim_core/includes/pimCommandMgr.h:54-118`).
  ```cpp
  // pimEntitlement.h:320 -- this function's ONLY direct call on Ea depends on this:
  pimXmlFile* GetXMLPtr() { return xmlPtr; }   // NOT declared const
  ```
  Re-confirmed this function's only direct call on `Ea` is
  `Ea.GetXMLPtr()` (`:357`) — everything else routes through `A_cmds`. Since
  `GetXMLPtr()` itself is not `const`, changing the parameter to
  `const pimEntitlement& Ea` as literally suggested **fails to compile** —
  a `const` object cannot call a non-`const` member function, even a
  logically read-only one. **Confirmed safe prerequisite fix**:
  `const`-qualifying `GetXMLPtr()` (keeping its return type the same,
  non-`const` `pimXmlFile*` — ordinary "shallow const," since the method
  never mutates `xmlPtr` itself) resolves the compile error with **zero**
  behavior change anywhere: `pimCommandMgr`'s constructor
  (`pimCommandMgr(pimXmlFile *ptr, pimSessionInfo *S)`,
  `pimCommandMgr.h:55`) already accepts a plain non-`const` `pimXmlFile*`,
  so `A_cmds`'s construction is unaffected either way. An exhaustive search
  confirms **zero** existing `const pimEntitlement` usage anywhere in this
  entire codebase today, so this would be the first such usage — a pure
  widening of what's callable, never a narrowing of any existing call site.
  **CONFIRMED SCOPE CEILING, the hardening's real protection is narrower
  than "const" suggests**: even with both changes applied, the `const`
  only blocks calling a non-`const` **`pimEntitlement`-level** method
  directly on `Ea` — and this function has none to block (its only call,
  `GetXMLPtr()`, becomes const-callable by the fix itself). Read
  `pimCommandMgr.h` in full (`:54-118`): **none** of its public methods are
  declared `const` — including the exact 3 read-only accessors this
  function calls on `A_cmds`
  (`GetCommandInfoByName()`/`GetAllCommandNames()`/`GetAllCommandNamesByType()`).
  This means `const pimCommandMgr A_cmds(...)` would **also** fail to
  compile, so there is no way, even in principle, to make `A_cmds` itself
  reject a mutating call (`DeleteCommand()`/`AppendAddCommand()`) against
  `Ea`'s document via the type system — confirmed by an additional search
  finding **zero** `const pimXmlFile` usage in any header in this codebase,
  meaning no class here has ever been given a genuinely read-only view of a
  `pimXmlFile`. The `Ea` const-reference hardening is therefore confirmed
  to be a **documentation-only signal at this function's own top-level
  signature** — accurately reflecting today's actual (already-verified,
  read-only) usage, but structurally incapable of catching the realistic
  mistake a future maintainer could make (accidentally calling a mutating
  `A_cmds` method against `Ea`), since that mistake lives one layer below
  what a `const Ea` parameter can ever reach, and this codebase has no
  const-correct API anywhere in that layer to build on.
- **CONFIRMED BUG, found in a further dedicated pass on
  `pimSilentCreateEntitlement()`'s session-index lookup specifically: the
  out-parameter `entitlement_index` is set to a valid index *before* `A`'s
  parseability is verified, and is never invalidated on the later failure
  path — silently defeating the caller's only failure-detection sentinel.**
  (`pim_core/pim_core_src/pimSilent.cxx:510-530`, depends on
  `pim/pim_src/pimTop.cxx:1532-1536`.)
  ```cpp
  if (!pimGetSessionInfo()->AddEntitlement(B))
      { ...; return false; }                                  // :510-517, entitlement_index untouched -- safe
  else
  {
      entitlement_index = pimGetSessionInfo()->GetEntitlementSize() - 1;  // :520 -- SET HERE
      Eb = pimGetSessionInfo()->GetEntitlement(entitlement_index);        // :521
      if (!Ea.Init(A))
          Ea.Init(A, "EXTERNAL_INSTALLER");
      if (!Ea.GetXMLPtr())
          { ...; return false; }                               // :524-530 -- returns false, but
  }                                                             // entitlement_index is LEFT SET from :520
  ```
  Traced the caller (`pimTop.cxx:1534-1540`):
  ```cpp
  k = -1;
  pimSilentCreateEntitlement(AskedToInstallThisList[i], str, k);   // return value never checked
  if (k < 0) { abort = true; return pimGetLastError(); }
  ```
  the function's own `bool` return value is **never inspected** by its only
  caller — `k`'s sign is the **sole** failure-detection channel. Since `k`
  is already overwritten to a valid, non-negative index at `:520` — before
  `A`'s parse is verified at `:522-524` — a failure on the `:524-530` path
  leaves `k >= 0` even though the function is reporting failure via its
  return value, so `if (k < 0)` **cannot** catch this specific failure
  mode. `B` remains, permanently, in the session's live `EntitlementArray`
  at that index (`AddEntitlement(B)` already succeeded and is never rolled
  back), but was never fixed up with `A`'s requested PSF/shortcut/package
  data — the function returns before ever reaching
  `pimSilentFixupPSF()`/`pimSilentFixupShortcuts()` (`:736,739`) — so the
  caller proceeds to treat a half-created, unrequested-content entitlement
  as if creation fully succeeded.
  **Reachability, precisely characterized (checked while tracing this)**:
  not confirmed reachable via this codebase's own single call site.
  `pimSilentTestXmlIsUseable()` (`pimTop.cxx:1532`, called immediately
  before, gating entry into this branch, on the **identical** `A` input)
  already requires `pimIsProductXmlMatch()`'s internal
  `Ea.Init(A, NULL) || Ea.Init(A, "EXTERNAL_INSTALLER")`
  (`pimSilent.cxx:112,122`) to succeed for the same file before this call
  is ever reached — so by the time `pimSilentCreateEntitlement()` runs,
  `A`'s parseability under one of the same 2 schemas is already
  established, synchronously, in the same process. This makes the bug a
  confirmed gap in the function's own error-handling design — a real
  defect if this function is ever called from a 2nd site, or if the
  `pimSilentTestXmlIsUseable()` guard is ever changed or removed — not a
  live crash/corruption risk given the current call graph, the same
  "real bug, currently latent via the single call site" characterization
  already used for `pimEntitlement`'s copy-semantics hazard above.
  **Verified non-issue, related but distinct mechanism (checked while
  tracing why `GetEntitlementSize() - 1` reliably indexes the just-added
  entry)**: `pimSessionInfo::AddEntitlement(cStringT in)`
  (`pim_core/pim_core_src/pimSessionInfo.cxx:654-705`) has its own
  pre-existing-entry guard, `if (GetEntitlement(in) != NULL) return
  false;` (`:658`) — but `GetEntitlement(cStringT)` (`:760-774`) matches
  on `GetID()`, which is populated from the product's `<PRODUCT tag>`
  attribute (`pimEntitlement.cxx:874`), while `in` here is a filesystem
  path — the 2 values are never expected to be equal, so this guard is
  confirmed to never actually fire in practice. This means `AddEntitlement()`
  returning `true` always corresponds to a **brand-new** array entry
  actually just appended (`EntitlementArray += ptr`, `:683`), never a
  reused/pre-existing one — confirming the session-index lookup's own core
  arithmetic (`GetEntitlementSize() - 1` pointing at the entry `AddEntitlement()`
  just added) is otherwise sound; the confirmed bug above is strictly about
  when that valid index is reported back despite overall failure, not about
  the index ever being wrong while the function succeeds.
  **CONFIRMED SUFFICIENT, found in a further, dedicated pass verifying this
  bug's 2 proposed fixes** (`ai-context/change_impact.yaml`'s
  `modification_impact`: (a) "check the function's `bool` return value at
  its call site... in addition to `entitlement_index`'s sign", or (b) "have
  the function itself reset `entitlement_index` to `-1`... on every failure
  path after `:520`, not just before it"). **Scope check**: confirmed there
  is exactly **1** failure path to fix, not several — re-read the full
  function body (`:501-746`) and confirmed no other early `return` exists
  between `:520` (where the index is set) and the final `return true;`
  (`:743`); the "not just before it" phrasing describes the single
  `:524-531` path correctly, it just doesn't imply there are multiple.
  **Both fixes confirmed independently sufficient**: fix (a) needs no
  change to this file at all — `pimTop.cxx:1536` would simply become
  `if (!pimSilentCreateEntitlement(...) || k < 0)`; fix (b) needs no
  caller-side change at all, since the caller already checks nothing but
  `k`'s sign. Either alone makes `if (k < 0)` (or the equivalent) correctly
  fire on this failure.
  **NEW, deeper downstream consequence, traced one step further than the
  original finding's "caller proceeds as if creation fully succeeded"**:
  followed the caller's loop forward from the point where detection fails
  today. Execution falls through into the full per-entry post-creation
  block (`pimTop.cxx:1542-1751`) — further `SessionInfo.GetEntitlement(k)->...`
  calls against the half-created `Eb` — and, once the entire
  `AskedToInstallThisList` loop completes without any *other* entry
  triggering `abort`, reaches `pimTop.cxx:1777`'s `SessionInfo.Save()`.
  Traced `pimSessionInfo::Save()` (`pimSessionInfo.cxx:2220-2226`) to
  `xmlPtr->DoSave()` — this **persists** the session's own XML document,
  including the orphaned `<ENTITLEMENT id="...">` node
  `AddEntitlement(B)` appended for the half-created `B`
  (`pimSessionInfo.cxx:687-694`), to `sessioninfo.xml` on disk. Traced
  `pimXmlFile::PreWrite()`/`PostWrite()` (`pimXmlFile.cxx:216-236`, pure
  thread-lock bookkeeping, no I/O) and `~pimXmlFile()` (`:146-187`, frees
  the parser/serializer/mutex, never saves) to confirm `Save()` is the
  **only** path from `pimSilentInstallFromXML()`'s silent-install loop that
  could ever write this corruption to disk — nothing auto-persists it.
  **Confirmed either fix eliminates this**: once detection is fixed,
  `if (k < 0)` fires and `abort=true; return pimGetLastError();`
  (`pimTop.cxx:1538-1540`) returns **before** line 1777 is ever reached, so
  the corrupted `EntitlementArray`/`xmlPtr` state is simply discarded when
  the caller's stack-local `SessionInfo` object (`pimTop.cxx:1332`) is
  destroyed on return — confirmed via `~pimSessionInfo()`
  (`pimSessionInfo.cxx:175-199`), which deletes `EntitlementArray`'s
  contents and `xmlPtr` without saving either first.
  **CONFIRMED BUG in fix (b)'s own alternative literal wording**: "...or
  call `DropEntitlement(B)` to roll back the session add" is confirmed
  **non-functional as written**. `pimSessionInfo::DropEntitlement(cStringT in)`
  (`pimSessionInfo.cxx:784-801`) matches via
  `EntitlementArray[i]->GetID() == in` — the **identical** "match a
  filesystem path against a product tag" pattern already confirmed dead
  code in `AddEntitlement()`'s own guard, immediately above. `B` is the
  same path already passed to `AddEntitlement(B)`; passing it to
  `DropEntitlement(B)` suffers the identical mismatch and always returns
  `false`, silently rolling back nothing. A working rollback needs
  `DropEntitlement(entitlement_index)` instead — the sibling `int` overload
  (`pimSessionInfo.cxx:803-814`), confirmed correct by direct trace
  (bounds-checks, then `RemoveAt(ct)` + `delete`), and it uses the exact
  value already in hand — or `DropEntitlement(Eb->GetID())`, since `Eb`'s
  tag is already populated by `Init()` at this point; never the path.
  **Verified non-issue, given the downstream consequence traced above**: a
  full `DropEntitlement()`-based rollback is confirmed **not actually
  necessary** for correctness today — since the fixed detection always
  returns before `Save()` is ever reached, the simpler fix (reset
  `entitlement_index`, or check the return value) is fully sufficient on
  its own; a real rollback would only start to matter if this function
  ever gained a 2nd, longer-lived caller that keeps `SessionInfo` alive and
  keeps issuing calls after a failed `pimSilentCreateEntitlement()` — not
  the case today, given the single call site already traced above.
- **CONFIRMED BUG, found in a further dedicated pass on
  `pimSilentCanMatchNeeds()`'s package check reachability specifically:
  its underlying `pimPackageMgr` accessors crash on a null attribute
  dereference rather than merely mis-selecting.**
  (`pim_core/pim_core_src/pimSilent.cxx:307-308`, depends on
  `pim_core/pim_core_src/pimPackageMgr.cxx:611-666,550-575,113-170`.)
  ```cpp
  A_pkg.GetInstallPackageNames(A_wants);   // :307
  B_pkg.GetAllPackageNames(B_avail);       // :308
  ```
  ```cpp
  // pimPackageMgr.cxx:611-666, GetInstallPackageNames()
  DOMNode *attribMatch = map->getNamedItem(pimname);
  DOMNode *attribInstall = map->getNamedItem(piminstall);
  DOMNode *attribRequired = map->getNamedItem(pimrequired);
  if (pimGetBasePackMode())
  {
      if (XMLString::compareString(attribRequired->getNodeValue(), ...   // attribRequired dereferenced, NO null-check
  }
  else if (pimGetCreoNGCRIMode())
  {
      if (XMLString::compareString(attribRequired->getNodeValue(), ...   // attribRequired dereferenced, NO null-check
      else
      {
          DOMNode* attribParent = map->getNamedItem(pimparent);
          if (XMLString::compareString(attribParent->getNodeValue(), ... // attribParent dereferenced, NO null-check
      }
  }
  else if (XMLString::compareString(attribInstall->getNodeValue(), ...   // attribInstall dereferenced FIRST, NO null-check,
           || pimGetAllPacksMode())                                     // -- evaluated regardless of AllPacksMode (|| short-circuit order)
  {
      out += StrX(attribMatch->getNodeValue())...;                      // attribMatch dereferenced, NO null-check
  }
  ```
  Traced `A_pkg.GetInstallPackageNames()`/`B_pkg.GetAllPackageNames()`
  (used by both `pimSilentCanMatchNeeds()`'s package check and
  `pimSilentCreateEntitlement()`'s own package-selection loop, `:576-577`,
  which calls the identical pair on the identical `A`/`B` after this check
  already ran) into `pimPackageMgr` itself: neither function checks
  `map->getNamedItem(...)`'s result for `NULL` before calling
  `->getNodeValue()` on it — `GetAllPackageNames()` (`:550-575`) does this
  for `name` only; `GetInstallPackageNames()` (`:611-666`) does it for
  `name`, and — depending on which of the 3 global mode flags is active —
  `install`, `required`, or `parent` as well.
  **CONFIRMED DISCREPANCY, by direct contrast within the same class**:
  `pimPackageMgr::GetPackageInfoByName()` (`:113-170`), a sibling function
  in this exact file reading the exact same 4 attributes off the exact
  same `<PACKAGE>` node type, guards every one of them
  (`if (attribRequired) {...}`, `if (attribInstall) {...}`,
  `if (attribParent) {...}`, `if (attribLabel) {...}`) before
  dereferencing — direct, confirmed evidence that this class's own author
  anticipated these attributes can legitimately be absent on a real
  `<PACKAGE>` node, making `GetInstallPackageNames()`/`GetAllPackageNames()`'s
  unguarded access an inconsistency, not a deliberate "always present"
  assumption.
  **Reachability, precisely characterized (which attribute crashes depends
  on the active CLI mode, traced node-by-node through the `if`/`else if`
  chain)**: `install` is dereferenced unconditionally in **both default
  mode and `-allpacks` mode** — it is the *left* operand of `||`, so C++'s
  left-to-right short-circuit evaluation means it is always evaluated
  *before* `pimGetAllPacksMode()` is ever consulted, defeating the natural
  assumption that "install everything" mode would tolerate a missing
  `install` attribute. `required` is dereferenced unconditionally whenever
  `-basepack` or `-releaselink` is active (for every `<PACKAGE>` node, not
  just ones ultimately selected). `parent` is dereferenced additionally
  under `-releaselink` specifically, whenever that same node's `required`
  isn't `"Y"`. `name` (`attribMatch`) is dereferenced only for a node that
  the active mode's condition has already decided to select. Since
  `pimSilentCanMatchNeeds()`'s package check runs on **both** `A` (a
  user-supplied silent-install request XML) and `B` (the matched product
  definition on the install media) — and since no `<PACKAGE>`-node
  authoring/schema-validation code exists anywhere in this archive to
  confirm these 4 attributes are always populated by whatever external
  tooling builds real product XML — this is **not confirmed reachable**
  against a real product/media XML pair (none exists in this archive), but
  the crash mechanism itself, and the internal inconsistency against this
  same class's own defensively-coded sibling, are fully confirmed from
  source.
  **Confirmed narrower than the platform/language checks' analogous risk**:
  `pimPlatformMgr::GetInstallPlatformNames()`/`GetAllPlatformNames()` and
  `pimLanguageMgr`'s equivalents share the identical unguarded
  `name`/`install` pattern (2 vulnerable attributes each), but lack this
  function's extra `-basepack`/`-releaselink` branching — so the package
  check specifically has the **widest** exposure of the 3 sibling checks
  (4 distinct vulnerable attributes vs. 2), a genuinely new, previously
  unexamined angle on the already-documented package-check
  mode-dependence, not a restatement of it.
- **CONFIRMED BUG, found in a further dedicated pass on
  `pimSilentCreateEntitlement()`'s quality-agent flag copy specifically:
  a disable request can be silently overridden by `B`'s own required flag,
  unlike the codebase's own GUI path for the identical business rule.**
  (`pim_core/pim_core_src/pimSilent.cxx:593-599`, depends on
  `pim_core/pim_core_src/pimEntitlement.cxx:3225-3248,3277-3300,6499-6509,7051-7060`.)
  ```cpp
  bool tf;
  if (Ea.IsQualityAgentEnabled(tf))
  {
      if (tf)
          Eb->SetQualityAgent(true);    // always succeeds if Eb's <QUALITYAGENT> node exists
      else
          Eb->SetQualityAgent(false);   // return value discarded -- can silently fail
  }
  ```
  ```cpp
  // pimEntitlement.cxx:3225-3248 -- SetQualityAgent()'s actual logic
  bool pimEntitlement::SetQualityAgent(bool in)
  {
      ...
      if (node)
      {
          ret = true;
          if (in)
              ...set enable="Y"...
          else if (xmlPtr->FindNodelistAttribMatch(pimQUALITYAGENT, pimrequired, "Y") == NULL) // not required
              ...set enable="N"...
          else
              ret = false;   // asked to make FALSE; but it is required -- SILENTLY REFUSED
      }
      return (ret);
  }
  ```
  Traced `Ea.IsQualityAgentEnabled(tf)`/`Eb->SetQualityAgent(tf)` into
  `pimEntitlement::SetQualityAgent()`: **enabling** (`in==true`) always
  succeeds whenever `Eb`'s `<QUALITYAGENT>` node exists at all — no
  restriction. **Disabling** (`in==false`) is silently **refused** — the
  function's own `bool` return stays `false`, `enable` is left unchanged —
  whenever `Eb`'s own node is marked `required="Y"`, a business rule about
  `B`'s own media definition, entirely unrelated to what `A` requested.
  `pimSilentCreateEntitlement()` discards this return value at both call
  sites (`:596,598`), so a user's explicit "disable quality agent" request
  in `A` can be silently overridden by `B`'s own required-flag, with **zero
  diagnostic anywhere** — no log, no property, no error.
  **CONFIRMED, by direct contrast with this codebase's own GUI path**:
  `pim_ui/pim_ui_src/pimCustomDlg.cxx:1630-1655` (building the "Customize"
  dialog's QualityAgent checkbox) calls `IsQualityAgentRequired()` *first*
  and, when required, disables ("greys out") the "QualityAgentOptIn"
  checkbox control entirely
  (`ui_do_operation(..., "QualityAgentOptIn", UI_check_sensitive_Attr, false, ...)`,
  `:1650-1654`) — structurally preventing a live user from ever attempting
  the disable `pimSilentCreateEntitlement()` blindly attempts.
  `uiCustomTree::OnUpdate()` (`:1727-1730`) only ever calls
  `CurrentApp->SetQualityAgent(State)` when the checkbox was actually
  interactive, i.e. never on a required entitlement. No equivalent
  precondition check exists in the silent-install path.
  **Reachability, precisely characterized**: unlike the package check
  (protected by `pimSilentCanMatchNeeds()`'s own earlier validation),
  nothing in this pipeline pre-validates QualityAgent state at all —
  `pimSilentCanMatchNeeds()` (`:227-346`) never inspects `<QUALITYAGENT>`
  in either `A` or `B`. A plausible, real-world trigger: `A` is an older
  saved/exported install-request XML with quality agent explicitly
  disabled, applied against `B`, a newer product release whose media
  definition has since made quality agent `required="Y"` (a policy
  change) — the user's prior, explicit opt-out is silently discarded on
  reapplication, with no upstream gate to catch it.
  **CONFIRMED DOWNSTREAM CONSEQUENCE, traced for the first time**: `Eb`'s
  final `enable` state is read at real install-execution time by
  `pimEntitlement::OnInstall()` (`:6499-6509`) and `OnReconfigure()`
  (`:7051-7060`) to write (`pimAppendValue(PtcKey, "QualityAgentOptIn", "1")`)
  or remove (`pimRemoveName(PtcKey, "QualityAgentOptIn")`) a real Windows
  registry value. The GUI's own "PHM" (Product Health Monitoring) message
  key and "legal text" control naming
  (`pimUICustomDlgEnablePHM`/`CustomizeScreenQualityAgentLegalText`)
  confirm this is a genuine, user-facing telemetry/usage-reporting opt-in
  setting with consent implications — not merely a latent
  XML-consistency concern.
  **Verified non-issue**: the enable direction carries no equivalent
  restriction — a request to *turn on* quality agent, or a `B` with no
  `<QUALITYAGENT>` node at all (nothing to set, `SetQualityAgent()`
  harmlessly returns `false`), is never a live corruption risk; only a
  *disable* request against a *required* `B` silently fails.
- **CONFIRMED BUG, found in a further dedicated pass on
  `pimSilentCreateEntitlement()`'s package-selection loop specifically:
  a "don't install" request can be silently overridden by `B`'s own
  required or already-installed flag — the identical shape as the
  quality-agent bug above, applied to packages.**
  (`pim_core/pim_core_src/pimSilent.cxx:571-586`, depends on
  `pim_core/pim_core_src/pimPackageMgr.cxx:250-336`,
  `pim_ui/pim_ui_src/pimCustomDlg.cxx:1819-1904`.)
  ```cpp
  A_pkg.GetInstallPackageNames(A_wants);
  B_pkg.GetAllPackageNames(B_avail);

  for (int i = 0; i < (int)B_avail.GetSize(); i++)
  {
      if (A_wants.Find(B_avail[i]) != -1)
          B_pkg.SetPackageInstallState(B_avail[i], true);
      else
          B_pkg.SetPackageInstallState(B_avail[i], false);   // return value discarded
  }
  B_pkg.Refresh();
  ```
  ```cpp
  // pimPackageMgr.cxx:304-332 -- SetPackageInstallState()'s disable branch
  if (install)
      ...set install="Y"...
  else
  {
      if (attribRequired && attribRequired->getNodeValue() == "Y")
      {
          xml->PostWrite();
          return false;   // asked to make FALSE; but it is required -- SILENTLY REFUSED
      }
      if (attribInstalled && strcmp("prime_converter", name) &&
          attribInstalled->getNodeValue() == "Y")
      {
          xml->PostWrite();
          return false;   // already installed (except prime_converter) -- ALSO REFUSED
      }
      ...set install="N"...
  }
  ```
  Traced `SetPackageInstallState(name, false)`'s own logic: it refuses and
  leaves the package's `install` attribute **unchanged** — returning
  `false` — whenever that package is marked `required="Y"`, **or** already
  marked `installed="Y"` (with a single named exception, `"prime_converter"`,
  explicitly carved out of the 2nd check). `pimSilentCreateEntitlement()`'s
  loop discards this return value at both call sites (`:582,584`), so a
  required or already-installed `B` package silently keeps whatever
  install state it already had, contrary to `A`'s explicit non-request,
  with **zero diagnostic anywhere**.
  **CONFIRMED, by direct contrast with this codebase's own GUI path,
  mirroring the quality-agent precedent exactly**:
  `pim_ui/pim_ui_src/pimCustomDlg.cxx`'s `uiCustomTree::RefreshPkg()`
  (`:1819-1904`) reads the identical `req`/`installed` flags via
  `GetPackageInfoByName()` and, when either is true
  (`if (req || installed)`, `:1900`), proactively **disables the
  checkbox** (`ui_do_operation(..., UI_check_sensitive_Attr, false, ...)`,
  `:1903`) — structurally preventing a live user from ever attempting the
  disable this function blindly attempts. No equivalent precondition
  check exists in the silent-install path.
  **New nuance, found in the same trace**: even on a refused disable, the
  function's own special-case handling for 2 hardcoded package names —
  `"prime_converter"`/`"creo_simulate"` (`:276-302`) — still runs
  **before** the required/installed check and toggles their respective
  global mode flags (`pimSetPrimeInWSMode()`/`pimSetSimulateInMode()`) to
  the *attempted* (refused) value regardless. Confirmed inconsistency: for
  these 2 packages only, the persisted XML `install` attribute stays
  unchanged (refused), while the corresponding global flag reflects the
  caller's requested-but-refused value — a genuine, if narrow, state
  divergence.
  **Reachability, precisely characterized**: unlike quality agent (where
  nothing in the pipeline pre-validates it at all), package selection *is*
  validated earlier by `pimSilentCanMatchNeeds()` — but that check only
  confirms every package `A` *wants* exists in `B`; it never inspects
  whether a package `A` *doesn't* want is `required`/`installed` in `B`,
  so this bug's trigger condition is completely outside that check's
  scope. A plausible, real-world trigger: `B`'s media has since made a
  package `required="Y"` (or the machine already has it `installed="Y"`
  from a prior run) that `A`'s older request XML doesn't select — the
  user's implicit "I don't want this" is silently discarded.
  **Downstream consequence**: whatever this loop settles as each
  package's final `install` attribute feeds directly into
  `pimPackageMgr::Refresh()`/`RefreshAFeatureNode()` (already documented
  above), which governs real `<CDSECTION>`/`<MSI>` feature
  install-eligibility — so a silently-retained required/installed package
  continues to have its real features installed, contrary to `A`'s
  explicit non-request, exactly mirroring the quality-agent bug's own
  real-world registry-value consequence.
  **Verified non-issue**: the enable direction (`install=true`) carries no
  equivalent restriction — `SetPackageInstallState(name, true)` always
  succeeds when the node exists, matching quality agent's own asymmetry.
- **CONFIRMED BUG: `pimSilentTestXmlIsUseable()` leaks a `pimXmlFile` on the
  XML-syntax-error path.** (`pim_core/pim_core_src/pimSilent.cxx:35-50`.)
  ```cpp
  pimXmlFile* A_tmp = XNew pimXmlFile();
  if (A_tmp)
  {
      A_tmp->SetXml(AskedToInstallThis);
      A_tmp->UseNS();
      A_tmp->UsePrettyPrint();
      A_tmp->DoRead();
      if (A_tmp->ErrorOccured())
      {
          ...
          pimHitError(PIM_GENERAL_XML_ERROR);
          return false;        // <-- A_tmp never deleted on this path
      }
      delete A_tmp;            // <-- only reached when there was NO error
  }
  ```
  The `delete A_tmp;` that correctly frees the object on the success path
  is placed **after** the `if (A_tmp->ErrorOccured())` block, so the early
  `return false;` inside that block skips it entirely. Confirmed by direct
  contrast with the function's own success path 2 lines later. Consequence:
  every silent-install attempt against a syntactically-invalid user XML
  file (`-XML`/`-XMLALL` pointing at malformed XML) leaks one `pimXmlFile`
  object. Low practical severity (a silent install is a short-lived,
  single-invocation process, and a syntax error here is itself terminal —
  `pimSilentInstallFromXML()` aborts the whole run once this returns
  `false`, see `docs/classes/pimGetApplicationsList.md`), but a clear,
  reproducible, one-line copy-paste-shaped bug.
- **CONFIRMED BUG, more severe: `pimSilentCanMatchNeeds()` leaks 2
  `pimXmlFile` objects whenever either input file fails to parse.**
  (`pim_core/pim_core_src/pimSilent.cxx:227-346`.)
  ```cpp
  pimXmlFile* A_tmp = XNew pimXmlFile();
  pimXmlFile* B_tmp = XNew pimXmlFile();
  A_tmp->SetXml(A); ... A_tmp->DoRead();
  B_tmp->SetXml(B); ... B_tmp->DoRead();
  if (!A_tmp->ErrorOccured() && !B_tmp->ErrorOccured())
  {
      ... // every internal `return false;` here IS preceded by
          // `delete A_tmp; delete B_tmp;` (3 confirmed instances: :271-273,
          // :298-300, :335-337), and the final `return true;` (:342) is too
      return true;
  }
  return false;   // <-- reached whenever EITHER file has a parse error;
  }                //     neither A_tmp nor B_tmp is ever deleted here
  ```
  Every exit point **inside** the `if (!...ErrorOccured() && !...ErrorOccured())`
  block correctly deletes both objects before returning — a consistent,
  deliberate pattern — which makes the outer fallthrough `return false;`
  (reached only when the `if` condition itself is false, i.e. at least one
  of `A`/`B` failed to parse) stand out clearly as the one path the pattern
  was not applied to: a **2-object leak** per call, twice the size of
  `pimSilentTestXmlIsUseable()`'s own leak above and in the same function
  family. Given `pimSilentCanMatchNeeds()` is only reached after
  `pimIsProductXmlMatch()`/`pimIsVersionMatch()` have already confirmed `A`
  and `B` relate (see Called By in `pimSilentTestXmlIsUseable()`'s own
  body, `:87`), a parse failure at this specific point is a narrower window
  than the syntax-error check above, but the leak itself is unconditional
  whenever it does occur.
- **CONFIRMED (imprecise header comment, but — see the correction below —
  NOT a validation gap): `pimSilentCanMatchNeeds()`'s package-availability
  check silently changes meaning under 3 unrelated global command-line
  modes.** (`pim_core/pim_core_src/pimSilent.cxx:307-339`, depends on
  `pim_core/pim_core_src/pimPackageMgr.cxx:611-666`.)
  ```cpp
  // pimSilent.cxx:307-308 -- what pimSilentCanMatchNeeds() treats as "what A wants":
  A_pkg.GetInstallPackageNames(A_wants);
  B_pkg.GetAllPackageNames(B_avail);
  ```
  ```cpp
  // pimPackageMgr.cxx:611-666 -- GetInstallPackageNames()'s ACTUAL logic:
  if (pimGetBasePackMode())
  {
      if (attribRequired->getNodeValue() == "Y")
          out += name;                    // ONLY "required" packages
  }
  else if (pimGetCreoNGCRIMode())
  {
      if (attribRequired->getNodeValue() == "Y")
          out += name;
      else if (attribParent->getNodeValue() == "extraoptions")
          out += name;                    // required OR "extraoptions" packages
  }
  else if (attribInstall->getNodeValue() == "Y" || pimGetAllPacksMode())
  {
      out += name;                        // normal case: install="Y" ...
                                           // ...OR literally every package if -allpacks
  }
  ```
  This function's own header comment (`:220-222`) states its contract
  plainly: *"returns false if any packages marked for install in A are not
  known in B."* That is only true in the **default** case (no special mode
  active), where `GetInstallPackageNames()` does check `install="Y"` as
  documented. By direct contrast, its 2 siblings used in the same function —
  `pimPlatformMgr::GetInstallPlatformNames()` and
  `pimLanguageMgr::GetInstallLanguageNames()` — perform exactly that simple
  `install="Y"` check unconditionally, with no dependency on any global mode
  flag. Confirmed consequences, both reachable via ordinary command-line
  flags documented in `docs/classes/pimGetApplicationsList.md`:
  - **`-allpacks`** (`pimGetAllPacksMode()`): `A_wants` becomes **every**
    package name in `A`'s XML, regardless of `A`'s own `install="Y"`
    markings, so `pimSilentCanMatchNeeds()` requires `B` to carry every
    package `A`'s schema declares.
  - **`-basepack`** (`pimGetBasePackMode()`): `A_wants` becomes only
    packages marked `required="Y"` in `A` — `A`'s actual `install="Y"`
    selections are not consulted at all, so `pimSilentCanMatchNeeds()`
    accepts a match as long as `B` has every *required* package, regardless
    of `B`'s coverage of non-required ones.
  - A 3rd, CreoNGCRI-mode-specific variant (`pimGetCreoNGCRIMode()`) applies
    a 3rd, different definition (required packages, plus any package whose
    parent is `"extraoptions"`), reachable under yet another command-line
    mode.
  None of these 3 modes is unique to the `A`/`B` pair being compared —
  they are process-wide flags set once by `pimCommandLineArgs()` (see
  `docs/classes/pimGetApplicationsList.md`), so whichever mode is active
  silently governs every `pimSilentCanMatchNeeds()` call for the entire
  silent-install run.
  **CORRECTED (found in a later, dedicated pass on
  `pimSilentCreateEntitlement()` itself)**: an earlier version of this
  finding characterized `-allpacks` as "overly strict" (rejecting a
  otherwise-valid match over a package the user never asked for) and
  `-basepack` as a "validation gap" (accepting a match despite `B` missing
  packages the user's XML explicitly wants). Both characterizations were
  **too strong**. `pimSilentCreateEntitlement()`'s own package-selection
  loop (`:576-586`) calls the **identical** `GetInstallPackageNames()` on
  the same `A`, under the same mode flags, to decide what to actually mark
  for install on `B` — and `pimSilentCreateEntitlement()` only ever runs
  immediately after a successful `pimSilentCanMatchNeeds()` call on the same
  `A`/`B` pair within the same process, so the 2 calls are guaranteed to see
  identical mode flags and therefore an identical `A_wants`. This means
  `pimSilentCanMatchNeeds()` validates *exactly* the set of packages
  `pimSilentCreateEntitlement()` will subsequently attempt to install —
  under `-basepack`, the non-required packages `pimSilentCanMatchNeeds()`
  doesn't check for are also packages `pimSilentCreateEntitlement()` was
  never going to try installing; under `-allpacks`, requiring `B` to carry
  everything is consistent with that flag's own "install everything" intent.
  **The 2 functions are consistent with each other, not in conflict** — this
  is confirmed behavior consistent with the mode flags' evident design
  intent, not a functional gap. The one part of the original finding that
  still stands: both functions' own header comments describe a plain
  `install="Y"` check, which remains an incomplete description of the
  actual, mode-dependent behavior — worth fixing as a documentation
  accuracy issue, not a behavioral one. See
  `pimSilentCreateEntitlement()`'s own entry above.
  **Reachability, more precisely characterized (found in a further,
  dedicated pass on this bug specifically)**: the 3 mode flags are **not**
  a clean, mutually exclusive 3-way choice. `-allpacks`/`-basepack` **are**
  enforced mutually exclusive by the CLI parser itself
  (`pim/pim_src/pimGeneralInit.cxx:309-320`) — each `else if` branch
  explicitly clears the *other* flag when set (`install_all_packs = true;
  install_base_pack = false;` and vice versa), so only the last of the 2
  named on a given command line wins, silently, with no conflict warning.
  But `-releaselink` (which sets `pimGetCreoNGCRIMode()` via
  `pimSetCreoNGCRIMode(1)`, `pimGeneralInit.cxx:291-293`) has **no such
  interaction with either flag** — nothing clears it when `-allpacks`/
  `-basepack` is set, and nothing clears them when `-releaselink` is set.
  A real command line can freely combine `-releaselink -basepack` (or
  `-releaselink -allpacks`). Since `GetInstallPackageNames()`'s `if`/`else
  if` chain (`pimPackageMgr.cxx:632-655`, reproduced above) checks
  `pimGetBasePackMode()` **first**, `pimGetCreoNGCRIMode()` **second**,
  combining `-releaselink -basepack` makes BasePack's definition silently
  win for package selection — the CreoNGCRI-specific "required, or parent
  `extraoptions`" clause never executes for packages — even though
  `pimGetCreoNGCRIMode()` remains fully, independently active for every
  *other* purpose throughout the rest of the codebase (confirmed via at
  least 10 other call sites across `pim_ui`, e.g.
  `pimEntitlementTree.cxx:1219,1718`, `pimInstallMgrDlg.cxx:613,2159,2163,3183`,
  `pimCustomDlg.cxx:1023,1027`, `pimWelcomeRefresh.cxx:143`,
  `pimEntitlementRefresh.cxx:1199`, none of which defer to `-basepack`/
  `-allpacks`). This is a confirmed, reachable interaction via ordinary
  command-line flags — not a hypothetical 3-way conflict — since nothing in
  either the CLI parser or `GetInstallPackageNames()` prevents or warns
  about combining them.
  **Downstream consequence, newly traced in this pass**: whatever
  `pimSilentCreateEntitlement()`'s mode-dependent package-selection loop
  (`:576-586`) decides ultimately gets written by
  `pimPackageMgr::SetPackageInstallState()`
  (`pim_core/pim_core_src/pimPackageMgr.cxx:250-336`) directly onto the
  `<PACKAGE>` node's own `install="Y"`/`"N"` XML attribute, on `Eb`'s shared
  `xmlPtr` document. This is the **exact same attribute**
  `pimMSILoop::IsEligibleForInstall()`
  (`pim_core/pim_core_src/pimMSILoop.cxx:937-969`) reads at real
  install-execution time — via `pimEntitlement::InstallMSI()`
  (`pimEntitlement.cxx:5293-5330`) calling `pimMSICopier::MSIInstall()`/
  `MSICopy_low()` (`pim_core/pim_core_src/pimMSICopier.cxx:41-98`), which
  constructs a `pimMSILoop` (already a fully documented `pimLoop` subclass
  in this set, like `pimScriptLoop`/`pimShortcutLoop`) directly on this
  **same** `xmlPtr` — to decide whether a given MSI feature/CDSECTION tied
  to that package is **actually, physically installed** via the MSI copier.
  This confirms, for the first time, that this mode-dependent
  package-selection logic is not merely a validation-time or
  documentation-accuracy concern — it is the literal, unbroken, real-world
  determinant of which product features get installed onto the machine,
  identical for both `pimSilentCanMatchNeeds()`'s validation and
  `pimSilentCreateEntitlement()`'s selection (already confirmed consistent
  with each other above).
- **CONFIRMED BUG, found in a dedicated pass on `pimSilentFixupPSF()`
  itself: its own `pDiUmMMY<N>` collision-workaround placeholder can become
  permanently stuck in `Eb`'s final PSF/command set.**
  (`pim_core/pim_core_src/pimSilent.cxx:353-416`, depends on
  `pim_core/pim_core_src/pimCommandMgr.cxx:405-451,454-515`.)
  ```cpp
  // pimSilent.cxx:373-394 -- when A wants a name that already exists in B
  // under a DIFFERENT license type:
  B_cmds.GetAllCommandNamesByType(lictype2, b_cmd_names);   // ALL of B's old type
  dummy_str = "pDiUmMMY" + btkString::PrintInt(dummy_ct++);
  B_cmds.AppendAddCommand(dummy_str, desc, lictype2, licIdentifiers, features);
  for (int j = 0; j < (int)b_cmd_names.GetSize(); j++)
      B_cmds.DeleteCommand(b_cmd_names[j]);   // now succeeds: dummy keeps count >= 2

  // pimSilent.cxx:405-414 -- final cleanup, deletes anything left that
  // isn't one of A's wanted names (this SHOULD include "pDiUmMMY<N>"):
  for (int i = 0; i < (int)B_avail.GetSize(); i++)
      if (A_wants.Find(B_avail[i]) == -1)
          B_cmds.DeleteCommand(B_avail[i]);   // <-- fails silently if the
                                               //     dummy is now the LAST
                                               //     command of lictype2
  ```
  `pimCommandMgr::DeleteCommand()`/`CanDeleteCommand()`
  (`pim_core/pim_core_src/pimCommandMgr.cxx:405-451,454-515`) explicitly
  refuse to delete a command (`CanDeleteCommand()` returns `false`, an
  inline comment at `:405` reads "you can not delete the last command of a
  type") whenever fewer than 2 commands in the whole XML share its
  `<LICTYPE>`. The dummy is created specifically as a 2nd command of
  `lictype2` so the real colliding commands can be deleted — but by the time
  the *final* cleanup loop runs, if none of `A`'s newly-added commands
  happen to also be of type `lictype2`, the dummy is now the **sole**
  remaining command of that type, and `DeleteCommand("pDiUmMMY<N>")` fails
  the exact same "last of a type" check it was created to survive. Neither
  of `pimSilentFixupPSF()`'s 2 relevant `DeleteCommand()` call sites
  (`:391`, `:413`) checks the returned `bool`, so this failure is silent.
  Confirmed consequence: a spurious `pDiUmMMY<N>` PSF command — carrying
  `A`'s description/license-identifiers/features data mislabeled under `B`'s
  old license type — can permanently persist in the merged entitlement's
  PSF/command set.
  **Reachability, precisely characterized (found in a further, dedicated
  pass on this bug specifically)**: the dummy survives exactly when **no**
  entry in `A`'s final wanted command set independently declares its own
  license type as `lictype2` (the colliding name's *old* type in `B`) — the
  one `A_wants[i]` entry that triggered this branch is guaranteed *not* to
  qualify, since `lictype != lictype2` is the very condition that triggers
  the dummy-creation path, so only some *other*, unrelated `A_wants` entry
  happening to share `lictype2` could save the dummy from being stuck. This
  is a plausible, non-degenerate real-world scenario — e.g. a product
  changing a single command's license type between versions (a common kind
  of licensing-model change) while no other command shares the old type —
  not merely a contrived edge case.
  **Downstream consequence, newly traced in this pass**: a stuck dummy is
  not merely a cosmetic leftover PSF entry. `pimEntitlement::InstallScripts()`
  (`pim_core/pim_core_src/pimEntitlement.cxx:5627-5631`) constructs a
  `pimScriptLoop` directly on this **same** `pimXmlFile*` document (`Eb`'s
  `xmlPtr`) at real install-execution time — confirmed object identity, not
  merely a structurally similar one. `pimScriptLoop::OnInstall()`
  (`pim_core/pim_core_src/pimScriptLoop.cxx:71-127`) calls
  `CommandMgr.GetAllCommandNames(arr)` and, for any entitlement lacking a
  `SimulateLicenseTypes` XML property, reads `arr[0]`'s license identifiers
  via `GetCommandInfoByName()` **unconditionally, with no type-based
  selection** (`:111`), to populate the `[LM_LICENSE_FILE]` template
  substitution used when generating that entitlement's install scripts. If
  the stuck dummy ends up at index `0` of `arr` (document order after
  `pimSilentFixupPSF()` completes — not confirmed to reliably happen in
  general, since it depends on how many of `Eb`'s *original* commands
  survived the initial "drop" step undisturbed and thus retain earlier
  document positions than any newly `AppendAddCommand()`-ed node), its
  already-confirmed internally-mismatched license-identifier data (tagged
  type `lictype2`, populated with `A`'s identifiers for the different type
  `lictype`) would drive `[LM_LICENSE_FILE]` for the **entire entitlement's**
  generated install scripts — not just for the dummy's own inert entry.
  `GetAllCommandNames()`'s default `include_feature_less=false` filters out
  any `<PSF>` node lacking a `<FEATURE_NAME>` child, so whether the dummy
  even appears in `arr` at all further depends on whether its cloned
  `<PSF_TEMPLATE>` (for type `lictype2`) includes a `<FEATURE_NAME>` child —
  not independently confirmed in this pass (that template's own structure
  wasn't traced). Overall: the mechanism connecting `pimSilentFixupPSF()`'s
  stuck dummy to `pimScriptLoop`'s `[LM_LICENSE_FILE]` assembly is fully
  confirmed from both functions' code and the shared-object identity between
  them; whether the dummy specifically reaches index `0` in any real product
  XML is not confirmed either way in this archive.
  **FIX VERIFIED (PARTIALLY), found in a further, dedicated pass verifying
  the proposed fix — "either `pimSilentFixupPSF()` must guarantee no dummy
  survives, or `pimScriptLoop` must select the license command by
  type/role rather than array position 0"**.
  **Option 1, "guarantee no dummy survives", CONFIRMED ALREADY TRUE IN
  HALF THE CASES, STRUCTURALLY UNACHIEVABLE IN THE OTHER HALF without a
  new capability**: re-confirms the "Reachability" characterization above
  is precisely the dividing line — when some *other* `A_wants` entry
  independently shares `lictype2`, that entry's own `AppendAddCommand()`
  call (`:397-404`) already restores `lictype2`'s live count to ≥2 before
  the final cleanup loop runs, so `DeleteCommand("pDiUmMMY<N>")`
  **already succeeds today**, no fix needed. When no such entry exists,
  `pimCommandMgr::CanDeleteCommand()`'s floor (`count > 1`,
  `pimCommandMgr.cxx:454-515`) has **no override or bypass anywhere in
  this class** — read in full, confirming it is structurally
  **impossible** to reduce any license type to 0 live commands through
  the existing delete-based API, regardless of operation ordering within
  `pimSilentFixupPSF()`. A complete fix in this remaining case requires a
  genuinely **new** `pimCommandMgr` capability: an in-place "retype"
  operation that mutates the *existing* colliding command's own
  `<LICTYPE>` (and its other type-specific children) directly, reusing
  `AppendAddCommand()`'s own template-cloning machinery
  (`FindPsfTempateNode()` + deep-cloning a `<PSF_TEMPLATE>`'s children,
  `pimCommandMgr.cxx:310-378`) against the node already present instead of
  a newly-created one — never passing through a 0-count instant, so
  `CanDeleteCommand()`'s floor is never even consulted. Confirmed this is
  architecturally sound (all the necessary building blocks already exist
  and are exercised by `AppendAddCommand()`'s own node-creation branch),
  but is a new method, not a change to `pimSilentFixupPSF()`'s own logic.
  **Option 2, "select by type/role, not array position 0", CONFIRMED
  ARCHITECTURALLY PRECEDENTED, BUT CONCRETELY UNDER-SPECIFIED**:
  `pimScriptLoop::OnInstall()` itself, a few lines below the `arr[0]`
  lookup, already performs exactly this kind of role-based selection for
  a *different* purpose —
  `xmlPtr->FindNodelistAttribMatch(pimPSF, pimid, "parametric")`
  (`pim_core/pim_core_src/pimScriptLoop.cxx:135`) — confirming the
  mechanism this option proposes is not hypothetical; it's a live pattern
  in the very same function. But no canonical `id` (or `<LICTYPE>`) value
  representing "the license-bearing command" for `[LM_LICENSE_FILE]`
  purposes is confirmed to exist anywhere in this archive — no sample
  product/media XML or schema documentation defines one — so option 2 is
  feasible in mechanism but not yet implementable as literally stated.
  **Further confirmed, extending the already-flagged
  `include_feature_less` uncertainty above**: traced `AppendEditCommand()`
  (`pim_core/pim_core_src/pimCommandMgr.cxx:270-308`, which
  `AppendAddCommand()` delegates to for `Description`/`Features`) and
  confirmed it only ever **updates** an existing `<FEATURE_NAME>` child's
  text — it has no code path to **create** one if the cloned
  `<PSF_TEMPLATE>` lacked it. So the dummy's presence in `arr` at all is
  fixed entirely by `lictype2`'s own template authoring, never by
  anything `pimSilentFixupPSF()` or `pimScriptLoop` itself does — an
  independent, unconfirmed gate on this whole consequence chain, on top
  of the already-unconfirmed document-order question.
  **New, related design smell, independent of the dummy bug entirely**:
  `arr[0]`'s "just take the first command" assumption is confirmed
  fragile on its own terms — this exact function already relies on
  multiple, semantically distinct PSF commands coexisting in the same
  document (the `id="parametric"` lookup presupposes at least one other
  role-distinct command can exist), so even in a perfectly healthy,
  dummy-free document, nothing in the schema or code guarantees `arr[0]`
  is the intended "primary" license-bearing command — document order,
  not role, is all that currently decides it.
- **CONFIRMED DISCREPANCY between this function's own comment and its
  code**: the same collision-handling block's comment says "we need to
  create a dummy for type B; and remove **the foobar command**" (singular —
  implying only the one colliding name is removed), but the code calls
  `GetAllCommandNamesByType(lictype2, b_cmd_names)` and deletes **every**
  command of `lictype2` in `b_cmd_names`'s loop, not just the single
  colliding name. Whenever `B`'s old type `lictype2` had other,
  non-colliding commands beyond the one `A` wants renamed, this function
  removes all of them too, relying entirely on the immediately-following
  loop (`:397-404`) to re-add anything `A` still wants of that type —
  anything `A` does *not* want of that type is permanently dropped from
  `Eb`'s PSF set as a side effect of resolving one unrelated name collision.
  **Reachability, precisely characterized (found in a further, dedicated
  pass on this discrepancy specifically)**: this function's own opening
  "Drop" step (`:361-365`) already deletes **every** original command in
  `B` before this collision-handling block ever runs — but
  `pimCommandMgr::CanDeleteCommand()`'s live "last of a type" count
  (confirmed via `pimCommandMgr.cxx:454-515`, which counts `<PSF>` nodes by
  `<LICTYPE>` **at call time**, not a fixed snapshot) makes that Drop loop
  **deterministically leave exactly 1 surviving command per originally
  distinct license type** — whichever one is processed last, in
  `B_avail`'s fixed snapshot order, for that type (every earlier same-type
  deletion in the loop succeeds since ≥2 members remain each time; the
  final one always fails, since deleting it would leave 0). This means
  that on a given `lictype2`'s **first** collision in a call,
  `GetAllCommandNamesByType(lictype2, b_cmd_names)` can only ever return
  **that one straggler** — identical to the single colliding command the
  comment already expects, not "other, non-colliding" original `B`
  commands (those are already gone by this point). The discrepancy only
  actually manifests when **2 or more** `A_wants[i]` entries, in the
  *same* call, independently collide against different `B` command names
  that originally shared the *same* `lictype2` — a real, reachable
  scenario (e.g. a product consolidating several old commands of one
  license type into fewer, differently-typed commands across a version
  upgrade), but narrower than "any non-colliding command of that type"
  might suggest in isolation.
  **Downstream consequence, newly traced in this pass — an internal
  interaction with the already-documented stuck-dummy bug above, not an
  external consumer**: because `AppendAddCommand()` (`:389`) runs *before*
  the `b_cmd_names` delete loop (`:390-391`), and `b_cmd_names` was
  captured *before* that new dummy existed, a 2nd (or later) collision on
  the same `lictype2` deletes the *previous* iteration's straggler/dummy
  while its own brand-new dummy survives untouched — the type's member
  count never drops below 1 at any deletion, so every one of these
  in-between deletions **succeeds**. The practical effect: when `N`
  collisions in one call target the same original `lictype2`, this
  "delete entire type" behavior acts as an **unintentional cleanup
  mechanism** for dummies `1` through `N-1` — they are swept away here,
  mid-loop, rather than surviving to the final cleanup loop (`:410-413`)
  or the stuck-dummy risk described above. Only the **last** dummy created
  for a given shared `lictype2` is actually exposed to the
  already-documented stuck-dummy risk and its traced `pimScriptLoop`
  `[LM_LICENSE_FILE]` consequence — narrowing, not retracting, that
  earlier finding: the risk is real, but confirmed to apply to at most 1
  dummy per originally-distinct license type per call, not every dummy
  this function creates.
- **CONFIRMED, found in a further dedicated pass specifically on this
  function's `DeleteCommand()` return-value check — not a new function, a
  precise, per-call-site trace of what checking it would actually reveal,
  and a genuinely new generalization of the already-documented stuck-dummy
  finding.** (`pim_core/pim_core_src/pimSilent.cxx:364,391,413`, depends on
  `pim_core/pim_core_src/pimCommandMgr.cxx:270-451,454-515`.)
  There are exactly 3 `DeleteCommand()` call sites in this function, and
  checking the discarded return value would mean 3 different things at
  each:
  ```cpp
  // Site 1, the "Drop" step (:361-365)
  for (int i = 0; i < (int)B_avail.GetSize(); i++)
      B_cmds.DeleteCommand(B_avail[i]);   // comment already documents: "if more
                                            // than one command exists ... this would succeed"
  // Site 2, collision-handling (:390-391)
  for (int j = 0; j < (int)b_cmd_names.GetSize(); j++)
      B_cmds.DeleteCommand(b_cmd_names[j]);   // preceded by AppendAddCommand() at :389
  // Site 3, final cleanup (:410-413)
  for (int i = 0; i < (int)B_avail.GetSize(); i++)
      if (A_wants.Find(B_avail[i]) == -1)
          B_cmds.DeleteCommand(B_avail[i]);
  ```
  **Site 1 (`:364`) — verified non-issue**: a `false` return here is the
  Drop step's own **expected, by-design terminal state** — this codebase's
  own inline comment already documents it, and the already-confirmed
  "leaves exactly 1 surviving command per originally-distinct license
  type" behavior (see the collision-deletes-entire-type finding above)
  depends entirely on this loop hitting that `false` return for every
  type's last representative. Checking or logging it here would produce
  noise on every multi-command type in `B`, not a signal — there is
  nothing to fix at this call site.
  **Site 2 (`:391`) — confirmed, by an exhaustive counting proof, to
  *always* succeed, conditionally**: `AppendAddCommand(dummy_str, ...)`
  (`:389`) runs *before* this delete loop, adding a `dummy_str` command of
  type `lictype2` first. Since `b_cmd_names` (captured at `:386`, *before*
  the dummy existed) contains only pre-existing `lictype2` commands, the
  count of `lictype2` members remaining just before deleting the *k*-th of
  *n* names in `b_cmd_names` is always `(n - k) + 1` (the not-yet-deleted
  originals, plus the dummy) — which is `> 1` for every `k` from `1` to
  `n`, including the last. So **every** delete in this loop is guaranteed
  to pass `CanDeleteCommand()`'s "last of a type" check, *provided*
  `AppendAddCommand()` actually created the dummy. That precondition is
  itself never checked: `AppendAddCommand()`'s own return value is also
  discarded at `:389`, and tracing it (`pimCommandMgr.cxx:310-403`) shows
  it can fail to create a new node if `FindPsfTempateNode(lictype2)` finds
  no matching `<PSF_TEMPLATE>` for that license type — not confirmed
  reachable or unreachable in this archive (no product/media XML sample
  exists to check whether every `<PSF>` command's license type always has
  a corresponding template). If that precondition were ever violated, this
  loop's deletes would fall back to the ordinary "last of a type" risk,
  **and** the subsequent re-add loop (`:397-404`) would find the original,
  wrong-type node still present under the same name (`FindPsfNode()`
  matches by `name`, not `<LICTYPE>`) and merely update its
  license-identifiers/description/features in place via
  `AppendEditCommand()` — which never touches the `<LICTYPE>` child
  (confirmed via `pimCommandMgr.cxx:270-308`) — leaving the **wrong**
  license type permanently attached to that command, silently defeating
  the entire collision-fixup mechanism. This cascading failure is not
  confirmed reachable in this archive, but the code path enabling it is
  fully confirmed from source.
  **Site 3 (`:413`) — confirmed, generalizing the already-documented
  stuck-dummy finding beyond dummies specifically**: this is where
  checking the return value would matter most. The stuck-dummy bug above
  is one instance of a broader mechanism: **any** command surviving to
  this final loop — dummy or genuinely original — that is the *sole*
  remaining representative of its license type gets silently,
  permanently stuck whenever `A_wants` contains **no** command of that
  type at all, for exactly the same "last of a type" reason. Concretely: a
  license type that survives the Drop step as a single, non-colliding
  straggler (never touched by the collision-handling block, since that
  block only ever processes names actually present in `A_wants`), and
  that `A` simply does not want *any* command of, cannot be removed here —
  `CanDeleteCommand()` counts exactly 1 member of that type and refuses.
  This means `pimSilentFixupPSF()`'s own stated goal — its file header
  comment, "we only need to eliminate the names from B that are not part
  of A_wants" — is confirmed **structurally unachievable** for an entire
  license type whenever `A` requests no command of it: some leftover
  command of that unwanted type will *always* survive in `Eb`, not merely
  in a contrived edge case. **Downstream consequence, distinct in
  character from the dummy case**: this stuck leftover carries **real,
  valid** license data (unlike the dummy's internally-mismatched data), so
  it is not a data-corruption risk — but if it lands at index `0` of
  `pimScriptLoop::OnInstall()`'s unconditional `GetAllCommandNames()`
  read (see the stuck-dummy finding's own downstream trace above, same
  mechanism, same document-order caveat), `[LM_LICENSE_FILE]` would be
  populated from a license type the user's `A` never selected at all —
  a real, valid, but *unrequested* license silently determining the
  entitlement's generated install scripts, a distinct correctness concern
  from the dummy's outright data-mismatch case.
- **CONFIRMED BUG, found in a further dedicated pass specifically on this
  function's `AppendAddCommand()` return-value check — not a new function,
  a precise, per-call-site trace of what checking it would actually
  reveal, mirroring the `DeleteCommand()` return-value pass above but
  finding a more severe, more clearly reachable failure mode.**
  (`pim_core/pim_core_src/pimSilent.cxx:389,403`, depends on
  `pim_core/pim_core_src/pimCommandMgr.cxx:270-403,923-929`.) There are
  exactly 2 `AppendAddCommand()` call sites in this function, and both
  discard the return value:
  ```cpp
  // Site 1, dummy creation, inside collision-handling (:389)
  B_cmds.AppendAddCommand(dummy_str, desc, lictype2, licIdentifiers, features);

  // Site 2, the main re-add loop (:397-404)
  // "now we know that all the names we want to use are either not
  //  already used or match the same type..."
  A_cmds.GetCommandInfoByName(A_wants[i], lictype, unused, desc, licIdentifiers, features, unused, unused);
  B_cmds.AppendAddCommand(A_wants[i], desc, lictype, licIdentifiers, features);
  ```
  Traced `AppendAddCommand()`'s own implementation
  (`pimCommandMgr.cxx:310-403`): it is **add-or-update** — if `FindPsfNode(name)`
  already finds a node with that `name`, creation is skipped entirely and
  the existing node is simply updated (this branch **always** succeeds,
  confirmed via its final `return AppendEditCommand(name, ...)`, which
  does its own `FindPsfNode(name)` lookup that is guaranteed to re-find the
  same node). But when `name` does **not** already exist, a **new** node
  must be created, and that creation depends on
  `FindPsfTempateNode(LicType)` (`:923-929`) finding a matching
  `<PSF_TEMPLATE>` for the target license type — if none exists, `Node`
  stays `NULL`, no node is ever created, and the trailing
  `AppendEditCommand()` call's own `FindPsfNode(name)` lookup also fails,
  so the **entire function returns `false` with zero trace of the attempt
  anywhere in `Eb`'s document** — no dummy, no leftover, nothing.
  **Site 1 (`:389`), the dummy — confirmed lower reachability**: `lictype2`
  here is not an arbitrary value — it is read moments earlier
  (`GetCommandInfoByName(A_wants[i], lictype2, ...)`, `:379`) directly from
  an **existing, live** `<PSF>` command already present in `Eb`'s own
  document. Since something had to create that original command, a
  corresponding `<PSF_TEMPLATE>` for `lictype2` plausibly already exists in
  `B`'s own template set — not 100% guaranteed absent a schema or sample
  XML in this archive, but structurally far less likely to be missing than
  a type sourced from an entirely different document (see Site 2).
  **Site 2 (`:403`), the main re-add loop — confirmed, more severe and more
  clearly reachable**: this is the loop that actually fulfills `A`'s
  request for every wanted command name. When `A_wants[i]` is a name that
  does **not** already exist anywhere in `Eb`'s document (i.e. the earlier
  `if (B_cmds.GetCommandInfoByName(A_wants[i], ...))` check at `:379`
  returned `false` — `A` wants a command `B`'s original template never had
  under this name at all), a **new** node must be created, and its
  `lictype` comes from `A_cmds.GetCommandInfoByName()` — a value read from
  `A`'s document, entirely independent of what `<PSF_TEMPLATE>` entries `B`
  actually ships. If `B`'s media doesn't happen to define a template for
  that license type at all (a real, plausible cross-version/cross-product
  mismatch — e.g. `A` is an older or customized request XML naming an
  add-on license feature the currently-matched `B` media doesn't include),
  `AppendAddCommand()` silently fails, and `A`'s entire request for that
  named PSF/license command is **dropped without a trace** — not stuck
  like a dummy, not present with wrong data, simply **absent** from `Eb`,
  as if the user never asked for it. **Confirmed by this function's own
  reasoning**: its inline comment introducing this loop ("now we know that
  all the names we want to use are either not already used or match the
  same type...") shows the author explicitly reasoned through the
  name/type-collision problem the earlier block handles, but never
  considered whether a `<PSF_TEMPLATE>` exists at all for a genuinely new
  name's license type — a confirmed blind spot in the function's own
  design, not a deliberately accepted risk.
  **Downstream consequence, traced for the first time**: since the failed
  `AppendAddCommand()` never adds anything to `Eb`, the name in question
  never even appears in the final cleanup loop's `B_avail`
  re-fetch (`:407-408`) — there is nothing to strand, unlike the
  `DeleteCommand()` findings above. The user's requested licensed
  feature/capability is simply never provisioned in the generated
  entitlement at all, with **no dummy, no log, no property, and no error
  anywhere in the entire pipeline** to indicate the omission — a strictly
  silent drop, arguably harder to diagnose than either of the
  `DeleteCommand()` findings, since there is no leftover artifact for a
  later investigation to even notice.
- **CONFIRMED BUG, found in a dedicated pass on `pimSilentFixupShortcuts()`
  itself: a stale Program Menu group name from one shortcut can be copied
  onto a different, unrelated shortcut.** (`pim_core/pim_core_src/pimSilent.cxx:447-467`,
  depends on `pim_ui/pim_ui_src/pimShortcutMgr.cxx:169-211`.)
  ```cpp
  btkString unused, str;   // declared ONCE, outside the loop (:428)
  for (int i = 0; i < (int)A_wants.GetSize(); i++)
  {
      if (B_avail.Find(A_wants[i]) != -1)
      {
          ...
          A_shtcuts.GetShortcutProgramMenu(A_wants[i], str);   // return value IGNORED
          B_shtcuts.SetShortcutProgramMenu(A_wants[i], str);   // called UNCONDITIONALLY

          if (A_shtcuts.GetShortcutStartDir(A_wants[i], str))  // <-- correct pattern,
              B_shtcuts.SetShortcutStartDir(A_wants[i], str);  //     3 lines later
      }
      ...
  }
  ```
  `pimShortcutMgr::GetShortcutProgramMenu(id, value)`
  (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:191-211`) leaves `value` **entirely
  untouched** when the shortcut has no `<PROGRAMSMENU>` child — it simply
  `return`s `false` without writing anything. Since `str` is declared once
  outside the loop and reused across every iteration, if `A_wants[i]`'s
  shortcut lacks a `<PROGRAMSMENU>` entry, `str` still holds whatever value
  a **previous, unrelated** iteration's successful `GetShortcutProgramMenu()`
  call left in it — and the immediately following, unconditional
  `SetShortcutProgramMenu(A_wants[i], str)` writes that stale, wrong value
  into the *current* shortcut's Program Menu group on `Eb` regardless.
  (On the very first iteration, `str` is simply empty, so the consequence is
  data loss — an unset/blanked Program Menu group — rather than
  cross-contamination.) Confirmed by direct contrast with the very next
  field, `GetShortcutStartDir()`/`SetShortcutStartDir()` (`:465-466`), which
  correctly gates the `Set` call on the `Get` call's return value — the
  exact pattern the `ProgramMenu` pair 3 lines earlier omits.
  **Reachability, confirmed in a further, dedicated pass on this bug
  specifically**: `<PROGRAMSMENU>` is confirmed to be an **independently
  optional** per-shortcut child element, not a mandatory one — evidenced by
  `pim_core/pim_core_src/pimShortcutLoop.cxx:88-341`, which parses
  `<PROGRAMSMENU>`/`<STARTMENU>`/`<DESKTOP>`/`<QUICKLAUNCH>` as sibling
  children of `<SHORTCUT>` in the same `else if` chain, each independently
  present-or-absent and independently acted on (a shortcut offered only via
  Desktop/Quicklaunch, not a Start Menu program group, legitimately has no
  `<PROGRAMSMENU>` node at all). This makes the bug's trigger condition — a
  shortcut lacking `<PROGRAMSMENU>` immediately following, in loop order, a
  different shortcut that has one — a normal, expected configuration, not a
  contrived edge case.
  **Downstream consequence, newly traced in the same pass — a concrete,
  deterministic result, unlike the ambiguous-document-order caveat on the
  `pimSilentFixupPSF()` finding above**: `pimEntitlement::InstallShortcuts()`
  (`pim_core/pim_core_src/pimEntitlement.cxx:5778-5787`) calls
  `pimShortcuts::GetInstance().Create(xmlPtr)` on the **same** `pimXmlFile*`
  document `pimSilentFixupShortcuts()` mutates — confirmed object identity.
  `pimShortcutLoop::OnInstall()`
  (`pim_core/pim_core_src/pimShortcutLoop.cxx:324-326,654-671`) later reads
  that same shortcut's `<PROGRAMSMENU>` text content and uses it **directly
  as a filesystem subfolder path**
  (`CreateShortcutProgramsmenu()`, `location /= (cStringT)programsmenu;`,
  `:670`) to physically place the installed `.lnk` shortcut file. Confirmed
  consequences, both directly observable at install time:
  - If the stale value is **non-empty** (copied from a different,
    earlier-processed shortcut), the current shortcut's icon is installed
    into **that other shortcut's Start Menu folder** instead of its own —
    a visible, user-facing misplacement.
  - If the stale value is **empty** (the first loop iteration touching this
    code path), `pimShortcutLoop::OnInstall()`'s `if (!programsmenu.IsEmpty())`
    guard (`:324`) skips `CreateShortcutProgramsmenu()` entirely — the
    shortcut's Program-Menu placement is silently dropped, even if this same
    shortcut's independent boolean "create in Programs Menu" flag
    (`SetShortcutProgramsMenuState()`, set correctly via the already-known
    `B_avail[i]` bug above, or correctly via `A_wants[i]` if that bug doesn't
    also fire) was set `true`.
  **The proposed fix, verified in a further, dedicated pass specifically on
  the fix itself — not a new function, a deeper scrutiny of one already-
  recommended remediation**:
  ```cpp
  if (A_shtcuts.GetShortcutProgramMenu(A_wants[i], str))   // gate on the Get's return value,
      B_shtcuts.SetShortcutProgramMenu(A_wants[i], str);   // mirroring GetShortcutStartDir()/SetShortcutStartDir()
  ```
  **CONFIRMED SUFFICIENT for the stale-copy corruption bug**: traced
  `pimShortcutMgr::SetShortcutProgramMenu()`'s own implementation
  (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:169-189`) to confirm precisely
  what *not* calling it leaves behind — it only ever mutates an
  **existing** `<PROGRAMSMENU>` child's text content (`child->setTextContent(...)`
  inside a `while(child)` search loop); when the call is skipped (the fixed
  code's behavior whenever `Get` returns `false`), `Eb`'s shortcut node is
  **not touched at all**, so its own original, media-template-authored
  `<PROGRAMSMENU>` value (if any) survives completely intact — the
  semantically correct outcome, exactly matching what the already-correct
  `GetShortcutStartDir()`/`SetShortcutStartDir()` pattern already does for
  the start-directory field. The fix eliminates every corruption path this
  bug's Risk Analysis entry documents, with no partial or residual
  corruption case remaining.
  **NEW, previously unexamined limitation found while verifying this fix
  (shared by the very pattern the fix mirrors, not introduced or left
  unaddressed by the fix itself)**: `SetShortcutProgramMenu()` — and
  `pimShortcutMgr::SetShortcutState()` (`:426-454`), the shared primitive
  behind `SetShortcutProgramsMenuState()`'s create/remove toggle — can
  **only** mutate a child node that **already exists** on `Eb`'s shortcut;
  neither function ever calls `createElement()`/`appendChild()` to add a
  missing `<PROGRAMSMENU>` node. Confirmed identical in
  `SetShortcutStartDir()` (`:213-233`) itself — the "already correct"
  reference pattern shares this exact same structural ceiling, previously
  unexamined because it was only ever cited as the good contrasting
  example, never scrutinized on its own. Consequence: even with the fix
  applied, if `A`'s request XML wants to newly grant Program-Menu placement
  (or a custom group name) to a shortcut whose `B` media template never
  defined a `<PROGRAMSMENU>` child at all, that customization is **silently,
  completely unrealizable** — both `SetShortcutProgramMenu()` (the name
  text) and `SetShortcutProgramsMenuState()` (the enable/disable toggle)
  return `false` and change nothing, discarded by the caller exactly as
  before. This is not a corruption risk and not something the proposed fix
  was ever meant to solve — `<PROGRAMSMENU>` being independently optional
  per shortcut (see Reachability above) means some shortcuts structurally
  cannot be granted Program-Menu placement via this function, by design,
  regardless of the fix. `pimShortcutLoop::OnInstall()`'s own parsing
  (`pimShortcutLoop.cxx:159-189`) confirms this is a silent, non-crashing
  outcome at install time too: a `<PROGRAMSMENU>` child that was never
  created simply never matches that `else if` branch, leaving its local
  `programsmenu` at its initialized default — the shortcut installs with no
  Program-Menu entry, with no error anywhere in the pipeline. A more
  complete remediation, if this narrower customization gap is ever judged
  worth closing, would have `SetShortcutProgramMenu()`/`SetShortcutState()`
  create the missing child node when absent, rather than only the minimal
  `if`-guard verified here — see Risk Analysis for the full characterization.
- **CONFIRMED SUFFICIENT, found in a further, dedicated pass verifying
  `pimSilentFixupShortcuts()`'s already-known array-index bug's proposed
  fix** (bug originally documented from the caller side in
  `docs/classes/pimShortcutMgr.md`'s Risk Analysis;
  `pim_core/pim_core_src/pimSilent.cxx:447-460`, depends on
  `pim_ui/pim_ui_src/pimShortcutMgr.cxx:417-454,295-320,29-101`).
  ```cpp
  for (int i = 0; i < (int)A_wants.GetSize(); i++)
  {
      if (B_avail.Find(A_wants[i]) != -1)
      {
          A_shtcuts.GetShortcutInfoByID(A_wants[i], unused, unused, sm, pm, dt, ql);

          B_shtcuts.SetShortcutStartMenuState(B_avail[i], sm);      // fix: A_wants[i]
          B_shtcuts.SetShortcutProgramsMenuState(B_avail[i], pm);   // fix: A_wants[i]
          B_shtcuts.SetShortcutDesktopState(B_avail[i], dt);        // fix: A_wants[i]
          B_shtcuts.SetShortcutQuicklaunchState(B_avail[i], ql);    // fix: A_wants[i]
          ...
      }
  }
  ```
  **The fix — replace `B_avail[i]` with `A_wants[i]` in all 4 calls — is
  confirmed sufficient**, with no residual gap: the enclosing
  `if (B_avail.Find(A_wants[i]) != -1)` guard already proves `A_wants[i]`'s
  id string exists in `Eb`'s document, so `FindShortcutNode(A_wants[i])`
  (called inside `SetShortcutState()`, `:417-424,426-454`) is guaranteed to
  locate the same, correct shortcut the 2 immediately-following field
  copies (`GetShortcutProgramMenu`/`SetShortcutStartDir`, already correctly
  using `A_wants[i]`) also target. No staleness risk either: `B_avail` is
  captured once, via a single `GetAllShortcutIDs()` call before either loop
  begins (`:434`), and nothing in this function — before or after the fix —
  mutates `Eb`'s document *structure* (only existing attribute values), so
  the snapshot stays accurate for the loop's full duration.
  **NEW, previously unexamined limitation, generalized from the
  Program-Menu-fix pass above**: that pass traced `SetShortcutState()`
  (`:426-454`) only as the primitive behind `SetShortcutProgramsMenuState()`.
  Read in full for this pass: all 4 of `SetShortcutStartMenuState()`,
  `SetShortcutProgramsMenuState()`, `SetShortcutDesktopState()`, and
  `SetShortcutQuicklaunchState()` delegate identically to this one shared
  primitive, each passing only a different `Match` tag
  (`pimSTARTMENU`/`pimPROGRAMSMENU`/`pimDESKTOP`/`pimQUICKLAUNCH`).
  `SetShortcutState()` requires finding **both** the shortcut node (fixed,
  now guaranteed) **and** a child element matching `Match` on that node
  before it does anything — and since all 4 of these child elements are
  already-confirmed independently optional per shortcut (see the
  Program-Menu-fix finding above), it can legitimately return `false` for a
  **correctly identified** shortcut simply because that one specific
  location option was never offered by `Eb`'s template. None of the 4
  array-index call sites check this return value, before or after the
  fix — so even a fully-corrected caller can silently no-op a location
  toggle `A` explicitly requested, with the same "silent, non-crashing,
  by-design ceiling" character as the Program-Menu bug's own fix, just
  spread across all 4 toggles instead of 1.
  **Verified non-issue, found while tracing this same call chain**:
  `A_shtcuts.GetShortcutInfoByID(A_wants[i], unused, unused, sm, pm, dt, ql)`
  (`:455`) discards its own `bool` return, and `sm`/`pm`/`dt`/`ql` are
  declared once, outside the loop (`:426`) — the same shape as the
  confirmed-live Program-Menu and `<PROPERTY>`-copy staleness bugs
  elsewhere in this file. Traced `GetShortcutInfoByID()`'s implementation
  (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:29-101`) to confirm this instance
  is **not** reachable: it fails only when `FindShortcutNode(id)` (`:35`)
  finds no matching node, but `A_wants[i]` is sourced from
  `A_shtcuts.GetAllShortcutIDs()` (`:295-320`) scanning the **same**
  document by the **same** `id` attribute `FindShortcutNode()` matches on
  — so the node is always found. Even so, this call site's design is more
  defensive than its Program-Menu sibling: on success, all 4 output
  booleans are unconditionally reset to `false` (`:40-43`) *before* the
  child-scan, so a shortcut lacking a given location child yields a
  correct `false` rather than a carried-over stale value — the exact
  safeguard `GetShortcutProgramMenu()` (`:191-211`) omits.
  **Adjacent, pre-existing risk, found in the same trace, unrelated to and
  not introduced by the array-index fix**: `GetAllShortcutIDs()`
  (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:312-313`) reads
  `map->getNamedItem(pimid)` and immediately dereferences
  `attribMatch->getNodeValue()` with no null check — a latent crash if any
  `<SHORTCUT>` node in either document ever lacks an `id` attribute. This
  underlies both `A_wants` and `B_avail` alike (both populated via this
  same function), so it is a shared precondition of this entire function's
  algorithm, not a defect specific to the array-index bug or its fix.
- **CONFIRMED BUG, upgraded from a lower-confidence structural note (found
  in a further, dedicated pass specifically on this caveat): `pimSilentCreateEntitlement()`'s
  `<MSI>`-node property-copy loop collapses multiple, independently-`format`ted
  MSI packages into one.** (`pim_core/pim_core_src/pimSilent.cxx:604-684`.)
  ```cpp
  // pimSilent.cxx:609-631 -- reads from A (Ea): LAST <MSI> node wins, silently
  next_nl = Ea.GetXMLPtr()->getDocument()->getElementsByTagName(pimMSI);
  for (index = 0, max = next_nl->getLength(); index < max; index++)
  {
      ...
      if (attribFormat)
          format = StrX(attribFormat->getNodeValue()).localForm();   // overwritten every iteration
      if (InstallCommand)
          Cmd = StrX(InstallCommand->getTextContent()).localForm();  // overwritten every iteration
  }
  // pimSilent.cxx:640-682 -- writes to B (Eb): the ONE surviving value pair
  // is stamped onto EVERY <MSI> node, not just the one it came from
  next_nl = Eb->GetXMLPtr()->getDocument()->getElementsByTagName(pimMSI);
  for (index = 0, max = next_nl->getLength(); index < max; index++)
  {
      ((DOMElement*)nodeb)->setAttribute(pimformat, x_format.unicodeForm());
      // ... InstallCommand_b's <MSIARGUMENT> text set to Cmd, unconditionally
  }
  ```
  Both `format`/`Cmd` are declared **once**, outside both loops — the read
  loop's every iteration silently overwrites whatever the previous `<MSI>`
  node contributed, so only the **last** node in `A`'s document order
  survives; the write loop then stamps that **one** surviving value pair
  identically onto **every** `<MSI>` node in `B`, erasing any distinctions
  between `B`'s own, independently-configured MSI packages.
  **Reachability, confirmed in a further, dedicated pass on this bug
  specifically (not merely hypothetical, upgrading the prior "no product XML
  available to test" caveat)**: `pimMSILoop::pimMSIExec()`
  (`pim_core/pim_core_src/pimMSILoop.cxx:154-161`, the actual MSI
  install-execution driver, already a fully documented `pimLoop` subclass in
  this set) independently iterates **every** `<MSI>` node in the document via
  the **identical** `getElementsByTagName(pimMSI)` call this function uses,
  skipping only those `IsEligibleForInstall()` rejects (`:192-193`), and
  executes **each eligible node as its own, separate MSI install action**
  (`:186-...`, its own `msipath`/`msi_arguments`/`InstallCommand` per node).
  This confirms multi-`<MSI>`-node product XML — multiple, independently
  install-eligible MSI packages per entitlement — is a real, designed-for,
  normally-executed configuration in this codebase, not a hypothetical edge
  case. `pimMSILoop::pimMSIExec()`'s own `<MSIARGUMENT>`-reading (`:249-260`)
  even correctly concatenates **all** `<MSIARGUMENT>` children of a single
  `<MSI>` node, by contrast with `pimSilentCreateEntitlement()`'s cruder
  single-value read via `GetChildNodeByNodeName()` (which returns only the
  first match) — further evidence the rest of this codebase treats
  multi-value MSI configuration as normal, not exceptional.
  **Downstream consequence, newly traced in this pass**: the single
  `format`/`Cmd` pair `pimSilentCreateEntitlement()` stamps onto every
  `<MSI>` node in `B` is exactly what `pimMSILoop::pimMSIExec()` reads
  **per node** at real install-execution time. `attribFormat`
  (`pim_core/pim_core_src/pimMSILoop.cxx:596-614`) directly selects that
  node's install **UI mode** — `"full"` (a user-facing installer wizard the
  user must click through or cancel), `"basic"`, or silent — and the
  `<MSIARGUMENT>` text feeds directly into that node's `msiexec.exe`
  command line. If `B` legitimately has 2+ MSI packages meant to install
  under **different** UI modes or with different install arguments (e.g.
  one silent background package alongside one that legitimately requires
  the full interactive wizard), this bug forces **all** of them to adopt
  whichever single mode/argument pair happened to belong to the **last**
  `<MSI>` node read from `A`'s document — a confirmed, install-time
  consequential data-corruption bug that can visibly change which
  installer UI a user sees (or is silently denied) during a multi-package
  MSI install, not merely a latent XML-consistency concern.
  **The proposed fix, verified in a further, dedicated pass specifically on
  the fix itself — not a new function, a deeper scrutiny of one already-
  recommended remediation ("match by node identity, e.g. the `pimname`/
  `pimPRODUCTCODE` attribute, instead of collapsing all nodes' values into
  one pair of local variables")**: **CONFIRMED both candidate attributes
  genuinely exist on real `<MSI>` nodes in this codebase** — traced
  `pimMSILoop::pimMSIExec()`'s own attribute reads
  (`pim_core/pim_core_src/pimMSILoop.cxx:202-207`, already the confirmed
  real install-execution consumer of this same node type): it reads both
  `map->getNamedItem(pimname)` and `map->getNamedItem(pimPRODUCTCODE)` off
  the identical `<MSI>` nodelist this function iterates, confirming the
  fix's suggested keys are not speculative. **But the fix's own "name or
  ProductCode" phrasing is under-specified — verified that the 2 keys have
  opposite, not interchangeable, tradeoffs for the cross-version matching
  this function exists to perform**: `PRODUCTCODE` is a Windows Installer
  GUID that, per standard MSI authoring convention (not itself confirmed
  from this archive, since no MSI-authoring documentation exists in it,
  but a well-established external convention), is expected to change on
  most new MSI builds — meaning a `PRODUCTCODE`-only match would likely
  find **no** correspondence at all between `A`'s (older/customized)
  request XML and `B`'s (newly matched) media in the realistic case this
  function is designed to handle, silently degrading the "fix" to "no
  customization ever survives" — a quieter but real regression risk,
  trading the confirmed corruption bug for a confirmed loss of the
  feature's entire purpose. `name`, by contrast, is a stable,
  human-authored label far more likely to persist across a product's
  version history, making it the practically workable primary key — but
  is not enforced unique by any schema in this archive, so 2 `<MSI>`
  nodes with the same `name` (not confirmed to occur, but not ruled out
  either) could still collide under name-only matching. A robust
  implementation would need to try `PRODUCTCODE` first (exact package
  identity, when it happens to match) and fall back to `name` (broader,
  version-tolerant reach) — neither the original bug nor the proposed
  fix's own phrasing specifies this fallback design.
  **New, previously unexamined asymmetry found while verifying the fix,
  orthogonal to the name-collapsing bug**: even before per-node matching,
  the write loop's own `format`-attribute handling (`:654-661`) only
  overwrites `B`'s `format` attribute **if `B`'s own node already has
  one** (`if (attribFormat_F) {...}`, no `else` branch to create it),
  while the `<MSIARGUMENT>` child-element handling immediately below
  (`:663-673`) **does** create a missing child. Any correctly-scoped
  per-node fix must consciously preserve (or deliberately correct) this
  existing create-vs-update-only asymmetry between the 2 fields it
  copies, not just the node-identity matching — a design detail the
  original bug report and its proposed fix did not address.
- **CONFIRMED, found in a further dedicated pass on this function's
  `<PROPERTY>` skip list specifically: each of the 4 skipped names has a
  distinct, confirmed downstream reason to stay `B`'s own.**
  (`pim_core/pim_core_src/pimSilent.cxx:503,690-693`.)
  ```cpp
  StringXArray PropertyNamesToKeepUnchanged;   // :503, function-local, fresh every call
  ...
  PropertyNamesToKeepUnchanged += "[SHIPCODE]";
  PropertyNamesToKeepUnchanged += "[VERSION]";
  PropertyNamesToKeepUnchanged += "[SOURCE]";
  PropertyNamesToKeepUnchanged += "CustomActions";
  ```
  - **`[SHIPCODE]`**: read immediately after `Eb`'s creation by
    `pimSilentInstallFromXML()` (`pim/pim_src/pimTop.cxx:1556-1559`) to
    populate the session's own `SHIPCODE_PROPERTY` from `Eb`'s **own**
    value — must reflect which physical media/build `B` actually is, not
    whatever `A`'s request XML happens to declare.
  - **`[VERSION]`**: originally populated by `pimEntitlement::Init()`
    itself (`pim_core/pim_core_src/pimEntitlement.cxx:944,1195`) directly
    from `Eb`'s own `<PRODUCT version>` attribute at `Eb`'s own
    initialization time — intrinsic to which XML file `Eb` was created
    from, not something `A` should redefine — and later read by
    `pimCustomActionsLoop`
    (`pim_core/pim_core_src/pimCustomActions.cxx:542`) for version-gated
    custom-action behavior and by `pimSessionInfo`
    (`pim_core/pim_core_src/pimSessionInfo.cxx:1801,2316`) for
    cross-entitlement version matching.
  - **`[SOURCE]`**: unconditionally **overwritten anyway** by the same
    caller loop moments after `pimSilentCreateEntitlement()` returns
    (`pim/pim_src/pimTop.cxx:1598`, `SetProperty("[SOURCE]",
    ArgZero.GetHead())`) — its exclusion here is consistent with, but
    largely superseded by, that later explicit set; a low-severity
    inclusion in this list, since the caller's own set would mask any
    mistake here regardless.
  - **`CustomActions`**: gates **many** real install/uninstall lifecycle
    hook points — `PreUpgrade`, `PreInstall`, `PreMSIConfigure`,
    `PostCopy`, `PostInstall`, `PostUpgrade`, `PreReconfigure`,
    `PostReconfigure`, `PreUninstall`, and more — via
    `pimEntitlement::OnInstallCustomActions()`/`OnUninstallCustomActions()`'s
    `xmlPtr->GetProperty("CustomActions", str)` truthy-check
    (`pim_core/pim_core_src/pimEntitlement.cxx:5151,5882,6024,6669,6753,6837,6842,7023,7159,7228,7309`),
    each constructing a `pimCustomActionsLoop` — a **newly-discovered
    `pimLoop` subclass** (`pim_core/includes/pimCustomActions.h` +
    `pim_core_src/pimCustomActions.cxx`) not previously mentioned anywhere
    in this doc set — on the same `xmlPtr`. The property's mere
    **presence**, not even its specific value, determines whether `B`'s
    own baked-in custom-action fixups run at all across the entire
    install/uninstall lifecycle; overwriting it with `A`'s
    presence/absence would silently enable or disable `B`'s own
    media-specific fixups.

  **Verified non-issue, not a bug (checked while tracing the skip check
  itself)**: the copy loop's `name` variable (`:688`) is declared once
  outside the `do`/`while` loop, so a `<PROPERTY>` node lacking a `name`
  attribute would leave `name` holding a stale value from a prior
  iteration when `PropertyNamesToKeepUnchanged.Find(name)` runs — but the
  immediately following copy loop's own guard
  (`for (int i = 0; attribName != NULL && i < ...)`, `:712`) means **0
  copies happen either way** for such a node, so the stale value has no
  observable effect — unlike the superficially similar, but genuinely
  live, bug already documented above in `pimSilentFixupShortcuts()`'s
  Program Menu handling.
  **The proposed hardening fix, verified in a further, dedicated pass
  specifically on this loop's fix — not a new function, a deeper scrutiny
  of one already-recommended, low-priority remediation ("initialize the
  copy loop's `name` variable fresh at the top of each iteration")**:
  **CONFIRMED still purely cosmetic today, with a precisely identified
  reason it isn't merely academic**. Traced exactly *which* guard does the
  real work: whether the outer `continue` (`:710-711`) fires due to a
  coincidental stale-name match against the skip list, or falls through
  because the stale name happens *not* to match, the **inner for-loop's
  own `attribName != NULL` condition (`:712`) is the sole, always-present
  safety net** — it independently guarantees 0 copies for a nameless
  node regardless of what the outer `continue` did or didn't do. The
  outer `continue`'s stale-name behavior is therefore **never the
  operative protection**, in either direction — a subtlety the original
  "verified non-issue" framing didn't spell out. Applying the fix
  (`name.Clear()`, or re-declaring `name` inside the loop — confirmed
  equivalent, both simply make `name` empty each nameless iteration
  instead of stale) changes **nothing observable**: `PropertyNamesToKeepUnchanged.Find("")`
  finds no match, so the `continue` no longer fires for such a node, but
  the inner loop's own guard still independently produces 0 copies — the
  exact same net result. **The fix's real value, precisely characterized**:
  it removes a **latent maintenance trap**, not a live bug. A plausible
  future refactor — e.g. a maintainer "simplifying" the inner loop's
  condition to just `i < map->getLength()`, reasoning (incorrectly) that
  the outer `continue` already handles the no-name case — would silently
  reintroduce exactly the kind of stale-value corruption already confirmed
  live in `pimSilentFixupShortcuts()`'s Program Menu bug, since the outer
  `continue`'s protection was never real to begin with. The fix is
  confirmed sufficient to close this specific latent trap, at the trivial
  cost proposed.
  **CONFIRMED BUG, found in the same pass, by tracing this loop's full
  return-value chain — a new finding distinct from the `name`-staleness
  issue**: `pimXmlFile::GetProperty()`
  (`pim_core/pim_core_src/pimXmlFile.cxx:783-808`) is called at **both**
  value-copy sites (`:719`, the `pimname`/text-content case; `:725`, the
  general-attribute case) with its own `bool` return value **discarded**.
  `GetProperty()` internally calls `GetPropertyNode(name, &matchedNode)`
  (`:755-781`), which searches `Ea`'s **entire document**, by `name`, for
  the **first** `<PROPERTY>` node matching in document order — not
  necessarily the same node this `do`/`while` loop is currently iterating.
  If that search fails, or the matched node lacks the specific `attrib`
  being requested, `GetProperty()` returns `false` and leaves `value`
  **entirely untouched** — but `Eb->GetXMLPtr()->SetProperty(name, value,
  ...)` (`:720`, `:726`) is called **unconditionally** regardless,
  potentially writing a **stale value carried over from a previous,
  unrelated attribute or property earlier in the same loop** onto `B`.
  **Reachability, precisely characterized**: for the *current* node's own
  attributes, this can only actually diverge when `Ea`'s document contains
  **2 or more `<PROPERTY>` nodes sharing the same `name`** — not prevented
  by any schema in this archive (`ai-context/xml_schema.yaml` confirms no
  XSD is enforced) — in which case `GetPropertyNode()`'s document-order
  search returns the **first** duplicate, not necessarily the one
  currently being iterated; if that first duplicate lacks the specific
  attribute the current node has, the copy silently uses a stale value
  instead. Not confirmed reachable against a real product XML (none exists
  in this archive), but the mechanism is fully confirmed from source, and
  mirrors — in a previously unexamined location — the exact "discarded
  `Get()` return + reused stale variable" pattern already confirmed live
  in `pimSilentFixupShortcuts()`'s Program Menu bug.
  **A more robust alternative, already available in the code's own
  hands**: for the general-attribute branch (`:722-727`), `attrib`
  (obtained via `map->item(i)`, `:714`) **is already the exact DOM
  attribute node** whose value is wanted — `attrib->getNodeValue()` would
  read it directly, with no document-wide re-search, no dependency on
  `name` uniqueness, and no staleness risk at all, making the current
  indirect `GetProperty()`/`SetProperty()` roundtrip unnecessary for this
  branch specifically. The `pimname`/text-content branch (`:717-720`)
  genuinely needs the indirect lookup, since the wanted value (the
  property's text content) isn't available directly from `attrib` — but
  its discarded return value is not otherwise addressed by this
  simplification and would still need an explicit check.

## Usage Example (as evidenced by call sites)

```cxx
// pim/pim_src/pimTop.cxx:1530-1540 (pimSilentInstallFromXML) -- the one
// confirmed call site for both pimSilentTestXmlIsUseable() and
// pimSilentCreateEntitlement(), run once per AskedToInstallThisList entry
for (i = 0, max = (unsigned)AskedToInstallThisList.GetSize(); i < max && !abort; i++)
{
    if (pimSilentTestXmlIsUseable(AskedToInstallThisList[i], files, str))
    {
        k = -1;
        pimSilentCreateEntitlement(AskedToInstallThisList[i], str, k);
        if (k < 0)
        {
            abort = true;
            return pimGetLastError();
        }
        ...
    }
    else
        abort = true;
}
```

---
*Extends this documentation set's `pim_core`-focused extension series:
discovered while tracing `pimSilentInstallFromXML()`'s per-entry validation
loop (see `docs/classes/pimGetApplicationsList.md`), and documented at full
depth as a free-function subsystem rather than a class, matching this set's
established treatment of `pimGetApplicationsList`.*
