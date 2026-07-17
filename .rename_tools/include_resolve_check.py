#!/usr/bin/env python3
"""Every #include "FMT*.h" must resolve to a file that exists, CASE-EXACTLY.

Usage: python .rename_tools/include_resolve_check.py

This is the check the Windows build cannot perform: NTFS is case-insensitive, so
#include "FMTSpatialSchedule.h" resolves happily against FMTspatialschedule.h and the
compiler stays silent -- which is exactly how the 5 broken includes survived until now.
Resolution here is done against the git index, which IS case-sensitive.
"""
import os, re, sys, subprocess, collections

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
ROOTS = ['Include', 'Source', 'FMTWrapperCore', 'UI', 'Excel', 'Examples', 'Templates', 'Modules']
INC = re.compile(r'#\s*include\s*"([^"]+)"')


def main():
    tracked = subprocess.run(['git', 'ls-files'], cwd=ROOT,
                             capture_output=True, text=True).stdout.split()
    by_name = collections.defaultdict(set)
    for f in tracked:
        by_name[os.path.basename(f)].add(f)
    lower = {}
    for name in by_name:
        lower.setdefault(name.lower(), []).append(name)

    bad = []
    for root in ROOTS:
        base = os.path.join(ROOT, root)
        if not os.path.isdir(base):
            continue
        for dp, dn, fn in os.walk(base):
            if 'build' in dp.split(os.sep):
                continue
            for f in fn:
                if not f.lower().endswith(('.h', '.hpp', '.cpp', '.cxx', '.cc')):
                    continue
                p = os.path.join(dp, f)
                for i, line in enumerate(open(p, encoding='latin-1', newline=''), 1):
                    m = INC.match(line.strip())
                    if not m:
                        continue
                    inc = os.path.basename(m.group(1))
                    if not inc.startswith(('FMT', 'FMt')):
                        continue
                    if inc in by_name:
                        continue
                    other = lower.get(inc.lower())
                    why = ('casse: le fichier reel est %s' % other[0]) if other else 'fichier inexistant'
                    bad.append('%s:%d  #include "%s"  -> %s'
                               % (os.path.relpath(p, ROOT), i, m.group(1), why))
    if bad:
        print('FAIL: %d include(s) ne resolvent pas :' % len(bad))
        for b in bad:
            print('   ' + b)
        return 1
    print('includes OK: tous les #include FMT* resolvent, casse exacte')
    return 0


if __name__ == '__main__':
    sys.exit(main())
