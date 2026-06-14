# FMV Replacement Examples

## Example 1: Final Fantasy VII Crisis Core

### Game ID
`ULUS10410` (US) / `ULES10410` (EU) / `ULJM05043` (JP)

### replacements.json

```json
{
  "game_id": "ULUS10410",
  "game_title": "Crisis Core -Final Fantasy VII-",
  "replacements": [
    {
      "original": "ms0:/PSP_GAME/MOVIE/opening.pmf",
      "replacement": "opening.mp4",
      "width": 1920,
      "height": 1080,
      "enabled": true
    },
    {
      "original": "ms0:/PSP_GAME/MOVIE/ending.pmf",
      "replacement": "ending.mp4",
      "width": 1920,
      "height": 1080,
      "enabled": true
    },
    {
      "original": "ms0:/PSP_GAME/MOVIE/intro01.pmf",
      "replacement": "intro01.mp4",
      "width": 1920,
      "height": 1080,
      "enabled": true
    }
  ]
}
```

## Example 2: Metal Gear Solid: Peace Walker

### Game ID
`ULUS10438` (US)

### Directory Structure
```
memstick/PSP/GAME/ULUS10438/replacements/
├── replacements.json
├── main_opening.mp4
├── intro_codec.mp4
├── ending_day.mp4
└── ending_night.mp4
```

### replacements.json

```json
{
  "game_id": "ULUS10438",
  "game_title": "Metal Gear Solid: Peace Walker",
  "replacements": [
    {
      "original": "ms0:/PSP_GAME/MOVIES/opening.pmf",
      "replacement": "main_opening.mp4",
      "width": 1920,
      "height": 1080,
      "enabled": true
    },
    {
      "original": "ms0:/PSP_GAME/MOVIES/codec_intro.pmf",
      "replacement": "intro_codec.mp4",
      "width": 1280,
      "height": 720,
      "enabled": true
    }
  ]
}
```

## Example 3: Creating Replacements from Scratch

### Step-by-Step Process

#### 1. Find the Video Files in the ISO

```bash
# Extract PSP ISO and find video files
7z x game.iso -o./extracted/
find extracted/ -name "*.pmf" -o -name "*.mpeg"
```

#### 2. Extract Original Video (if needed)

```bash
# Extract PSMF video to standard format
ffmpeg -i video.pmf -c:v libx264 -crf 20 -c:a aac original.mp4
```

#### 3. Prepare Enhanced Version

```bash
# Upscale using AI or filters
ffmpeg -i original.mp4 -vf "scale=1920:1080:flags=lanczos" \
  -c:v libx264 -crf 18 -preset slow enhanced.mp4
```

#### 4. Create Manifest

```json
{
  "game_id": "ULUS12345",
  "game_title": "Your Game",
  "replacements": [
    {
      "original": "ms0:/PSP_GAME/MOVIE/video.pmf",
      "replacement": "enhanced.mp4",
      "width": 1920,
      "height": 1080
    }
  ]
}
```

#### 5. Test in PPSSPP

1. Enable FMV Replacements in settings
2. Load the game
3. Trigger the video playback
4. Verify the replacement plays

## Example 4: Batch Processing Multiple Videos

### Bash Script for Encoding

```bash
#!/bin/bash
# Script to encode all videos in a directory

INPUT_DIR="./original_videos"
OUTPUT_DIR="./replacements"
WIDTH=1920
HEIGHT=1080

mkdir -p "$OUTPUT_DIR"

for video in "$INPUT_DIR"/*.mp4; do
    filename=$(basename "$video")
    echo "Processing $filename..."
    
    ffmpeg -i "$video" \
        -vf "scale=$WIDTH:$HEIGHT:force_original_aspect_ratio=decrease,pad=$WIDTH:$HEIGHT:(ow-iw)/2:(oh-ih)/2" \
        -c:v libx264 -crf 18 -preset slow \
        -c:a aac -b:a 192k \
        "$OUTPUT_DIR/$filename"
done

echo "Done! All videos encoded to $OUTPUT_DIR"
```

### Running the Script

```bash
chmod +x encode_videos.sh
./encode_videos.sh
```

## Example 5: Mixed Resolution Setup

For games with videos of different aspect ratios:

```json
{
  "game_id": "ULUS99999",
  "game_title": "Mixed Aspect Game",
  "replacements": [
    {
      "original": "ms0:/PSP_GAME/MOVIE/fullscreen.pmf",
      "replacement": "fullscreen_1920x1080.mp4",
      "width": 1920,
      "height": 1080,
      "enabled": true
    },
    {
      "original": "ms0:/PSP_GAME/MOVIE/letterbox.pmf",
      "replacement": "letterbox_1920x1440.mp4",
      "width": 1920,
      "height": 1440,
      "enabled": true
    },
    {
      "original": "ms0:/PSP_GAME/MOVIE/pillarbox.pmf",
      "replacement": "pillarbox_1440x1080.mp4",
      "width": 1440,
      "height": 1080,
      "enabled": true
    }
  ]
}
```

## Tips for Best Results

1. **Match Timing**: Ensure replacement video matches original length or is compatible with cutscene timing
2. **Audio Sync**: Use original audio or ensure replacement audio is properly synced
3. **Frame Rate**: Use 29.97 fps or 30 fps for PSP games
4. **Bitrate**: Higher bitrate = better quality but slower on older systems
5. **Testing**: Always test on your target platform before distribution
