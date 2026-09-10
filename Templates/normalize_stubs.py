from __future__ import annotations

import html
import keyword
import re
from pathlib import Path


def fix_html(text: str) -> str:
    return html.unescape(text)


def fix_cpp_types(text: str) -> str:
    replacements = {
        "basic_string": "str",
        "vector": "list",
    }

    for old, new in replacements.items():
        text = re.sub(
            rf"\b{re.escape(old)}\b",
            new,
            text,
        )

    return text


def fix_keywords(text: str) -> str:
    def repl(match: re.Match) -> str:
        name = match.group(1)

        if keyword.iskeyword(name):
            return f"def {name}_("

        return match.group(0)

    return re.sub(
        r"def\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(",
        repl,
        text,
    )


def fix_boost(text: str) -> str:
    enum_needed = "Boost.Python.enum" in text

    text = re.sub(
        r"\(\s*Boost\.Python\.instance\s*\)",
        "",
        text,
    )

    text = re.sub(
        r"\(\s*Boost\.Python\.enum\s*\)",
        "(enum.Enum)",
        text,
    )

    if enum_needed and "import enum" not in text:
        text = "import enum\n" + text

    return text


def ensure_future_annotations(text: str) -> str:
    future = "from __future__ import annotations"

    if future not in text:
        text = future + "\n" + text

    return text


def cleanup_blank_lines(text: str) -> str:
    return re.sub(r"\n{3,}", "\n\n", text)


def normalize_stub(path: Path) -> None:
    print(f"Normalizing: {path.resolve()}")

    text = path.read_text(
        encoding="utf-8",
        errors="ignore",
    )

    print("  before:")
    print("    Boost.Python.instance:", "Boost.Python.instance" in text)
    print("    Boost.Python.enum    :", "Boost.Python.enum" in text)
    print("    &gt;                 :", "&gt;" in text)
    print("    basic_string        :", "basic_string" in text)
    print("    vector              :", "vector" in text)

    text = fix_html(text)
    text = fix_keywords(text)
    text = fix_boost(text)
    text = fix_cpp_types(text)
    text = ensure_future_annotations(text)
    text = cleanup_blank_lines(text)

    path.write_text(
        text,
        encoding="utf-8",
    )

    print("  after:")
    print("    Boost.Python.instance:", "Boost.Python.instance" in text)
    print("    Boost.Python.enum    :", "Boost.Python.enum" in text)
    print("    &gt;                 :", "&gt;" in text)
    print("    basic_string        :", "basic_string" in text)
    print("    vector              :", "vector" in text)


def main() -> None:
    count = 0

    for stub in Path(".").rglob("*.pyi"):
        normalize_stub(stub)
        count += 1

    print(f"\nProcessed {count} stub files")


if __name__ == "__main__":
    main()