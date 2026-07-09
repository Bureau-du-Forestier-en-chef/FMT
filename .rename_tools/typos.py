import re, glob, json, os

prot = set(json.load(open('.rename_tools/prot.json')))

# misspelled fragment -> correct fragment (case-insensitive match, case preserved)
TYPOS = {
    'neihgbor': 'neighbor',
    'binairies': 'binaries',
    'spatialy': 'spatially',
    'feasability': 'feasibility',
    'refactortorization': 'refactorization',
    'verticies': 'vertices',
    'specificier': 'specifier',
    'developement': 'development',
    'dispertion': 'dispersion',
}

ROOTS = ['Include', 'Source', 'FMTWrapperCore']
EXTS = ('.h', '.hpp', '.cpp')

# gather all identifier tokens
idents = set()
for r in ROOTS:
    for dp, dn, fn in os.walk(r):
        if 'build' in dp.split(os.sep):
            continue
        for f in fn:
            if not f.lower().endswith(EXTS):
                continue
            t = open(os.path.join(dp, f), encoding='latin-1').read()
            for m in re.findall(r'[A-Za-z_]\w*', t):
                idents.add(m)


def fix_token(tok, bad, good):
    # replace bad fragment (any case) with good, matching the case of bad's first letter
    def repl(m):
        s = m.group(0)
        return good.capitalize() if s[0].isupper() else good
    return re.sub(bad, repl, tok, flags=re.IGNORECASE)


groups = {}
for bad, good in TYPOS.items():
    mp = {}
    for tok in idents:
        if re.search(bad, tok, flags=re.IGNORECASE):
            new = fix_token(tok, bad, good)
            if new != tok and tok not in prot:
                mp[tok] = new
    groups[bad] = mp

json.dump(groups, open('.rename_tools/typo_groups.json', 'w'), indent=1, sort_keys=True)
for bad, mp in groups.items():
    prot_hits = [t for t in idents if re.search(bad, t, flags=re.IGNORECASE) and t in prot]
    print("=== %s -> %s  (%d identifiers%s) ===" % (
        bad, TYPOS[bad], len(mp), ", %d protected/UI skipped" % len(prot_hits) if prot_hits else ""))
    for k in sorted(mp):
        print("   %-32s %s" % (k, mp[k]))
