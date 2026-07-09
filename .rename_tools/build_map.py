import json

proposal = json.load(open('.rename_tools/proposal_lower.json'))
corr = json.load(open('.rename_tools/corrections_lower.json'))
override = corr['_override']
leave = set(corr['_leave'])
fragments = corr.get('_fragments', {})

final = {}
for k, v in proposal.items():
    if k in leave:
        continue
    final[k] = v
final.update(override)
for k in leave:
    final.pop(k, None)

# fragment fixes: repair recurring compound pieces the segmenter left glued.
for k in list(final):
    v = final[k]
    for a, b in fragments.items():
        v = v.replace(a, b)
    final[k] = v

# sanity: no identity mappings, no target collisions producing dup old->same isn't an issue
final = {k: v for k, v in final.items() if k != v}
json.dump(final, open('.rename_tools/map_lower.json', 'w'), indent=1, sort_keys=True)
print("final lower map size:", len(final))

# report any suspicious targets (still all-lowercase multi = maybe unsplit)
susp = [k for k, v in final.items() if v.islower() and len(v) > 12]
print("suspicious (target still all-lowercase, len>12):", len(susp))
for k in sorted(susp):
    print("   ", k, "->", final[k])
