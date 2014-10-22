#ifndef SCHUNK_H
#define SCHUNK_H

#include "rs232.h"

namespace schunk
{

class SerialConnection
{
  public:
    SerialConnection(unsigned int module_id, unsigned int port,
      unsigned int baudrate, unsigned int timeout = 0)
      : _module_id(module_id)
      , _baudrate(baudrate)
      , _port(port)
      , _timeout(timeout)
    {}

    bool open()
    {
      return RS232_OpenComport(_port, _baudrate, "8N1");
    }
    void close()
    {
      return RS232_CloseComport(_port);
    }

  private:
    unsigned char _module_id;
    unsigned int _baudrate;
    unsigned int _port;
    unsigned int _timeout;
};

}

#endif
