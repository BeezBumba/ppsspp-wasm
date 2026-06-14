// Copyright (c) 2026 PPSSPP Project.
// Licensed under GPL 2.0 or later.

#include "Core/FMVReplacement.h"
#include "Common/File/FileUtil.h"
#include "Common/File/Path.h"
#include "Common/Log.h"
#include "Common/StringUtils.h"
#include <algorithm>
#include <cctype>
#include <fstream>

// Simple JSON parsing (without external dependency)
#include <sstream>

FMVReplacement g_fmvReplacement;

FMVReplacement::FMVReplacement() : enabled_(false) {
}

FMVReplacement::~FMVReplacement() {
    Clear();
}

bool FMVReplacement::LoadReplacementManifest(const std::string &gameId, const Path &gameDir) {
    if (gameId.empty() || !gameDir.IsAbsolute()) {
        return false;
    }

    currentGameId_ = gameId;
    Path replacementDir = GetReplacementDirectory(gameId);
    Path manifestPath = replacementDir / "replacements.json";

    if (!File::Exists(manifestPath)) {
        DEBUG_LOG(Log::ME, "FMV replacement manifest not found: %s", manifestPath.c_str());
        return false;
    }

    INFO_LOG(Log::ME, "Loading FMV replacements from: %s", manifestPath.c_str());
    return ParseManifest(manifestPath);
}

bool FMVReplacement::HasReplacement(const std::string &originalPath) const {
    if (!enabled_) {
        return false;
    }

    std::string normalized = NormalizePath(originalPath);
    auto it = replacements_.find(normalized);
    return it != replacements_.end() && it->second.enabled;
}

Path FMVReplacement::GetReplacementPath(const std::string &originalPath) const {
    if (!enabled_) {
        return Path();
    }

    std::string normalized = NormalizePath(originalPath);
    auto it = replacements_.find(normalized);
    if (it != replacements_.end() && it->second.enabled) {
        return it->second.replacementFile;
    }

    return Path();
}

const FMVReplacement::ReplacementEntry* FMVReplacement::GetReplacementEntry(
    const std::string &originalPath) const {
    if (!enabled_) {
        return nullptr;
    }

    std::string normalized = NormalizePath(originalPath);
    auto it = replacements_.find(normalized);
    if (it != replacements_.end() && it->second.enabled) {
        return &it->second;
    }

    return nullptr;
}

void FMVReplacement::Clear() {
    replacements_.clear();
    currentGameId_.clear();
}

Path FMVReplacement::GetReplacementDirectory(const std::string &gameId) {
    // Return: memstick/PSP/GAME/[GAMEID]/replacements/
    Path baseDir = File::GetExeDirectory();
    return baseDir / "memstick" / "PSP" / "GAME" / gameId / "replacements";
}

std::string FMVReplacement::NormalizePath(const std::string &path) const {
    std::string normalized = path;

    // Convert to lowercase
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    // Replace backslashes with forward slashes
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    // Remove leading/trailing whitespace
    normalized.erase(0, normalized.find_first_not_of(" \t\n\r"));
    normalized.erase(normalized.find_last_not_of(" \t\n\r") + 1);

    return normalized;
}

bool FMVReplacement::ParseManifest(const Path &manifestPath) {
    std::ifstream file(manifestPath.ToString(), std::ios::binary);
    if (!file.is_open()) {
        ERROR_LOG(Log::ME, "Failed to open replacement manifest: %s", manifestPath.c_str());
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    // Simple JSON parsing - look for replacement entries
    // Expected format:
    // {
    //   "replacements": [
    //     {
    //       "original": "path/to/video.pmf",
    //       "replacement": "opening.mp4",
    //       "width": 480,
    //       "height": 272
    //     }
    //   ]
    // }

    replacements_.clear();
    size_t pos = 0;
    Path replacementDir = GetReplacementDirectory(currentGameId_);

    while ((pos = content.find("\"original\"", pos)) != std::string::npos) {
        // Extract original path
        size_t startQuote = content.find('"', pos + 11);
        if (startQuote == std::string::npos) break;
        size_t endQuote = content.find('"', startQuote + 1);
        if (endQuote == std::string::npos) break;

        std::string originalPath = content.substr(startQuote + 1, endQuote - startQuote - 1);

        // Extract replacement filename
        size_t replPos = content.find("\"replacement\"", pos);
        if (replPos == std::string::npos || replPos > content.find(',', endQuote)) {
            pos = endQuote;
            continue;
        }

        startQuote = content.find('"', replPos + 14);
        if (startQuote == std::string::npos) break;
        endQuote = content.find('"', startQuote + 1);
        if (endQuote == std::string::npos) break;

        std::string replacementFile = content.substr(startQuote + 1, endQuote - startQuote - 1);
        Path fullReplacementPath = replacementDir / replacementFile;

        if (!File::Exists(fullReplacementPath)) {
            WARN_LOG(Log::ME, "Replacement file not found: %s", fullReplacementPath.c_str());
            pos = endQuote;
            continue;
        }

        ReplacementEntry entry;
        entry.originalPath = originalPath;
        entry.replacementFile = fullReplacementPath;
        entry.gameId = currentGameId_;
        entry.enabled = true;

        // Try to extract width and height if present
        size_t widthPos = content.find("\"width\"", pos);
        if (widthPos != std::string::npos && widthPos < content.find(',', endQuote)) {
            size_t colonPos = content.find(':', widthPos);
            std::string widthStr;
            for (size_t i = colonPos + 1; i < content.length() && std::isdigit(content[i]); ++i) {
                widthStr += content[i];
            }
            if (!widthStr.empty()) {
                entry.width = std::stoi(widthStr);
            }
        }

        std::string normalized = NormalizePath(originalPath);
        replacements_[normalized] = entry;

        INFO_LOG(Log::ME, "Added FMV replacement: %s -> %s", originalPath.c_str(),
                 fullReplacementPath.c_str());

        pos = endQuote;
    }

    INFO_LOG(Log::ME, "Loaded %zu FMV replacements", replacements_.size());
    return !replacements_.empty();
}
