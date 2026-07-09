import re, glob, json, os

# 1) Harvest a domain word set from EXISTING camelCase / PascalCase identifiers
#    across all headers (split on capital-letter boundaries).
words = set()
for f in glob.glob('Include/*.h') + glob.glob('FMTWrapperCore/Include/*.h'):
    t = open(f, encoding='utf-8', errors='ignore').read()
    for ident in re.findall(r'[A-Za-z_]\w*', t):
        # Only harvest from identifiers that are ALREADY split (contain a capital
        # after a lowercase, i.e. real camelCase/PascalCase). All-lowercase
        # identifiers are exactly the concatenations we want to split, so they
        # must NOT pollute the dictionary.
        if not re.search(r'[a-z][A-Z]|[A-Z][a-z]', ident):
            continue
        if ident.islower():
            continue
        for piece in re.findall(r'[A-Z]+(?=[A-Z][a-z])|[A-Z]?[a-z]+|[A-Z]+|[0-9]+', ident):
            p = piece.lower()
            if len(p) >= 2:
                words.add(p)

# 2) Seed with common method verbs/prefixes and short domain tokens that
#    may not appear split elsewhere.
seed = """get set is has have got add remove build make made to from push pop back front clear
create write read load save update find compute calculate check init initialize contains
convert construct presolve postsolve fill apply copy swap merge split splitter sort parse
validate generate append insert erase count size empty print log solve run do does can could
will would should must process filter export import enable disable evaluate reduce predict
standardize modify try include exclude grow shrink new old of on off at by in into out up down
all any none each every first last next prev previous min max sum avg mean var across around
area mask age lock period periods periodic theme themes action actions transition transitions
output outputs origin yield yields yld ylds model models node nodes edge edges vertex verticies
vertices graph graphs event events cluster clusters clusterer scheme schemes schedule scheduler
scheduling schedules constraint constraints solution objective value values bound bounds bounded
alldual alldualbounded row rows col cols matrix caching factor factors weighted name names type
ctype types data datas datenow date now path paths cache ptr ref refactortorization refactorization
map key keys index indexes indices spatial temp temperature natural growth report reports carbon
file files folder line lines token tokens series serie serie size level levels weight weights rule
rules spread border borders pair pairs category categories cell cells decision tree base bases
iterator global local movable static dynamic operability operable operabilities handler handlers
logger parser section spec specification option optional optline constant number numbers worker
task tasks replanning heuristic heuristics parallel version core exception exceptions warning
error errors nested neighbors neighbor perimeter dispertion dispersion conflicts conflict
predictor predictors disturbances forest updated coordinates coordinate binary binaries binarized
binairies dual duals primal primals lower upper sum count reference references active inactive
open opening close closing returntime return time date shunting yard drift probability raster
polygon polygons destroy created around inventory harvest coef coefficient goals goal tabou tabous
penalty penalties peak peaks proportion proportions proportional variation variations variability
variabilities variable variables cost costs status result results memory available largest
smallest closest common source sources src info description descriptor descriptors clock duration
seconds major minor patch build date method relation repetition ressources resources delete
statistic statistics summary development targeted usage worth testing thread threads throwed
throwedon registered signals signal preexisting existing division associativity compress compressed
compresstime peak stack stacked stocked resolve simulate simplify summarize spawn hold refine
release greedy initial final selection generate schemes deduct deduced feasability feasibility
adjacency adjacent bitset intersect leaking division approximation distance choice nochoice
opes op ope eq gap xy gaps requests request expression handler nodesonly turned based transitions
respect lock plannng planning model do project template templates naming general links forcing
rows links infeasibles infeasibility flipped shunting available memory description activity
potential general updated scheduling around events fork bindings binding activevertex bases
midposition coord positive overided outputsin matrix valid membermelements members elements
usageof anyusageof what handle draw dist layer layers scheme cash deduction allow cashdeduction
across acrossperiod general constraintsnaming matrixnaming matrixngraph valagg agg past future
futur same different atleast staticpass biggest peakfrom summarydevelopment""".split()
words.update(seed)

