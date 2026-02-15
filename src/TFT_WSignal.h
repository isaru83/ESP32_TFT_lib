#ifndef TFT_WSIGNAL_H_INCLUDED
#define TFT_WSIGNAL_H_INCLUDED

/**
 * @file WSignal.h
 * @brief Système léger de signaux/slots pour la communication entre widgets.
 *
 * Cette classe permet de connecter des widgets entre eux sans dépendance directe.
 * Un widget peut "émettre" (emit) un signal, et d'autres peuvent s'y connecter
 * pour recevoir une fonction callback.
 *
 *
 *
 * === DÉTAILS TECHNIQUES ===
 *
 * - Le système utilise une map avec une clé unique composée d’un pointeur vers l’émetteur
 *   et d’un pointeur de fonction membre (fonction du récepteur).
 *
 * - Les pointeurs de fonctions membres (`void (Class::*)(void*)`) ne peuvent pas être
 *   comparés directement sur certaines architectures (comme l’ESP32 avec GCC 8.x).
 *   Il est donc nécessaire de fournir un comparateur personnalisé (`SignalKeyCompare`).
 *
 * - Les appels de fonctions sont effectués dynamiquement via pointeurs membres.
 *   Le `reinterpret_cast` est utilisé pour convertir de manière contrôlée les types
 *   de fonctions membres. Ce cast peut être signalé en rouge par VSCode, mais il est
 *   valide tant que les types correspondent strictement.
 *
 * === LIMITATIONS ===
 *
 * - Pas de vérification automatique des types lors du `emit()`, c’est à l’utilisateur
 *   de garantir que les types transmis au slot sont compatibles.
 * - Fonctionne uniquement avec des fonctions membres prenant un `void*` en argument.
 *   (les types peuvent être castés à l’intérieur si besoin).
 *
 * === AVANTAGES ===
 *
 * - Faible empreinte mémoire, adapté pour l’ESP32.
 * - Pas besoin de `std::function` ou de `new`, tout est géré en pile.
 * - Permet de relier des composants indépendants sans couplage fort.
 *
 * @author Benjamin Grammatico
 */

#include "TFT_CoreApp.h"
#include "TFT_Widget.h"

using WsgnlFctnPtr = void (TFT_Widget::*)(void*);

struct SignalKey 
{
    TFT_Widget* sender;
    WsgnlFctnPtr signal;

    bool operator==(const SignalKey& other) const 
    {
        return sender == other.sender && signal == other.signal;
    }
};

struct SignalKeyCompare 
{
    bool operator()(const SignalKey& a, const SignalKey& b) const 
    {
        if (a.sender != b.sender)
            return a.sender < b.sender;
        // Comparer les pointeurs vers fonctions membres
        return std::less<void*>()(
            reinterpret_cast<void*>(a.signal),
            reinterpret_cast<void*>(b.signal)
        );
    }
};

struct WLinkSignature 
{
    TFT_Widget* obj;
    WsgnlFctnPtr objFunct;
};


#endif