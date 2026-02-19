#include "TFT_CoreApp.h"

TFT_CoreApp *TFT_CoreAppPtr = nullptr;

TFT_CoreApp::TFT_CoreApp(TFT_eSPI *tft) :
    _tft(tft),
    _currentDrawBuffer(0),
    _queue(nullptr),
    _spiSemaphore(nullptr),
    _textSprite(nullptr),
    _textBuffer(nullptr),
    _touchSensor(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT),
    _eventFlag(false),
    _tEvent({0,0}),
    _spiSD(HSPI),
    _touchTimer(30)
{
    TFT_CoreAppPtr = this;
}

TFT_CoreApp::~TFT_CoreApp()
{
    if (_buffers[0]) free(_buffers[0]);
    if (_buffers[1]) free(_buffers[1]);
    if (_textBuffer) free(_textBuffer);
    if (_queue) vQueueDelete(_queue);
    if (_spiSemaphore) vSemaphoreDelete(_spiSemaphore);
    if (_bufferSemaphore) vSemaphoreDelete(_bufferSemaphore);
    if (_textSprite)
    {
        _textSprite->deleteSprite();
        delete _textSprite;
    }
    _spiSD.end();
}

bool TFT_CoreApp::init()
{
    Serial.println("Initialisation SPI SD...");
    _spiSD.begin(PIN_SCK, PIN_MISO, PIN_MOSI);

    SdSpiConfig sdConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(20), &_spiSD);

    if (!_sd.begin(sdConfig)) 
        Serial.println("Erreur SD !");
    else 
        Serial.println("SD OK !");

    _touchSensor.begin();

    _tft->init();
    Serial.println("TFT init ok");
    _tft->setRotation(0);
    _tft->setTextFont(2);
    int width = _tft->width();
    int height = _tft->height();

    _buffers[0] = (uint16_t *)ps_malloc(width * height * sizeof(uint16_t));
    _buffers[1] = (uint16_t *)ps_malloc(width * height * sizeof(uint16_t));
    if (!_buffers[0] || !_buffers[1])
        return false;
    
    memset(_buffers[0], 0, width * height * sizeof(uint16_t));
    memset(_buffers[1], 0, width * height * sizeof(uint16_t));
    _textBuffer = (uint16_t *)ps_malloc(TEXT_BUFFER_MAX_W * TEXT_BUFFER_MAX_H * sizeof(uint16_t));
    if (!_textBuffer)
    {
        Serial.println("Ps_malloc Failed!!!");
        return false;
    }

    _textSprite = new TFT_eSprite(_tft);
    _textSprite->setColorDepth(16);
    if (!_textSprite)
        return false;
    if(!_textSprite->createSprite(TEXT_BUFFER_MAX_W, TEXT_BUFFER_MAX_H))
        return false;
    _textSprite->setTextFont(2);

    _queue = xQueueCreate(2, sizeof(uint16_t *));
    if (!_queue)
        return false;
    _spiSemaphore = xSemaphoreCreateMutex();
    if (!_spiSemaphore)
        return false;
    _bufferSemaphore = xSemaphoreCreateBinary();
    if (!_bufferSemaphore) return false;
    // Donne le sémaphore au début (buffer dispo)
    xSemaphoreGive(_bufferSemaphore);

    xTaskCreatePinnedToCore(DisplayTask,"DisplayTask",8192,this,1,NULL,0);
    return true;
}

TFT_Size TFT_CoreApp::getScreenSize()
{
    TFT_Size ret;
    ret.w = _tft->width();
    ret.h = _tft->height();
    return ret;
}

void TFT_CoreApp::drawRect(TFT_Rect rect, uint16_t color)
{
    uint16_t *buf = getDrawBuffer();
    if(rect.x < _tft->width() && rect.y < _tft->height())
    {
        uint16_t wMax,hMax;
        if(rect.x + rect.w > _tft->width())
            wMax = (rect.x + rect.w) - _tft->width();
        else
            wMax = rect.x + rect.w;

        if(rect.y + rect.h > _tft->height())
            hMax = (rect.y + rect.w) - _tft->width();
        else
            hMax = rect.y + rect.h;

        for(uint16_t i = rect.x;i < wMax;i++)
            for(uint16_t j = rect.y;j < hMax;j++)
                buf[j * _tft->width() + i] = color;
    }
}

