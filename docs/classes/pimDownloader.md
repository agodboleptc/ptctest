# `pimDownloader`

`pim_core/includes/pimDownloader.h` (132 lines) / `pim_core/pim_core_src/pimDownloader.cxx` (1263 lines, download-transport implementation; wrapper-class logic confined to lines 1-213)

## Purpose

`pimDownloader` is the process-wide singleton "owner" wrapper around
`pimDownloadLoop` (see `docs/classes/pimDownloadLoop.md`), the Loop subclass
that fetches product content into a local cache. Its wrapper-class shape
(construction, `TryLock` gate, `Status()`/`Cancel()`/`Kill()`) is
structurally identical to `pimCopier`/`pimShortcuts`/`pimRegEdit` (full-
duration lock hold, flag-only `Cancel()`), but the file it lives in is by far
the largest of the 7 `.cxx` files (1263 lines) because it also hosts the
free-function HTTP(S) transport layer (`pimSendNRecvFromPTC`,
`pimRecvFromPTC`, and their `_low` implementations) and the `pimDownloadData`
buffering helper class — none of which are part of `pimDownloader` itself,
but which this file's header (`pimDownloader.h:116-131`) also declares.

## Responsibilities

- Gate entry into a download operation via `xmlMutex.TryLock()`
  (`pimDownloader.cxx:92`).
- On success, construct a fresh `pimDownloadLoop`, associate it with the
  calling `pimEntitlement` via `SetEntitlement(E)`, and start it via the
  Loop's own custom `Execute(const btkFSEntry&)` override (not the base
  `pimLoop::Execute()`; see `docs/classes/pimDownloadLoop.md`'s "Structural
  note") — `pimDownloader.cxx:90-110`.
- Translate `pimDownloadLoop`'s state, **plus its own reported
  percent/time-remaining** (obtained by calling `DownloadLoop->GetStatus()`
  first thing inside `Status()`, `pimDownloader.cxx:119`), into
  `DownloadStatus` for callers. This is the only one of the 4 flag-only-
  `Cancel()` wrappers (`pimShortcuts`/`pimRegEdit`/`pimServices`/
  `pimDownloader`) whose `Status()` actually forwards real progress data —
  `percent_downloaded`/`estimate_remaining` are populated here, unlike the
  dead `percent_done` parameters documented for `pimShortcuts`/`pimRegEdit`/
  `pimServices`.
- Hold `xmlMutex` for the full download duration, released only by
  `Status()` on `IsDone()` (`pimDownloader.cxx:152`) or by `Kill()`
  (`pimDownloader.cxx:194`).
