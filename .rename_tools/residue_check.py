#!/usr/bin/env python3
"""Assert no old class name survives as a CODE token. Run after every batch.

Usage: python .rename_tools/residue_check.py batch.json

A plain grep is useless here: most surviving occurrences are deliberate freezes --
#include paths, boost make_nvp archive tags, and the exposed R/Python names. This reuses
rename.py's own segmentation so it looks exactly where rename.py writes, and nowhere else.
"""
import os, re, sys, json

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rename  # noqa: E402

ROOT = os.path.dirname(HERE)
ROOTS = ['Include', 'Source', 'FMTWrapperCore', 'UI', 'Modules', 'Templates', 'Excel', 'Examples']

# (file, old name) pairs that must KEEP the old spelling. Inside a comment there is no way
# to tell an exposed-name string from a code token -- rename.py treats the whole comment as
# editable prose -- so these commented-out bindings were restored by hand: if anyone ever
# uncomments them, the exposed R/Python name must still be the frozen lowercase one.
ALLOWED = {
    ('Include/PYexportExceptions.h', 'FMTexceptionhandler'),
    ('Include/RexportExceptions.h', 'FMTexceptionhandler'),
    # commented-out binding block for the ghost class FMTspatialaction
    ('Include/RexportSpatial.h', 'FMTaction'),
    # commented-out .derives<> line
    ('Include/RexportHeuristics.h', 'FMTlpsolver'),
}


def main():
    mapping = json.load(open(sys.argv[1]))
    pat = re.compile(r'(?<![A-Za-z0-9_])(' +
                     '|'.join(re.escape(k) for k in sorted(mapping, key=len, reverse=True)) +
                     r')(?![A-Za-z0-9_])')
    hits = []
    for root in ROOTS:
        base = os.path.join(ROOT, root)
        if not os.path.isdir(base):
            continue
        for dp, dn, fn in os.walk(base):
            if 'build' in dp.split(os.sep):
                continue
            for f in fn:
                if not f.lower().endswith(rename.EXTS):
                    continue
                p = os.path.join(dp, f)
                t = open(p, encoding='latin-1', newline='').read()
                for seg, editable in rename.split_code_strings(t):
                    if not editable and not rename.string_is_editable(seg):
                        continue
                    rel = os.path.relpath(p, ROOT).replace(os.sep, '/')
                    for m in pat.finditer(seg):
                        if (rel, m.group(1)) in ALLOWED:
                            continue
                        hits.append('%s  %s' % (rel, m.group(1)))
    if hits:
        print('FAIL: old names still present as code (%d):' % len(hits))
        for h in sorted(set(hits))[:40]:
            print('   ' + h)
        return 1
    print('residue OK: no old class name left in code')
    return 0


if __name__ == '__main__':
    sys.exit(main())
