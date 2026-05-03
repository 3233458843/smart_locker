import json
import re
import shlex
import subprocess
import sys
from pathlib import Path

TARGETS = ['serve.c', 'events_init.c']

p = Path('build/compile_commands.json')
data = json.loads(p.read_text(encoding='utf-8'))
failed = False
for target in TARGETS:
    item = next((x for x in data if x['file'].replace('\\', '/').endswith('/' + target)), None)
    if not item:
        print(f'missing compile command for {target}')
        failed = True
        continue

    cmd = re.sub(r'\s-o\s+[^ ]+\s+-c\s+', ' -fsyntax-only -c ', item['command'], count=1)
    args = shlex.split(cmd, posix=False)
    res = subprocess.run(args, text=True, capture_output=True)
    if res.stdout:
        print(res.stdout, end='')
    if res.stderr:
        print(res.stderr, end='')
    print(f'{target}: exit={res.returncode}')
    if res.returncode != 0:
        failed = True

sys.exit(1 if failed else 0)
