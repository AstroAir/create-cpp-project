#!/usr/bin/env python3
"""
Asset Pipeline for Game Engine
Processes and optimizes game assets
"""

import os
import sys
import json
import shutil
from pathlib import Path

class AssetProcessor:
    def __init__(self, assets_dir="assets", output_dir="processed_assets"):
        self.assets_dir = Path(assets_dir)
        self.output_dir = Path(output_dir)
        self.manifest = {"assets": []}

    def process_all_assets(self):
        """Process all assets in the assets directory"""
        print("Starting asset processing...")

        if not self.assets_dir.exists():
            print(f"Assets directory '{self.assets_dir}' not found!")
            return False

        # Create output directory
        self.output_dir.mkdir(exist_ok=True)

        # Process different asset types
        self.process_textures()
        self.process_models()
        self.process_audio()
        self.process_shaders()

        # Generate manifest
        self.generate_manifest()

        print("Asset processing complete!")
        return True

    def process_textures(self):
        """Process texture files"""
        texture_dir = self.assets_dir / "textures"
        if not texture_dir.exists():
            return

        output_texture_dir = self.output_dir / "textures"
        output_texture_dir.mkdir(exist_ok=True)

        for texture_file in texture_dir.glob("*"):
            if texture_file.suffix.lower() in ['.png', '.jpg', '.jpeg', '.tga', '.bmp']:
                print(f"Processing texture: {texture_file.name}")
                # Copy for now - in real implementation, would optimize
                shutil.copy2(texture_file, output_texture_dir / texture_file.name)
                self.manifest["assets"].append({
                    "type": "texture",
                    "source": str(texture_file),
                    "output": str(output_texture_dir / texture_file.name)
                })

    def process_models(self):
        """Process 3D model files"""
        model_dir = self.assets_dir / "models"
        if not model_dir.exists():
            return

        output_model_dir = self.output_dir / "models"
        output_model_dir.mkdir(exist_ok=True)

        for model_file in model_dir.glob("*"):
            if model_file.suffix.lower() in ['.obj', '.fbx', '.gltf', '.dae']:
                print(f"Processing model: {model_file.name}")
                shutil.copy2(model_file, output_model_dir / model_file.name)
                self.manifest["assets"].append({
                    "type": "model",
                    "source": str(model_file),
                    "output": str(output_model_dir / model_file.name)
                })

    def process_audio(self):
        """Process audio files"""
        audio_dir = self.assets_dir / "sounds"
        if not audio_dir.exists():
            return

        output_audio_dir = self.output_dir / "sounds"
        output_audio_dir.mkdir(exist_ok=True)

        for audio_file in audio_dir.glob("*"):
            if audio_file.suffix.lower() in ['.wav', '.ogg', '.mp3']:
                print(f"Processing audio: {audio_file.name}")
                shutil.copy2(audio_file, output_audio_dir / audio_file.name)
                self.manifest["assets"].append({
                    "type": "audio",
                    "source": str(audio_file),
                    "output": str(output_audio_dir / audio_file.name)
                })

    def process_shaders(self):
        """Process shader files"""
        shader_dir = self.assets_dir / "shaders"
        if not shader_dir.exists():
            return

        output_shader_dir = self.output_dir / "shaders"
        output_shader_dir.mkdir(exist_ok=True)

        for shader_file in shader_dir.glob("*"):
            if shader_file.suffix.lower() in ['.glsl', '.vert', '.frag', '.hlsl']:
                print(f"Processing shader: {shader_file.name}")
                shutil.copy2(shader_file, output_shader_dir / shader_file.name)
                self.manifest["assets"].append({
                    "type": "shader",
                    "source": str(shader_file),
                    "output": str(output_shader_dir / shader_file.name)
                })

    def generate_manifest(self):
        """Generate asset manifest file"""
        manifest_file = self.output_dir / "asset_manifest.json"
        with open(manifest_file, 'w') as f:
            json.dump(self.manifest, f, indent=2)
        print(f"Generated manifest: {manifest_file}")

def main():
    processor = AssetProcessor()
    success = processor.process_all_assets()
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
