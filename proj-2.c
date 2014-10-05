#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proj-2.h"

int main (int argc, char *argv[])
{
  int task;
  GRAMMAR * g;
  if (argc < 2)
  {
    printf("Missing argument: task number\n1 String Generation Test\n2 FIRST Sets\n3 FOLLOW Sets\n");
    return 1;
  }
  task = atoi(argv[1]);
  g = newGrammar();
  switch (task)
  {
    case 1:
      printStrTest(g);
      break;
    case 2:
      calcFIRST(g);
      printSET(g, 0);
      break;
    case 3:
      calcFIRST(g);
      calcFOLLOW(g);
      printSET(g, 1);
      break;
    default:
      printf("Invalid argument: task number\n1 String Generation Test\n2 FIRST Sets\n3 FOLLOW Sets\n");
      return 2;
  }
  deleteGrammar(g);
  return 0;
}
