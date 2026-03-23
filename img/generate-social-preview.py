#!/usr/bin/env python3
"""Generate the GitHub social preview PNG (1280x640) from social-preview.svg."""
import os, subprocess
DIR = os.path.dirname(os.path.abspath(__file__))
SVG_PATH = os.path.join(DIR, "social-preview.svg")
PNG_PATH = os.path.join(DIR, "social-preview.png")
subprocess.run(["inkscape", SVG_PATH, "--export-type=png", f"--export-filename={PNG_PATH}", "--export-width=1280", "--export-height=640"], check=True, capture_output=True)
print(f"Saved {PNG_PATH}")
