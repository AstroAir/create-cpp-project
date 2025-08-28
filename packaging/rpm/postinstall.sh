#!/bin/bash
# Post-installation script for cpp-scaffold RPM package

# Update desktop database
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database -q /usr/share/applications || true
fi

# Update MIME database
if command -v update-mime-database >/dev/null 2>&1; then
    update-mime-database /usr/share/mime || true
fi

# Update icon cache
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
fi

# Add to PATH
PROFILE_FILE="/etc/profile.d/cpp-scaffold.sh"
if [ ! -f "$PROFILE_FILE" ]; then
    cat > "$PROFILE_FILE" << 'EOF'
# Add cpp-scaffold to PATH
if [ -d "/usr/bin" ] && [[ ":$PATH:" != *":/usr/bin:"* ]]; then
    export PATH="/usr/bin:$PATH"
fi
EOF
    chmod 644 "$PROFILE_FILE"
fi

# Create symlink
if [ ! -L "/usr/local/bin/cpp-scaffold" ] && [ -f "/usr/bin/cpp-scaffold" ]; then
    ln -sf /usr/bin/cpp-scaffold /usr/local/bin/cpp-scaffold || true
fi

echo "cpp-scaffold has been successfully installed!"
echo "You can now run 'cpp-scaffold' from anywhere in your terminal."

exit 0
