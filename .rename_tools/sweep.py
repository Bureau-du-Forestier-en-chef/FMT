import re, glob, json, os, collections, sys
sys.path.insert(0, '.rename_tools')
from rename import split_code_strings

prot = set(json.load(open('.rename_tools/prot.json')))
leave = set(json.load(open('.rename_tools/corrections_lower.json')).get('_leave', []))

ROOTS = ['Include', 'Source', 'FMTWrapperCore', 'Modules', 'Templates']
EXTS = ('.h', '.hpp', '.cpp', '.cxx', '.cc')


def code_of(path):
    t = open(path, encoding='latin-1', newline='').read()
    return ''.join(seg for seg, ed in split_code_strings(t)
                   if ed and not seg.lstrip().startswith(('//', '/*')))


def is_camel_ok(n):
    # single lowercase word OR lowercase-first with internal capital
    if n[0].islower() and (any(c.isupper() for c in n) or n.islower()):
        # still, an all-lowercase MULTI-word is NOT ok, but we can't tell here;
        # treat all-lowercase as "review" separately.
        return None if n.islower() else True
    return False  # PascalCase / other


defre = re.compile(r'\b([A-Za-z_]\w*)::([A-Za-z_]\w*)\s*\(')
methods = collections.defaultdict(set)   # method name -> set(classes)
for r in ROOTS:
    for dp, dn, fn in os.walk(r):
        if 'build' in dp.split(os.sep):
            continue
        for f in fn:
            if not f.lower().endswith(EXTS):
                continue
            p = os.path.join(dp, f)
            code = code_of(p)
            for m in defre.finditer(code):
                cls, meth = m.group(1), m.group(2)
                methods[meth].add(cls)

pascal = []
lower_multi = []
for meth, classes in methods.items():
    if meth in prot or meth in leave:
        continue
    if meth == 'serialize' or meth.startswith('operator') or meth.startswith('~'):
        continue
    if meth == classes and len(classes) == 1:  # constructor-ish (rare via ::)
        pass
    if meth in classes:   # constructor Class::Class
        continue
    if meth[0].isupper():
        pascal.append((meth, sorted(classes)))
    elif meth.islower() and re.search(r'[a-z]{7,}', meth):
        # all-lowercase, long -> likely multi-word that slipped through
        lower_multi.append((meth, sorted(classes)))

print("=== PascalCase method definitions still present (potential misses / intended?) ===")
for m, c in sorted(pascal):
    print("  %-30s  %s" % (m, ','.join(c)))
print("\n=== all-lowercase (>=7 letters) method definitions still present (review) ===")
for m, c in sorted(lower_multi):
    print("  %-30s  %s" % (m, ','.join(c)))
