# `pimGetApplicationsList` (and its 2 sibling free functions)

**File:** `pim/includes/pimGeneralInit.h` (declarations, lines 70-71/79 of 130+, not read in full — see Scope note) / `pim/pim_src/pimGeneralInit.cxx` (implementation, lines 88/120-146/474-494 read directly; the rest of this 500+-line command-line-argument/init file was not read in full — see Scope note)
**Module:** `pim` (not `pim_core` or `pim_ui`)

> **Not a class.** Unlike every other entry in this documentation set,
> `pimGetApplicationsList` is a **free function** operating on a file-scope
> `static StringXArray applications_list` in `pimGeneralInit.cxx` — there is
> no `pimGetApplicationsList` class, object, or instance. This doc covers it
> and its 2 directly-related sibling free functions
> (`pimAddApplicationsToList`, `IsApplicationInInstallList`) plus the
> command-line-argument-parsing logic in `pimCommandLineArgs()` that
> populates the shared state they all read/write, at the same evidentiary
> depth (every claim traced to a cited file/line) as this set's class docs.
> Sections that don't apply to a free function/static-state pair (Members as
> a table of instance fields, Ownership Model, a per-instance Lifetime) are
> adapted or omitted accordingly.
>
> **Enrichment (a later, dedicated pass on `IsApplicationInInstallList()`
> specifically)**: re-confirmed its call-site list is unchanged (still
> exactly the 2 calls inside `AddMandatoryAppsForSilentInstall()`, its only
> caller) and added a new Risk Analysis finding: its substring-match
> approach, while confirmed more robust than the 2 exact-match consumers to
> this file's own bare-tag-vs-full-path format split, carries its own
> confirmed structural false-positive risk in the opposite direction — see
> Risk Analysis.
>
> **Enrichment (a later, dedicated pass on `AddMandatoryAppsForSilentInstall()`
> specifically — `pim/pim_src/pimTop.cxx:1282-1316`, not itself in
> `pimGeneralInit.cxx`/`.h` but this doc's confirmed sole populator of
> "mandatory" entries)**: this pass **corrects** the earlier framing of the
> `InstallPreReqSilent()` bug's "common case" (see Risk Analysis) and adds 2
> new confirmed findings: (1) the `qualityagent.xml` mandatory-injection
> branch is nested entirely inside `if (creobaseXml.Exists())`, so it is
> silently skipped on any media that ships `qualityagent.xml` without also
> shipping `creobase.xml` — contradicting this function's own name and the
> "Mandatory Installation of Quality Agent" intent documented in both
> `pimGeneralInit.h`'s and `pimTop.cxx`'s own changelogs (`13-Aug-26`
> entries `$$34`/`$$148`); and (2) `-allpacks` mode's utility-XML entries are
> appended only to `pimSilentInstallFromXML()`'s own local list, never to the
> shared `applications_list`, so `InstallPreReqSilent()`'s later, independent
> `pimGetApplicationsList()` call can never see them — every `-allpacks`
> utility entitlement's prerequisites are silently never checked, a
> confirmed, narrower, and more precise instance of the loop-bound bug than
> originally stated. `AddMandatoryAppsForSilentInstall()` itself has **no
> header declaration anywhere in this archive** (there is no `pimTop.h`) —
> no Doxygen patch was created for it, the same situation as `Cmp_cStrings`.
> See Risk Analysis.

## Purpose

Implements the `-APPLICATIONS`/`-XML`/`-XMLALL` family of command-line flags
that let a caller (interactively or, more commonly, a silent/scripted
install) limit or directly specify which product XML files should be
installed, out of everything otherwise available on a CD image or PTC.com
web-media feed.

## Responsibilities

