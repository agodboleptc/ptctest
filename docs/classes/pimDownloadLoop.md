# Class: `pimDownloadLoop`

**File:** `pim_core/includes/pimDownloadLoop.h` (97 lines) / `pim_core/pim_core_src/pimDownloadLoop.cxx` (940 lines)
**Module:** `pim_core`
**Inherits:** `pimLoop`

## Purpose

Install-step class that downloads a product's payload files (external media
pieces, MSI packages, SFX packages, and plain `<CDSECTION>` archive files) from a
network source into a local cache directory before the copy/MSI/SFX steps consume
them.

## Responsibilities

- Compute total download size/unit-count up front (`CalculateDownloadSize`) by
  scanning `<EXTERNAL_MEDIA>`, `<MSI>`, `<SFX>`, and `<CDSECTION>` elements, so
  progress can be reported either by cumulative byte size or by file count
  (whichever is available).
- Download each payload category **sequentially, one category fully before the
  next** (`OnExecute`): external media, then MSI, then SFX, then CDSECTION items.
- Track and expose progress (`GetStatus`) as a percent-complete plus an estimated
  time remaining, computed from elapsed time vs. remaining download size/units.
- Write each download to a `.part` temporary file before finalizing it (evidenced
  by the `part` variable construction in `DownloadOne`), consistent with a
  resumable/atomic-download pattern (the actual finalization/rename logic and any
  resume behavior were not traced further in this pass).
- Nominally verify a downloaded `.exe`/`.cab` file's digital signature via
  `TestDigitialSignature` — **see Risk Analysis: this check does not actually
  enforce anything in the current build.**

## Dependencies

- `pimLoop` (base class).
- `pimEntitlement` (`ePtr` member, set via `SetEntitlement`) — this Loop is
  explicitly bound to the entitlement it's downloading for, unlike most other Loop
  types which only receive a `pimXmlFile*`.
- `pimDownloadData` (a small multi-buffer byte-accumulator class, up to 10
  fixed-size chunks, with `Append`/`Write`/`Size`/`DidTimeout`/`Raw` — used to
  receive streamed HTTP response data).
- `pimIsDigitiallySignedByPTC` (external — not traced to its defining file in
  this pass; presumably `pim_util` or `btk`).
- `pimSendNRecvFromPTC`/`pimRecvFromPTC` (declared alongside this class in
  `pimDownloadLoop.h`, implemented elsewhere in `pim_core` — the actual
  HTTP/browser-stream transport).

## Members

| Member | Type | Access | Purpose |
|---|---|---|---|
| `CacheDir` | `btkFSEntry` | private | Local destination directory for downloads, set via the overridden `Execute(const btkFSEntry&)` |
| `MediaID` | `btkString` | private | This entitlement's `[MEDIAID]` property, read once at the start of `OnExecute` |
| `Spent`, `StartInterval` | `btkTimeval` | private | Per-download timing, used for the time-remaining estimate |
| `OverallStartTime` | `btkTimeval` | private | Set once at the start of `OnExecute`, for overall elapsed-time tracking |
| `DownloadUnits`, `RemainingDownloadUnits` | `double` | private | File-count-based progress fallback |
| `DownloadSize`, `RemainingDownloadSize` | `double` | private | Byte-size-based progress (preferred when available) |
| `ePtr` | `pimEntitlement*` | private | The entitlement this download serves; not owned |
| `data_in_progress` | `pimDownloadData*` | private | The in-flight download buffer, if any |

## Public APIs

| Method | Purpose |
|---|---|
| `pimDownloadLoop(pimXmlFile*)` | Ctor; zeroes all size/unit counters, `ePtr = NULL` |
| `SetEntitlement(pimEntitlement&)` | Binds this Loop to the entitlement it downloads for |
| `Execute(const btkFSEntry &Where)` | **Overloads (does not override) `pimLoop::Execute()`** — see Risk Analysis. Directly manipulates the inherited `mCancel`/`mDone`/`mPause` flags under `Mutex`, sets `CacheDir`, then calls `thrThread::Execute(thrAttached)` directly, **bypassing `pimLoop::Execute()`'s own logic** (which additionally clears the `Errors`/`Warnings` buffers) |
| `GetStatus(int &PercentComplete, btkTimeval &TimeRemaining)` | Reports current download progress |

## Protected APIs

| Method | Purpose |
|---|---|
| `GetNextExternalMedia` / `GetNextMSI` / `GetNextSFX` / `GetNextCDSection` | Extract the from/to paths and declared size for the Nth element of each respective category |
| `CalculateDownloadSize()` | Pre-scans all 4 categories to total `DownloadUnits`/`DownloadSize` before downloading begins |
| `DownloadOne(WhereFrom, WhereTo, DownloadSize)` | Performs one file's download: creates the destination directory if needed, downloads to a `.part` file, and (per the surrounding `OnExecute` logic) returns `1` on success, a negative value on unrecoverable error/timeout, presumably `0` for a retry-eligible condition (exact return-code contract not fully enumerated in this pass) |
| `TestDigitialSignature(btkFSEntry&)` | See Risk Analysis — checks `.exe`/`.cab` files against `pimIsDigitiallySignedByPTC`, but **always returns `true`** |
| `OnExecute()` | Thread entry point: computes download size, then processes External Media, MSI, SFX, and CDSECTION categories in that fixed order, checking cancel/pause and updating `RemainingDownloadUnits` between each item |
| `OnTerminate()` | Standard thread teardown |

