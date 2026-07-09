import re, os, json, subprocess, collections

combined = {}
for f in ['map_lower', 'map_pascal', 'map_hpp', 'map_extra', 'map_typos_all', 'map_typo1']:
    p = '.rename_tools/%s.json' % f
    if os.path.exists(p):
        combined.update(json.load(open(p)))
OLDNAMES = set(combined)

EXT_TYPES = [
    'GDALDataset', 'GDALRasterBand', 'GDALDriver', 'GDALColorTable',
    'OGRLayer', 'OGRFeature', 'OGRGeometry', 'OGRPolygon', 'OGRLinearRing',
    'OGRSpatialReference', 'OGRFieldDefn', 'OGRFeatureDefn', 'OGRPoint',
    'OGRMultiPolygon', 'OGRDataSource', 'CPLXMLNode',
    'OsiSolverInterface', 'OsiClpSolverInterface', 'ClpSimplex', 'CoinPackedMatrix',
    'CoinMessageHandler', 'CoinPackedVector', 'CoinWarmStart',
]
EXT_NS = ['std', 'boost', 'bp', 'Rcpp', 'chrono', 'icl']

BASE = '382c1443'
files = subprocess.check_output(['git', 'ls-tree', '-r', '--name-only', BASE]).decode().splitlines()
files = [f for f in files if f.lower().endswith(('.h', '.hpp', '.cpp', '.cc', '.cxx'))]

# collisions: (context) -> set of (file, method)
recv = collections.defaultdict(set)   # method -> set of external receiver-var names
ns = collections.defaultdict(set)     # method -> set of external namespaces

for f in files:
    try:
        code = subprocess.check_output(['git', 'show', BASE + ':' + f],
                                       stderr=subprocess.DEVNULL).decode('latin-1')
    except subprocess.CalledProcessError:
        continue
    # external-typed variable names in this file
    extvars = {}
    for t in EXT_TYPES:
        for m in re.finditer(r'\b' + t + r'\s*[\*&]*\s*([A-Za-z_]\w*)', code):
            extvars[m.group(1)] = t
    for var, t in extvars.items():
        for m in re.finditer(re.escape(var) + r'\s*(?:->|\.)\s*([A-Za-z_]\w*)\s*\(', code):
            if m.group(1) in OLDNAMES:
                recv[m.group(1)].add(t)
    for n in EXT_NS:
        for m in re.finditer(r'\b' + n + r'\s*::\s*([A-Za-z_]\w*)\s*(?:\(|<)', code):
            if m.group(1) in OLDNAMES:
                ns[m.group(1)].add(n)

print("=== map methods CALLED on external-typed receivers (collisions) ===")
for meth in sorted(recv):
    print("  %-24s -> %-22s  on: %s" % (meth, combined[meth], sorted(recv[meth])))
print("\n=== map methods CALLED via external namespace ===")
for meth in sorted(ns):
    print("  %-24s -> %-22s  ns: %s" % (meth, combined[meth], sorted(ns[meth])))
