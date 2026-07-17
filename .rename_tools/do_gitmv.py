#!/usr/bin/env python3
"""Rename the files of batch 6 with git mv. Usage: do_gitmv.py [--apply]

Two-step pivot through a temp name. On this repo core.ignorecase=true, so a case-only
`git mv FMTaction.h FMTAction.h` can silently no-op; the pivot behaves identically on
NTFS, APFS and ext4 (CMakeFMTMSYS2rcran44.sh builds under MSYS2).

Content is untouched here, so similarity stays 100% and git records real renames --
git log --follow and git blame survive.
"""
import os, sys, json, subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)


def git(*args):
    r = subprocess.run(['git'] + list(args), cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        raise SystemExit('git %s failed: %s' % (' '.join(args), r.stderr.strip()))
    return r.stdout


def main():
    apply = '--apply' in sys.argv
    plan = json.load(open(os.path.join(HERE, 'filemap.json')))
    # FMTspatialschedule first: it is the isolated case that already has 5 broken #includes
    # pointing at the CamelCase name, so it validates the mechanics on its own.
    order = sorted(plan, key=lambda k: (0 if 'spatialschedule' in k.lower() else 1, k))
    for i, old in enumerate(order, 1):
        new = plan[old]
        if not apply:
            print('%3d  %s -> %s' % (i, old, new))
            continue
        tmp = old + '.tmprename'
        git('mv', old, tmp)
        git('mv', tmp, new)
    print('--- %d fichiers. %s' % (len(plan), 'APPLIQUE' if apply else 'DRY-RUN'))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
