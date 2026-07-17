#!/usr/bin/env python3
"""Restore R/Python exposed class names that leaked inside commented-out bindings.

Usage: python .rename_tools/restore_exposed.py [--apply]

rename.py freezes string literals, but treats comments as editable prose by design (doc
comments must follow the rename, and an apostrophe in a comment must not swallow real
code). So a binding that is entirely commented out has its exposed name renamed too.

Harmless at runtime -- it is dead code -- but if anyone uncomments it, the exposed name
must be the frozen one. This walks the bindings, finds any exposed name that matches a NEW
class name, and puts the old spelling back. The C++ type stays renamed.
"""
import os, re, sys, json, glob

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
PREEXISTING = {'FMTSpatialSchedule', 'FMTSerie', 'FMTGCBMtransition'}
EXPOSED = re.compile(r'((?:class_<|derives<)[^)]*>\s*\(\s*")(FMT[A-Za-z0-9_]+)(")')


def main():
    apply = '--apply' in sys.argv
    mapping = json.load(open(os.path.join(HERE, 'classmap.json')))
    rev = {v: k for k, v in mapping.items()}
    total = 0
    for f in sorted(glob.glob(os.path.join(ROOT, 'Include', 'Rexport*.h')) +
                    glob.glob(os.path.join(ROOT, 'Include', 'PYexport*.h'))):
        t = open(f, encoding='latin-1', newline='').read()
        hits = []

        def sub(m):
            name = m.group(2)
            if name in PREEXISTING or name not in rev:
                return m.group(0)
            hits.append('%s -> %s' % (name, rev[name]))
            return m.group(1) + rev[name] + m.group(3)

        new = EXPOSED.sub(sub, t)
        if hits:
            total += len(hits)
            print('%s' % os.path.relpath(f, ROOT))
            for h in hits:
                print('    ' + h)
            if apply:
                open(f, 'w', encoding='latin-1', newline='').write(new)
    print('--- %d nom(s) expose(s) restaure(s). %s' % (total, 'APPLIQUE' if apply else 'DRY-RUN'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
