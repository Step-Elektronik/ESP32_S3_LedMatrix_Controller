#!/usr/bin/env python3
"""Prepares the files that GitHub Actions publishes to the "firmware" branch.

Devices read firmware.json from that branch (AppConfig::OnlineUpdate), compare
its version with their own and, after the user confirms, download the .bin
file it names and check its size and SHA-256.

Usage:
  python tools/publish_firmware.py <firmware.bin> <output-dir> <commit-sha> [commit-subject]
"""

import datetime
import hashlib
import json
import os
import pathlib
import re
import sys

ESP_IMAGE_MAGIC = 0xE9
MAXIMUM_NOTES_LENGTH = 200


def firmware_version(root: pathlib.Path) -> str:
    text = (root / "include" / "AppConfig.h").read_text(encoding="utf-8")
    match = re.search(
        r'#define\s+FIRMWARE_VERSION\s+"(\d{1,3})\.(\d{1,3})\.(\d{1,3})"', text)
    if not match:
        sys.exit('FIRMWARE_VERSION "major.minor.patch" not found in include/AppConfig.h')
    return ".".join(match.groups())


def main() -> None:
    if len(sys.argv) < 4:
        sys.exit(__doc__)
    root = pathlib.Path(__file__).resolve().parent.parent
    firmware = pathlib.Path(sys.argv[1])
    output = pathlib.Path(sys.argv[2])
    commit = sys.argv[3]
    subject = sys.argv[4] if len(sys.argv) > 4 else ""

    data = firmware.read_bytes()
    if not data or data[0] != ESP_IMAGE_MAGIC:
        sys.exit(f"{firmware} is not an ESP32 application image")

    version = firmware_version(root)
    # A unique name per build keeps the raw.githubusercontent.com cache from
    # serving an older binary under a newer firmware.json.
    name = f"firmware-{version}-{commit[:7]}.bin"
    output.mkdir(parents=True, exist_ok=True)
    (output / name).write_bytes(data)
    # Same file for manual installation through the portal, which requires
    # the name firmware.bin.
    (output / "firmware.bin").write_bytes(data)

    manifest = {
        "version": version,
        "file": name,
        "size": len(data),
        "sha256": hashlib.sha256(data).hexdigest(),
        "commit": commit,
        "date": datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "notes": subject[:MAXIMUM_NOTES_LENGTH],
    }
    (output / "firmware.json").write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    (output / "README.md").write_text(
        "# Firmware\n\n"
        "Bu dal GitHub Actions tarafından master dalına yapılan her push'ta "
        "otomatik olarak yeniden oluşturulur; elle değiştirmeyin.\n\n"
        f"- Sürüm: `{version}`\n"
        f"- Kaynak commit: `{commit}`\n"
        f"- `firmware.bin`: portaldaki Yazılım Güncelleme bölümünden elle yüklenebilir.\n"
        f"- `firmware.json`: cihazların çevrimiçi güncelleme bilgisi.\n",
        encoding="utf-8")

    github_output = os.environ.get("GITHUB_OUTPUT")
    if github_output:
        with open(github_output, "a", encoding="utf-8") as stream:
            stream.write(f"version={version}\n")
    print(f"Prepared firmware {version}: {name}, {len(data)} bytes")


if __name__ == "__main__":
    main()
