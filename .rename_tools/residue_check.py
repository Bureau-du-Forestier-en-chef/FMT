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

# An exposed R/Python name legitimately KEEPS the old spelling -- that is the whole point of
# the freeze. In live code it sits in a string literal and is skipped anyway, but inside a
# commented-out binding rename.py sees editable prose, so restore_exposed.py puts the old
# name back there too. Blank those positions out before scanning, otherwise every frozen
# name reads as a residue.
EXPOSED = re.compile(r'((?:class_<|derives<)[^)]*>\s*\(\s*")(FMT[A-Za-z0-9_]+)(")')


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
                t = EXPOSED.sub(lambda m: m.group(1) + '_' * len(m.group(2)) + m.group(3), t)
                rel = os.path.relpath(p, ROOT).replace(os.sep, '/')
                for seg, editable in rename.split_code_strings(t):
                    if not editable and not rename.string_is_editable(seg):
                        continue
                    for m in pat.finditer(seg):
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
