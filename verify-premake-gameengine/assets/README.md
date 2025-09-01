# verify-premake-gameengine Assets

This directory contains all the assets for the verify-premake-gameengine game engine project.

## Directory Structure

```
assets/
├── textures/       # Texture files (.png, .jpg, .tga, etc.)
├── models/         # 3D model files (.obj, .fbx, .gltf, etc.)
├── sounds/         # Audio files (.wav, .ogg, .mp3, etc.)
├── music/          # Background music files
├── shaders/        # Shader programs (.glsl, .hlsl, etc.)
├── fonts/          # Font files (.ttf, .otf, etc.)
├── scenes/         # Scene definition files (.json, .xml, etc.)
├── scripts/        # Game scripts (.lua, .py, etc.)
└── config/         # Configuration files

```

## Asset Guidelines

### Textures
- Use power-of-2 dimensions when possible (256x256, 512x512, 1024x1024, etc.)
- Prefer PNG for images with transparency
- Use JPG for photographs and images without transparency
- Keep texture sizes reasonable for target platforms

### Models
- Use consistent units (meters recommended)
- Optimize polygon count for target performance
- Include proper UV mapping
- Export with materials when possible

### Audio
- Use uncompressed formats (WAV) for short sound effects
- Use compressed formats (OGG) for longer audio files
- Normalize audio levels
- Use appropriate sample rates (44.1kHz for most cases)

### Shaders
- Follow consistent naming conventions
- Include comments explaining complex operations
- Test on target graphics APIs
- Provide fallback shaders for older hardware

## Asset Pipeline

The project includes an automated asset pipeline that:
- Validates asset formats
- Optimizes assets for target platforms
- Generates asset manifests
- Handles asset dependencies

Run the asset pipeline with:
```bash
python tools/asset_pipeline/process_assets.py
```

## Adding New Assets

1. Place assets in the appropriate subdirectory
2. Update any relevant configuration files
3. Run the asset pipeline to process new assets
4. Test assets in the engine

## Asset Naming Conventions

- Use lowercase with underscores: `player_texture.png`
- Include descriptive names: `explosion_sound_01.wav`
- Use consistent prefixes for related assets: `ui_button_normal.png`, `ui_button_hover.png`
