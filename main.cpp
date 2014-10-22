#include "schunk.h"

int main(int argc, char **argv)
{
  schunk::SerialConnection serial(0x0B, 0, 9600);

    if(serial.open()){
      printf("unable to open serial port to motor\n");
    return 1;
  }

  serial.close();
  return 0;
}
