#!/usr/bin/env python3
"""Assert the R/Python exposed class names stayed frozen. Run after every batch.

The exposed name is the string argument of class_<>() / bp::class_<>() / .derives<>().
Renaming it breaks client scripts, and for .derives<> it breaks R inheritance with NO
compile error -- so this check, not the compiler, is what guards it.

rename.py freezes string literals, but it treats comments as editable prose (by design:
doc comments must follow the rename, and an apostrophe in a comment must not swallow real
code). So exposed names inside COMMENTED-OUT binding code do leak, and get restored by
hand. This check catches that.

PREEXISTING lists names already exposed in CamelCase before this refactor -- they are
accepted exceptions, not regressions.
"""
import re, sys, glob, os

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
PREEXISTING = {'FMTSpatialSchedule', 'FMTSerie', 'FMTGCBMtransition'}
EXPOSED = re.compile(r'(?:class_<|derives<)[^)]*>\s*\(\s*"(FMT[A-Za-z0-9_]+)"')


def main():
    bad = []
    for f in sorted(glob.glob(os.path.join(ROOT, 'Include', 'Rexport*.h')) +
                    glob.glob(os.path.join(ROOT, 'Include', 'PYexport*.h'))):
        for i, line in enumerate(open(f, encoding='latin-1', newline=''), 1):
            for m in EXPOSED.finditer(line):
                name = m.group(1)
                if name in PREEXISTING:
                    continue
                # Frozen spelling is all-lowercase after the FMT prefix.
                if not name[3:].islower():
                    bad.append('%s:%d  %s' % (os.path.relpath(f, ROOT), i, name))
    if bad:
        print('FAIL: exposed R/Python names no longer frozen (%d):' % len(bad))
        for b in bad:
            print('   ' + b)
        return 1
    print('freeze OK: every exposed class name still lowercase')
    return 0


if __name__ == '__main__':
    sys.exit(main())
