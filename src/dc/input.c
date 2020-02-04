//
// Created by cpasjuste on 03/02/2020.
//

#include <kos.h>

static uint32 last_buttons = 0;

uint32 get_input() {

    maple_device_t *cont = NULL;
    cont_state_t *state = NULL;

    cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

    if (cont) {
        state = (cont_state_t *) maple_dev_status(cont);

        if (!state) {
            return 1;
        }

        if (last_buttons != state->buttons) {
            last_buttons = state->buttons;
            return last_buttons;
        }
    }

    return 0;
}
