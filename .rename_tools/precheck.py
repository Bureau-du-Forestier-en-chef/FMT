#!/usr/bin/env python3
"""Blocking pre-check for the class rename. Run before every batch.

Usage: python .rename_tools/precheck.py [batch.json]
  no arg -> checks the full classmap.json

Fails on:
  1. two old names mapping to the same new name
  2. a new name colliding with a type that already exists under that spelling
  3. a batch key absent from the master classmap (typo in the batch file)
"""
import os, re, sys, json, collections

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
ROOTS = ['Include', 'Source', 'FMTWrapperCore', 'UI', 'Excel']
DECL = re.compile(r'(?:^|\n)[ \t]*(?:(?:public|private)[ \t]+)?(?:ref[ \t]+)?(?:class|struct)'
                  r'[ \t]+(?:FMTEXPORT[ \t]+)?(FMT[A-Za-z0-9_]+)[ \t]*(?![A-Za-z0-9_(])')


def existing_types():
    found = set()
    for root in ROOTS:
        base = os.path.join(ROOT, root)
        if not os.path.isdir(base):
            continue
        for dp, dn, fn in os.walk(base):
            if 'build' in dp.split(os.sep):
                continue
            for f in fn:
                if f.lower().endswith(('.h', '.hpp', '.cpp', '.cxx', '.cc')):
                    t = open(os.path.join(dp, f), encoding='latin-1', newline='').read()
                    found |= {m.group(1) for m in DECL.finditer(t)}
    return found


def main():
    master = json.load(open(os.path.join(HERE, 'classmap.json')))
    mapping = master
    if len(sys.argv) > 1:
        batch = json.load(open(sys.argv[1]))
        unknown = [k for k in batch if master.get(k) != batch[k]]
        if unknown:
            print('FAIL: batch entries not matching classmap.json: %s' % unknown)
            return 1
        mapping = batch

    rc = 0
    rev = collections.defaultdict(list)
    for k, v in mapping.items():
        rev[v].append(k)
    dupes = {v: ks for v, ks in rev.items() if len(ks) > 1}
    if dupes:
        print('FAIL: several old names collapse onto one new name: %s' % dupes)
        rc = 1

    existing = existing_types()
    # An already-applied rename looks like a collision (old name gone, new name present).
    # Separate the two so the check stays re-runnable after a batch has landed.
    applied = {k: v for k, v in mapping.items() if k not in existing and v in existing}
    pending = {k: v for k, v in mapping.items() if k not in applied}

    clash = sorted(set(pending.values()) & (existing - set(pending)))
    if clash:
        print('FAIL: new names already taken by an existing type: %s' % clash)
        rc = 1

    if rc == 0:
        print('precheck OK: %d pending, %d already applied, no collision'
              % (len(pending), len(applied)))
    return rc


if __name__ == '__main__':
    sys.exit(main())
