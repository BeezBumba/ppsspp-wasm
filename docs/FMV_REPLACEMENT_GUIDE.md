# FMV Replacement Guide for PPSSPP

## Overview

The FMV (Full Motion Video) Replacement system allows you to replace in-game PSP videos with higher quality alternatives. This is useful for:

- Upscaling low-quality in-game videos
- Replacing videos with fan-made or enhanced versions
- Adding custom cinematic sequences
- Improving visual fidelity of cutscenes

## Supported Video Formats

- **MP4** (H.264/H.265) - Recommended for compatibility
- **WebM** (VP8/VP9)
- **MKV** (Matroska)
- **AVI** (for legacy support)

## Directory Structure

Replacements should be organized in the following directory structure:

```
memstick/
└── PSP/
    └── GAME/
        └── [GAME_ID]/              # PSP game ID (e.g., ULUS12345)
            └── replacements/
                ├── replacements.json  # Manifest file
                ├── opening.mp4
                ├── ending.mp4
                └── ...
```

### Finding Your Game ID

The Game ID can be found in:
1. PARAM.SFO file in PSP_GAME directory
2. Game properties in PPSSPP
3. PPSSPP log files

## Replacement Manifest (replacements.json)

Create a `replacements.json` file in your game's `replacements/` directory:

```json
{
  "game_id": "ULUS12345",
  "game_title": "Your Game Title",
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
    }
  ]
}
```

### Manifest Fields

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `game_id` | string | Yes | PSP Game ID |
| `game_title` | string | No | Human-readable game title |
| `original` | string | Yes | Path to original video in game |
| `replacement` | string | Yes | Filename of replacement (relative to replacements/ dir) |
| `width` | integer | No | Video width in pixels |
| `height` | integer | No | Video height in pixels |
| `enabled` | boolean | No | Enable/disable this replacement (default: true) |

## Finding Original Video Paths

1. **Enable logging** in PPSSPP settings
2. **Play the game** and watch cutscenes
3. **Check logs** for entries like:
   ```
   scePsmfPlayer: Loading video from: ms0:/PSP_GAME/MOVIE/opening.pmf
   ```
4. **Use these paths** in your replacements.json

## Video Preparation Tips

### Recommended Settings

- **Format**: MP4 with H.264 codec
- **Resolution**: 1920x1080 or native PSP resolution (480x272)
- **Frame Rate**: 29.97 fps or 30 fps
- **Bitrate**: 5000-8000 kbps for good quality
- **Audio**: AAC codec, 48 kHz, 192 kbps

### FFmpeg Encoding Example

```bash
# Upscale and encode a video
ffmpeg -i original_video.mp4 -vf scale=1920:1080 -c:v libx264 \
  -crf 18 -preset slow -c:a aac -b:a 192k output.mp4

# Convert any format to MP4
ffmpeg -i input.avi -c:v libx264 -crf 20 -c:a aac -b:a 192k output.mp4
```

## Enabling FMV Replacements

1. **Copy replacement files** to the appropriate directory
2. **Enable in settings**:
   - Open PPSSPP Settings
   - Navigate to Developer Settings
   - Enable "FMV Replacements"
3. **Load the game** - replacements will be loaded automatically

## Troubleshooting

### Replacements not loading

- Check that manifest file is in correct location: `memstick/PSP/GAME/[GAMEID]/replacements/replacements.json`
- Verify JSON syntax using a JSON validator
- Enable logging and check for error messages
- Ensure replacement video files exist and are readable

### Video not playing correctly

- Check that video format is supported (MP4, WebM, MKV, AVI)
- Verify codec is compatible (H.264, H.265, VP8, VP9)
- Try re-encoding with recommended FFmpeg settings above
- Check that width/height in manifest match actual video

### Performance issues

- Use lower resolution videos (1920x1080 or lower)
- Reduce bitrate while maintaining quality
- Use H.264 codec instead of H.265 for better compatibility
- Check CPU usage - video decoding is CPU-intensive

## Advanced Usage

### Per-Game Configurations

You can have different replacements for different versions of the same game:

```
memstick/PSP/GAME/
├── ULUS12345/    # US version
│   └── replacements/
│       └── replacements.json
├── ULES12345/    # EU version
│   └── replacements/
│       └── replacements.json
└── ULJS12345/    # JP version
    └── replacements/
        └── replacements.json
```

### Disabling Specific Replacements

Set `"enabled": false` in replacements.json to temporarily disable a replacement:

```json
{
  "original": "ms0:/PSP_GAME/MOVIE/intro.pmf",
  "replacement": "intro.mp4",
  "enabled": false
}
```

## Technical Details

### How It Works

1. When a game requests a video file, PPSSPP checks the replacement manifest
2. If a replacement is found and enabled, the replacement video is loaded instead
3. FFmpeg decodes the video using its built-in codec support
4. Video is displayed with original audio (or replacement audio if provided)

### Integration Points

- **MediaEngine**: Primary video decoding layer
- **FileLoader**: File system abstraction for loading videos
- **scePsmf**: PSP PSMF module emulation
- **sceMpeg**: PSP MPEG codec emulation

## Performance Considerations

- FMV replacement uses the same decoding as original videos
- Higher resolution videos require more CPU/GPU
- Seek operations may be slower with replacement videos
- Memory usage depends on video resolution and length

## Compatibility

FMV replacements work with:
- All game titles that use standard PSP video formats
- PSMF (PlayStation Stream Format) videos
- MPEG videos
- Most in-game cutscenes and cinematics

May not work with:
- Real-time rendered videos
- Audio that's not stored separately
- Proprietary video formats

## Support and Contributing

For issues, suggestions, or to share replacement packs:

- GitHub Issues: https://github.com/hrydgard/ppsspp/issues
- Discord: https://discord.gg/5NJB6dD
- Community Forum: https://forums.ppsspp.org/

## License

This feature is part of PPSSPP and is licensed under GPL 2.0 or later.
