#!/usr/bin/env bash
set -euo pipefail

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
PKG_ROOT="$ROOT/packaging/vsk"
VERSION="0.3.0"
ARCH="${VSK_DEB_ARCH:-amd64}"
OUT="$ROOT/dist"

rm -rf "$PKG_ROOT/usr/bin" "$PKG_ROOT/usr/lib" "$PKG_ROOT/usr/share/doc/vsk" "$PKG_ROOT/usr/share/vsk"
mkdir -p "$PKG_ROOT/usr/bin" "$PKG_ROOT/usr/lib/vsk" "$PKG_ROOT/usr/share/doc/vsk" "$PKG_ROOT/usr/share/vsk/examples" "$OUT"

gcc -std=c11 -O2 -pipe -fPIE -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wformat -Werror=format-security "$ROOT/vsk.c" -lm -pie -o "$PKG_ROOT/usr/lib/vsk/vskc"

cat > "$PKG_ROOT/usr/bin/vsk" <<'LAUNCHER'
#!/bin/sh
set -eu
exec /usr/lib/vsk/vskc "$@"
LAUNCHER
chmod 0755 "$PKG_ROOT/usr/bin/vsk" "$PKG_ROOT/usr/lib/vsk/vskc"

cp "$ROOT/README.md" "$ROOT/VSK_SPEC.md" "$PKG_ROOT/usr/share/doc/vsk/"
cp "$ROOT/demo_independent.vsk" "$PKG_ROOT/usr/share/vsk/examples/hello.vk"
chmod 0644 "$PKG_ROOT/usr/share/doc/vsk/"* "$PKG_ROOT/usr/share/vsk/examples/hello.vk"

sed -i "s/^Version: .*/Version: $VERSION/; s/^Architecture: .*/Architecture: $ARCH/" "$PKG_ROOT/DEBIAN/control"
dpkg-deb --build --root-owner-group "$PKG_ROOT" "$OUT/vsk_${VERSION}_${ARCH}.deb"
dpkg-deb --info "$OUT/vsk_${VERSION}_${ARCH}.deb"
printf 'Built %s\n' "$OUT/vsk_${VERSION}_${ARCH}.deb"