- `pimCommandLineArgs(int argc, const char *argv[])` (`pimGeneralInit.cxx:148-`,
  the top-level `pim`-module command-line parser, called once per process from
  every `pimTop.cxx` Run entry point) populates the shared
  `static StringXArray applications_list` (`:88`) from **3 different flags,
  in 3 different formats** — see Risk Analysis for why this matters:
  - `-APPLICATIONS <arg>` (`:218-225`): splits `<arg>` on `:` and appends each
    **bare word** (e.g. `creobase.xml`) as-is.
  - `-XML <arg>` (`:226-241`): treats `<arg>` as **one filesystem path**
    (`btkFSEntry`), expands it to absolute if relative, and appends the path.
  - `-XMLALL <arg>` (`:242-262`): treats `<arg>` as a directory, globs
    `*.xml` inside it, and appends **each file's absolute path**.
  - `-FLEX` (`:214-217`, via `pimSetProductMode(PIM_FLEXONLY_MODE)`,
    `:114-126`): if `applications_list` is still empty at the point `-FLEX`
    is processed, seeds it with exactly 2 hardcoded bare tags,
    `FLEX_ADMIN_XML`/`FLEX_LMGRD_XML`.
- `pimGetApplicationsList(StringXArray &out)` (`:474-489`) copies
  `applications_list` into `out` and returns whether it was non-empty; a
  debug-log dump of the list's contents runs on every call (`LG_DEBUG_START`/
  `LG_COUT` loop, `:480-484`).
- `pimAddApplicationsToList(btkFSEntry &app)` (`:491-494`) appends one more
  entry — always a **filesystem path** (`btkFSEntry`, implicitly converted),
  never a bare tag — to the same shared `applications_list`. Confirmed called
  from `AddMandatoryAppsForSilentInstall()` (`pim/pim_src/pimTop.cxx:1282-1316`,
  every silent install's sole "mandatory apps" injection point) to add
  `creobase.xml`'s full path if it exists on the media and isn't already
  requested, and `qualityagent.xml`'s likewise — **but see Risk Analysis: the
  qualityagent.xml branch is confirmed nested inside the creobase.xml
  existence check, so it is not actually unconditional**, despite the
  function's name and its changelog's "Mandatory Installation of Quality
  Agent" intent.
- `IsApplicationInInstallList(const btkString &application)`
  (`pimGeneralInit.cxx:133-146`) — a **substring** search
  (`applications_list[i].Pos(application) != -1`) over the same shared list,
  used as an idempotency/"already requested" check.

## Dependencies

- `StringXArray`/`btkFSEntry` (external `btk` types).
- `pimTop.cxx`'s `pimCommandLineArgs()` caller (every Run entry point calls
  it once at process start, before any of these 3 sibling functions are
  used).

## Shared State (in place of a Members table)

| Name | Type | Scope | Purpose |
|---|---|---|---|
| `applications_list` | `static StringXArray` | file-scope in `pimGeneralInit.cxx:88` | The single list all 3 sibling functions read/write; contents are format-heterogeneous depending on which command-line flag(s) populated it — see Risk Analysis. |

## Public APIs

| Function | Purpose |
|---|---|
| `bool pimGetApplicationsList(StringXArray &out)` | Copies `applications_list` into `out`; returns `true` iff non-empty. |
| `void pimAddApplicationsToList(btkFSEntry &app)` | Appends one filesystem path to `applications_list`. |
| `bool IsApplicationInInstallList(const btkString &application)` | Substring-searches `applications_list` for `application`; returns `false` immediately if `application` is empty. Format-agnostic (robust to the bare-tag-vs-full-path split — see Risk Analysis) but carries its own confirmed structural false-positive risk in the opposite direction — see Risk Analysis. |
| `int pimCommandLineArgs(int argc, const char *argv[])` | Not itself part of this "class," but the sole populator of `applications_list` via `-APPLICATIONS`/`-XML`/`-XMLALL`/`-FLEX` — documented here because its exact per-flag format is the direct cause of this doc's headline findings. |

## Private Utilities

None — all 4 functions above are declared `extern`/public in `pimGeneralInit.h`;
there is no private/internal helper specific to this group.

## Called By

