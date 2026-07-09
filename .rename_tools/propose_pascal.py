import json, re

cand = json.load(open('.rename_tools/cand2.json'))
where = json.load(open('.rename_tools/cand2_where.json'))
prot = set(json.load(open('.rename_tools/prot.json')))

# Files that are pure binding layers: Pascal names appearing ONLY here are
# boost.python/Rcpp local variables / module handles, not FMT methods.
BINDING = re.compile(r'^(PYexport|Rexport|PYdefinitions|Rdefinitions|Rdeclarations)')

# Manual overrides for acronym-leading / special method names.
OVERRIDE = {
    'MIPparameters': 'mipParameters',
    'OGRlayertoRaster': 'ogrLayerToRaster',
    'GetADecision': 'getADecision',
    'GetCleanLinewfor': 'getCleanLinewfor',
    'DoReFactortorization': 'doReFactortorization',
}
# Names to skip entirely (not real FMT methods, or externally-bound).
SKIP = set()

def camel(name):
    if name in OVERRIDE:
        return OVERRIDE[name]
    # lowercase leading run of capitals but keep the last cap if followed by lower
    # simple rule: just lowercase the first character
    return name[0].lower() + name[1:]

pmap = {}
skipped = []
for n, c in cand.items():
    if not n[0].isupper():
        continue
    if n in prot:
        skipped.append((n, 'protected-UI'))
        continue
    if n.startswith(('FMT', 'Py', 'R_', 'OGR')) and n not in OVERRIDE:
        skipped.append((n, 'prefixed/external'))
        continue
    if n.endswith('Module') or n.endswith('class'):
        skipped.append((n, 'binding-local'))
        continue
    files = where.get(n, [])
    if files and all(BINDING.match(f) for f in files):
        skipped.append((n, 'binding-only:' + ','.join(files)))
        continue
    if n in SKIP:
        skipped.append((n, 'manual-skip'))
        continue
    pmap[n] = camel(n)

pmap = {k: v for k, v in pmap.items() if k != v}
json.dump(pmap, open('.rename_tools/map_pascal.json', 'w'), indent=1, sort_keys=True)
print("pascal map size:", len(pmap))
print("skipped:", len(skipped))
for n, r in sorted(skipped):
    print("  SKIP %-24s %s" % (n, r))
