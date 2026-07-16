#!/usr/bin/env python3
"""Report @DocString pairing health. Run before/after every batch.

Documentation/commentsPythonandR.py pairs the "@DocString(K)" string in the R/Python
bindings with the "// DocString: K" marker in the headers. The class rename edits the
comment markers, so the strings must follow -- hence the DOCSTRING_KEY allowlist in
rename.py.

The pairing is ALREADY broken for methods (the camelCase pass renamed the markers but not
the strings). This script is a non-regression guard: the orphan count must not grow.
"""
import os, re, glob, sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
KEY = re.compile(r'"@DocString\(\s*(.+?)\)?"')
MARK = re.compile(r'^\s*// DocString:(.+)')


def main():
    keys = set()
    for f in glob.glob(os.path.join(ROOT, 'Include', 'Rexport*.h')) + \
             glob.glob(os.path.join(ROOT, 'Include', 'PYexport*.h')):
        for line in open(f, encoding='latin-1', newline=''):
            keys |= {m.group(1).strip() for m in KEY.finditer(line)}

    marks = set()
    for root in ['Include', 'Source', 'FMTWrapperCore']:
        for dp, dn, fn in os.walk(os.path.join(ROOT, root)):
            if 'build' in dp.split(os.sep):
                continue
            for f in fn:
                if f.lower().endswith(('.h', '.hpp', '.cpp')):
                    for line in open(os.path.join(dp, f), encoding='latin-1', newline=''):
                        m = MARK.match(line)
                        if m:
                            marks.add(m.group(1).strip())

    orphans = sorted(k for k in keys if k not in marks)
    print('keys=%d markers=%d orphans=%d' % (len(keys), len(marks), len(orphans)))
    if len(sys.argv) > 1:
        baseline = int(sys.argv[1])
        if len(orphans) > baseline:
            print('FAIL: orphans grew from %d to %d' % (baseline, len(orphans)))
            for o in orphans[:20]:
                print('   ' + o)
            return 1
        print('OK: orphans %d <= baseline %d' % (len(orphans), baseline))
    return 0


if __name__ == '__main__':
    sys.exit(main())