- Also hosts (as free functions in the same file, not as class members):
  `pimSetTimeouts()`/`pimGetTimeouts()` (module-level timeout configuration,
  explicitly commented "called from the main thread only so don't need to be
  protected for threading", `pimDownloader.cxx:51-63`), `pimSendNRecvInit()`
  (reads `PIM_SET_CONNECT_RESPONSE_TIMEOUT`/`PIM_SET_READ_RESPONSE_TIMEOUT`
  environment variable overrides via a scrambled-string lookup,
  `pimDownloader.cxx:65-73`), and the `pimSendNRecvFromPTC`/`pimRecvFromPTC`
  HTTP(S) transport functions used by `pimDownloadLoop` to actually move
  bytes (implementation beyond line 213 not re-traced line-by-line in this
  pass — out of scope for the wrapper-class documentation depth targeted
  here; flagged as a further extension candidate).

## Dependencies

- `pimDownloadLoop` (the Loop subclass it owns; full class doc at
  `docs/classes/pimDownloadLoop.md`, including the confirmed digital-
  signature-check no-op).
- `pimEntitlement` (passed by reference into `Download()`, forwarded to
  `DownloadLoop->SetEntitlement()`).
- `pimDownloadData` (declared in the same header; a bounded 10-slot
  scatter-buffer helper used by the transport layer, not by the wrapper
  class itself).
- `threadlibcxx` (`thrMutex`, `thrRWLock`).
- `bs_pro_browser.h`, `btkhstream.h`, `pimScramble.h` — transport-layer/
  environment-variable-obfuscation dependencies for the free functions in
  this file (not used by `pimDownloader` the class itself).

## Members

| Member | Type | Purpose |
|---|---|---|
| `OnlyDownloader` | `static pimDownloader*` | The singleton instance pointer. |
| `xmlMutex` | `thrMutex` | "One download operation at a time" gate. |
| `statusMutex` | `thrRWLock` | Guards `DownloadLoop` pointer swaps and `pause_flag`/`cancel_flag`. |
| `pctMutex` | `thrRWLock` | Declared; not referenced in the wrapper-class portion of `pimDownloader.cxx` (lines 1-213) — dead member in that scope, same pattern as its 6 siblings (not re-checked against the transport-layer code beyond line 213). |
| `pause_flag`, `cancel_flag` | `bool` | Caller-facing intent flags. |
| `InMemoryXML` | `pimXmlFile*` | Declared; not assigned in the wrapper-class portion — dead member, same pattern as its siblings. |
| `DownloadLoop` | `pimDownloadLoop*` | The transiently-owned Loop instance; `NULL` when idle. |
| `CacheDir` | `btkFSEntry` | Destination cache directory, set from `Download()`'s `in` parameter and forwarded into `DownloadLoop->Execute(CacheDir)`. |

## Public/Protected APIs

- `static pimDownloader& GetInstance()` — lazy singleton construction.
- `int Download(pimXmlFile*, pimEntitlement &E, const btkFSEntry &in)` — the
  only one of the 7 wrappers' primary "start" methods that takes an
  entitlement reference as a parameter (matching
  `docs/classes/pimDownloadLoop.md`'s note that `pimDownloadLoop::SetEntitlement()`
  is required for its category-processing logic to resolve download sources).
- `DownloadStatus Status(int &percent_downloaded, btkTimeval &estimate_remaining)`
  — see Responsibilities; genuinely populates both output parameters via
  `DownloadLoop->GetStatus()`.
- `void Pause()` / `void Resume()` — flag-only; no corresponding call into
  `DownloadLoop`.
- `void Cancel()` — sets `cancel_flag` **only** (`pimDownloader.cxx:176-181`),
  same flag-only shape as `pimShortcuts`/`pimRegEdit`/`pimServices`.
- `void Kill()` — if in progress, calls `DownloadLoop->Kill()`, deletes it,
  unlocks `xmlMutex`.
- `bool GetErrors(btkString &out)` — unlike `pimCopier`'s equivalent (which
  always proxies through regardless of error state), this one explicitly
  checks `DownloadLoop->HasErrors()` first and returns `false` (leaving `out`
  untouched) if there are none, only calling `GetErrors()` and returning
  `true` when errors are actually present (`pimDownloader.cxx:202-213`) — a
  slightly richer contract than the other wrappers' bare `GetErrors()`.

## Private/Protected Utilities

- `pimDownloader()` (protected ctor) — initializes `statusMutex`/`pctMutex`;
  `InMemoryXML`/`DownloadLoop` to `NULL`.
- No separate `_low` helper exists for this class — `Download()` itself
  contains the full setup-and-launch logic inline (`pimDownloader.cxx:90-110`),
  unlike `pimCopier`/`pimMSICopier`/`pimSFXCopier`/`pimShortcuts`/`pimRegEdit`/
  `pimServices`, all of which factor this into a distinct `*_low()` method.

## Called By

- `pimEntitlement`'s download step (`pimEntitlement.cxx:6058`) — confirmed
  full lifecycle: `Download()` in a busy-retry loop (`:6072-6088`), then a
  progress-polling loop that reads `Downloader.GetErrors(pimLoop::Errors)`
  each iteration to detect a download failure early (`:6093-6105`), with the
  same cancel-then-`KILL_COUNT`-grace-period-then-`Kill()` idiom
  (`:6112-6128`) documented for `pimShortcuts`/`pimServices`.
- `pimTestURLDownload()` (`pim/pim_src/pimTop.cxx:3146`) — a standalone
  diagnostic/utility entry point (`Test the URL download` per its own header
  comment, `pimTop.cxx:3138`) that also calls `pimDownloader::GetInstance()`.
  This makes `pimDownloader` the **only one of the 7 wrapper classes with a
  confirmed second caller outside `pimEntitlement.cxx`** — every other
  wrapper's `GetInstance()` is called exclusively from within
  `pimEntitlement.cxx` in this archive.

## Calls Into

- `pimDownloadLoop` (constructs, `SetEntitlement()`s, calls its custom
  `Execute(const btkFSEntry&)` override, `Kill()`s, queries `GetStatus()`/
  `HasErrors()`/`IsPaused()`/`IsDone()`/`GetErrors()`/`Wait()` on it — not
  `Cancel()`).

## Lifetime

