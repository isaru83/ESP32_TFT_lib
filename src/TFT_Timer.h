#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <Arduino.h>

// Classe pour gérer une fréquence fixe bloquante (ex: boucle cadencée)
class FrameRate
{
public:
    // time en millisecondes : durée minimale entre deux appels run()
    FrameRate(unsigned int time) : _time(time), _prevTime(0) {}
    // Bloque jusqu'à ce que _time ms soient écoulées depuis le dernier run()
    void run()
    {
        unsigned long now = millis();
        if (now - _prevTime >= _time)
        {
            _prevTime = now;
            return;
        }
        // Attente active (bloquante) du temps restant
        while (millis() - _prevTime < _time)
            ;
        _prevTime = millis();
    }

private:
    unsigned int _time;         // Durée cible entre deux appels (ms)
    unsigned long _prevTime;    // Dernier timestamp d'exécution
};

// Classe pour gérer un timer non bloquant (ex: déclencheur périodique)
class StopWatch
{
public:
    // delay en millisecondes : délai avant déclenchement
    StopWatch(unsigned long delay) 
        : _delay(delay), _startTime(0), _remainTime(0), _isStartedFlag(false) {}

    // Démarre le timer (si pas déjà démarré)
    void start()
    {
        if (!_isStartedFlag)
        {
            _startTime = millis();
            _remainTime = 0;
            _isStartedFlag = true;
        }
    }

    // Teste si le délai est écoulé (true si oui)
    bool getTrigg()
    {
        if (!_isStartedFlag)
            return false;

        unsigned long now = millis();
        _remainTime = now - _startTime;

        if (_remainTime >= _delay)
        {
            // On ne reset pas automatiquement ici, l'utilisateur choisit
            return true;
        }
        return false;
    }

    // Réinitialise le timer (arrêt et remise à zéro)
    void reset()
    {
        _isStartedFlag = false;
        _remainTime = 0;
    }

    // Réinitialise et relance immédiatement
    void reinit()
    {
        reset();
        start();
    }

    // Change la durée du timer (en ms)
    void setDelay(unsigned long delay)
    {
        _delay = delay;
    }

    // Retourne le temps restant avant déclenchement (en secondes)
    unsigned int remainTimeSecond() const
    {
        if (_isStartedFlag && _delay > _remainTime)
            return (unsigned int)((_delay - _remainTime) / 1000);
        return 0;
    }

    // Indique si le timer est actif
    bool isStarted() const
    {
        return _isStartedFlag;
    }

private:
    unsigned long _delay;       // Délai ciblé (ms)
    unsigned long _startTime;   // Timestamp de départ (ms)
    unsigned long _remainTime;  // Temps écoulé depuis start (ms)
    bool _isStartedFlag;        // Flag indiquant si timer démarré
};

#endif // TIMER_H_INCLUDED
