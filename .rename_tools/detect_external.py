import re, os, json

# Combined applied map (old -> new) from every batch.
combined = {}
for f in ['map_lower', 'map_pascal', 'map_hpp', 'map_extra', 'map_typos_all', 'map_typo1']:
    p = '.rename_tools/%s.json' % f
    if os.path.exists(p):
        combined.update(json.load(open(p)))
new2old = {v: k for k, v in combined.items()}

ROOTS = ['Include', 'Source', 'FMTWrapperCore', 'UI', 'Modules', 'Templates']
EXTS = ('.h', '.hpp', '.cpp', '.cxx', '.cc')

# external namespaces / library owners whose members must NOT have been renamed
EXT_NS = r'(?:std|boost|bp|python|Rcpp|chrono|filesystem|icl|dll|iostreams|Ort|CPLString)'

text = []
for r in ROOTS:
    for dp, dn, fn in os.walk(r):
        if 'build' in dp.split(os.sep):
            continue
        for f in fn:
            if f.lower().endswith(EXTS):
                text.append(open(os.path.join(dp, f), encoding='latin-1').read())
allcode = '\n'.join(text)

# 1) NEW names that currently appear qualified by an external namespace => reverted wrongly
ns_hits = {}
for new in new2old:
    m = re.findall(EXT_NS + r'\s*::\s*' + re.escape(new) + r'\b', allcode)
    if m:
        ns_hits[new] = len(m)

# 2) Is each such name ALSO a genuine FMT method (defined on an FMT class)?
def is_fmt_method(name):
    return re.search(r'\bFMT\w*(?:<[^>]*>)?\s*::\s*' + re.escape(name) + r'\s*\(', allcode) is not None

print("=== NEW names appearing under an EXTERNAL namespace (wrongly renamed) ===")
for new in sorted(ns_hits, key=lambda k: -ns_hits[k]):
    print("  %-26s (old: %-22s) x%-3d  alsoFMTmethod=%s"
          % (new, new2old[new], ns_hits[new], is_fmt_method(new)))
print("total external-namespace names:", len(ns_hits))
json.dump({new2old[n]: n for n in ns_hits}, open('.rename_tools/ext_ns.json', 'w'), indent=1)