Singleton constructed on first `GetInstance()` call (from either
`pimEntitlement`'s download step or `pimTestURLDownload()`, whichever runs
first), lives for process lifetime. `DownloadLoop` is transient per
operation; cleaned up by `Status()` on completion or by `Kill()`.

## Ownership Model

Free-standing process-wide singleton; no owner object. Owns one
`pimDownloadLoop` instance at a time for the duration of one download
operation. Unlike `pimServices` (see `docs/classes/pimServices.md`), this
class's `Download()` does **not** release `xmlMutex` early — the lock is
correctly held for the full operation, consistent with `pimCopier`/
`pimShortcuts`/`pimRegEdit`.

## Thread Safety

- `xmlMutex.TryLock()`/full-duration-hold pattern verified correct (unlike
  `pimServices`) — `Download()`'s `xmlMutex.Unlock()` at line 152 only
  occurs inside `Status()`'s `IsDone()` branch, not inline in `Download()`
  itself.
- Same flag-only `Cancel()` gap as `pimShortcuts`/`pimRegEdit`/`pimServices`,
  compensated at the confirmed `pimEntitlement` call site via the same
  grace-period-then-`Kill()` idiom.
- The module-level free functions in this file (`pimSetTimeouts()`/
  `pimGetTimeouts()`) are explicitly documented by their own source comment
  as safe only because they are "called from the main thread only"
  (`pimDownloader.cxx:51-52`) — this is a comment-enforced, not
  compiler/mutex-enforced, invariant; nothing prevents a future caller on a
  worker thread from violating it.
- `pimTestURLDownload()` being a second, independent `GetInstance()` caller
  means it is technically possible for that diagnostic entry point and a
  live `pimEntitlement` download step to race for the same singleton if both
  were ever invoked in the same process at once — the `TryLock()` gate would
  correctly serialize them (return `false` to whichever loses the race,
  same as any two `pimEntitlement` download steps would), so this is a
  contention/UX risk (one operation silently waits/fails-to-start) rather
  than a correctness risk, unlike the `pimServices` finding.

## Extension Points

- Any new caller needing real download progress should use this class's
  `Status()` directly (it is one of only 2 of the 7 wrappers — with
  `pimMSICopier` — whose `Status()` genuinely reports usable progress data).
- The HTTP(S) transport free functions in this same file
  (`pimSendNRecvFromPTC`/`pimRecvFromPTC` and their `_low` counterparts)
  were not re-traced to full depth in this pass; a future extension of this
  documentation set could give them their own treatment (they are
  substantial — the bulk of this file's 1263 lines) rather than folding them
  into this wrapper-class doc.

## Risk Analysis

- No use-after-free-class finding here (contrast `pimServices`) — the
  locking discipline in the wrapper-class portion of this file is sound.
- The "main thread only" comment-enforced invariant on
  `pimSetTimeouts()`/`pimGetTimeouts()` is a latent risk if any future
  caller invokes them from a background thread — no mutex protects the
  underlying `connect_response_timeout`/`stream_read_timeout` statics
  (`pimDownloader.cxx:48-49`).
- Dead members `pctMutex`/`InMemoryXML` (in the wrapper-class scope), same
  pattern as its siblings.
- `pimTestURLDownload()` existing as a second caller is a reminder that
  "sole caller is `pimEntitlement`" (stated elsewhere in this documentation
  set as a general pattern for the 7 wrappers) is not an absolute guarantee
  for every one of them — `pimDownloader` is the confirmed exception.

## Usage Example

```cpp
// Traced from pimEntitlement's download step, pimEntitlement.cxx:6052-6141
pimDownloader& Downloader = pimDownloader::GetInstance();
int dl_ret = false;
while (dl_ret == false)
{
    dl_ret = Downloader.Download(xmlPtr, *this, path); // *this: the calling pimEntitlement
    if (!dl_ret) { Sleep(1); /* check cancel */ }
}
while ((stat = Downloader.Status(pct, remaining)) == pimDownloader::InProgess)
{
    if (Downloader.GetErrors(pimLoop::Errors)) { /* surface download error, return */ }
    Sleep(1);
    if (IsCancelFlagSet())
    {
        int ct = KILL_COUNT;
        Downloader.Cancel(); // flag only
        while (((stat = Downloader.Status(pct, remaining)) == pimDownloader::CancelFlagSet) && ct > 0)
        { Sleep(1); ct--; }
        if ((stat = Downloader.Status(pct, remaining)) == pimDownloader::CancelFlagSet)
            Downloader.Kill();
        return;
    }
}
```
