#!/usr/bin/env python3
"""Safe camelCase renamer for the FMT codebase.

Usage: python rename.py mapping.json [--apply]
  mapping.json : {"oldName":"newName", ...}

Applies whole-word replacement across all .h/.hpp/.cpp files (excluding build/ and
.git/), skipping matches inside double/single-quoted string literals so that R/Python
binding names and message strings are preserved.

Any mapping key present in the protected set (tokens used in UI/, loaded from prot.json
next to this script, overridable with --prot=) is dropped with a warning, so the managed
interface never breaks.

String literals are frozen by default. Two categories are renamed anyway, via the strict
allowlist in string_is_editable():
  - "@DocString(X)"  : pairing key consumed by Documentation/commentsPythonandR.py, which
                       matches it against the "// DocString: X" marker in the header. Not
                       an API, and it must stay in sync with the comment (which IS renamed).
  - "FMTx::method"   : exception/log message. Not an API, nobody parses it.
Everything else stays frozen: exposed R/Python names (both class_<>("X") and the
.derives<>("X") parent name), boost make_nvp archive tags, and #include paths. Note that
"FMTaction" can appear twice on one binding line with opposite policies -- hence the
per-segment decision rather than a global regex.
"""
import sys, os, re, json

HERE = os.path.dirname(os.path.abspath(__file__))
BS = chr(92)  # backslash
ROOTS = ['Include', 'Source', 'FMTWrapperCore', 'UI', 'Modules', 'Templates']
EXTS = ('.h', '.hpp', '.cpp', '.cxx', '.cc')

DOCSTRING_KEY = '"@DocString('
EXC_MESSAGE = re.compile(r'^"[A-Za-z_:]*FMT[A-Za-z0-9_]+::')


def string_is_editable(seg):
    """seg includes its quotes. Default: FREEZE; the allowlist is deliberately narrow.

    Freezing by default is the safe direction: the renameable categories can be
    characterized positively, while the freeze-worthy ones are heterogeneous and get
    discovered over time. An oversight here costs a cosmetic doc desync, never a
    silently broken R/Python API.
    """
    return seg.startswith(DOCSTRING_KEY) or bool(EXC_MESSAGE.match(seg))


def iter_files():
    for r in ROOTS:
        if not os.path.isdir(r):
            continue
        for dp, dn, fn in os.walk(r):
            parts = dp.split(os.sep)
            if 'build' in parts or '.git' in parts:
                continue
            for f in fn:
                if f.lower().endswith(EXTS):
                    yield os.path.join(dp, f)


def split_code_strings(text):
    """Return list of (segment, editable).

    Comment-aware: // and /* */ comments are editable prose (renaming in doc
    comments is desired) but quotes inside them are NOT treated as literal
    delimiters, so apostrophes in comments ("it's") can't swallow real code.
    Only genuine "..." string and '...' char literals are marked non-editable.
    """
    out = []
    i = 0
    n = len(text)
    buf = []

    def flush():
        if buf:
            out.append((''.join(buf), True))
            buf.clear()

    while i < n:
        c = text[i]
        two = text[i:i + 2]
        if two == '//':                      # line comment -> editable
            j = text.find('\n', i)
            if j == -1:
                j = n
            buf.append(text[i:j])
            i = j
        elif two == '/*':                    # block comment -> editable
            j = text.find('*/', i + 2)
            j = n if j == -1 else j + 2
            buf.append(text[i:j])
            i = j
        elif c == '"' or c == "'":           # string / char literal -> skip
            flush()
            q = c
            s = [c]
            i += 1
            while i < n:
                s.append(text[i])
                if text[i] == BS and i + 1 < n:
                    s.append(text[i + 1])
                    i += 2
                    continue
                if text[i] == q:
                    i += 1
                    break
                i += 1
            out.append((''.join(s), False))
        else:
            buf.append(c)
            i += 1
    flush()
    return out


def main():
    if len(sys.argv) < 2:
        print("need mapping.json")
        sys.exit(1)
    mapping = json.load(open(sys.argv[1], encoding='utf-8'))
    apply = '--apply' in sys.argv

    global ROOTS
    for a in sys.argv:
        if a.startswith('--roots='):
            ROOTS = a.split('=', 1)[1].split(',')

    # prot.json protects METHOD-rename tokens and happens to contain core CLASS names
    # (FMTmask, FMTmodel, FMTexception...). Using it for the class rename would silently
    # drop them, so the class batches pass --prot=prot_classes.json instead.
    protname = 'prot.json'
    for a in sys.argv:
        if a.startswith('--prot='):
            protname = a.split('=', 1)[1]
    prot_path = os.path.join(HERE, protname)
    prot = set(json.load(open(prot_path))) if os.path.exists(prot_path) else set()
    dropped = [k for k in list(mapping) if k in prot]
    for k in dropped:
        del mapping[k]
    if dropped:
        print("SKIPPED (protected UI tokens): " + ", ".join(sorted(dropped)))
    if not mapping:
        print("empty mapping after protection filter")
        return

    keys = sorted(mapping, key=len, reverse=True)
    pat = re.compile(r'(?<![A-Za-z0-9_])(' + '|'.join(re.escape(k) for k in keys) + r')(?![A-Za-z0-9_])')
    total = 0
    filecount = 0
    for path in iter_files():
        try:
            # latin-1 maps every byte 0..255 to a codepoint 1:1, so read+write
            # round-trips ALL bytes losslessly (preserves cp1252 accents like the
            # "Québec" copyright header). newline='' preserves CRLF/LF verbatim.
            t = open(path, encoding='latin-1', newline='').read()
        except Exception:
            continue
        cnt = 0
        newsegs = []
        for seg, editable in split_code_strings(t):
            if not editable and not string_is_editable(seg):
                newsegs.append(seg)
                continue

            def rep(m):
                nonlocal cnt
                cnt += 1
                return mapping[m.group(1)]
            newsegs.append(pat.sub(rep, seg))
        if cnt:
            total += cnt
            filecount += 1
            print("%5d  %s" % (cnt, path))
            if apply:
                open(path, 'w', encoding='latin-1', newline='').write(''.join(newsegs))
    print("--- %d occurrences across %d files. %s" % (total, filecount, 'APPLIED' if apply else 'DRY-RUN'))


if __name__ == '__main__':
    main()
