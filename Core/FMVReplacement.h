// Copyright (c) 2026 PPSSPP Project.
// Licensed under GPL 2.0 or later.

#pragma once

#include <map>
#include <string>
#include <vector>
#include "Common/File/Path.h"

/**
 * FMV Replacement System
 * 
 * Allows users to replace in-game Full Motion Videos (FMVs) with custom versions.
 * Supports MP4, WebM, MKV, and AVI formats via FFmpeg.
 */

class FMVReplacement {
public:
    struct ReplacementEntry {
        std::string originalPath;      // Path to original video in game
        Path replacementFile;           // Path to replacement video file
        int width = 0;                  // Optional: video width
        int height = 0;                 // Optional: video height
        std::string gameId;             // Game ID for this replacement
        bool enabled = true;            // Whether this replacement is active
    };

    FMVReplacement();
    ~FMVReplacement();

    /**
     * Load replacement manifest for a specific game
     * @param gameId PSP game ID (e.g., "ULUS12345")
     * @param gameDir Base directory of the game
     * @return true if manifest loaded successfully
     */
    bool LoadReplacementManifest(const std::string &gameId, const Path &gameDir);

    /**
     * Check if a replacement exists for a given file path
     * @param originalPath Path to the original video file
     * @return true if replacement is available and enabled
     */
    bool HasReplacement(const std::string &originalPath) const;

    /**
     * Get the replacement file for a video
     * @param originalPath Path to the original video file
     * @return Path to replacement file, or empty if none exists
     */
    Path GetReplacementPath(const std::string &originalPath) const;

    /**
     * Get metadata for a replacement
     * @param originalPath Path to the original video file
     * @return Replacement entry with metadata, or nullptr if not found
     */
    const ReplacementEntry* GetReplacementEntry(const std::string &originalPath) const;

    /**
     * Enable or disable all replacements
     * @param enabled true to enable, false to disable
     */
    void SetEnabled(bool enabled) { enabled_ = enabled; }

    /**
     * Check if replacement system is enabled
     * @return true if enabled
     */
    bool IsEnabled() const { return enabled_; }

    /**
     * Clear all loaded replacements
     */
    void Clear();

    /**
     * Get the directory where replacements should be stored
     * @param gameId PSP game ID
     * @return Path to replacements directory
     */
    static Path GetReplacementDirectory(const std::string &gameId);

private:
    std::map<std::string, ReplacementEntry> replacements_;
    bool enabled_ = false;
    std::string currentGameId_;

    /**
     * Parse JSON manifest file
     * @param manifestPath Path to the manifest file
     * @return true if parsed successfully
     */
    bool ParseManifest(const Path &manifestPath);

    /**
     * Normalize file paths for comparison
     * @param path Path to normalize
     * @return Normalized path
     */
    std::string NormalizePath(const std::string &path) const;
};

// Global FMV replacement manager
extern FMVReplacement g_fmvReplacement;
