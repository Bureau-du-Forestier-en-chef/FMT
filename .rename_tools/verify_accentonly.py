import subprocess
# For every file that differs from HEAD, confirm the ONLY differences are
# non-ASCII (accent) bytes. We strip bytes >127 AND normalise line endings
# (splitlines) so CRLF-vs-LF blob normalisation is not counted as a difference.
files = subprocess.check_output(['git', 'diff', '--name-only', 'HEAD']).decode().split()
bad = []
for p in files:
    try:
        head = subprocess.check_output(['git', 'show', 'HEAD:' + p], stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError:
        head = b''
    now = open(p, 'rb').read()
    hs = bytes(b for b in head if b <= 127).decode('ascii').splitlines()
    ns = bytes(b for b in now if b <= 127).decode('ascii').splitlines()
    if hs != ns:
        bad.append(p)
print("changed files:", len(files))
print("files with NON-accent (identifier/other) differences:", len(bad))
for p in bad:
    print("  ", p)