## Private Utilities

None beyond the protected methods above.

## Called By

`pimDownloader::Download()` (see Ownership Model below).

## Calls Into

`pimSendNRecvFromPTC`/`pimRecvFromPTC` (network transport), `pimIsDigitiallySignedByPTC` (external), `pimXmlFile` (reading the 4 payload-category element lists), `pimDownloadData` (buffering).

## Lifetime

Created on demand by `pimDownloader`, lives for one entitlement's full download
operation (all 4 categories).

## Ownership Model

**Correction relative to this documentation set's earlier assumption**: `pimDownloader`
(`pim_core/includes/pimDownloader.h`) is a **process-wide singleton**
(`static pimDownloader *OnlyDownloader`, `GetInstance()`), following the same
pattern confirmed against `pimCopier.cxx` — not per-entitlement as originally
documented. `Download(pimXmlFile*, pimEntitlement&, const btkFSEntry&)` uses the
same `TryLock`-based `1`/`0`/negative contract as the other 6 singleton wrappers,
meaning **only one entitlement's download can be actively in flight across the
entire process at a time** — every other entitlement waiting to download must
poll until the singleton is free.

## Thread Safety

Inherits `pimLoop`'s mutex-guarded lifecycle flags. Notably, its own `Execute(const
btkFSEntry&)` override manipulates `mCancel`/`mDone`/`mPause` directly under
`Mutex` rather than calling `pimLoop::Execute()` — functionally similar, but see
Risk Analysis for the one behavioral difference this introduces.

## Extension Points

- New payload category to download: add a `GetNextXxx` extractor and a
  corresponding loop section in both `CalculateDownloadSize()` and `OnExecute()`,
  following the existing 4-category pattern.
- Enabling real signature enforcement: `TestDigitialSignature` already computes
  `non_ptc_signer`/`signatory` — wiring its result into an actual pass/fail return
  (and calling sites that currently ignore the return value, if any exist beyond
  what was traced here) would be the natural extension point.

## Risk Analysis

- **`TestDigitialSignature` is a confirmed no-op for enforcement purposes.** It
  calls `pimIsDigitiallySignedByPTC(file, non_ptc_signer, signatory)`, but:
  1. Even on a failed signature check, the only action taken is a debug log line,
     and that logging is itself gated behind `#if ERROR_ON_UNSIGNED`.
  2. **`ERROR_ON_UNSIGNED` is not `#define`d anywhere in this archive** — grepped
     across every header and source file. Since it's tested with `#if` (not
     `#ifdef`), an undefined macro evaluates to `0`, meaning this logging block is
     compiled out entirely in the traced configuration.
  3. The function **unconditionally `return true;`** regardless of the signature
     check's outcome.
  Net effect: downloaded `.exe`/`.cab` files are not actually blocked or flagged
  for failing PTC-signature verification in this build, despite a function
  existing that looks like it performs that check. If `ERROR_ON_UNSIGNED` is
  supplied by the real build system, this conclusion would need revisiting — but
  even then, the function still always returns `true`, so no caller can act on a
  failed check regardless. This is worth flagging to whoever owns supply-chain
  integrity for downloaded installer payloads.
- **`Execute(const btkFSEntry&)` bypasses `pimLoop::Execute()`** — it does not
  clear the `Errors`/`Warnings` buffers the way the base class's `Execute()`
  does. If a `pimDownloadLoop` instance is ever reused for a second download
  (rather than being freshly constructed each time, which is the pattern observed
  at its call site), stale error/warning text from a prior run could persist and
  be misattributed to the new run. Not confirmed as an actual bug given the
  observed one-shot construction pattern, but a latent risk if that pattern ever
  changes.
- **Download error/timeout handling is explicitly marked incomplete in the
  source itself** — `OnExecute()`'s per-category loops contain a literal
  `// TODO handle TIMEOUT/ERRORS` comment at the point where a negative
  `DownloadOne` return simply causes the whole download to abort (`return;`) with
  no retry, partial-resume, or distinct error reporting beyond that.
- **Sequential, non-parallel category processing** — all external-media items
  download fully before any MSI item starts, and so on. A single slow/stalled
  download in an earlier category delays every later category, even if they're
  otherwise independent.

## Usage Example (as evidenced by call sites)

```cxx
// pattern inside pimEntitlement's download-driving code (pimDownloader owner)
pimDownloader &Dl = pimDownloader::GetInstance();
int ret = Dl.Download(xmlPtr, *this, cacheDir);   // 0 if another download is in progress process-wide
if (ret == 1) {
    int pct; btkTimeval remaining;
    pimDownloader::DownloadStatus st;
    do {
        st = Dl.Status(pct, remaining);
        Sleep(200);
    } while (st == pimDownloader::InProgess);
}
```

---
*Extends Phase 5 of the requested 20-phase documentation set (remaining Loop subclasses, done at the same depth as `pimCopyLoop`).*
