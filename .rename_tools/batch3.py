import re, json, sys
sys.path.insert(0, '.rename_tools')
import importlib.util

# Reuse the word dictionary + segmenter from propose.py without running its
# file-writing main by loading it as a module (its top-level builds `words`,
# `PREFIXES`, `seg_with_prefix`, `camel`). propose.py already executed its
# analysis at import time, which is fine here.
spec = importlib.util.spec_from_file_location('propose', '.rename_tools/propose.py')
propose = importlib.util.module_from_spec(spec)
spec.loader.exec_module(propose)

candidates = """
APIpush_back CopyToPeriod GetXSize GetYSize IsPeriodStart P1dev
addbounds allowwithoutyield allowyields anyoperables basefactor basemask basenode
basesource canshrink cleardevelopments compressmasks containsdevelopment copydata
copyextent emptyage emptylock emptyperiod emptyylds filluplastactions filtermask
findsets findsetswithfiltered getallseries getamountofpaths getbuildtype getcellsize
getdevsset getfirstblock getfirstconstblock getfirstperiod getgeotransform getinidsvariables
getinitialbounds getinproportion getinproportions getinvariables getlastdisturbance
getlocklowerbound getlockupperbound getlower getoutvariablesproportions getprojection
gettransferrow gettransferrownames getupper getvariablenames getvertexfromvertexinfo
getvertextoyieldinfo getyieldbound getyldbounds getylds isdependant isnotransfer
naturalgrowth nedges newdata newdev newedge newelement newschedule nochoice periodnode
presolvelist pushtodata rebasecache reverselect sameedgesas setbuildtype setextentfrom
setorbound specificiermask statsdiff tempnode testedmask timesincelastaction tostring
unshrink updatematrixindex updatevarsmap vecfastpass
""".split()

# single English words / STL-conforming / ambiguous -> leave unchanged
LEAVE = set("""add allow area build find load save swap update replace reserve shrink
results properties initialize in push_front push_back""".split())

OVERRIDE = {
    'APIpush_back': 'apiPushBack',
    'GetXSize': 'getXSize',
    'GetYSize': 'getYSize',
    'P1dev': 'p1dev',
    'nedges': 'nEdges',
    'getyldbounds': 'getYldBounds',
    'getylds': 'getYlds',
    'emptyylds': 'emptyYlds',
    'getyieldbound': 'getYieldBound',
    'nochoice': 'noChoice',
    'setorbound': 'setOrBound',
    'vecfastpass': 'vecFastPass',
    'reverselect': 'reverSelect',
    'specificiermask': 'specificierMask',
    'anyoperables': 'anyOperables',
    'cleardevelopments': 'clearDevelopments',
    'copyextent': 'copyExtent',
    'getamountofpaths': 'getAmountOfPaths',
    'getdevsset': 'getDevsSet',
    'getprojection': 'getProjection',
    'gettransferrow': 'getTransferRow',
    'gettransferrownames': 'getTransferRowNames',
    'isdependant': 'isDependant',
    'isnotransfer': 'isNoTransfer',
    'setextentfrom': 'setExtentFrom',
    'testedmask': 'testedMask',
    'timesincelastaction': 'timeSinceLastAction',
    'allowwithoutyield': 'allowWithoutYield',
    'rebasecache': 'rebaseCache',
    'getinidsvariables': 'getInIdsVariables',
}

FRAG = {'Returntime': 'ReturnTime', 'Staticpass': 'StaticPass'}

m = {}
unresolved = []
for n in candidates:
    if n in LEAVE:
        continue
    if n in OVERRIDE:
        m[n] = OVERRIDE[n]
        continue
    seg = propose.seg_with_prefix(n) if n.islower() else None
    if n[0].isupper():
        m[n] = n[0].lower() + n[1:]
    elif seg and len(seg) > 1:
        v = propose.camel(seg)
        for a, b in FRAG.items():
            v = v.replace(a, b)
        m[n] = v
    else:
        unresolved.append(n)

m = {k: v for k, v in m.items() if k != v}
json.dump(m, open('.rename_tools/map_hpp.json', 'w'), indent=1, sort_keys=True)
print("batch3 map size:", len(m), " unresolved:", len(unresolved))
print("unresolved:", unresolved)
print("--- map ---")
for k in sorted(m):
    print("  %-28s %s" % (k, m[k]))
