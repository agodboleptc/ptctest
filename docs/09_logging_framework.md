# PIM Logging Framework

> Traced from `pim_core/includes/pimLog.h`, `pim_core/pim_core_src/pimLog.cxx` (full,
> 173 lines), `pim_core/includes/pimMessage.h`/`pimMessage.cxx`, `pim_core/includes/`
> `pim_msg.h` (417 lines, 412 message-ID constants), and
> `pim_core/messages/usascii/pim.msg` (3,814 lines, the message-catalog source). No
> source was modified.

## 1. Two Cooperating Systems

PIM has **two separate, complementary text-producing systems** that are easy to
conflate but serve different purposes:

1. **The logger** (`btklog`-based, external `btk` toolkit) — writes timestamped,
   level-filtered lines to a rotating log file on disk. Driven by the
   `LG_INFO`/`LG_ERROR`/`LG_DEBUG`/`LG_DEBUG_START`/`LG_COUT`/`LG_END` macro family
   seen throughout every module.
2. **The message catalog** (`pimMessage`/`pim_msg.h`/`.msg` files) — resolves a
   numeric message ID plus up to 10 positional string arguments into a
   **localized** string, used both for log lines (passed to `LG_*` as the text) and
   directly for UI display (error dialogs, status labels).

A typical call site combines both: `LG_ERROR(LOG_SERVICE, pimMessage(pimLogXxx,
arg).cString());` — the message system produces the text, the logger decides whether/
where it's written.

## 2. Logger Components

| Component | Role |
|---|---|
| `LOG_SERVICE` (global `LogAreaT`) | The single log "area" every `LG_*` call targets, created once via `GetLoggerManager()`/`InitLog()` |
| `GetLoggerManager()` | Re-attaches the **current OS thread** to the `"LogService"` logger and returns the `LOG_SERVICE` area — called at the top of every `pimLoop`-subclass's `OnExecute()` (confirmed in `pimCopyLoop.cxx`, `pimEntitlement.cxx`, etc.), since each install step runs on its own thread and `btklog` apparently needs an explicit per-thread logger binding |
| `InitLog()` (`pim_core/pim_core_src/pimLog.cxx:95`) | One-time process-level setup: picks the log directory, sets the log base name, creates the manager/logger/area, and sets the initial log level. Called once from each `pim*Run` function (`docs/03_application_startup.md` §2) |
| `InitDbgLog()` | Declared in `pimLog.h`; not read/traced in this pass |
| `pimProcessOldLogs(btkFSEntry)` | Log rotation (§4) |
| `CreateFullLogPath(btkFSEntry&)` | Resolves a filename against the same directory logic as `InitLog()` (`PIM_LOG_ENV` or My Documents), for callers that need a full path to a PIM-managed file outside the logger itself |

## 3. Log Levels

`btklog`'s level enum is used with exactly two values observed in this codebase:

- `LogLevelT::LG_T_INFO` — the **default** level, set unconditionally unless `PIM_DEBUG` is present in the environment.
- `LogLevelT::LG_T_TRACE` — set instead when the `PIM_DEBUG` environment variable exists (any value, checked via plain `getenv("PIM_DEBUG")`, not the obfuscated-name helper used for `PIM_LOG_ENV`).

No other level (`LG_T_WARNING`, `LG_T_ERROR`, etc., if `btklog` defines them) was
observed being explicitly set in this file — filtering appears to be a simple
"trace everything" vs. "info and above" binary switch, externally controlled by an
environment variable rather than a runtime UI setting.

## 4. Output Location and Rotation

- **Base filename**: `pim_installmgr` (`#define PIM_LOG_FILE_NAME`), producing
  `pim_installmgr.log`, `.log.1`, `.log.2`, etc.
- **Directory**: the `PIM_LOG_ENV` environment variable if set (its literal name is
  itself passed through `BTK_UNSCRAMBLE_31_S("PIM_LOG_ENV")` before the `btkGetEnv`
  lookup — an obfuscation of the env-var *name* string in the binary, not of its
  *value*); otherwise the current user's "My Documents" folder
  (`pimGetMyDocuments`, from `pim_util`).
- **Rotation** (`pimProcessOldLogs`, run once at the top of `InitLog()`): lists all
  `pim_installmgr.log.*` files whose extension is purely numeric (via a custom
  `btkFSList::FilterObj` checking `strspn(ext, "0123456789") == ext.GetLength()`),
  sorts by modification time, and if more than 2 exist, **deletes everything except
  the most recent 2**, then renames the survivors to `.log.1`/`.log.2`/`.log.3` in
  order — the comment states "Rename Logs ... latest log as .3 always," i.e. the
  numbering is reassigned every run so the highest number is always the newest, not a
  stable per-run identifier.
- **Debug environment dump**: when `PIM_DEBUG` is set, `InitLog()` spawns the `env`
  shell command via `btkPipedProcess` and logs its entire output line-by-line at
  `LG_DEBUG` level (§7, Risk).

## 5. Message Catalog (`pimMessage` / `pim_msg.h` / `.msg` files)

### 5.1 Resolution Path

