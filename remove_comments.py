from pathlib import Path
import sys

p = Path(r"c:\Users\Skate\Desktop\FoAna\FolderAnalayzer45.cpp")
backup = p.with_suffix('.cpp.bak')
backup.write_text(p.read_text(encoding='utf-8'), encoding='utf-8')

code = p.read_text(encoding='utf-8')

def remove_comments(code):
    res = []
    i = 0
    n = len(code)
    state = 'normal'  # normal, single, multi, string
    string_char = ''
    while i < n:
        ch = code[i]
        nxt = code[i+1] if i+1<n else ''
        if state == 'normal':
            if ch == '/' and nxt == '/':
                state = 'single'
                i += 2
                continue
            elif ch == '/' and nxt == '*':
                state = 'multi'
                i += 2
                continue
            elif ch == '"' or ch == "'":
                state = 'string'
                string_char = ch
                res.append(ch)
                i += 1
                continue
            else:
                res.append(ch)
                i += 1
                continue
        elif state == 'single':
            if ch == '\n':
                state = 'normal'
                res.append(ch)
            i += 1
            continue
        elif state == 'multi':
            if ch == '*' and nxt == '/':
                state = 'normal'
                i += 2
            else:
                i += 1
            continue
        elif state == 'string':
            res.append(ch)
            if ch == '\\':
                # escape char, include next char too
                if i+1<n:
                    res.append(code[i+1])
                    i += 2
                    continue
            elif ch == string_char:
                state = 'normal'
                i += 1
                continue
            i += 1
            continue
    return ''.join(res)

clean = remove_comments(code)
# Optionally, remove trailing blank lines introduced by comment removal
# but we keep existing newlines for safety
p.write_text(clean, encoding='utf-8')
print('Comments removed; backup saved to', backup)
