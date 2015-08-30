//             CS360 Assignment #1  8-28-2015
//                DUE: Friday, 9-4-2015
//
//                (Chapter 2, PROBLEM 3)
//
//Given the following t.c file
//Under Linux, use    gcc -m32 t.c   to generate an a.out
//Run a.out as        a.out one two three > outfile
//DO the requirements 1 to 4 as specified in the TEXT.
//
/************* t.c file ********************/
#include <stdio.h>
#include <stdlib.h>
int *FP;

struct frame {
	struct frame* next;
};

int C(int x, int y)
{
  int u, v, w, i, *p;

  printf("enter C\n");
  // PRINT ADDRESS OF u,v,w;
  printf("&u=%8x &v=%8x &w=%8x\n", &u, &v, &w);
  u=10; v=11; w=12;

  asm("movl %ebp, FP");    // CPU's ebp register is the FP
  printf("FP = %8x\n", FP);

  /*********** Write C code to DO ************
  1 to 4 AS SPECIFIED in the PROBLEM 3 of the TEXT
  *******************************************/
//(1). Print the stack frame link list
//
//(2). Print the stack contents from FP-8 to the frame of main()
//     YOU MAY JUST PRINT 100 entries of the stack contents.
//
//(3). On a hard copy of the print out, identify the stack contents
//     as LOCAL VARIABLES, PARAMETERS, stack frame pointer of each function.
//
//(4). Find where are argc, argv and env located in the stack.
//     What is exactly argv?

  int *frame_pointer = FP;
  for (; frame_pointer != 0; frame_pointer = *frame_pointer) {
	  printf("new FP = %8x\n", frame_pointer);
  }
  return 0;

}


int B(int x, int y)
{
  int g,h,i;
  printf("enter B\n");
  // PRINT ADDRESS OF g,h,i
  printf("&g=%8x &h=%8x &i=%8x\n", &g, &h, &i);
  g=7; h=8; i=9;
  C(g,h);
  printf("exit B\n");
  return 0;
}

int A(int x, int y)
{
  int d,e,f;
  printf("enter A\n");
  // PRINT ADDRESS OF d, e, f
  printf("&d=%8x &e=%8x &f=%8x\n", &d, &e, &f);
  d=4; e=5; f=6;
  B(d,e);
  printf("exit A\n");
  return 0;
}

int main(int argc, char *argv[ ], char *env[ ])
{
  int a,b,c;
  printf("enter main\n");

  printf("&argc=%x argv=%x env=%x\n", &argc, argv, env);
  printf("&a=%8x &b=%8x &c=%8x\n", &a, &b, &c);

  a=1; b=2; c=3;
  A(a,b);
  printf("exit main\n");
  return 0;
}
