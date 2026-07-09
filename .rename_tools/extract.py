import re, glob, json, collections, sys, os
sys.path.insert(0, '.rename_tools')
from rename import split_code_strings

prot = set(json.load(open('.rename_tools/prot.json')))
kw = {'if','for','while','switch','return','sizeof','catch','and','or','not','new','delete',
'throw','case','do','else','static_cast','dynamic_cast','reinterpret_cast','const_cast',
'decltype','typeid','alignof','noexcept','explicit','template','typename','using','friend',
'public','private','protected','class','struct','enum','namespace','const','static','virtual',
'inline','constexpr','operator','defined','assert','mutable','override','final','union','auto',
'nullptr','true','false','this','goto','continue','break','default','volatile','register','extern'}

files = glob.glob('Include/*.h') + glob.glob('FMTWrapperCore/Include/*.h')

classnames = set()
for f in files:
    t = open(f, encoding='utf-8', errors='ignore').read()
    for m in re.finditer(r'\b(class|struct)\s+(?:FMTEXPORT\s+)?([A-Za-z_]\w*)', t):
        classnames.add(m.group(2))

cand = collections.Counter()
where = collections.defaultdict(set)
declre = re.compile(r'(?:^|[;{}:])\s*(?:[A-Za-z_][\w:<>,&*\s\[\]]*?[\s&*>])([A-Za-z_]\w*)\s*\(', re.M)
for f in files:
    raw = open(f, encoding='utf-8', errors='ignore').read()
    t = re.sub(r'/\*.*?\*/', '', raw, flags=re.S)
    t = re.sub(r'//[^\n]*', '', t)
    for m in declre.finditer(t):
        n = m.group(1)
        if n in kw or n in prot or n in classnames:
            continue
        if n == 'serialize' or n.startswith('operator') or n.isupper():
            continue
        if n[0].islower() and any(c.isupper() for c in n):
            continue  # already camelCase
        if not (n[0].isupper() or n.islower()):
            continue
        cand[n] += 1
        where[n].add(os.path.basename(f))

items = sorted(cand.items(), key=lambda x: (-x[1], x[0]))
print("distinct candidates (code-only, decl-ish):", len(items))
json.dump({n: c for n, c in items}, open('.rename_tools/cand2.json', 'w'))
json.dump({n: sorted(where[n]) for n, _ in items}, open('.rename_tools/cand2_where.json', 'w'))
pas = [n for n, _ in items if n[0].isupper()]
low = [n for n, _ in items if n.islower()]
print("Pascal:", len(pas), " lower:", len(low))
print("--- Pascal (all):")
print(' '.join(pas))
