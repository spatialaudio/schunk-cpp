#include "schunk.h"
#include "iostream"

int main(int argc, char **argv)
{
  schunk::Module module(0x0B, 0, 9600, 10.0);
  float position;

  std::cout << module.get_pos(position) << std::endl;
  std::cout << position << std::endl;

  std::cout << module.move_pos(270.0, 40.0, true) << std::endl;
  std::cout << module.get_pos(position) << std::endl;
  std::cout << position << std::endl;

  std::cout << module.move_pos_rel(-270.0, 10.0, false) << std::endl;
  std::cout << module.get_pos(position) << std::endl;
  std::cout << position << std::endl;
}
