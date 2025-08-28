# Resources Directory

This directory contains resources needed for packaging and distribution:

## Icons
- `icon.ico` - Windows icon (256x256, 128x128, 64x64, 48x48, 32x32, 16x16)
- `icon.icns` - macOS icon bundle
- `icon.png` - Linux icon (256x256 PNG)

## Packaging Resources
- `Info.plist` - macOS bundle information
- `entitlements.plist` - macOS security entitlements
- `cpp-scaffold.desktop.in` - Linux desktop entry template
- `dmg_setup.scpt` - macOS DMG setup script
- `banner.bmp` - Windows installer banner (493x58)
- `dialog.bmp` - Windows installer dialog (493x312)
- `dmg_background.png` - macOS DMG background image

## Creating Icons

### Windows (.ico)
Use a tool like GIMP or online converters to create a multi-resolution ICO file:
```bash
# Using ImageMagick
convert icon.png -define icon:auto-resize=256,128,64,48,32,16 icon.ico
```

### macOS (.icns)
Use the `iconutil` command on macOS:
```bash
# Create iconset directory
mkdir icon.iconset
# Add different sizes (16x16@1x, 16x16@2x, 32x32@1x, etc.)
# Then convert to icns
iconutil -c icns icon.iconset
```

### Linux (.png)
Use a 256x256 PNG file with transparent background.

## Banner Images

### Windows Installer
- Banner: 493x58 pixels, BMP format
- Dialog: 493x312 pixels, BMP format

### macOS DMG
- Background: 520x320 pixels, PNG format with transparency

## Notes
- All images should use the project's branding colors
- Icons should be recognizable at small sizes (16x16)
- Use vector graphics when possible for scalability
- Test icons on different backgrounds (light/dark themes)
