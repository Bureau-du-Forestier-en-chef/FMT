#!/usr/bin/env python3
"""Propose the FMTlowercase -> FMTCamelCase class mapping.

Usage: python .rename_tools/propose_classes.py
  writes .rename_tools/classmap.json

WARNING -- run this ONCE, before the first batch. It derives the map by scanning the tree,
so re-running it after a batch has landed silently DROPS every class already renamed (the
old spelling is gone from the tree). classmap.json must stay complete: batch 6 needs it to
rename the files. To change one entry after the fact, patch classmap.json directly instead
of regenerating.

Splitting is longest-match left-to-right over an ATOMIC word list, and any name whose
remainder is not fully covered is a HARD FAILURE reported for manual handling. A greedy
splitter with compound words in the list silently mis-splits ("modelparameters" ->
model|par|ameters), which is worse than not splitting at all.

Acronym policy comes from Documentation/camelCase_inventory.md: acronyms in Pascal, except
FMT and GCBM which stay uppercase.
"""
import os, re, json, collections

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SCAN_ROOTS = ['Include', 'Source', 'FMTWrapperCore']
# Classes declared in UI/ and Excel/ are out of scope (see prot_classes.json).
EXCLUDE_DECL_ROOTS = ['UI', 'Excel']

WORDS = """action parser actual development comparator age bounds yield handler area base
edge properties vertex binding spatial complex constant constants constraint coordinate
data debug exception logger default path error event container iterator sorter relation
exponential schedule expression forest fork free function call futur graph stats to
landscape layer lifespan lifespans line list lock lookup heuristic model solver mask
filter matrix build parameters move object scheduler task operating cluster clusterer
binary scheme operator optimization output node cache hasher origin value source parallel
writer per planning predictor quiet replanning solution serializable spec nodes theme time
transition version warning yields request decision tree pools""".split()

# Acronyms: Pascal-cased per the documented rule, except FMT/GCBM which stay uppercase.
ACRONYMS = {
    'lp': 'Lp', 'sa': 'Sa', 'se': 'Se', 'ses': 'Ses', 'sr': 'Sr', 'nss': 'Nss',
    'nn': 'Nn', 'nep': 'Nep', 'mt': 'Mt', 'yld': 'Yld', 'gcbm': 'GCBM',
}

# Names the automaton cannot resolve, or where the automaton is wrong. Each is a decision.
OVERRIDE = {
    # 'oparea' reads as "operating area" -> Op + Area. Kept as an override rather than
    # adding 'op' to the atomic table, which would sit under operator/operating/optimization
    # and only fire as a last resort -- not worth the collateral risk for a single class.
    'FMTopareaschedulertask': 'FMTOpAreaSchedulerTask',
    # 'se' here is Structured Exception (SEH, <eh.h>), NOT the 'se' of FMTsemodel.
    'FMTseException': 'FMTSeException',
    # Typo kept on purpose: the camelCase pass fixed typos in methods only, never classes.
    'FMTfuturdevelopment': 'FMTFuturDevelopment',
    # Author already capitalized these; keep them.
    'FMTyieldmodelRandom': 'FMTYieldModelRandom',
    'FMTyieldmodelTSLA': 'FMTYieldModelTSLA',
    'FMTyieldmodelUnitCoverage': 'FMTYieldModelUnitCoverage',
    'FMTGCBMtransition': 'FMTGCBMTransition',
}

# Ghost: no declaration survives, only commented-out code. Must not be mapped.
SKIP = {'FMTspatialaction', 'FMT_pickle_suite'}

TABLE = sorted(set(WORDS) | set(ACRONYMS), key=len, reverse=True)


def split(stem):
    """Longest-match over the atomic table. Returns None on any uncovered remainder."""
    out, i = [], 0
    while i < len(stem):
        for w in TABLE:
            if stem.startswith(w, i):
                out.append(ACRONYMS.get(w, w.capitalize()))
                i += len(w)
                break
        else:
            return None
    return ''.join(out)


def declared_classes():
    """Map class name -> set of roots declaring it."""
    pat = re.compile(r'(?:^|\n)[ \t]*(?:class|struct)[ \t]+(?:FMTEXPORT[ \t]+)?(FMT[A-Za-z0-9_]+)'
                     r'[ \t]*(?![A-Za-z0-9_(])')
    found = collections.defaultdict(set)
    for root in SCAN_ROOTS + EXCLUDE_DECL_ROOTS:
        base = os.path.join(ROOT, root)
        if not os.path.isdir(base):
            continue
        for dp, dn, fn in os.walk(base):
            if 'build' in dp.split(os.sep):
                continue
            for f in fn:
                if not f.lower().endswith(('.h', '.hpp', '.cpp', '.cxx', '.cc')):
                    continue
                t = open(os.path.join(dp, f), encoding='latin-1', newline='').read()
                for m in pat.finditer(t):
                    found[m.group(1)].add(root)
    return found


def main():
    found = declared_classes()
    mapping, failures, skipped = {}, [], []
    for name, roots in sorted(found.items()):
        if name in SKIP:
            continue
        if not roots & set(SCAN_ROOTS):
            skipped.append((name, 'declared only in ' + ','.join(sorted(roots))))
            continue
        if name in OVERRIDE:
            mapping[name] = OVERRIDE[name]
            continue
        stem = name[3:]
        if not stem or not stem[0].islower():
            continue  # already CamelCase
        new = split(stem.lower()) if stem.islower() else None
        if new is None:
            failures.append(name)
            continue
        mapping['FMT' + stem] = 'FMT' + new

    mapping = {k: v for k, v in mapping.items() if k != v}
    json.dump(mapping, open(os.path.join(HERE, 'classmap.json'), 'w'), indent=1, sort_keys=True)

    print('mapped   : %d' % len(mapping))
    print('skipped  : %d' % len(skipped))
    for n, r in skipped:
        print('   SKIP %-32s %s' % (n, r))
    print('FAILURES : %d  (must be resolved by hand, add to OVERRIDE)' % len(failures))
    for n in failures:
        print('   FAIL %s' % n)


if __name__ == '__main__':
    main()
