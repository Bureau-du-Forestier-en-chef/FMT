import os, sys

APPLY = '--apply' in sys.argv

# Precise, verified reverts of names that collide with EXTERNAL library APIs.
# Left side = current (wrongly-renamed) text, right side = correct external name.
REVERTS = [
    ('std::getLine', 'std::getline'),
    ('std::lowerBound', 'std::lower_bound'),
    ('std::upperBound', 'std::upper_bound'),
    ('->getXSize(', '->GetXSize('),   # GDALRasterBand (arrow); FMTlayer uses '.'
    ('->getYSize(', '->GetYSize('),
    ('.isEmpty(', '.IsEmpty('),       # OGRGeometry; no FMT class defines IsEmpty
    ('->isEmpty(', '->IsEmpty('),
]

ROOTS = ['Include', 'Source', 'FMTWrapperCore', 'UI', 'Modules', 'Templates']
EXTS = ('.h', '.hpp', '.cpp', '.cxx', '.cc')

total = 0
for r in ROOTS:
    for dp, dn, fn in os.walk(r):
        if 'build' in dp.split(os.sep):
            continue
        for f in fn:
            if not f.lower().endswith(EXTS):
                continue
            p = os.path.join(dp, f)
            code = open(p, encoding='latin-1', newline='').read()
            n = 0
            for a, b in REVERTS:
                c = code.count(a)
                if c:
                    code = code.replace(a, b)
                    n += c
            if n:
                total += n
                print("%5d  %s" % (n, p))
                if APPLY:
                    open(p, 'w', encoding='latin-1', newline='').write(code)
print("--- %d reverts. %s" % (total, 'APPLIED' if APPLY else 'DRY-RUN'))
