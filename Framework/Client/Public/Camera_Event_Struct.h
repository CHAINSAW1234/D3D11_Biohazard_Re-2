#pragma once
#include "Client_Defines.h"

BEGIN(Client)
struct Header {
    uint32_t version;
    uint32_t magic;
    uint64_t uknLong1, idxOffset, uknLong3, uknLong4;
    uint32_t mcamCount;
    wstring  name;
    vector<uint64_t> mcamEntry;
};

struct TrackHeader {
    uint64_t zero;
    uint16_t s1, s2;
    uint32_t numFrames, framerate;
    _float EndFrame;
    uint64_t FramesOffset, DataOffset;
};

struct MCAMHeader {
    uint32_t version;
    uint32_t magic;
    uint64_t offsets[5];
    uint16_t uknShort, frameRate;
    _float frameCount, blending;
    std::wstring name3;
    TrackHeader TranslateHeader, RotationHeader, ZoomHeader;
};

struct MCAMIndex {
    uint32_t ukn1, ukn2;
    uint16_t Index, Switch;
    uint32_t ukn3, ukn4, ukn5;
};

typedef struct MCAM {
    MCAMHeader CAMHeader;
    vector<uint16_t> TranslationFrame;
    vector<_float3> Translations;

    vector<uint16_t> RotationFrame;
    vector<_float4> Rotations;

    vector<uint16_t> ZoomFrame;
    vector<_float3> Zooms;
} MCam;

inline void skipToNextLine(std::ifstream& file) {
    char byte;
    while (file.tellg() % 16 != 0 && file.read(&byte, 1) && byte == 0);
}

inline wstring readWString(std::ifstream& file) {
    std::wstring result;
    wchar_t ch;
    while (file.read(reinterpret_cast<char*>(&ch), sizeof(wchar_t)) && ch != 0) {
        result += ch;
    }
    return result;
}

END