import re
import sys
import pathlib

# 把 _教研/<章号>/part*.md 按中文节号拼成一份完整讲义
# 用法: py -3 merge.py 02 "第二课　组合与依赖：为什么借书必须传引用"

CN = '一二三四五六七八九十'


def cn_to_int(s):
    """把节号里的中文数字转成整数，支持 一 / 十 / 十一 / 十二 / 二十"""
    if s == '十':
        return 10
    if s.startswith('十'):
        return 10 + CN.index(s[1]) + 1
    if s.endswith('十'):
        return (CN.index(s[0]) + 1) * 10
    if '十' in s:
        a, b = s.split('十')
        return (CN.index(a) + 1) * 10 + CN.index(b) + 1
    return CN.index(s) + 1


def main():
    chap = sys.argv[1]
    title = sys.argv[2] if len(sys.argv) > 2 else None

    here = pathlib.Path(__file__).resolve().parent
    study = here.parent
    part_dir = here / chap

    parts = sorted(part_dir.glob('part*.md'))
    if not parts:
        print(f'no part files under {part_dir}')
        return 1

    sections = {}   # 节号 -> (标题行, 正文)
    for p in parts:
        text = p.read_text(encoding='utf-8')
        # 按行首 '## ' 切块
        chunks = re.split(r'(?m)^## ', text)
        for chunk in chunks[1:]:
            head, _, body = chunk.partition('\n')
            head = head.strip()
            m = re.match(r'^([一二三四五六七八九十]+)、', head)
            if not m:
                print(f'[warn] {p.name}: 无法识别节号 -> {head}')
                continue
            n = cn_to_int(m.group(1))
            if n in sections:
                print(f'[warn] 节号重复: {head} (覆盖前一份)')
            sections[n] = ('## ' + head, body.rstrip() + '\n')

    if not sections:
        print('no sections parsed')
        return 1

    missing = [cn for i, cn in enumerate(CN[:10], start=1) if i not in sections]
    if missing:
        print(f'[warn] 缺少节: {missing}')

    out = []
    if title:
        out.append('# ' + title + '\n')
    for n in sorted(sections):
        head, body = sections[n]
        out.append(head + '\n')
        out.append(body)

    result = '\n'.join(out)

    dest = study / '讲义' / f'{chap}-合并.md'
    dest.write_text(result, encoding='utf-8')
    print(f'merged {len(sections)} sections -> {dest}')
    print(f'chars: {len(result)}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
