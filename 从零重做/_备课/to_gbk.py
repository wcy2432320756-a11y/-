# -*- coding: utf-8 -*-
"""把我生成/修改过的 UTF-8 源码转成 GBK 无 BOM（Dev-C++ 的世界用）。

用法：
    python to_gbk.py            # 转「从零重做\」下全部 .cpp / .h
    python to_gbk.py <目录>     # 转指定目录

特点：
- 转之前把原始 UTF-8 内容备份到 _备课/_utf8_backup/（保留目录结构）；
- 本来就是 GBK（或纯 ASCII）的文件自动跳过，不会二次破坏。
"""
import os
import sys


def main():
    base = os.path.dirname(os.path.abspath(__file__))  # _备课
    root = os.path.dirname(base)                        # 从零重做
    target = sys.argv[1] if len(sys.argv) > 1 else root

    backup_dir = os.path.join(base, "_utf8_backup")
    converted = 0

    for dirpath, dirs, files in os.walk(target):
        if "_备课" in dirpath or "backup" in dirpath.lower():
            continue
        for f in files:
            if not f.lower().endswith((".cpp", ".h")):
                continue
            path = os.path.join(dirpath, f)
            data = open(path, "rb").read()
            bom = data.startswith(b"\xef\xbb\xbf")
            body = data[3:] if bom else data
            try:
                text = body.decode("utf-8")   # 能按 UTF-8 读，说明是 UTF-8 文件
            except UnicodeDecodeError:
                continue                      # 已经是 GBK，跳过
            out = text.encode("gbk")
            if not bom and out == body:
                continue                      # 纯 ASCII，无变化
            rel = os.path.relpath(path, root)
            bk = os.path.join(backup_dir, rel)
            os.makedirs(os.path.dirname(bk), exist_ok=True)
            open(bk, "wb").write(data)        # 备份原始内容
            open(path, "wb").write(out)       # 覆盖为 GBK 无 BOM
            print("已转 GBK:", rel)
            converted += 1

    print(f"共转换 {converted} 个文件。备份在: {backup_dir}")


if __name__ == "__main__":
    main()