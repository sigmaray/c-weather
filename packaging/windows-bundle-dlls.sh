#!/usr/bin/env bash
# Copy MinGW DLLs required by a PE binary into DEST dir (portable Windows package).
set -euo pipefail

bin="${1:?usage: $0 <exe> <dest-dir>}"
dest="${2:?usage: $0 <exe> <dest-dir>}"

mkdir -p "$dest"
cp -f "$bin" "$dest/"

# Resolve mingw bin dir from the linked DLL paths.
mapfile -t dlls < <(ldd "$bin" | awk '/\/mingw64\/bin\// { print $3 }' | sort -u)
for dll in "${dlls[@]}"; do
  cp -f "$dll" "$dest/"
done

# GDK pixbuf loaders (loaded at runtime, not always in ldd).
mingw_root="$(cygpath -u "${MINGW_PREFIX:-/mingw64}" 2>/dev/null || echo /mingw64)"
loader_dir="$mingw_root/lib/gdk-pixbuf-2.0"
if [[ -d "$loader_dir" ]]; then
  mkdir -p "$dest/lib/gdk-pixbuf-2.0"
  cp -a "$loader_dir/." "$dest/lib/gdk-pixbuf-2.0/"
fi

echo "Packaged $(basename "$bin") + ${#dlls[@]} DLLs into $dest"
