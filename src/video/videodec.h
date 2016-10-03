#ifndef _PSP2_VIDEODEC_H_
#define _PSP2_VIDEODEC_H_
// https://gitlab.slkdev.net/RPCS3/rpcs3/blob/0e5c54709d7fa2a76b8944ca999b5b28a722be31/rpcs3/Emu/ARMv7/Modules/sceVideodec.h
enum {
    SCE_VIDEODEC_TYPE_HW_AVCDEC = 0x1001,
};

typedef struct SceVideodecQueryInitInfoHwAvcdec {
  uint32_t size;
  uint32_t horizontal;
  uint32_t vertical;
  uint32_t numOfRefFrames;
  uint32_t numOfStreams;
} SceVideodecQueryInitInfoHwAvcdec;

typedef struct SceAvcdecQueryDecoderInfo {
  uint32_t horizontal;
  uint32_t vertical;
  uint32_t numOfRefFrames;
} SceAvcdecQueryDecoderInfo;

typedef struct SceAvcdecDecoderInfo {
  uint32_t frameMemSize;
} SceAvcdecDecoderInfo;

struct SceAvcdecBuf {
  void *pBuf;
  uint32_t size;
};

typedef struct SceAvcdecCtrl {
  uint32_t handle;
  struct SceAvcdecBuf frameBuf;
} SceAvcdecCtrl;

struct SceVideodecTimeStamp {
  uint32_t upper;
  uint32_t lower;
};

typedef struct SceAvcdecAu {
  struct SceVideodecTimeStamp pts;
  struct SceVideodecTimeStamp dts;
  struct SceAvcdecBuf es;
} SceAvcdecAu;

struct SceAvcdecFrameOptionRGBA {
  uint8_t alpha;
  uint8_t cscCoefficient;
  uint8_t reserved[14];
};

union SceAvcdecFrameOption {
  uint8_t reserved[16];
  struct SceAvcdecFrameOptionRGBA rgba;
};

struct SceAvcdecFrame {
  uint32_t pixelType;
  uint32_t framePitch;
  uint32_t frameWidth;
  uint32_t frameHeight;

  uint32_t horizontalSize;
  uint32_t verticalSize;

  uint32_t frameCropLeftOffset;
  uint32_t frameCropRightOffset;
  uint32_t frameCropTopOffset;
  uint32_t frameCropBottomOffset;

  union SceAvcdecFrameOption opt;

  void *pPicture[2];
};

struct SceAvcdecInfo {
  uint32_t numUnitsInTick;
  uint32_t timeScale;
  uint8_t fixedFrameRateFlag;

  uint8_t aspectRatioIdc;
  uint16_t sarWidth;
  uint16_t sarHeight;

  uint8_t colourPrimaries;
  uint8_t transferCharacteristics;
  uint8_t matrixCoefficients;

  uint8_t videoFullRangeFlag;

  uint8_t padding[3];

  struct SceVideodecTimeStamp pts;
};

typedef struct SceAvcdecPicture {
  uint32_t size;
  struct SceAvcdecFrame frame;
  struct SceAvcdecInfo info;
} SceAvcdecPicture;

typedef struct SceAvcdecArrayPicture {
  uint32_t numOfOutput;
  uint32_t numOfElm;
  SceAvcdecPicture **pPicture;
} SceAvcdecArrayPicture;

SceInt32 sceVideodecInitLibrary(SceUInt32 codecType, const SceVideodecQueryInitInfoHwAvcdec *pInitInfo);
SceInt32 sceAvcdecQueryDecoderMemSize(SceUInt32 codecType, const SceAvcdecQueryDecoderInfo *pDecoderInfo, SceAvcdecDecoderInfo *pMemInfo);
SceInt32 sceAvcdecCreateDecoder(SceUInt32 codecType, SceAvcdecCtrl *pCtrl, const SceAvcdecQueryDecoderInfo *pDecoderInfo);
SceInt32 sceAvcdecDecode(SceAvcdecCtrl *pCtrl, const SceAvcdecAu *pAu, SceAvcdecArrayPicture *pArrayPicture);
#endif
