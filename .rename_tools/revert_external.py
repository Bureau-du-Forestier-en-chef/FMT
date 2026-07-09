import re, os, json, subprocess, sys

APPLY = '--apply' in sys.argv

combined = {}
for f in ['map_lower', 'map_pascal', 'map_hpp', 'map_extra', 'map_typos_all', 'map_typo1']:
    p = '.rename_tools/%s.json' % f
    if os.path.exists(p):
        combined.update(json.load(open(p)))
new2old = {v: k for k, v in combined.items()}
NEWNAMES = set(new2old)

# FMT-owned names that live under an external namespace but are ours (keep renamed).
FMT_OWNED = {'pyHash'}

EXT_NS = ['std', 'boost', 'bp', 'python', 'Rcpp', 'chrono', 'filesystem', 'icl',
          'dll', 'iostreams', 'Ort', 'osi', 'Osi']

EXT_TYPES = [
    'GDALDataset', 'GDALRasterBand', 'GDALDriver', 'GDALColorTable', 'GDALDatasetH',
    'GDALRasterBandH', 'OGRLayer', 'OGRFeature', 'OGRGeometry', 'OGRPolygon',
    'OGRLinearRing', 'OGRSpatialReference', 'OGRFieldDefn', 'OGRFeatureDefn',
    'OGRPoint', 'OGRMultiPolygon', 'OGRGeometryH', 'OGRLayerH', 'CPLXMLNode',
    'OsiSolverInterface', 'OsiClpSolverInterface', 'CoinPackedMatrix',
    'CoinMessageHandler', 'CoinPackedVector', 'CoinWarmStart', 'CoinWarmStartBasis',
]

BASE = '382c1443'
orig_files = subprocess.check_output(['git', 'ls-tree', '-r', '--name-only', BASE]).decode().splitlines()
orig_files = [f for f in orig_files if f.lower().endswith(('.h', '.hpp', '.cpp', '.cc', '.cxx'))]
orig_code = []
for f in orig_files:
    try:
        orig_code.append(subprocess.check_output(['git', 'show', BASE + ':' + f],
                                                  stderr=subprocess.DEVNULL).decode('latin-1'))
    except subprocess.CalledProcessError:
        pass
ORIG = '\n'.join(orig_code)


def orig_has(pattern):
    return re.search(pattern, ORIG) is not None


ROOTS = ['Include', 'Source', 'FMTWrapperCore', 'UI', 'Modules', 'Templates']
EXTS = ('.h', '.hpp', '.cpp', '.cxx', '.cc')

report = {}
for r in ROOTS:
    for dp, dn, fn in os.walk(r):
        if 'build' in dp.split(os.sep):
            continue
        for f in fn:
            if not f.lower().endswith(EXTS):
                continue
            p = os.path.join(dp, f)
            code = open(p, encoding='latin-1', newline='').read()
            edits = []

            # (1) external-namespace-qualified: EXT::NEW -> EXT::OLD
            for ns in EXT_NS:
                for m in re.finditer(r'\b' + ns + r'\s*::\s*(' + '|'.join(map(re.escape, NEWNAMES)) + r')\b', code):
                    new = m.group(1)
                    if new in FMT_OWNED:
                        continue
                    old = new2old[new]
                    if old != new and orig_has(r'\b' + ns + r'\s*::\s*' + re.escape(old) + r'\b'):
                        edits.append((ns + '::' + new, ns + '::' + old))

            # (2) calls on external-typed receivers: var(->|.)NEW(  -> var\1OLD(
            #     find receiver vars declared with an external type in THIS file
            extvars = set()
            for t in EXT_TYPES:
                for m in re.finditer(r'\b' + t + r'\s*[\*&]*\s*([A-Za-z_]\w*)', code):
                    extvars.add(m.group(1))
            for var in extvars:
                for m in re.finditer(re.escape(var) + r'\s*(->|\.)\s*(' + '|'.join(map(re.escape, NEWNAMES)) + r')\s*\(', code):
                    new = m.group(2)
                    if new in FMT_OWNED:
                        continue
                    old = new2old[new]
                    if old != new:
                        edits.append((var + m.group(1) + new + '(', var + m.group(1) + old + '('))

            if edits:
                report[p] = sorted(set(edits))
                if APPLY:
                    for a, b in set(edits):
                        code = code.replace(a, b)
                    open(p, 'w', encoding='latin-1', newline='').write(code)

total = 0
for p, edits in sorted(report.items()):
    print(p)
    for a, b in edits:
        print("    %-40s -> %s" % (a, b))
        total += 1
print("--- %d distinct revert patterns across %d files. %s" % (total, len(report), 'APPLIED' if APPLY else 'REPORT'))
