# -*- coding: utf-8 -*-
"""读 out_bytes.bin，判断程序输出的是 GBK 还是 UTF-8 字节。"""
import os

base = os.path.dirname(os.path.abspath(__file__))
p = os.path.join(base, "out_bytes.bin")
data = open(p, "rb").read()
print("原始字节:", data[:100])
for enc in ("gbk", "utf-8"):
    try:
        print(f"按 {enc} 解码:", data.decode(enc).strip()[:80])
    except Exception as e:
        print(f"按 {enc} 解码失败: {e}")