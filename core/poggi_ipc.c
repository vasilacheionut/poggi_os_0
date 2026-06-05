#include "../include/poggi_ipc.h"
#include "../include/poggi_hal.h"

static PoggiMailbox global_mailbox;

void poggi_ipc_init(void)
{
    global_mailbox.data = 0;
    global_mailbox.has_message = 0;
}

int poggi_ipc_send(int value)
{
    // Dacă există deja un mesaj necitit, blocăm scrierea (coadă plină)
    if (global_mailbox.has_message)
    {
        return 0; // Eșec: Mailbox plin
    }

    global_mailbox.data = value;
    global_mailbox.has_message = 1;
    return 1; // Succes
}

int poggi_ipc_receive(int *out_value)
{
    if (!global_mailbox.has_message)
    {
        return 0; // Eșec: Nu e niciun mesaj disponibil
    }

    *out_value = global_mailbox.data;
    global_mailbox.has_message = 0; // Consumăm mesajul, eliberăm cutia
    return 1;                       // Succes
}