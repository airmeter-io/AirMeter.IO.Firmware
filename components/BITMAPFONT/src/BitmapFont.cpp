#include "BitmapFont.h"
#include <stdio.h>
#include <vector>

#define MAGIC 0xEDB00BFC

BitmapFont::BitmapFont(const std::string& pPath) : BitmapFont(pPath.c_str()) {

}
BitmapFont::BitmapFont(const char *pPath) : _groups(nullptr) {
    FILE* f=fopen(pPath, "rb");
    if(!f) {
        fclose(f);
        return;
    }
    if(fread(&_header,sizeof(_header), 1 , f)!=1)  {
        fclose(f);
        return;
    }

    if(_header.magic!=MAGIC) {
         fclose(f);
        return;
    }
    auto minY = -1;
    auto maxY = -1;
    _groups = (BitmapFontCharacterGroup*)calloc(sizeof(BitmapFontCharacterGroup), _header.numGroups);
    for(auto i = 0; i < _header.numGroups; i++) {
        if(fread(&_groups[i].groupHeader,sizeof(_groups[i].groupHeader), 1 , f)!=1)  {
            fclose(f);
            return;
        }

        _groups[i].glyphs = (BitmapFontCharacterGroupGlyph*)calloc(
            sizeof(BitmapFontCharacterGroupGlyph)
            ,_groups[i].groupHeader.numCodePoints);
        
        for(auto j = 0; j < _groups[i].groupHeader.numCodePoints;j++) {

            if(fread(&_groups[i].glyphs[j].glyph,sizeof(_groups[i].glyphs[j].glyph), 1 , f)!=1)  {
                fclose(f);
                return;
            }
            if(minY == -1 || minY > _groups[i].glyphs[j].glyph.yOffset)
                minY = _groups[i].glyphs[j].glyph.yOffset;
            if(maxY == -1 || maxY < _groups[i].glyphs[j].glyph.yOffset+_groups[i].glyphs[j].glyph.height)
                maxY =  _groups[i].glyphs[j].glyph.yOffset+_groups[i].glyphs[j].glyph.height;        
        }
        _groups[i].data = (uint8_t*)calloc(1, _groups[i].groupHeader.dataLength);
        if(fread(_groups[i].data, _groups[i].groupHeader.dataLength, 1 , f)!=1)  {
            fclose(f);
            return;
        }

        uint16_t offset = 0;
        for(auto j = 0; j < _groups[i].groupHeader.numCodePoints;j++) {
            auto bitsSize = _groups[i].glyphs[j].glyph.width*_groups[i].glyphs[j].glyph.height;
            _groups[i].glyphs[j].offset = offset;
            offset += bitsSize % 8 > 0 ? bitsSize/8+1 : bitsSize/8;            
        }
    }

    _header.yAdvance = maxY - minY;
    _yOffset = minY;
    fclose(f);
}

BitmapFont::~BitmapFont() {
    if(_groups) { 
        for(auto grp = 0; grp < _header.numGroups; grp++)
        {
            if(_groups[grp].data)
                free(_groups[grp].data);
            _groups[grp].data = nullptr;        
        }
        free(_groups);
        _groups = nullptr;
    }
}

bool BitmapFont::GetGlyph(uint32_t pCodePoint, FontCharacterGroupGlyph** pGlyph, uint8_t** pData) {
    for(auto grp = 0; grp < _header.numGroups; grp++) {
        auto group = _groups+grp;    
        if(pCodePoint>=group->groupHeader.codePointStart && pCodePoint<=group->groupHeader.codePointStart+group->groupHeader.numCodePoints) {
            auto glyph = group->glyphs+(pCodePoint-group->groupHeader.codePointStart);
            *pGlyph = &glyph->glyph;
            *pData = group->data+glyph->offset;
            return true;
        }
    }
    return false;
}

void BitmapFont::MeasureUtf8(const tiny_utf8::string& pText, uint32_t pWrapTextAt, Dimensions& pMeasuredSized) {
    pMeasuredSized.height = _header.yAdvance;
    pMeasuredSized.width = 0;
    
    auto curWidth = 0;
    auto sinceLastSpace = 0;
    for(auto ch : pText) {
        FontCharacterGroupGlyph* glyph;
        uint8_t* data;
        if(GetGlyph(ch,&glyph, &data)) {           
            if(pWrapTextAt == 0 || pWrapTextAt > curWidth+glyph->xAdvance) {
               curWidth+=glyph->xAdvance;
               if(curWidth>pMeasuredSized.width) 
                  pMeasuredSized.width = curWidth;
            } else
            {
                pMeasuredSized.height+=_header.yAdvance;
               
                if(sinceLastSpace>0) {
                    curWidth = sinceLastSpace+glyph->xAdvance;
                } else
                    curWidth = glyph->xAdvance;
                if(curWidth>pMeasuredSized.width)
                    pMeasuredSized.width = curWidth;
                sinceLastSpace = 0;
            }

            if(ch == ' ')
                sinceLastSpace = 0;
            else   
                sinceLastSpace+=glyph->xAdvance;            
        }
    }    
}