```
pimMessage(msgid, va0..va9)
  -> msgID_sput_buffer(buffer, MSGPUT_BUFFER_MAXSIZE, msgid, va0..va9)   [external, imp_pim_dll.h]
  -> resolves msgid against the loaded message catalog (compiled from .msg files)
  -> substitutes %0s/%1s/... positional placeholders with va0..va9
  -> strips a trailing '\r' if present
  -> exposed as both cString() (narrow) and wString() (wide)
```

`msgID_sput_buffer` itself is external (declared in `pim/includes/imp_pim_dll.h`,
`docs/01_repository_inventory.md` §5) — the actual catalog lookup/formatting logic is
not in this archive; only the call contract and the source catalogs are.

### 5.2 Catalog Source Format (`.msg` files)

`pim_core/messages/<lang>/pim.msg` (11 languages + a `usascii/pim.msg.LOCAL`
variant) use a Lisp-like s-expression format:

```
(MsgFile 138
    (message pimLogSecurityAuth 1
        (comment
none
        )
        (class Prompt)
        (text
Received Security Authentication request.
        )
    )
    (message pimLogExisitingSessionFile 3
        (comment
Session File name
        )
        (class Prompt)
        (text
Found existing SessionInfo file %0s.
        )
    )
    ...
)
```

- `MsgFile 138` — a numeric file/catalog ID matching the generated header's own
  comment (`pim_core/includes/pim_msg.h:1`: `/* Message file ID: 138 */`), confirming
  `pim_msg.h`'s 412 `#define pimXxx 0x8a5....L` constants are **machine-generated
  from this `.msg` source** (by an internal PTC message-compiler tool, not included
  in this archive).
- Each message has a **class**: exactly 4 distinct values found across the catalog —
  `Error`, `Warning`, `Info`, `Prompt`. This is a severity/intent taxonomy **orthogonal
  to the logger's own level filtering** (§3) — a message's `class` describes how it's
  meant to be presented (e.g. UI dialog icon), while the `LG_*` macro at the call site
  independently decides whether it's written to the log at all. The two are not
  automatically synchronized: nothing in the traced code enforces that a `class Error`
  message is always logged via `LG_ERROR`.
- `%0s`, `%1s`, etc. — positional placeholders matching `pimMessage`'s `va0..va9`
  parameters.
- `(comment ...)` — a documentation-only field for translators/maintainers (e.g.
  "Session File name" explaining what `%0s` will contain), not used at runtime.

### 5.3 Localization

11 language directories exist under `pim_core/messages/`: `chinese_cn`, `chinese_tw`,
`french`, `german`, `italian`, `japanese`, `korean`, `portuguese_br`, `russian`,
`spanish`, `usascii`. Each presumably defines the same message IDs with translated
`(text ...)` bodies (not individually diffed against each other in this pass). The
active language is selected elsewhere in the `pim`/`pim_util` layer
(`pimGetLangDirectory`, seen driving text-resource search paths in
`pim/pim_src/pimGeneralInit.cxx`, `docs/modules/pim.md`) — not through this file.

## 6. Typical Log Patterns (as observed across every module documented so far)

```cxx
LOG_SERVICE = GetLoggerManager();                       // once, at thread start (OnExecute)
LG_DEBUG(LOG_SERVICE, "pimEntitlement::" << Tag << " thread start" << (int)GetCurrentThreadId());
LG_INFO(LOG_SERVICE, pimMessage(pimLogLoadEntitlement, files[i]).cString());
LG_ERROR(LOG_SERVICE, lcl_src << " Cab file is missing. ");   // ad hoc string, no message ID
```

Two coexisting styles are used interchangeably even within the same file: **ad hoc
inline strings** (fast, unlocalized, unnumbered) and **catalog-backed `pimMessage`
calls** (localized, numbered, substitution-capable). There is no enforced convention
in the traced code for when to use which — user-facing text tends toward `pimMessage`,
pure debug trace tends toward ad hoc strings, but this is an observed tendency, not a
rule found stated anywhere.

## 7. Failure/Risk Notes Specific to Logging

- **Environment dump on `PIM_DEBUG`**: the entire process environment is captured via
  `env` and written to the debug log verbatim. If any sensitive value (a license
  server credential, an API key passed through an environment variable by whatever
  launched PIM) is present in the environment, enabling `PIM_DEBUG` will persist it to
  a log file in My Documents (or wherever `PIM_LOG_ENV` points) in plaintext. This is a
  genuine operational-security consideration for support engineers being asked to
  "just set `PIM_DEBUG=1` and send us the log."
- **Log rotation renumbering**: because `.log.1`/`.log.2`/`.log.3` are reassigned
  every run based on modification time rather than being stable identifiers, a
  troubleshooting workflow that references "check `pim_installmgr.log.2`" from a
  previous run's instructions may point at a different actual run's content the next
  time rotation occurs.
- **Message `class` vs. log level are independently maintained** (§5.2) — a
  translator or content editor changing a message's `class` in the `.msg` file has no
  effect on whether/how it's logged; a developer changing the `LG_*` macro at a call
  site has no effect on the message's own declared severity. Keeping these
  consistent is a manual discipline, not an enforced invariant.
- **No structured/machine-parseable log format** was found — `LG_*` macros take
  streamed (`<<`) text, producing free-form lines; automated log analysis would need
  to parse message text/IDs rather than consume structured fields.

---
*Phase 12 of the requested 20-phase documentation set. No source was modified.*
