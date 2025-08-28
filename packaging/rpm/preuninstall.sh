#!/bin/bash
# Pre-uninstallation script for cpp-scaffold RPM package

# Remove symlink
if [ -L "/usr/local/bin/cpp-scaffold" ]; then
    rm -f /usr/local/bin/cpp-scaffold || true
fi

# Remove profile script
PROFILE_FILE="/etc/profile.d/cpp-scaffold.sh"
if [ -f "$PROFILE_FILE" ]; then
    rm -f "$PROFILE_FILE" || true
fi

echo "cpp-scaffold is being removed..."

exit 0
