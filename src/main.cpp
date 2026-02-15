#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TFT_Screen.h>
#include <QMI8658.h>
#include "LevelBubble.h"

#define QMI_ADDRESS 0x6B                  // Define QMI8658C I2C address
#define QMI8658C_I2C_FREQUENCY 80000  

TFT_eSPI tft;
TFT_CoreApp coreApp(&tft);

TFT_Image aeraIc,backBtnIc,batIc,cubeIc,dimIc,laserOnIc,laserOnBtn,saveBtnIc,volIc;
TFT_Container *headCtn, *mainCtn, *footCtn;
TFT_Text distText("20,145m",TFT_WHITE,(TFT_Rect){0,0,190,30});
TFT_Text aeraText("2,456m",TFT_WHITE,(TFT_Rect){0,0,190,30});
TFT_Text volText("15,265m",TFT_WHITE,(TFT_Rect){0,0,190,30});
TFT_Scene *scene;

QMI8658 imu;

void callBackTest()
{
  Serial.println("Bouton Pressé!!");
}

StopWatch timer(100);

void setup() 
{
  Serial.begin(115200);
  delay(3000);  // attendre que le moniteur série démarre

  if(!coreApp.init())
    Serial.println("CoreApp init failed!");

  headCtn = new TFT_Container();
  mainCtn = new TFT_Container();
  footCtn = new TFT_Container();
  scene = new TFT_Scene("/background.img");

  Serial.println("CoreApp init OK");

  aeraIc.load("/aeraIc.img");
  backBtnIc.load("/back_btnIc.img");
  batIc.load("/batIc.img");
  cubeIc.load("/cubeIc.img");
  dimIc.load("/dimIc.img");
  laserOnIc.load("/laserOnIc.img");
  laserOnBtn.load("/laserOnBtnIc.img");
  saveBtnIc.load("/saveBtnIc.img");
  volIc.load("/volIc.img");

  headCtn->addWidget(&batIc,0,0);
  headCtn->addWidget(&laserOnIc,0,1,190,0,0,0);
  headCtn->updateLayout();

  mainCtn->addWidget(&dimIc,0,0,13,0,10,0);
  mainCtn->addWidget(&distText,0,1,10,0,10,0);
  mainCtn->addWidget(&aeraIc,1,0,13,0,18,0);
  mainCtn->addWidget(&aeraText,1,1,10,0,18,0);
  mainCtn->addWidget(&volIc,2,0,13,0,18,0);
  mainCtn->addWidget(&volText,2,1,10,0,18,0);
  mainCtn->addWidget(&cubeIc,3,0,13,0,18,0);
  mainCtn->updateLayout();

  footCtn->addWidget(&saveBtnIc,0,0,18,0,20,0);
  footCtn->addWidget(&laserOnBtn,0,1,51,0,0,80);
  footCtn->addWidget(&backBtnIc,0,2,43,0,20,0);
  footCtn->updateLayout();

  scene->addWidget(headCtn,0,0);
  scene->addWidget(mainCtn,1,0,0,0,20,0);
  scene->addWidget(footCtn,2,0,0,0,30,0);
  scene->updateLayout();

  if (!imu.begin()) 
  {
    Serial.println("Initialisation IMU échouée !");
    while (1);
  }
  imu.setAccelUnit_mg(true);   // affiche en mg
  imu.setGyroUnit_dps(true);   // affiche en °/s
  imu.setDisplayPrecision(3); // 3 décimales

  timer.start();
}

float a = 0.0 , b = 2.154, c = 0.547;

void loop()
{
  if(timer.getTrigg())
  {
    QMI8658_Data d;
    if (imu.readSensorData(d)) 
    {
      distText.setText(d.accelX);
      aeraText.setText(d.accelY);
      volText.setText(d.accelZ);
    }
    timer.reinit();
  }

  scene->draw();
  coreApp.exec();
  delay(10);
}

