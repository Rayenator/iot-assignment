#include <string.h>

#include "board.h"
#include "timex.h"
#include "ztimer.h"

/* Add sx127x radio driver necessary includes here */
#include "sx127x.h"
#include "sx127x_netdev.h"
#include "sx127x_params.h"

/* Add loramac necessary includes here */
#include "net/loramac.h"     /* core loramac definitions */
#include "semtech_loramac.h" /* package API */

/* Declare the sx127x radio driver descriptor globally here */

static sx127x_t sx127x;      /* The sx127x radio driver descriptor */

/* Declare the loramac descriptor globally here */

static semtech_loramac_t loramac;  /* The loramac stack descriptor */

/* Device and application parameters required for OTAA activation here */
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x05, 0xE0, 0x1E };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0x4E, 0x9E, 0xC3, 0x38, 0xA4, 0x17, 0x25, 0xEE, 0x22, 0x11, 0xF6, 0x56, 0x4F, 0x4D, 0xBE, 0x7A };

int main(void)
{
    /* initialize the radio driver */

    sx127x_setup(&sx127x, &sx127x_params[0], 0);
    loramac.netdev = &sx127x.netdev;
    loramac.netdev->driver = &sx127x_driver;
    /* initialize loramac stack */
    semtech_loramac_init(&loramac);

    /* configure the device parameters */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);

    /* change datarate to DR5 (SF7/BW125kHz) */
    semtech_loramac_set_dr(&loramac, 5);
    
    /* start the OTAA join procedure */
    if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed");
        return 1;
    }
    puts("Join procedure succeeded");

    while (1) {
        char *message = "This is RIOT!";

        /* send the message here */
        if (semtech_loramac_send(&loramac,
                                 (uint8_t *)message, strlen(message)) != SEMTECH_LORAMAC_TX_DONE) {
            printf("Cannot send message '%s'\n", message);
        }
        else {
            printf("Message '%s' sent\n", message);
        }

        /* wait 20 seconds between each message */
        ztimer_sleep(ZTIMER_MSEC, 20 * MS_PER_SEC);
    }

    return 0; /* should never be reached */
}