# Known verb-like prefixes: if a name starts with one, split prefix off and
# segment the remainder, so getXxx / setXxx / isXxx are always handled.
PREFIXES = ['presolve', 'postsolve', 'initialize', 'contains', 'construct', 'generate',
            'calculate', 'evaluate', 'standardize', 'disable', 'enable', 'update', 'append',
            'validate', 'convert', 'compute', 'create', 'remove', 'insert', 'export', 'import',
            'change', 'clean', 'clear', 'build', 'reset', 'write', 'apply', 'erase', 'fill',
            'find', 'load', 'save', 'read', 'push', 'bound', 'unbound', 'locate', 'process',
            'get', 'set', 'has', 'had', 'add', 'got', 'can', 'are', 'was', 'need', 'is', 'do',
            'to', 'from', 'copy', 'make', 'sort', 'draw', 'hold', 'log']

# very common short words that are valid on their own (leave single-word names alone)
SINGLE_OK = set(words)

def segment(name):
    """Return list of word pieces if name segments fully into known words, else None.
    Prefers longer leading words (greedy from left with backtracking)."""
    n = len(name)
    memo = {}
    def helper(i):
        if i == n:
            return []
        if i in memo:
            return memo[i]
        best = None
        for j in range(n, i, -1):
            w = name[i:j]
            if w in words:
                rest = helper(j)
                if rest is not None:
                    cand = [w] + rest
                    if best is None or len(cand) < len(best):
                        best = cand
        memo[i] = best
        return best
    return helper(0)

def seg_with_prefix(name):
    """Try to peel a known prefix, segment the remainder; fall back to plain segment."""
    for pre in sorted(PREFIXES, key=len, reverse=True):
        if name.startswith(pre) and len(name) > len(pre):
            rest = segment(name[len(pre):])
            if rest is not None:
                return [pre] + rest
    return segment(name)

def camel(pieces):
    return pieces[0] + ''.join(p.capitalize() for p in pieces[1:])

# Names we must NOT rename: external customization points / macro artifacts.
EXCLUDE = {'hash_value', 'from_python', 'to_python'}
def is_excluded(name):
    if name in EXCLUDE:
        return True
    if name.endswith('_overloads'):     # boost.python overload macro names
        return True
    if name.startswith('_'):            # deliberate leading-underscore names
        return True
    return False

cand = json.load(open('.rename_tools/cand2.json'))
where = json.load(open('.rename_tools/cand2_where.json'))

proposal = {}
single = []
unresolved = []
for name, cnt in cand.items():
    if not name.islower():
        continue  # handle lowercase here; Pascal handled separately
    if is_excluded(name):
        continue
    if '_' in name:
        # underscore -> camelCase, segmenting each underscore-part too
        pieces = []
        for part in name.split('_'):
            if not part:
                continue
            s = seg_with_prefix(part)
            pieces.extend(s if s else [part])
        proposal[name] = camel(pieces)
        continue
    seg = seg_with_prefix(name)
    if seg is None:
        unresolved.append(name)
    elif len(seg) == 1:
        single.append(name)  # single real word -> leave unchanged
    else:
        proposal[name] = camel(seg)

json.dump(proposal, open('.rename_tools/proposal_lower.json', 'w'), indent=0)
json.dump(sorted(single), open('.rename_tools/single_lower.json', 'w'), indent=0)
json.dump(sorted(unresolved), open('.rename_tools/unresolved_lower.json', 'w'), indent=0)
print("lower proposal:", len(proposal), " single(left as-is):", len(single), " unresolved:", len(unresolved))
print("\n--- sample proposals (multi-word) ---")
for k in sorted(proposal)[:80]:
    print("  %-32s -> %s" % (k, proposal[k]))
print("\n--- unresolved (need manual) ---")
print(' '.join(sorted(unresolved)))
