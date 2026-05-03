import json
import re
import shlex
import subprocess
import sys
from pathlib import Path

p = Path('build/compile_commands.json')
data = json.loads(p.read_text(encoding='utf-8'))
target = 'serve.c'
item = next((x for x in data if x['file'].replace('\\', '/').endswith('/' + target)), None)
if not item:
    print('missing compile command for serve.c')
    sys.exit(1)

cmd = re.sub(r'\s-o\s+[^ ]+\s+-c\s+', ' -fsyntax-only -c ', item['command'], count=1)
args = shlex.split(cmd, posix=False)
res = subprocess.run(args, text=True, capture_output=True)
if res.stdout:
    print(res.stdout, end='')
if res.stderr:
    print(res.stderr, end='')
print(f'exit={res.returncode}')
sys.exit(res.returncode)
