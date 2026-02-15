#include "LevelBubble.h"

LevelBubble::LevelBubble(): _bgImg(nullptr), _sightImg(nullptr)
                                                , _bblImg(nullptr), _imuReadTimer(IMU_TIMER)
                                                , _maxTilt(0),_maxOffsetX(0),_maxOffsetY(0)
                                                ,_display(true)
{
    
}

LevelBubble::~LevelBubble()
{
    if(_bgImg != nullptr)
        delete _bgImg;
    if(_sightImg != nullptr)
        delete _sightImg;
    if(_bblImg != nullptr)
        delete _bblImg;
}

void LevelBubble::setPos(uint16_t x,uint16_t y)
{
    _rect.x = x;
    _rect.y = y;
    _bgImg->setPos(x,y);
    _sightImg->setPos(x,y);
}

bool LevelBubble::init(const char *bgPath,const char *sightPath,const char *bblPath)
{
    if (!_imu.begin()) 
        return false;

    _imu.setAccelUnit_mg(true);   // affiche en mg
    _imu.setGyroUnit_dps(true);   // affiche en °/s
    _imu.setDisplayPrecision(3); // 3 décimales

    _bgImg = new TFT_Image(bgPath);
    if(_bgImg == nullptr)
        return false;

    _sightImg = new TFT_Image(sightPath);
    if(_sightImg == nullptr)
    {
        delete _bgImg;
        return false;
    }

    _bblImg = new TFT_Image(bblPath);
    if(_bblImg == nullptr)
    {
        delete _bgImg;
        delete _sightImg;
        return false;
    }

    uint16_t w = _bgImg->getRect().w;
    uint16_t h = _bgImg->getRect().h;
    setSize(w,h);
    _imuReadTimer.start();

    _maxTilt = 800.0f; // ±800 mg => maximum déplacement
    _maxOffsetX = _rect.w / 2 - _bblImg->getRect().w / 2;
    _maxOffsetY = _rect.h / 2 - _bblImg->getRect().h / 2;
    return true;
}

void LevelBubble::setColorKey(uint16_t color)
{
    if(_bgImg!=nullptr)
        _bgImg->setColorKey(color);
    if(_sightImg!=nullptr)
        _sightImg->setColorKey(color);
    if(_bblImg!=nullptr)
        _bblImg->setColorKey(color);
}

void LevelBubble::draw()
{
    if(_imuReadTimer.getTrigg())
    {
        if (_imu.readSensorData(_imuData))
        {
            // Inverser le déplacement : plus l'inclinaison est grande, plus la bulle se déplace en sens inverse
            float xNorm = constrain(_imuData.accelX, -_maxTilt, _maxTilt) / _maxTilt;
            float yNorm = constrain(_imuData.accelY, -_maxTilt, _maxTilt) / _maxTilt;

            // Déplacement de la bulle par rapport au centre
            int dx = -xNorm * _maxOffsetX;
            int dy = -yNorm * _maxOffsetY;

            // Calcul position finale
            int bubbleX = _rect.x + _rect.w / 2 + dx - _bblImg->getRect().w / 2;
            int bubbleY = _rect.y + _rect.h / 2 + dy - _bblImg->getRect().h / 2;

            _bblImg->setPos(bubbleX, bubbleY);

            WEmit(this,(WsgnlFctnPtr)&LevelBubble::TXUpdateX,&_imuData.accelX);
            WEmit(this,(WsgnlFctnPtr)&LevelBubble::TXUpdateY,&_imuData.accelY);
            WEmit(this,(WsgnlFctnPtr)&LevelBubble::TXUpdateZ,&_imuData.accelZ);
        }

        _imuReadTimer.reinit();
    }

    _bgImg->draw();
    _bblImg->draw();
    _sightImg->draw();


}
