#ifndef TFT_COREAPP_H_INCLUDED
#define TFT_COREAPP_H_INCLUDED

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "TFT_Widget.h"
#include "TFT_Timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "CST816S.h"
#include <Wire.h>
#include <SPI.h>
#include <SdFat.h>
#include "TFT_Image.h"
#include "TFT_Animation.h"
#include "TFT_WSignal.h"
#include <map>

// Broches SPI SD
#define PIN_SCK   39
#define PIN_MISO  40
#define PIN_MOSI  38
#define SD_CS     41


#define COLORKEY_R  101
#define COLORKEY_G   62
#define COLORKEY_B  112

#define COLORKEY  ( ((COLORKEY_R >>3)<<11) | ((COLORKEY_G>>2)<<5) | (COLORKEY_B>>3) )

#define TOUCH_SDA 48
#define TOUCH_SCL 47
#define TOUCH_INT 46
#define TOUCH_RST -1

#define TEXT_BUFFER_MAX_W 240
#define TEXT_BUFFER_MAX_H 50

using namespace std;

class TFT_Image;

typedef struct
{
    int x;
    int y;
    uint8_t gestureID;
    uint8_t points;
    uint8_t event; // 0 = Down, 1 = Up, 2 = Contact
} TouchEvent;

class TFT_CoreApp
{
    friend class TFT_Image;
    friend class TFT_Text;
    friend class TFT_Anim;

    public:
        TFT_CoreApp(TFT_eSPI *tft);
        ~TFT_CoreApp();

        bool init();
        TFT_Size getScreenSize();

        void drawImg(TFT_Image *img);      
        void drawImg(TFT_Image *img,TFT_Rect destRect);
        void drawImg(TFT_Image *img,TFT_Rect srcrect,TFT_Rect dstrect); 
        void drawRect(TFT_Rect rect, uint16_t color);
        void drawCircle(uint16_t x, uint16_t y, uint16_t radius,uint16_t color);
        
        void exec();

        bool event() { return _eventFlag; }
        TouchEvent *getTouchEvent() { return &_tEvent; }

        static bool Collision(int x, int y, const TFT_Rect &rect);
        static uint16_t swapBytes(uint16_t val);

        void addLink(TFT_Widget* txObj, WsgnlFctnPtr txFptr, TFT_Widget* rxObj, WsgnlFctnPtr rxFptr);
        void deleteLink(TFT_Widget* txObj, WsgnlFctnPtr txFptr, TFT_Widget* rxObj, WsgnlFctnPtr rxFptr);
        void applySignal(TFT_Widget* txObj, WsgnlFctnPtr txPtr, void* msg);

    private:
    
        uint16_t *getDrawBuffer();
        bool submitDrawBuffer();

        TFT_eSPI *getTftPtr() { return _tft; }
        TFT_eSprite *getTextSprite() { return _textSprite; }
        uint16_t *getTextBuffer() { return _textBuffer; }

        void handleTouch();

        SdFat *getSdPtr() { return &_sd; }

        void pushImg(uint16_t *data, TFT_Rect rect);
        void pushImg(uint16_t *data, TFT_Rect rect, uint16_t colorKey);
        void pushImg(uint16_t *data, TFT_Rect srcrect, TFT_Rect dstrect, uint16_t colorKey);
        void pushImg(uint16_t *data,TFT_Rect srcrect,TFT_Rect dstrect);
        void pushLine(uint16_t x, uint32_t y,uint16_t lenght);
        uint16_t *openImg(const char *path,uint16_t *w, uint16_t *h);
        static void DisplayTask(void *param);

        TFT_eSPI *_tft;
        uint16_t *_buffers[2];
        volatile int _currentDrawBuffer;

        QueueHandle_t _queue;
        SemaphoreHandle_t _spiSemaphore;

        TFT_eSprite *_textSprite;
        uint16_t *_textBuffer;

        CST816S _touchSensor;
        bool _eventFlag;
        TouchEvent _tEvent;
        SPIClass _spiSD;
        StopWatch _touchTimer;
        SdFat _sd;
        SemaphoreHandle_t _bufferSemaphore;

        std::map<SignalKey, std::vector<WLinkSignature>, SignalKeyCompare> _table;
};

void WConnect(TFT_Widget* txObj, WsgnlFctnPtr txFptr, TFT_Widget* rxObj, WsgnlFctnPtr rxFptr);

void WDisconnect(TFT_Widget* txObj, WsgnlFctnPtr txFptr, TFT_Widget* rxObj, WsgnlFctnPtr rxFptr);

void WEmit(TFT_Widget* txObj, WsgnlFctnPtr txFptr, void* msg);

#endif
