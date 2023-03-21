#include "stm32f746xx.h"
#include <stdint.h>

void SysCall(void);

#define STACK_SIZE 256

typedef struct {
  unsigned int sp;
  unsigned int stack[STACK_SIZE];
} tast_t;

volatile tast_t *sched_current_task;
volatile tast_t *sched_next_task;
volatile tast_t *running_task;

volatile tast_t taskA;
volatile tast_t taskB;

int cntA;
int cntB;

void TaskA(void)
{
  while(1) {
    cntA++;
    SysCall();
  }
}

void TaskB(void)
{
  while(1) {
    cntB++;
    SysCall();
  }
}

void ChangeRunningTask(void)
{
  running_task = sched_next_task;

  // task schedulling: round-robin for 2 tasks
  tast_t *tmp = sched_current_task;
  sched_current_task = sched_next_task;
  sched_next_task = tmp;
}

int main(void)
{
  taskA.stack[STACK_SIZE-1] = 0x01000000UL; // xPSR
  taskB.stack[STACK_SIZE-1] = 0x01000000UL; // xPSR

  taskA.stack[STACK_SIZE-2] = (unsigned int)TaskA | 1; // PC for return from interrupt
  taskB.stack[STACK_SIZE-2] = (unsigned int)TaskB | 1; // PC for return from interrupt

  // ################## Fill registers with stub values only for debug #####################
  //taskA.stack[STACK_SIZE-3] = LR; // old saved LR
  taskA.stack[STACK_SIZE-4] = 12; // saved R12
  taskA.stack[STACK_SIZE-5] = 3; // saved R3
  taskA.stack[STACK_SIZE-6] = 2; // saved R2
  taskA.stack[STACK_SIZE-7] = 1; // saved R1
  taskA.stack[STACK_SIZE-8] = 0; // saved R0
  taskA.stack[STACK_SIZE-9] = 11; // saved R11
  taskA.stack[STACK_SIZE-10] = 10; // saved R10
  taskA.stack[STACK_SIZE-11] = 9; // saved R9
  taskA.stack[STACK_SIZE-12] = 8; // saved R8
  taskA.stack[STACK_SIZE-13] = 7; // saved R7
  taskA.stack[STACK_SIZE-14] = 6; // saved R6
  taskA.stack[STACK_SIZE-15] = 5; // saved R5
  taskA.stack[STACK_SIZE-16] = 4; // saved R4

  //taskB.stack[STACK_SIZE-3] = LR; // old saved LR
  taskB.stack[STACK_SIZE-4] = 12; // saved R12
  taskB.stack[STACK_SIZE-5] = 3; // saved R3
  taskB.stack[STACK_SIZE-6] = 2; // saved R2
  taskB.stack[STACK_SIZE-7] = 1; // saved R1
  taskB.stack[STACK_SIZE-8] = 0; // saved R0
  taskB.stack[STACK_SIZE-9] = 11; // saved R11
  taskB.stack[STACK_SIZE-10] = 10; // saved R10
  taskB.stack[STACK_SIZE-11] = 9; // saved R9
  taskB.stack[STACK_SIZE-12] = 8; // saved R8
  taskB.stack[STACK_SIZE-13] = 7; // saved R7
  taskB.stack[STACK_SIZE-14] = 6; // saved R6
  taskB.stack[STACK_SIZE-15] = 5; // saved R5
  taskB.stack[STACK_SIZE-16] = 4; // saved R4
  // ######################################################################################

  taskA.sp = &taskA.stack[STACK_SIZE-16];
  taskB.sp = &taskB.stack[STACK_SIZE-16];

  sched_next_task = &taskB;
  sched_current_task = &taskA;
  running_task = sched_current_task;

  __NVIC_EnableIRQ(SVCall_IRQn);
  /* Switch to unprivileged mode with PSP stack */
  __set_PSP((uint32_t)(&(taskA.stack[STACK_SIZE-16])));
  __set_CONTROL(0x03);

  TaskA();

  while(1);
}
