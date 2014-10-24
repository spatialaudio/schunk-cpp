#ifndef SCHUNK_H
#define SCHUNK_H

#include "rs232.h"
#include <time.h>
#include <string>
#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>  // for std::chrono::seconds
#include <thread>  // for std::this_thread::sleep_for

namespace schunk
{

const uint16_t _crc16_tbl[256] = {
0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040};

uint16_t crc16(const std::string &msg){
  uint16_t crc = 0x0000;
  for (unsigned int i=0; i<msg.size(); i++)
  {
    crc = ((crc & 0xFF00) >> 8) ^
      _crc16_tbl[(crc & 0x00FF) ^ (uint16_t)(msg[i] & 0x00FF)];
  }
  return crc;
}

template<typename type>
void pack(std::string &str, const type in)
{
  unsigned char const* c = reinterpret_cast<unsigned char const *>(&in);

  for (unsigned int i = 0; i< sizeof(type); i++)
  {
    str.push_back(c[i]);
  }
}

template<typename type>
type unpack(std::string &str)
{
  if ( str.size() < sizeof(type) )
  {
    std::cout << "ERROR: string to short for unpacking datatype" << std::endl;
  }

  type out;
  unsigned char* c = reinterpret_cast<unsigned char*>(&out);

  for (unsigned int i = 0; i < sizeof(type); i++)
  {
    c[sizeof(type)-1-i] = str.at(str.size()-1);
    str.erase(str.size()-1);
  }
  return out;
}

class Module
{
  public:
    Module(unsigned char module_id, unsigned int port,
      unsigned int baudrate, clock_t timeout = 0.0)
      : _module_id(module_id)
      , _baudrate(baudrate)
      , _port(port)
      , _timeout(timeout)
    {
      _enable_blocking = false;
      if ( !toggle_impulse_message(_enable_blocking) ) return;
      if ( _enable_blocking ) return;
      toggle_impulse_message(_enable_blocking);
    }

    bool move_pos(float position, bool blocking = false)
    {
      std::string data, response;
      pack<float>(data, position);
      return _cmd_message(0xB0, data, response, blocking);
    }

    bool move_pos(float position, float velocity, bool blocking = false)
    {
      std::string data, response;
      pack<float>(data, position);
      pack<float>(data, velocity);
      return _cmd_message(0xB0, data, response, blocking);
    }

    bool move_pos_rel(float position, bool blocking = false)
    {
      std::string data, response;
      pack<float>(data, position);
      return _cmd_message(0xB8, data, response, blocking);
    }

    bool move_pos_rel(float position, float velocity, bool blocking = false)
    {
      std::string data, response;
      pack<float>(data, position);
      return _cmd_message(0xB8, data, response, blocking);
    }

    bool get_pos(float &position)
    {
      std::string data, response;
      pack<float>(data, 0.0);
      pack<char>(data, 0x01);

      if ( !_cmd_message(0x95, data, response, false, 6) ) return false;

      response.erase(response.size() - 2);
      position = unpack<float>(response);

      return true;
    }

    bool toggle_impulse_message(bool &toogle)
    {
      std::string data, response;
      if ( !_cmd_message(0xE7, data, response, false) ) return false;

      if ( response == "ON" ) { toogle = true; return true; }
      if ( response == "OFF") { toogle = false; return true; }

      std::cout << "ERROR: Unexpected Response: " << response << std::endl;
      return false;
    }

  private:
    bool _cmd_message(char command
      , const std::string &data
      , std::string &response
      , bool blocking
      , const unsigned int expected_length = -1
      , const std::string expected_str = "")
    {
      if ( !_open() ) return false;

      if ( !_send(command, data) ) { _close(); return false; }

      if (_timeout <= 0.0) { _close(); return true; }

      if ( !_receive(response) ) { _close(); return false; }

      if ( !_check_response(response, command, expected_length, expected_str) )
      {
        _close();
        return false;
      }

      if ( blocking && _enable_blocking )
      {
        if ( !_receive(response) ) { _close(); return false; }
        if ( !_check_response(response, 0x94, 4) )
        {
          _close();
          return false;
        }
      }

      _close();
      return true;
    }

