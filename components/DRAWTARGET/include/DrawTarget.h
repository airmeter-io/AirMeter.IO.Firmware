#pragma once
#include "Common.h"



typedef struct {
   int32_t width;
   int32_t height;
} Dimensions;

typedef struct {
   int32_t x;
   int32_t y;
} Position;

typedef struct {
   Position pos;
   Dimensions size;
} Rectangle;


enum DrawColor : uint32_t  { Black = 0, White = 0xFFFFFF };

class DrawTarget {
public:
    virtual ~DrawTarget()  {}
    virtual void BltMonoBitmap(uint8_t* pData, Dimensions pBitmapSize, Position pDrawAt, Rectangle pClipRectangle, DrawColor pColor) = 0;
    virtual void DrawFilledRectangle(int16_t pX, int16_t pY, int16_t pW, int16_t pH, DrawColor pColor) = 0;
    virtual void DrawPixel(int16_t x, int16_t y, DrawColor color)  = 0;
    virtual void SetRotation(int16_t pRotation) = 0;
    virtual void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, DrawColor color) = 0;
    virtual void DrawDottedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, DrawColor color) = 0;    
    virtual void Fill(DrawColor pColor) = 0;
};

class DrawControl {
public:
    virtual ~DrawControl()  {}

    virtual void RenderToDisplay(bool pFastUpdate) = 0;
    virtual DrawTarget* GetDrawTarget() = 0;
};

class NullDrawTarget : public DrawTarget {
public:
    NullDrawTarget();
    ~NullDrawTarget();
    void BltMonoBitmap(uint8_t* pData, Dimensions pBitmapSize, Position pDrawAt, Rectangle pClipRectangle, DrawColor pColor) override;
    void DrawFilledRectangle(int16_t pX, int16_t pY, int16_t pW, int16_t pH, DrawColor pColor) override;
    void DrawPixel(int16_t x, int16_t y, DrawColor color)  override;
    void SetRotation(int16_t pRotation) override;
    void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, DrawColor color) override;
    void DrawDottedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, DrawColor color) override;    
    void Fill(DrawColor pColor) override;
};

class NullDrawControl : public DrawControl {
    NullDrawTarget* _drawTarget;
public:
    NullDrawControl();
    ~NullDrawControl();

    void RenderToDisplay(bool pFastUpdate) override;
    DrawTarget* GetDrawTarget() override;
};
