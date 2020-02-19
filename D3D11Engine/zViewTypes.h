// Definitions copied from g2ext, Union (c) 2018 Union team, and World of Gothic

#pragma once
#include "zTypes.h"
#include "oCGame.h"

#ifdef BUILD_GOTHIC_2_6_fix

class zCViewText {
public:
    int vtbl;
    int posx;
    int posy;
    zSTRING text;
    int font;
    float timer;
    int inPrintWin;
    zColor color;
    int timed;
    int colored;
};

template <class T>
class zCList {
public:
    T* data;
    zCList* next;
};

template <class T>
class zList {
public:
    int cmp;
    int count;
    T* last;
    T* root;
};
class _zCView {
public:
    int vtbl;
    int zCInputCallback_vtbl;

    typedef enum zEViewFX {
        VIEW_FX_NONE,
        VIEW_FX_ZOOM,
        VIEW_FX_MAX
    } zTViewFX;

    int m_bFillZ;                       // 0  (4) // [ 2]
    _zCView* next;                       // 4  (4) // [ 3]
    int viewID;                         // 8  (4) // [ 4]
    int flags;                          // 12 (4) // [ 5]
    int intflags;                       // 16 (4) // [ 6]
    int ondesk;                         // 20 (4) // [ 7]
    zTRnd_AlphaBlendFunc alphafunc;     // 24 (4) // [ 8]
    zColor color;                       // 28 (4) // [ 9]
    int alpha;                          // 32 (4) // [10]
    zList<_zCView> childs;               // 36 (12) // [11]
    _zCView* owner;                      // 48 (4) // [15]
    zCTexture* backTex;                 // 52 (4) // [16]
    int vposx;                          // 56 (4) // [17]
    int vposy;                          // 60 (4) // [18]
    int vsizex;                         // 64 (4) // [19]
    int vsizey;                         // 68 (4) // [20]
    int pposx;                          // 72 (4) // [21]
    int pposy;                          // 76 (4) // [22]
    int psizex;                         // 80 (4) // [23]
    int psizey;                         // 84 (4) // [24]
    int font;                           // 88 (4) // [25]
    zColor fontColor;                   // 92 (4) // [26]
    int px1;                            // 96 (4) // [27]
    int py1;                            // 100(4) // [28]
    int px2;                            // 104 (4)// [29]
    int py2;                            // 108 (4)// [30]
    int winx;                           // 112 (4)// [31]
    int winy;                           // 116 (4)// [32]
    zCList<zCViewText> textLines;              // 124 (8) // [33] && count + last + root
    float scrollMaxTime;                // 128 (4)
    float scrollTimer;                  // 132 (4)
    zTViewFX fxOpen;                    // 136 (4)
    zTViewFX fxClose;                   // 140 (4)
    float timeDialog;                   // 144 (4)
    float timeOpen;                     // 148 (4) // [40]
    float timeClose;                    // 152 (4) // [41]
    float speedOpen;                    // 156 (4)// [42]
    float speedClose;                   // 160 (4)// [43]
    int isOpen;                         // 176 (4)// [44]
    int isClosed;                       // 180 (4)// [45]
    int continueOpen;                   // 184 (4)// [46]
    int continueClose;                  // 188 (4)// [47]
    int removeOnClose;                  // 192 (4) // [48]
    int resizeOnOpen;                   // 196 (4) // [49]
    int maxTextLength;                  // 200 (4) // [50]
    zSTRING textMaxLength;              // 204 (20) // [51]
    float2 posCurrent[2];               // 224  (8)
    float2 posOpenClose[2];             // 232  (8)

    bool HasText() {
        return maxTextLength;
    }
    zColor& GetTextColor() {
        static zColor DefaultColor = zColor(158, 186, 203, 255); // BGRA
        if (maxTextLength) {
            //return fontColor;
        }
        return DefaultColor;
    }
};

const int MAX_ITEMS = 150;
const int MAX_EVENTS = 10;
const int MAX_SEL_ACTIONS = 5;
const int MAX_USERVARS = 4;
const int MAX_USERSTRINGS = 10;
class zCMenuItem : public _zCView {
public:
    zSTRING m_parFontName;
    zSTRING m_parText[MAX_USERSTRINGS];
    zSTRING m_parBackPic;
    zSTRING m_parAlphaMode;
    int m_parAlpha;
    int m_parType;
    int m_parOnSelAction[MAX_SEL_ACTIONS];
    zSTRING m_parOnSelAction_S[MAX_SEL_ACTIONS];
    zSTRING m_parOnChgSetOption;
    zSTRING m_parOnChgSetOptionSection;
    int m_parOnEventAction[MAX_EVENTS];
    int m_parPosX;
    int m_parPosY;
    int m_parDimX;
    int m_parDimY;
    float m_parSizeStartScale;
    int m_parItemFlags;
    float m_parOpenDelayTime;
    float m_parOpenDuration;
    float m_parUserFloat[MAX_USERVARS];
    zSTRING m_parUserString[MAX_USERVARS];
    int m_parFrameSizeX;
    int m_parFrameSizeY;
    zSTRING m_parHideIfOptionSectionSet;
    zSTRING m_parHideIfOptionSet;
    int m_parHideOnValue;
    int m_iRefCtr;
    _zCView* m_pInnerWindow;
    int m_pFont;
    int m_pFontHi;
    int m_pFontSel;
    int m_pFontDis;
    int m_bViewInitialized;
    int m_bLeaveItem;
    int m_bVisible;
    int m_bDontRender;
    zCArray<zSTRING>m_listLines;
    zSTRING id;
    int inserted;
    int changed;
    int active;
    int open;
    int close;
    int opened;
    int closed;
    int disabled;
    _zCView* orgWin;
    float fxTimer;
    float openDelayTimer;
    float activeTimer;
    int registeredCPP;
    int firstTimeInserted;
};

class zCMenuItemText : public zCMenuItem {
public:
    enum zCMenuItemTextEnum3 {
        MODE_SIMPLE,
        MODE_ENUM,
        MODE_MULTILINE
    };

    zCMenuItemTextEnum3 m_mode;
    zSTRING m_fullText;
    int m_numOptions;
    int m_topLine;
    int m_viewLines;
    int m_numLines;
    int m_unformated;
};
#endif