    bool _open()
    {
      if ( RS232_OpenComport(_port, _baudrate, "8N1") )
      {
        std::cout << "ERROR: Unable to open port" << std::endl;
        return false;
      }
      return true;
    }

    void _close()
    {
      RS232_CloseComport(_port);
    }

    bool _send(char command, const std::string &data)
    {
      std::string msg;
      msg += std::string(1, 0x05);
      msg += std::string(1, _module_id);
      pack<unsigned char>(msg, 1+data.size());
      pack<char>(msg, command);
      msg += data;
      pack<uint16_t>(msg, crc16(msg));  // append crc16 checksum

      unsigned char *buffer = new unsigned char[msg.size()];
      msg.copy(reinterpret_cast<char*>(buffer), msg.size(), 0);

      if ( msg.size() != RS232_SendBuf(_port, buffer, msg.size()) )
      {
        std::cout << "ERROR: Failed to send whole message" << std::endl;
        return false;
      }
      return true;
    }

    bool _receive(std::string &response)
    {
      clock_t timeout = clock() + _timeout * CLOCKS_PER_SEC;

      // receive header
      unsigned char header[3];
      if ( !_receive_bytes(header, 3, timeout) )
      {
        std::cout << "ERROR: Timeout while receiving Header" << std::endl;
        return false;
      }

      if ( header[0] != 0x03 && header[0] != 0x07 )
      {
          std::cout << "ERROR: Unexpected message type received" << std::endl;
          return false;
      }

      if (header[1] != _module_id)
      {
          std::cout << "ERROR: module id missmatch in received message" << std::endl;
          return false;
      }

      if (header[2] < 2)
      {
          std::cout << "ERROR: Received payload length too short" << std::endl;
          return false;
      }

      int payload_length = header[2];
      unsigned char *payload = new unsigned char[payload_length];
      if ( !_receive_bytes(payload, payload_length, timeout) )
      {
        std::cout << "ERROR: Timeout while receiving Payload" << std::endl;
        return false;
      }

      unsigned char crc[2];
      if ( !_receive_bytes(crc, 2, timeout) )
      {
        std::cout << "ERROR: Timeout while receiving CRC16" << std::endl;
        return false;
      }

      response.clear();
      response.append((char*)header, 3);
      response.append((char*)payload, payload_length);

      std::string crc_str;
      crc_str.append((char*)crc, 2);

      if ( crc16(response) != unpack<uint16_t>(crc_str) )
      {
        std::cout << "ERROR: CRC16 error in received message" << std::endl;
        return false;
      }
      response.erase(0,2);  // remove msg type and module id

      return true;
    }

    bool _receive_bytes(unsigned char* buffer, int exspected, clock_t timeout)
    {
      unsigned char* current = buffer;

      for (int received = 0; received < exspected; current = buffer + received)
      {
        if (clock() > timeout)
        {
          return false;
        }
        received += RS232_PollComport(_port, current, exspected - received);
        std::this_thread::sleep_for ( std::chrono::milliseconds(100) );
      }
      return true;
    }

    bool _check_response(std::string &response
      , char command
      , const int expected_length = -1
      , const std::string expected_str = "")
    {
      if ( response[0] == 2 )
      {
        printf("ERROR: code0x%02x\n", response[1]);
        return false;
      }

      if ( response[1] != command )
      {
        printf("ERROR: Unexpected command code (0x%02x) instead of 0x%02x \n", response[1], command);
        return false;
      }

      response.erase(0,2);
      if ( expected_length >=0 && expected_length != response.size() )
      {
        std::cout << "ERROR: Mismatch between expected length and response"
          " length" << std::endl;
        return false;
      }
      if ( !expected_str.empty() && expected_str != response )
      {
        std::cout << "ERROR: Unexpected data received" << std::endl;
        return false;
      }

      return true;
    }

    unsigned char _module_id;
    unsigned int _baudrate;
    unsigned int _port;
    clock_t _timeout;
    bool _enable_blocking;
};

}

#endif