typedef struct {
    uint16_t startIndex;
    uint16_t count;
    uint16_t width;
} LineInfo;

void BitmapFont::DrawUtf8(
    const tiny_utf8::string& pText, 
    DrawTarget& pTarget,
    Rectangle pDrawAt, 
    DrawColor pColor, 
    DrawTextJustify pJustify, 
    DrawTextVerticalAlign pVerticalAlign,
    uint pLineSpacing)
{
    auto length = pText.size();
    FontCharacterGroupGlyph* glyphs[length];
    uint8_t* datas[length];
    std::vector<LineInfo> lines;
    auto index = 0;
    auto curWidth = 0;
    auto sinceLastSpace = 0;
    auto lastSpace = -1;
    auto lineStart = 0;
    auto widthAtLastSpace = 0;
    auto height = _header.yAdvance;
    auto minY = 0;
    auto maxY = 0;
    for(auto ch : pText) {
        if(GetGlyph(ch,glyphs+index,datas+index)) {
            if(lineStart==0 && glyphs[index]->yOffset<minY)
                minY= glyphs[index]->yOffset;
            if(glyphs[index]->yOffset+glyphs[index]->height>maxY)
                maxY=glyphs[index]->yOffset+glyphs[index]->height;
            if(ch==' ')
                widthAtLastSpace = curWidth;
            if(pDrawAt.size.width == 0 || pDrawAt.size.width > curWidth+glyphs[index]->xAdvance)
               curWidth+=glyphs[index]->xAdvance;
            else
            {
                auto lineEnd = lastSpace == -1 ? index : lastSpace;
                lines.push_back({
                    .startIndex =  (uint16_t)lineStart,
                    .count = (uint16_t)(lineEnd - lineStart - 1),
                    .width = (uint16_t)(lastSpace == -1 ? curWidth : widthAtLastSpace)
                });
                lineStart = lastSpace == -1 ? index : lastSpace -1;
                
                
                if(sinceLastSpace>0) {
                    curWidth = sinceLastSpace+glyphs[index]->xAdvance;
                } else
                    curWidth = glyphs[index]->xAdvance;
                sinceLastSpace = 0;
            }

            if(ch == ' ') {
                sinceLastSpace = 0;
                lastSpace = index;
            } else   
                sinceLastSpace+=glyphs[index]->xAdvance;
            
            //pTarget.BltMonoBitmap(data, glyphs[index]->width, glyph->height, x+glyph->xOffset, pY+glyph->yOffset,pColor);
            //x+=glyph->xAdvance;
        } else 
            glyphs[index] = nullptr;
        index++;
    }


    if(index > lineStart) {
        lines.push_back({
            .startIndex =  (uint16_t)lineStart,
            .count = (uint16_t)(index - lineStart),
            .width = (uint16_t)curWidth
        });
    }

    height = (lines.size()-1)*pLineSpacing;
    height+=lines.size()*(maxY-minY);
    auto y = pDrawAt.pos.y-minY;
   // printf("Drawing text at %d+%d = %d\n", pDrawAt.pos.y, _header.yAdvance, y);
    if(pDrawAt.size.height>0) {
        switch(pVerticalAlign) {
            case Top :
                break;
            case Middle :
                if(height <= pDrawAt.size.height) 
                    y+=(pDrawAt.size.height - height)/2; 
                break;
            case Bottom :
                if(height <= pDrawAt.size.height) 
                    y+=pDrawAt.size.height - height; 
                break;                
        }
    }

    for(auto line : lines) {
        auto x = pDrawAt.pos.x;
        
        if(pDrawAt.size.width>0) {
            switch(pJustify) {
                case Left :
                    break;
                case Center :
                    if(line.width <= pDrawAt.size.width) {
                        x+=(pDrawAt.size.width - line.width)/2;
                    }

                    break;
                case Right :
                    
                    if(line.width <= pDrawAt.size.width) {
                        x+=pDrawAt.size.width - line.width;
                    }                
                    break;
            }
        }   

        for(auto gl = line.startIndex; gl < line.startIndex+line.count; gl++) {
            pTarget.BltMonoBitmap(
                datas[gl],
                { .width = glyphs[gl]->width, .height=glyphs[gl]->height },
                { .x = x+glyphs[gl]->xOffset, .y = y+glyphs[gl]->yOffset },
                pDrawAt,
                pColor);  
            x+=glyphs[gl]->xAdvance; 
        }    
    }
    y+=_header.yAdvance;
}