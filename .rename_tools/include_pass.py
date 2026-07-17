#!/usr/bin/env python3
"""Point every #include at the renamed header. Usage: include_pass.py [--apply]

Include paths live in string literals, which rename.py freezes on purpose (they sit next
to the exposed R/Python names it must not touch). So they get their own pass, driven by
filemap.json -- the same source of truth as the git mv.

Handles .h AND .hpp: FMTbounds.hpp, FMTgraph.hpp, FMTlayer.hpp, FMTlist.hpp, FMTspec.hpp.
Reads/writes latin-1 with newline='' like rename.py, to preserve cp1252 accents and CRLF.
"""
import os, re, sys, json

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
ROOTS = ['Include', 'Source', 'FMTWrapperCore', 'UI', 'Excel', 'Examples', 'Templates', 'Modules']
EXTS = ('.h', '.hpp', '.cpp', '.cxx', '.cc')


def main():
    apply = '--apply' in sys.argv
    plan = json.load(open(os.path.join(HERE, 'filemap.json')))
    # basename stem+ext -> new basename, e.g. "FMTaction.h" -> "FMTAction.h"
    names = {}
    for old, new in plan.items():
        names[os.path.basename(old)] = os.path.basename(new)
    pat = re.compile(r'(#\s*include\s*")(' +
                     '|'.join(re.escape(k) for k in sorted(names, key=len, reverse=True)) +
                     r')(")')
    total, nfiles = 0, 0
    for root in ROOTS:
        base = os.path.join(ROOT, root)
        if not os.path.isdir(base):
            continue
        for dp, dn, fn in os.walk(base):
            if 'build' in dp.split(os.sep):
                continue
            for f in fn:
                if not f.lower().endswith(EXTS):
                    continue
                p = os.path.join(dp, f)
                t = open(p, encoding='latin-1', newline='').read()
                n, cnt = pat.subn(lambda m: m.group(1) + names[m.group(2)] + m.group(3), t)
                if cnt:
                    total += cnt
                    nfiles += 1
                    print('%5d  %s' % (cnt, os.path.relpath(p, ROOT)))
                    if apply:
                        open(p, 'w', encoding='latin-1', newline='').write(n)
    print('--- %d includes across %d files. %s' % (total, nfiles, 'APPLIQUE' if apply else 'DRY-RUN'))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
