![](img/img1.PNG)

# ESP32_TFT_lib

**Bibliothèque C++ légère et orientée objet** pour créer des interfaces graphiques (GUI) sur écrans TFT avec ESP32 / ESP32-S3.

Développée from scratch pour mes prototypes embarqués : robots, domotique, dashboards, horloges tactiles, menus interactifs...

**Lightweight C++ object-oriented library** for building GUIs on TFT displays with ESP32 / ESP32-S3.  
From-scratch personal project for embedded prototypes: robots, home automation, dashboards, touch clocks, interactive menus...

## Fonctionnalités principales / Key features

- Widgets modulaires : boutons (abstraits & images), texte, images, conteneurs hiérarchiques
- Gestion de scènes multiples (TFT_Scene + TFT_SceneManager)
- Animations fluides et transitions (TFT_Animation)
- Timers et signaux internes (TFT_Timer, TFT_WSignal)
- Cœur applicatif simple (TFT_CoreApp + TFT_Screen)
- Approche OOP extensible (héritage, composition)

Compatible tactile : conçu pour fonctionner avec contrôleurs CST816S (I²C) – écrans ronds 1.28" GC9A01, LilyGO T-Display S3 Touch, Waveshare, etc.

**Requires** a low-level TFT driver :TFT_eSPI for pixel rendering.

## Installation / Setup

**PlatformIO** (recommandé) 

