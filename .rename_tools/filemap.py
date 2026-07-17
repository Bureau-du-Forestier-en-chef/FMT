#!/usr/bin/env python3
"""Build the file-rename plan for batch 6, and report the FMT* files it leaves alone.

Usage: python .rename_tools/filemap.py
  writes .rename_tools/filemap.json  {"old/path": "new/path", ...}

Driven by BASENAME, not by content: headers are not one-class-per-file (FMToutputnode.h
declares 4, FMTbounds.hpp declares 6), so the class->file link only holds for the file
whose name IS the class.
"""
import os, json, subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)

# Files whose class was already renamed before this refactor, so classmap.json -- which
# only holds the 137 classes WE renamed -- cannot know about them.
EXTRA = {
    'FMTspatialschedule': 'FMTSpatialSchedule',   # renamed in 7623a46c, files never followed
    'FMtVirtualLineGraph': 'FMTVirtualLineGraph',  # typo in the filename: FMt
    'FMTopratingareaclusterbinary': 'FMTOperatingAreaClusterBinary',  # typo: "oprating"
}


def main():
    mapping = dict(json.load(open(os.path.join(HERE, 'classmap.json'))))
    mapping.update(EXTRA)
    files = subprocess.run(['git', 'ls-files'], cwd=ROOT,
                           capture_output=True, text=True).stdout.split()
    plan, keep = {}, []
    for f in files:
        d, b = os.path.split(f)
        stem, ext = os.path.splitext(b)
        if not stem.startswith(('FMT', 'FMt')) or ext.lower() not in ('.h', '.hpp', '.cpp'):
            continue
        if stem in mapping:
            plan[f] = '/'.join([d, mapping[stem] + ext]) if d else mapping[stem] + ext
        else:
            keep.append(f)
    json.dump(plan, open(os.path.join(HERE, 'filemap.json'), 'w'), indent=1, sort_keys=True)
    print('a renommer : %d fichiers' % len(plan))
    print('conserves  : %d fichiers' % len(keep))
    for f in sorted(keep):
        print('   ' + f)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
