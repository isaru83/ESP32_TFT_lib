#ifndef LEVELBUBBLE_H_INCLUDED
#define LEVELBUBBLE_H_INCLUDED

#include <QMI8658.h>
#include "TFT_Scene.h"

#define QMI_ADDRESS 0x6B                  
#define QMI8658C_I2C_FREQUENCY 80000

#define IMU_TIMER  100

class LevelBubble : public TFT_Widget
{
    public:
    LevelBubble();
    bool init(const char *bgPath,const char *sightPath,const char *bblPath);
    ~LevelBubble();

    void draw() override;
    void setPos(uint16_t x,uint16_t y) override;
    void setColorKey(uint16_t color);

    void TXUpdateX(void *msg){}
    void TXUpdateY(void *msg){}
    void TXUpdateZ(void *msg){}

    private:
    TFT_Image *_bgImg, *_sightImg, *_bblImg;
    StopWatch _imuReadTimer;
    float _maxTilt;
    int _maxOffsetX, _maxOffsetY ;
    bool _display;
    QMI8658 _imu;
    QMI8658_Data _imuData;
};


#endif