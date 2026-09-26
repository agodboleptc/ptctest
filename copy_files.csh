#!/bin/csh -f
#
# copy_files.csh
#
# Copy files listed in a text file from a source base directory to the
# current directory, recreating each file's directory structure here.
#
# Usage:
#   ./copy_files.csh <list_file> <source_base_dir>
#
# Example:
#   ./copy_files.csh files.txt /net/j/q27/spg/system_1
#
# The list file must contain one RELATIVE path per line, e.g.:
#   proe/coretools/ctsyscall/pro_sys_info.c
#   libs/ui/intinc/uint.h
#

# ---- argument checking ---------------------------------------------------
if ($#argv != 2) then
    echo "Usage: $0 <list_file> <source_base_dir>"
    exit 1
endif

set listfile = "$1"
set srcbase  = "$2"

if (! -f "$listfile") then
    echo "Error: list file '$listfile' not found."
    exit 1
endif

if (! -d "$srcbase") then
    echo "Error: source base directory '$srcbase' not found."
    exit 1
endif

# ---- counters ------------------------------------------------------------
set ok   = 0
set fail = 0

# ---- process each listed file -------------------------------------------
foreach rel ("`cat '$listfile'`")

    # Skip blank lines
    if ("$rel" == "") continue

    # Normalize Windows-style backslashes to forward slashes
    set rel = `echo "$rel" | sed 's/\\\\/\//g'`

    set src = "${srcbase}/${rel}"
    set dst = "./${rel}"
    set dstdir = "$dst:h"

    if (! -f "$src") then
        echo "MISSING: $src"
        @ fail++
        continue
    endif

    # Create the destination directory structure
    if (! -d "$dstdir") then
        mkdir -p "$dstdir"
        if ($status != 0) then
            echo "MKDIR FAILED: $dstdir"
            @ fail++
            continue
        endif
    endif

    # Copy, preserving metadata
    cp -p "$src" "$dst"
    if ($status == 0) then
        echo "COPIED:  $rel"
        @ ok++
    else
        echo "COPY FAILED: $src -> $dst"
        @ fail++
    endif

end

# ---- summary -------------------------------------------------------------
echo ""
echo "Done. Copied: $ok   Failed/Missing: $fail"
exit 0