- `pim_core/pim_core_src/pimGetAvailable.cxx:373` (`pimGetAvailable::OnExecute()`,
  see `docs/classes/pimGetAvailable.md`) — calls `pimGetApplicationsList()`
  into a local `LimitToThisList`, then filters web-fetched entitlement tags
  via **exact match**, `LimitToThisList.Find(str) == -1` (`:419`, using the
  `<ENTITLEMENT>` tag's bare text content as `str`).
- `pim/pim_src/pimTop.cxx:390` (an unnamed-in-this-pass local/physical-image
  entitlement-loading routine — the enclosing function itself was not
  identified by name in this pass) — calls `pimGetApplicationsList()` into
  `LimitToThisList`, then filters locally-discovered CD-image XML files via
  the **same exact-match pattern**, `LimitToThisList.Find(files[i].GetTail()) == -1`
  (`:421`, `files[i].GetTail()` being the bare filename of a file found via a
  local `*.xml` glob).
- `pim/pim_src/pimTop.cxx:904-966` (`InstallPreReqSilent()`, called from the
  silent-install Run path at `:1780`, comment: "attempt to satisfy all
  preRequisites") — calls `pimGetApplicationsList()` into
  `AskedToInstallThisList` and then **uses only its `.GetSize()`** as the
  bound of a loop that indexes `pimGetSessionInfo()->GetEntitlement(i)` — a
  completely different array. **See Risk Analysis: this is a confirmed bug**,
  the most severe finding in this pass.
- `pim/pim_src/pimTop.cxx:1425` (`pimSilentInstallFromXML()`, "NO UI just
  install based upon the application xml passed in" per its own comment) —
  calls `pimGetApplicationsList()` into `AskedToInstallThisList`, then calls
  `AddMandatoryAppsForSilentInstall(AskedToInstallThisList, CdimageXmlsDir)`
  (`:1428`, see below), then iterates `AskedToInstallThisList` **by index,
  treating every entry as a filesystem path** (`btkFSEntry aFile =
  AskedToInstallThisList[i]; ... aFile.Exists() ... aFile.CanRead()`,
  `:1486-1499`) — the one confirmed call site whose usage is actually
  consistent with `-XML`/`-XMLALL`'s path-producing format.
- `pim/pim_src/pimTop.cxx:1282-1316` (`AddMandatoryAppsForSilentInstall()`,
  called only from the `pimSilentInstallFromXML()` site above, exactly once
  per silent-install run reaching that point — **no header declares it
  anywhere in this archive, there is no `pimTop.h`, so no Doxygen patch
  exists for it**) — calls `IsApplicationInInstallList()` (substring match,
  format-agnostic) to check whether `creobase.xml`/`qualityagent.xml` were
  already requested, and if not (and if the file exists on the media),
  appends its full path via both a direct
  `AskedToInstallThisList += creobaseXml;`/`+= qualityAgentXml;` and
  `pimAddApplicationsToList(...)` (i.e. into both the local copy *and* the
  shared global state). **CONFIRMED, corrected in a later dedicated pass on
  this function**: this is **not actually unconditional for
  `qualityagent.xml`** — its entire injection branch (`:1303-1314`) is
  nested inside the outer `if (creobaseXml.Exists())` (`:1287`), so on any
  media that ships `qualityagent.xml` without also shipping `creobase.xml`,
  quality agent's "mandatory" install is silently skipped too, despite this
  function's own name and both `pimGeneralInit.h`'s and `pimTop.cxx`'s
  changelogs independently recording a `13-Aug-26` entry titled "Mandatory
  Installation of Quality Agent" (`$$34`/`$$148`). See Risk Analysis.

## Calls Into

- `StringXArray`/`btkFSEntry`'s own `Find`/`Pos`/`+=`/`Exists`/`CanRead`
  (external `btk` container/filesystem APIs).
- `LG_DEBUG_START`/`LG_COUT`/`LG_DEBUG` (logging).

## Lifetime

`applications_list` is a file-scope `static`, initialized once at program
load and living for the entire process — there is no construction/destruction
event to document (unlike every class in this set). It is populated exactly
once per process, during the single `pimCommandLineArgs()` call every
`pimTop.cxx` Run entry point makes near its own start, and is only ever
appended to afterward (by `AddMandatoryAppsForSilentInstall()`'s
`pimAddApplicationsToList()` calls) — never cleared or reset.

## Ownership Model

Not applicable in the class sense — this is shared, mutable, file-scope
global state with no owner, accessible to any function in `pimGeneralInit.cxx`
and, indirectly, to any of this doc's confirmed callers via the 3 public
accessor/mutator functions. No encapsulation beyond "don't touch
`applications_list` directly from outside this file," which the codebase
itself already follows (no direct external reference to the `static` array
was found; every confirmed access goes through one of the 3 functions).

## Thread Safety

No lock of any kind guards `applications_list`. Not confirmed to be a
practical problem: every confirmed access happens during single-threaded,
sequential phases of a Run entry point (argument parsing, then later
synchronous reads) — no confirmed caller reads or writes it from a background
thread in this pass, unlike `pimGetAvailable`/`pimGetMediaDetails`'s
confirmed cross-thread usage documented elsewhere in this set.

## Extension Points

- A new command-line flag that limits or specifies applications should reuse
  `IsApplicationInInstallList()`'s substring-match convention if it needs to
  interoperate with existing `-XML`/`-XMLALL`-populated (path-format) data —
  see Risk Analysis for why the alternative, exact-match convention used
  elsewhere is not format-agnostic.
- Fixing the heterogeneous-format problem at its root (see Risk Analysis)
  would most naturally mean storing `(rawArg, resolvedTag)` pairs or a
  dedicated struct instead of a flat `StringXArray` of sometimes-tags,
  sometimes-paths — every confirmed consumer would need updating in lockstep.

## Risk Analysis

- **CONFIRMED BUG, HIGH SEVERITY: `InstallPreReqSilent()`'s prerequisite loop
  is bounded by the wrong array's size.** (`pim/pim_src/pimTop.cxx:904-966`,
  called from the silent-install path at `:1780`.)
  ```cpp
  StringXArray AskedToInstallThisList;
  pimGetApplicationsList(AskedToInstallThisList);
  for (i = 0, max = (int) AskedToInstallThisList.GetSize(); i < max; i++)
  {
      ...
      while (pimGetSessionInfo()->GetEntitlement(i)->GetNextPrerequisite(&ptr_pr_E, pr_idx, true))
      ...
  }
  ```
  `AskedToInstallThisList` (the command-line-args-derived application list) is
  **never read by index or value anywhere in this function** — it is used
  solely for `.GetSize()`, which then bounds a loop indexing
  `pimGetSessionInfo()->GetEntitlement(i)`, a **completely unrelated array**
  (the session's actual parsed product entitlements). The two arrays have no
  guaranteed size relationship: `AskedToInstallThisList` reflects whatever
  `-APPLICATIONS`/`-XML`/`-XMLALL`/`-FLEX` flags were passed, **plus**
  whatever `AddMandatoryAppsForSilentInstall()` injected into the same
  shared `applications_list` earlier in the one confirmed call path
  (`pimSilentInstallFromXML()`, `:1428`, always reached before
  `InstallPreReqSilent()` at `:1780`) — while `SessionInfo`'s entitlement
  count reflects the actual product XML files parsed for this install.
  **CORRECTION (found in a later, dedicated pass on
  `AddMandatoryAppsForSilentInstall()` itself)**: the original framing here
  — "commonly zero entries, the common case" — overstated how often the
  list is actually empty in the one real call path. Since
  `AddMandatoryAppsForSilentInstall()` unconditionally *attempts* to inject
  `creobase.xml` (and, if that file exists, `qualityagent.xml` too) before
  `InstallPreReqSilent()` ever runs, the list is typically **non-empty** —
  it is only actually empty when no application-limiting flag was passed
  **and** the media's root directory has no `creobase.xml` file at all
  (e.g. a non-Creo product line's media). The more precisely confirmed,
  and more subtle, consequence is a **size mismatch, not just an empty
  list**: see the 2 new findings below (quality agent's injection itself
  being conditionally skipped, and `-allpacks` utility entries being
  invisible to this loop) for exactly how that mismatch arises in the one
  traced call path. Whenever the list ends up smaller than the entitlement
  count, `InstallPreReqSilent()` misses trailing entitlements entirely,
  silently skipping their prerequisite satisfaction; a larger list than the
  entitlement count is not demonstrated in any confirmed call path today,
  but would hit `pimSessionInfo::GetEntitlement(i)`'s own bounds check
  (`pim_core/pim_core_src/pimSessionInfo.cxx:776-782`, returns `NULL` if out
  of range) immediately dereferenced with no `NULL` check
  (`->GetNextPrerequisite(...)`) — a **latent crash risk**, not a live bug
  in any call path traced so far. This reads as a copy-paste/wrong-variable
  bug — the loop bound was very likely intended to be
  `pimGetSessionInfo()->GetEntitlementSize()`.
- **CONFIRMED BUG: `applications_list` mixes 2 incompatible content formats,
  and 2 of its 3 real consumers only work correctly with one of them.**
  `-APPLICATIONS` stores bare tags (`"creobase.xml"`); `-XML`/`-XMLALL` store
  full filesystem paths (`"D:\cdimage\pim\xml\creobase.xml"`); and
  `AddMandatoryAppsForSilentInstall()` **unconditionally** injects 2 more
  full-path entries into every silent install's list via
  `pimAddApplicationsToList()`, regardless of which flag (if any) the user
  passed. Of the 3 confirmed consumers of this list:
  - `pimGetAvailable::OnExecute()` and the `pim_core/pim_src/pimTop.cxx:390`
    local-image loader both filter via **exact match**
    (`LimitToThisList.Find(bareTagOrTail) == -1`) — which can **never** match
    a full-path entry against a bare tag/tail. Any use of `-XML`/`-XMLALL` (or
    simply running a silent install where `AddMandatoryAppsForSilentInstall()`
    has already injected its 2 full-path entries) guarantees these 2 filters
    will fail to recognize those entries, trimming entitlements that should
    have matched — in the worst case (an otherwise-empty list populated only
    by `-XMLALL`, or by the 2 mandatory-app injections with no other
    entries), trimming **every** entitlement, since `LimitToThisList.GetSize()
    > 0` is true (entering the filter branch) while every `.Find()` call
    fails.
  - `IsApplicationInInstallList()` correctly uses a **substring** search
    (`.Pos()`), which is format-agnostic — a full path contains its own bare
    tag as a substring, so this one consumer works regardless of which flag
    populated the list. This is the only one of the 3 consumers that is
    robust to the confirmed heterogeneous content.
  - `pimSilentInstallFromXML()` treats every entry as a **filesystem path**
    (`aFile.Exists()`/`aFile.CanRead()`) — correct for `-XML`/`-XMLALL`-style
    content, but would silently fail (`aFile.Exists()` false for a bare
    relative tag not present in the current working directory) if fed
    `-APPLICATIONS`-style bare tags instead.
  No code in this codebase distinguishes *which* flag populated a given
  entry once it's in `applications_list` — the format is implicit and
  caller-assumed, not enforced or tagged.
- **CONFIRMED BUG, found in a dedicated pass on
  `AddMandatoryAppsForSilentInstall()` itself: quality agent's "mandatory"
  injection is silently conditional on `creobase.xml` existing on the
  media, not on `qualityagent.xml`'s own presence.**
  (`pim/pim_src/pimTop.cxx:1282-1316`.)
  ```cpp
  void AddMandatoryAppsForSilentInstall(StringXArray &AskedToInstallThisList, btkFSEntry & CdimageXmlsDir)
  {
      const btkString CREO_BASE_XML = "creobase.xml";
      btkFSEntry creobaseXml = CdimageXmlsDir;
      creobaseXml /= CREO_BASE_XML;
      if (creobaseXml.Exists()) {                    // <-- gates BOTH blocks below
          ...                                        //     (creobase.xml injection)
          const btkString QUALITY_AGENT_XML = "qualityagent.xml";
          if (!IsApplicationInInstallList(QUALITY_AGENT_XML))
          {
              btkFSEntry qualityAgentXml = CdimageXmlsDir;
              qualityAgentXml /= QUALITY_AGENT_XML;
              if (qualityAgentXml.Exists())           // qualityagent's OWN existence check
              {
                  AskedToInstallThisList += qualityAgentXml;
                  pimAddApplicationsToList(qualityAgentXml);
              }
          }
      }                                                // if creobase.xml doesn't exist, this
  }                                                    // whole block (incl. qualityagent) never runs
  ```
  The `qualityagent.xml` injection block is nested entirely inside
  `if (creobaseXml.Exists())` — it has its own, separate `Exists()` check
  for `qualityagent.xml` itself, but that check is only ever reached if
  `creobase.xml` is *also* present on the same media. On any media that
  ships `qualityagent.xml` without shipping `creobase.xml` (e.g. a
  Mathcad-only or WGM-only product line's install image), quality agent is
  silently **not** auto-installed, contradicting both this function's own
  name (`AddMandatoryAppsForSilentInstall`) and the explicit intent recorded
  independently in 2 changelogs on the same date: `pimGeneralInit.h`'s
  `13-Aug-26 Q-27-22 JAY $$34 Mandatory Installation of Quality Agent` and
  `pimTop.cxx`'s own `13-Aug-26 Q-27-22 JAY $$148 Mandatory Installation of
  Quality Agent`. Reads as an unintentional structural coupling (likely from
  nesting the new quality-agent block inside the pre-existing creobase
  block rather than adding it as a sibling `if`), not a deliberate
  dependency — nothing else in this function or its callers suggests
  quality agent is meant to depend on Creo base being present.
- **CONFIRMED BUG, found in the same dedicated pass: `-allpacks` mode's
  utility-XML entitlements are invisible to `InstallPreReqSilent()`'s
  prerequisite-checking loop.** When `pimGetAllPacksMode()` is set,
  `pimSilentInstallFromXML()` globs every `*.xml` file in the CD image
  directory and appends each `"Utilities"`-family entry directly to its own
  **local** `AskedToInstallThisList` (`pim/pim_src/pimTop.cxx:1430-1474`,
  `AskedToInstallThisList += files[fileCount];`) — **not** via
  `pimAddApplicationsToList()`, so these entries never reach the shared
  `applications_list` `pimGetApplicationsList()` reads from. Since every
  entry in that local list still becomes its own real `pimEntitlement` in
  creation order (`:1530-1755`, 1 entitlement per surviving entry, in
  order: flag-driven entries, then `AddMandatoryAppsForSilentInstall()`'s
  entries, then `-allpacks`' entries appended last), and
  `InstallPreReqSilent()` later makes its own **independent**
  `pimGetApplicationsList()` call (`:913`) that reflects only the
  flag-driven-plus-mandatory entries — never the `-allpacks` additions —
  its loop bound exactly covers indices `0` through
  `(flag-driven + mandatory count) - 1`, i.e. it happens to correctly reach
  every flag-driven/mandatory entitlement (no out-of-range risk in this
  specific path, since the `-allpacks` entries are appended *after* them),
  but it **never reaches any `-allpacks`-added utility entitlement at all**
  — their prerequisites are silently never checked whenever `-allpacks` is
  combined with a silent install. This is a narrower, more precisely
  evidenced instance of the loop-bound bug above, isolated to exactly one
  confirmed cause rather than "the list is usually empty."
- **Confirmed structural risk (not a demonstrated live bug), found in a
  dedicated pass on `IsApplicationInInstallList()` itself**:
  `IsApplicationInInstallList()`'s substring search (`.Pos()`), while
  confirmed more robust to the bare-tag-vs-full-path format mismatch than
  the 2 exact-match consumers above, is itself vulnerable to a **different**
  class of error a pure substring match always carries: a **false
  positive** whenever one product's filename (or a path fragment) happens
  to be a literal substring of another entry already in `applications_list`.
  For example, if `applications_list` ever contained a hypothetical entry
  like `"newcreobase.xml"`, calling `IsApplicationInInstallList("creobase.xml")`
  would return `true` (a substring match) even though the list never
  actually contains the standalone tag `"creobase.xml"`. Checked pairwise
  against every confirmed real argument passed to this function today
  (`CREO_BASE_XML = "creobase.xml"`, `CREO_BASE_XML_INSTALLED =
  "creobase.p.xml"`, `QUALITY_AGENT_XML = "qualityagent.xml"`,
  `AddMandatoryAppsForSilentInstall()`, `pim/pim_src/pimTop.cxx:1284-1316`)
  — none is a substring of any other, so **no false positive is
  demonstrated in this archive's actual call sites**. This is confirmed as
  a structural property of the substring-match approach itself, not a
  reproduced bug: any future product tag that happens to end with an
  existing tag's full name (e.g. a hypothetical `"xcreobase.xml"` alongside
  `"creobase.xml"`) would make `IsApplicationInInstallList("creobase.xml")`
  return `true` due to `xcreobase.xml`'s own presence, regardless of
  whether `"creobase.xml"` itself was ever actually requested. Worth noting
  for anyone adding a new product tag or extending this function's
  callers, since the failure mode (treating something as "already
  requested" when it wasn't) is the opposite direction of danger from the
  exact-match consumers' failure mode above (treating something as "not
  requested" when it was).
- **Lower-confidence, order-dependent observation**: `pimSetProductMode(PIM_FLEXONLY_MODE)`'s
  FLEX-tag seeding (`pimGeneralInit.cxx:118-124`) only fires if
  `applications_list.GetSize() < 1` *at the moment `-FLEX` is processed*.
  Since `pimCommandLineArgs()` processes `argv` sequentially, if a user's
  command line places `-APPLICATIONS`/`-XML`/`-XMLALL` **before** `-FLEX`,
  the hardcoded `FLEX_ADMIN_XML`/`FLEX_LMGRD_XML` entries are never added —
  not confirmed to cause an actual failure in this pass (would require
  tracing every consumer's FlexOnly-mode-specific behavior further), but a
  real, evidenced argument-order sensitivity in a flag (`-FLEX`) whose
  behavior should plausibly be order-independent.

## Usage Example (as evidenced by call sites)

```cxx
// pim/pim_src/pimTop.cxx:1425-1499 (pimSilentInstallFromXML) -- the one
// confirmed call site whose usage matches -XML/-XMLALL's path-producing format
StringXArray AskedToInstallThisList;
pimGetApplicationsList(AskedToInstallThisList);
AddMandatoryAppsForSilentInstall(AskedToInstallThisList, CdimageXmlsDir); // injects creobase.xml/qualityagent.xml paths unconditionally
for (i = 0, max = (unsigned)AskedToInstallThisList.GetSize(); i < max; i++)
{
    btkFSEntry aFile = AskedToInstallThisList[i];
    if (!aFile.Exists() || !aFile.CanRead())
        abort = true; // correct here, because this consumer treats every entry as a path
}
```

---
*Extends this documentation set's `pim_core`-focused extension series back
into the `pim` module: discovered while tracing
`pimGetAvailable::OnExecute()`'s use of `pimGetApplicationsList()`, and
documented at full depth as a free-function subsystem rather than a class,
given the significant confirmed bugs surfaced in its 4 real call sites.*
