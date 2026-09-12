#!/usr/bin/env python3
"""Serve and verify a staged RSVP Nano GitHub Pages site below its project path."""
from __future__ import annotations

import argparse
import functools
import http.server
import json
import threading
import urllib.error
import urllib.request
from pathlib import Path


REQUIRED_SITE_FILES = (
    "index.html",
    "service-worker.js",
    "asset-manifest.json",
    "firmware/release.json",
    "firmware/rsvp-nano-esp32-s3-touch-lcd-3.49-rev2.bin",
)


def get(url: str) -> tuple[int, bytes]:
    try:
        with urllib.request.urlopen(url) as response:
            return response.status, response.read()
    except urllib.error.HTTPError as error:
        return error.code, error.read()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--site", type=Path, required=True, help="Staged Pages directory.")
    parser.add_argument("--base-path", required=True, help="GitHub Pages project path, e.g. /rsvp-nano-tweaks/.")
    args = parser.parse_args()

    site = args.site.resolve()
    base_path = args.base_path
    if not base_path.startswith("/") or not base_path.endswith("/") or base_path == "/":
        parser.error("--base-path must be a non-root slash-delimited project path")

    missing = [name for name in REQUIRED_SITE_FILES if not (site / name).is_file()]
    if missing:
        raise SystemExit("Staged Pages site is missing: " + ", ".join(missing))

    release = json.loads((site / "firmware/release.json").read_text(encoding="utf-8"))
    rev2 = release.get("firmware", {}).get("lcd349-v2")
    if rev2 != "rsvp-nano-esp32-s3-touch-lcd-3.49-rev2.bin":
        raise SystemExit("release.json does not map lcd349-v2 to the required Rev2 full binary")

    class ProjectPagesHandler(http.server.SimpleHTTPRequestHandler):
        def translate_path(self, path: str) -> str:
            # GitHub Pages strips the repository project path before serving files.
            if not path.startswith(base_path):
                return str(site / "__outside_project_path__")
            return super().translate_path(path.removeprefix(base_path))

    handler = functools.partial(ProjectPagesHandler, directory=str(site))
    server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), handler)
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    try:
        root = f"http://127.0.0.1:{server.server_port}"
        # A project Pages URL maps /<repository>/ to the published artifact root.
        checks = {
            f"{base_path}": "index.html",
            f"{base_path}service-worker.js": "service-worker.js",
            f"{base_path}asset-manifest.json": "asset-manifest.json",
            f"{base_path}firmware/release.json": "firmware/release.json",
            f"{base_path}firmware/{rev2}": f"firmware/{rev2}",
        }
        for page_path, local_path in checks.items():
            status, body = get(root + page_path)
            expected = (site / local_path).read_bytes()
            if status != 200 or body != expected:
                raise SystemExit(f"Pages smoke check failed for {page_path}: HTTP {status}")
    finally:
        server.shutdown()
        thread.join()
        server.server_close()

    print(f"Pages smoke check passed for {base_path} (including Rev2 full firmware)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
