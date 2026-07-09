import subprocess, os
roots = ['Include', 'Source', 'FMTWrapperCore', 'UI', 'Modules', 'Templates']
exts = ('.h', '.hpp', '.cpp', '.cxx', '.cc')
mismatch = []
checked = 0
for r in roots:
    for dp, dn, fn in os.walk(r):
        if 'build' in dp.split(os.sep):
            continue
        for f in fn:
            if not f.lower().endswith(exts):
                continue
            p = os.path.join(dp, f).replace(os.sep, '/')
            try:
                orig = subprocess.check_output(['git', 'show', '382c1443:' + p],
                                               stderr=subprocess.DEVNULL)
            except subprocess.CalledProcessError:
                continue
            now = open(p, 'rb').read()
            co = sum(1 for b in orig if b > 127)
            cn = sum(1 for b in now if b > 127)
            checked += 1
            if co != cn:
                mismatch.append((p, co, cn))
print("files checked:", checked)
print("accent/non-ascii mismatches:", len(mismatch))
for p, co, cn in mismatch:
    print("  ", p, "orig", co, "now", cn)