uint16_t *TFT_CoreApp::openImg(const char *path,uint16_t *w, uint16_t *h)
{
    uint16_t *data = nullptr;
    uint32_t w32, h32;

    auto f = _sd.open(path);
    if (!f)
        return data;

    f.read((uint32_t*)&w32, sizeof(uint32_t));
    f.read((uint32_t*)&h32, sizeof(uint32_t));
    *w = (uint16_t)w32;
    *h = (uint16_t)h32;

    size_t size = (*w) * (*h) * sizeof(uint16_t);
    data = (uint16_t*)ps_malloc(size);
    if (data == NULL)
    {
        Serial.println("ps_malloc img echec...");
        f.close();
        return data;
    }

    if (f.read((uint8_t*)data, size) != size)
    {
        Serial.println("fread img echec...");
        f.close();
        free(data);
        data = nullptr;
        return data;
    }
    f.close();
    return data;
}

void TFT_CoreApp::pushLine(uint16_t x, uint32_t y,uint16_t lenght)
{
    uint16_t *buf = getDrawBuffer();
} 

void TFT_CoreApp::drawCircle(uint16_t x, uint16_t y, uint16_t radius,uint16_t color)
{

}

uint16_t *TFT_CoreApp::getDrawBuffer()
{
    return _buffers[_currentDrawBuffer];
}

bool TFT_CoreApp::submitDrawBuffer()
{
    // Attendre que le buffer soit dispo
    if (xSemaphoreTake(_bufferSemaphore, portMAX_DELAY) == pdFALSE)
        return false;

    uint16_t *buf = _buffers[_currentDrawBuffer];
    _currentDrawBuffer = (_currentDrawBuffer + 1) % 2;
    memset(_buffers[_currentDrawBuffer], 0, _tft->width() * _tft->height() * sizeof(uint16_t));

    return xQueueSend(_queue, &buf, portMAX_DELAY) == pdPASS;
}

void TFT_CoreApp::pushImg(uint16_t *data, TFT_Rect rect)
{
    uint16_t *buf = getDrawBuffer();
    uint16_t screenWidth = _tft->width();
    uint16_t screenHeight = _tft->height();

    for (uint16_t j = 0; j < rect.h; ++j)
    {
        uint16_t py = rect.y + j;
        if (py >= screenHeight) continue;

        uint16_t copyWidth = rect.w;
        uint16_t px = rect.x;

        if (px >= screenWidth) continue;
        if (px + copyWidth > screenWidth)
            copyWidth = screenWidth - px;

        uint16_t *dest = buf + py * screenWidth + px;
        uint16_t *src  = data + j * rect.w;
        memcpy(dest, src, copyWidth * sizeof(uint16_t));
    }
}


void TFT_CoreApp::pushImg(uint16_t *data, TFT_Rect rect, uint16_t colorKey)
{
    uint16_t *buf = getDrawBuffer();
    for (uint16_t j = 0; j < rect.h; ++j)
    {
        for (uint16_t i = 0; i < rect.w; ++i)
        {
            uint16_t px = rect.x + i;
            uint16_t py = rect.y + j;
            if (px >= _tft->width() || py >= _tft->height())
                continue;
            uint16_t c = data[j * rect.w + i];
            if (c != colorKey)
                buf[py * _tft->width() + px] = c;
        }
    }
}

void TFT_CoreApp::pushImg(uint16_t *data, TFT_Rect srcrect, TFT_Rect dstrect, uint16_t colorKey)
{
    uint16_t *buf = getDrawBuffer();
    for (uint16_t j = 0; j < srcrect.h; ++j)
    {
        for (uint16_t i = 0; i < srcrect.w; ++i)
        {
            uint16_t dst_x = dstrect.x + i;
            uint16_t dst_y = dstrect.y + j;
            // Hors de l'écran ?
            if (dst_x >= _tft->width() || dst_y >= _tft->height())
                continue;
                
            uint16_t c = data[(srcrect.y + j) * srcrect.w + (srcrect.x + i)];
            if (c != colorKey)
                buf[dst_y * _tft->width() + dst_x] = c;
        }
    }
}

void TFT_CoreApp::pushImg(uint16_t *data,TFT_Rect srcrect,TFT_Rect dstrect)
{
    uint16_t *buf = getDrawBuffer();
    uint16_t screenWidth = _tft->width();
    uint16_t screenHeight = _tft->height();

    for (uint16_t j = 0; j < srcrect.h; ++j)
    {
        uint16_t srcY = srcrect.y + j;
        uint16_t dstY = dstrect.y + j;
        if (dstY >= screenHeight)
            continue;
        if (dstrect.x >= screenWidth)
            continue;
        
        uint16_t lineWidth = srcrect.w;
        
        if (dstrect.x + lineWidth > screenWidth)
            lineWidth = screenWidth - dstrect.x;
        const uint16_t *srcPtr = &data[srcY * srcrect.w + srcrect.x];
        uint16_t *dstPtr = &buf[dstY * screenWidth + dstrect.x];

        memcpy(dstPtr, srcPtr, lineWidth * sizeof(uint16_t));
    }
}

