/*
 *      Author: Yuchen Hou
 */
//                   CS 360 LAB Assignment #1
//
//                     DUE: Week of 9-7-2015
//
//OBJECTIVES: Stack Usage and YOUR myprintf() FUNCTION
//
//An operating system supports only how to print char. All other printings are
//based on printing char. In Linux, putchar(char c); prints a char.
//

#include <stdio.h>
#include <stdlib.h>

int *FP;
//1. Use putchar() to write YOUR OWN prints(char *s) to print a string.
int prints(char *string){
	char *char_pointer = string;
	for(; *char_pointer != '\0'; char_pointer += 1){
		putchar(*char_pointer);
	}
	return 0;
}

//2. typedef unsigned int u32;
typedef unsigned int u32;
//   The following printu(u32 x) function prints an unsigned int (u32)x:
char *table = "0123456789ABCDEF";

int rpu(u32 x, char radix)
{
	int base = 10;
	if (radix == 'd'){
		base = 10;
	} else if (radix == 'o'){
		base = 8;
	} else if (radix == 'x'){
		base = 16;
	}
  char c;
  if (x){
     c = table[x % base];
     rpu(x / base, radix);
     putchar(c);
  }
  return 0;
}

int printu(u32 x)
{
  if (x==0)
     putchar('0');
  else
     rpu(x, 'd');
  putchar(' ');
  return 0;
}


//3. WRITE YOUR OWN functions:
//   int printd(int x): print an integer (which may be negative!!)
int printd(int x){
	if (x < 0){
		putchar('-');
		x = -x;
	}
	printu(x);
	return 0;
}

//   int printo(u32 x): print x in OCTal as 0.....
int printo(u32 x){
	putchar('0');
	putchar('o');
	if (x == 0){
		putchar('0');
	} else {
		rpu(x, 'o');
	}
	putchar(' ');
	return 0;
}

//   int printx(u32 x): print x in HEX.  as 0x....
int printx(u32 x){
	putchar('0');
	putchar('x');
	if (x == 0){
		putchar('0');
	} else {
		rpu(x, 'x');
	}
	putchar(' ');
	return 0;
}

//4. WRITE a myprintf(char *fmt, ...) for formatted printing, e.g.
//
//      myprintf("this is a string\n");   // print the string
//      myprintf("this %c is a %d test %x\n", 'a', 1234, 100);
//
//  Your myprintf() should print items by %c, %s, %u, %d, %o, %x,
//  just like printf().
//
//********************** HELP INFO ***********************************
int myprintf(char *fmt, ...) // SOME C compiler requires the 3 DOTs
{
//   Assume the call is myprintf(fmt, a,b,c,d);
//   Upon entry, the stack contains:
//
//                            |-> "....%c ..%s ..%d .. %x ....\n"
//                            |
//   HIGH                     |                                     LOW
//   ---------------------- --|------------------------------------------
//       |  d  | c | b | a | fmt |retPC| ebp | locals
//   -------------------------------------|------------------------------
//                                        |
//                                     CPU.ebp
//
//
//     1. Let char *cp point at the format string
	char *char_pointer = fmt;
//
//     2. Let int *ip  point at the first item to be printed on stack, e.g. a
	asm("movl %ebp, FP");    // CPU's ebp register is the FP
//	printf("FP = %8x\n", FP);
	int *item_pointer = FP + 3;

//  *************** ALGORITHM ****************
//   Use cp to scan the format string:
	for(; *char_pointer != '\0'; char_pointer += 1){
		//	    for each \n, spit out an extra \r
		if(*char_pointer == '\\'){
			if(char_pointer[1] == 'n'){
				putchar('\n');
				putchar('\r');
				char_pointer += 1;
			} else {
				putchar('\\');
			}
		}else if(*char_pointer != '%'){
			//		spit out any char that's NOT %
			putchar(*char_pointer);
		}else{
//			   Upon seeing a %: get next char, which must be one of 'c','s','u','d', 'o','x'
			char_pointer += 1;
			//			   Then call
			if(*char_pointer == 'c'){
				putchar(*item_pointer);
			} else if(*char_pointer == 's'){
				prints((char *)(long)(*item_pointer));
			} else if(*char_pointer == 'u'){
				printu(*item_pointer);
			} else if(*char_pointer == 'd'){
				printd(*item_pointer);
			} else if(*char_pointer == 'o'){
				printo(*item_pointer);
			} else if(*char_pointer == 'x'){
				printx(*item_pointer);
			}
//			   Advance ip to point to the next item on stack.
			item_pointer += 1;
		}
	}
   return 0;
}

//********************* t.c file *********************************

int mymain(int argc, char *argv[ ], char *env[ ])
{
  int i;

  myprintf("in mymain(): argc=%d\n", argc);

  for (i=0; i < argc; i++)
      myprintf("argv[%d] = %s\n", i, argv[i]);

  // WRITE CODE TO PRINT THE env strings
  for (i=0; env[i] != NULL; i++)
      myprintf("env[%d] = %s\n", i, env[i]);

  myprintf("---------- testing YOUR myprintf() ---------\n");
  myprintf("this is a test\n");
  myprintf("testing a=%d b=%x c=%c s=%s\n", 123, 123, 'a', "testing");
  myprintf("string=%s, a=%d  b=%u  c=%o  d=%x\n",
           "testing string", -1024, 1024, 1024, 1024);
  myprintf("mymain() return to main() in assembly\n");
  return 0;
}


//6. Run    gcc -m32 t.c s.s             to generate a.out
//   Run
//          a.out one two three four
//
//   to test your main, mymain() and myprintf()
//===============================================================
//
//7. Sample Solutions: samples/LAB1/
//                              |
//                             lab1.bin
//                             lab1.static
