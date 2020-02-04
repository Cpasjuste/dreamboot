//
// Created by cpasjuste on 03/02/2020.
//

#ifndef LOADER_INPUT_LINUX_H
#define LOADER_INPUT_LINUX_H

#define BIT(n) (1U<<(n))

#ifdef __LINUX__
#define INPUT_QUIT  BIT(1)
#define INPUT_LEFT  BIT(2)
#define INPUT_RIGHT BIT(3)
#define INPUT_UP    BIT(4)
#define INPUT_DOWN  BIT(5)
#define INPUT_A     BIT(6)
#define INPUT_B     BIT(7)
#define INPUT_X     BIT(8)
#define INPUT_Y     BIT(9)
#define INPUT_START BIT(10)
#else
#define INPUT_QUIT  BIT(20)
#define INPUT_LEFT  CONT_DPAD_LEFT
#define INPUT_RIGHT CONT_DPAD_RIGHT
#define INPUT_UP    CONT_DPAD_UP
#define INPUT_DOWN  CONT_DPAD_DOWN
#define INPUT_A     CONT_A
#define INPUT_B     CONT_B
#define INPUT_X     CONT_X
#define INPUT_Y     CONT_Y
#define INPUT_START CONT_START
#endif

uint32 get_input();

#endif //LOADER_INPUT_LINUX_H