void TFT_CoreApp::drawImg(TFT_Image *img)
{
    pushImg(img->_data,img->getRect());
}

void TFT_CoreApp::drawImg(TFT_Image *img,TFT_Rect destRect)
{
    pushImg(img->_data,img->getRect(),destRect);
}

void TFT_CoreApp::drawImg(TFT_Image *img,TFT_Rect srcrect,TFT_Rect dstrect)
{
    pushImg(img->_data,srcrect,dstrect);
}

void TFT_CoreApp::handleTouch()
{
    if (_touchTimer.getTrigg())
    {
        memset(&_tEvent,0,sizeof(TouchEvent));
        _eventFlag = false;
        if(_touchSensor.available())
        {
            _tEvent.x = _touchSensor.data.x;
            _tEvent.y = _touchSensor.data.y;
            _tEvent.gestureID = _touchSensor.data.gestureID;
            _tEvent.points = _touchSensor.data.points;
            _tEvent.event = _touchSensor.data.event;
            _eventFlag = true;
        }
        _touchTimer.reinit();
    }
}

void TFT_CoreApp::exec()
{
    submitDrawBuffer();
    handleTouch();
}

bool TFT_CoreApp::Collision(int x, int y, const TFT_Rect &rect)
{
    return (x >= rect.x) && (y >= rect.y) && (x < rect.x + rect.w) && (y < rect.y + rect.h);
}

void TFT_CoreApp::DisplayTask(void *param)
{
    TFT_CoreApp *app = (TFT_CoreApp *)param;
    uint16_t *buf;

    while (true)
    {
        if (xQueueReceive(app->_queue, &buf, portMAX_DELAY) == pdPASS)
        {
            xSemaphoreTake(app->_spiSemaphore, portMAX_DELAY);
            app->_tft->pushImage(0, 0, app->_tft->width(), app->_tft->height(), buf);
            xSemaphoreGive(app->_spiSemaphore);

            xSemaphoreGive(app->_bufferSemaphore);
        }
        //vTaskDelay(1);
    }
}

uint16_t TFT_CoreApp::swapBytes(uint16_t val)
{
    return (val >> 8) | (val << 8);
}

void TFT_CoreApp::addLink(TFT_Widget* txObj, WsgnlFctnPtr txFptr, TFT_Widget* rxObj, WsgnlFctnPtr rxFptr) {
    SignalKey key = {txObj, txFptr};
    WLinkSignature rxSign = {rxObj, rxFptr};
    _table[key].push_back(rxSign);
}

void TFT_CoreApp::deleteLink(TFT_Widget* txObj, WsgnlFctnPtr txFptr, TFT_Widget* rxObj, WsgnlFctnPtr rxFptr) {
    SignalKey key = {txObj, txFptr};
    auto it = _table.find(key);
    if (it == _table.end()) return;

    auto& vec = it->second;
    vec.erase(std::remove_if(vec.begin(), vec.end(),
        [=](const WLinkSignature& s) {
            return s.obj == rxObj && s.objFunct == rxFptr;
        }),
        vec.end());

    if (vec.empty()) {
        _table.erase(it);
    }
}

void TFT_CoreApp::applySignal(TFT_Widget* txObj, WsgnlFctnPtr txPtr, void* msg) {
    SignalKey key = {txObj, txPtr};
    auto it = _table.find(key);
    if (it != _table.end()) {
        for (const auto& slot : it->second) {
            (slot.obj->*slot.objFunct)(msg);
        }
    }
}

void WConnect(TFT_Widget* txObj, WsgnlFctnPtr txFptr, TFT_Widget* rxObj, WsgnlFctnPtr rxFptr) {
    TFT_CoreAppPtr->addLink(txObj, txFptr, rxObj, rxFptr);
}

void WDisconnect(TFT_Widget* txObj, WsgnlFctnPtr txFptr, TFT_Widget* rxObj, WsgnlFctnPtr rxFptr) {
    TFT_CoreAppPtr->deleteLink(txObj, txFptr, rxObj, rxFptr);
}

void WEmit(TFT_Widget* txObj, WsgnlFctnPtr txFptr, void* msg) {
    TFT_CoreAppPtr->applySignal(txObj, txFptr, msg);
}