import re, os, json, collections

combined = {}
for f in ['map_lower', 'map_pascal', 'map_hpp', 'map_extra', 'map_typos_all', 'map_typo1']:
    p = '.rename_tools/%s.json' % f
    if os.path.exists(p):
        combined.update(json.load(open(p)))
new2old = {v: k for k, v in combined.items()}
NEW = set(new2old)

# receiver types whose member functions are EXTERNAL (must not be renamed)
STD_TYPES = ['set', 'map', 'multiset', 'multimap', 'unordered_map', 'unordered_set',
             'vector', 'list', 'deque', 'string', 'wstring', 'basic_string',
             'ifstream', 'ofstream', 'fstream', 'istringstream', 'ostringstream',
             'stringstream', 'istream', 'ostream', 'array', 'pair', 'queue', 'stack',
             'priority_queue', 'shared_ptr', 'unique_ptr', 'weak_ptr']
EXT_TYPES = ['GDALDataset', 'GDALRasterBand', 'GDALDriver', 'OGRLayer', 'OGRFeature',
             'OGRGeometry', 'OGRPolygon', 'OGRMultiPolygon', 'OGRLinearRing',
             'OGRSpatialReference', 'OGRFieldDefn', 'OGRFeatureDefn', 'CPLXMLNode',
             'OsiSolverInterface', 'OsiClpSolverInterface', 'ClpSimplex',
             'CoinPackedMatrix', 'CoinMessageHandler']

ROOTS = ['Include', 'Source', 'FMTWrapperCore']
EXTS = ('.h', '.hpp', '.cpp')

# type regexes
std_decl = re.compile(r'std\s*::\s*(?:' + '|'.join(STD_TYPES) +
                      r')\s*(?:<[^;{}]*?>)?\s*[\*&]?\s*([A-Za-z_]\w*)')
ext_decl = re.compile(r'\b(?:' + '|'.join(EXT_TYPES) + r')\s*[\*&]*\s*([A-Za-z_]\w*)')

hits = collections.defaultdict(list)
for r in ROOTS:
    for dp, dn, fn in os.walk(r):
        if 'build' in dp.split(os.sep):
            continue
        for f in fn:
            if not f.lower().endswith(EXTS):
                continue
            p = os.path.join(dp, f)
            code = open(p, encoding='latin-1').read()
            recv = set(std_decl.findall(code)) | set(ext_decl.findall(code))
            recv.discard('')
            for var in recv:
                if len(var) < 2:
                    continue
                for m in re.finditer(r'\b' + re.escape(var) + r'\s*(?:->|\.)\s*(' +
                                     '|'.join(map(re.escape, NEW)) + r')\s*\(', code):
                    new = m.group(1)
                    if new != new2old[new]:
                        hits[(new, new2old[new])].append((p, var))

print("=== renamed names called on std/external-typed receivers ===")
for (new, old), occ in sorted(hits.items()):
    files = sorted(set(o[0] for o in occ))
    vars_ = sorted(set(o[1] for o in occ))
    print("  %-22s (was %-18s) x%d  vars=%s  files=%s"
          % (new, old, len(occ), vars_, [os.path.basename(x) for x in files]))
print("total suspect names:", len(hits))
