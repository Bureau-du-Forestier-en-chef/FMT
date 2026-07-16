#!/usr/bin/env python3
"""Prove a rename commit contains ONLY the rename -- byte for byte.

Usage: python .rename_tools/diff_check.py <commit> <lot.json>

Takes each source file as it was in the parent commit, applies the batch mapping with
rename.py's own logic, and compares the result byte-for-byte with what was committed.
An exact match proves the commit introduced no line-ending flip (CRLF -> LF), no
re-encoding (cp1252 -> UTF-8), no reformatting and no stray edit -- the diff is the
rename and nothing else.

Files that differ are reported: each one is either a deliberate manual edit or a bug.
"""
import os, re, sys, json, subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rename  # noqa: E402

ROOT = os.path.dirname(HERE)


def blob(rev, path):
    r = subprocess.run(['git', 'show', '%s:%s' % (rev, path)],
                       cwd=ROOT, capture_output=True)
    return r.stdout if r.returncode == 0 else None


def apply_map(data, mapping, pat):
    text = data.decode('latin-1')
    out = []
    for seg, editable in rename.split_code_strings(text):
        if not editable and not rename.string_is_editable(seg):
            out.append(seg)
            continue
        out.append(pat.sub(lambda m: mapping[m.group(1)], seg))
    return ''.join(out).encode('latin-1')


def main():
    commit, lot = sys.argv[1], sys.argv[2]
    mapping = json.load(open(lot))
    pat = re.compile(r'(?<![A-Za-z0-9_])(' +
                     '|'.join(re.escape(k) for k in sorted(mapping, key=len, reverse=True)) +
                     r')(?![A-Za-z0-9_])')

    files = subprocess.run(['git', 'diff', '--name-only', '%s^' % commit, commit],
                           cwd=ROOT, capture_output=True, text=True).stdout.split()
    exact, manual, skipped = [], [], []
    for f in files:
        if not f.lower().endswith(rename.EXTS):
            skipped.append(f)
            continue
        before, after = blob(commit + '^', f), blob(commit, f)
        if before is None or after is None:
            skipped.append(f)
            continue
        (exact if apply_map(before, mapping, pat) == after else manual).append(f)

    print('commit %s  (%s)' % (commit, os.path.basename(lot)))
    print('  renommage exact (octet pour octet) : %d fichiers' % len(exact))
    print('  ecart -> edition manuelle ou bug   : %d' % len(manual))
    for f in manual:
        print('     ' + f)
    print('  hors perimetre (non-source)        : %d' % len(skipped))
    return 1 if manual else 0


if __name__ == '__main__':
    sys.exit(main())
