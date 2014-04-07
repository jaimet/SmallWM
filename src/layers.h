#ifndef __SMALLWM_LAYERS__
#define __SMALLWM_LAYERS__

#include "clients.h"
#include "common.h"
#include "shared.h"
#include "utils.h"

/**
 * Manages how clients are layered around each other.
 */
class LayerManager
{
public:
    LayerManager(ClientContainer *clients, WMShared &shared) :
        m_clients(clients), m_shared(shared)
    {};

    void raise_layer(Window);
    void lower_layer(Window);
    void set_layer(Window, Layer);
protected:
    void relayer_clients();
    void delete_layer(Window);

private:
    /// A relation between each client and its current layer
    std::map<Window, Layer> m_layers;

    /// The clients which are being managed 
    ClientContainer *m_clients;

    /// The window manager's shared data
    WMShared &m_shared;
};

#endif