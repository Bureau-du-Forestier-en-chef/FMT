import re, os, json, subprocess

combined = {}
for f in ['map_lower', 'map_pascal', 'map_hpp', 'map_extra', 'map_typos_all', 'map_typo1']:
    p = '.rename_tools/%s.json' % f
    if os.path.exists(p):
        combined.update(json.load(open(p)))

# Load ORIGINAL code (pre-rename) so we test the OLD names.
BASE = '382c1443'
files = subprocess.check_output(['git', 'ls-tree', '-r', '--name-only', BASE]).decode().splitlines()
files = [f for f in files if f.lower().endswith(('.h', '.hpp', '.cpp', '.cc', '.cxx'))]
orig = []
for f in files:
    try:
        orig.append(subprocess.check_output(['git', 'show', BASE + ':' + f],
                                             stderr=subprocess.DEVNULL).decode('latin-1'))
    except subprocess.CalledProcessError:
        pass
code = '\n'.join(orig)

# A name is a genuine FMT method if defined out-of-line on an FMT class.
def fmt_defined(name):
    return re.search(r'\bFMT\w*(?:<[^>]*>)?\s*::\s*' + re.escape(name) + r'\s*\(', code) is not None

# For each OLD name, find the tokens that appear immediately before "::name("
# (its qualifiers) and before "name(" via -> or . (receiver hint is hard, skip).
def qualifiers(name):
    qs = set(re.findall(r'([A-Za-z_]\w*)\s*::\s*' + re.escape(name) + r'\s*\(', code))
    return qs

suspects = []
for old in sorted(combined):
    if fmt_defined(old):
        continue  # genuine FMT method (has FMTClass::old definition)
    qs = qualifiers(old)
    # external qualifiers
    ext = qs & {'std', 'boost', 'bp', 'python', 'Rcpp', 'chrono', 'filesystem',
                'icl', 'dll', 'iostreams', 'Ort', 'CPLString', 'osi', 'Osi'}
    suspects.append((old, combined[old], sorted(qs)))

print("names in map WITHOUT an FMTClass::name definition (%d):" % len(suspects))
for old, new, qs in suspects:
    print("  %-26s -> %-24s  qualifiers=%s" % (old, new, qs))
