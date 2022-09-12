/**
 * @file NexUpload.cpp
 *
 * The implementation of upload tft file for nextion.
 *
 * @author  Chen Zengpeng (email:<zengpeng.chen@itead.cc>)
 * @date    2016/3/29
 * @copyright
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
 /**
  * Kuttercraft 3D Printer Firmware
  *
  * Based on Marlin, Sprinter and grbl
  * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
  * Copyright (C) 2013 - 2016 Alberto Cotronei @MagoKimbra
  * Copyright (C) 2017 Kuttercraft Kuttercraft@gmail.com
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program. If not, see <http://www.gnu.org/licenses/>.
  *
  */
#include "../../base.h"

#if ENABLED(SDSUPPORT) && ENABLED(NEXTION)

  #include "NexUpload.h"

  NexUpload::NexUpload(const char *file_name, uint32_t upload_baudrate) {
    _file_name = file_name;
    _upload_baudrate = upload_baudrate;
  }

  NexUpload::NexUpload(const String file_Name, uint32_t upload_baudrate) {
    NexUpload(file_Name.c_str(), upload_baudrate);
  }

  void NexUpload::startUpload(void) {
    if (!_checkFile()) {
      SERIAL_LM(ER, "El archivo esta no puede leerse");
      return;
    }
    if (_getBaudrate() == 0) {
      SERIAL_LM(ER, "Error de baudrate");
      return;
    }
    if (!_setUploadBaudrate(_upload_baudrate)) {
      SERIAL_LM(ER, "Modificar baudrate");
      return;
    }
    if (!_uploadTftFile()) {
      SERIAL_LM(ER, "Error al actualizar");
      sendCommand("page 20");
      return;
    }
    card.closeFile();
    SERIAL_EM("Actualizacion completada");
  }

  uint16_t NexUpload::_getBaudrate(void) {
    uint32_t baudrate_array[7] = {115200, 57600, 38400, 19200, 9600, 4800, 2400};
    for (uint8_t i = 0; i < 7; i++) {
      if (_searchBaudrate(baudrate_array[i])) {
        _baudrate = baudrate_array[i];
        break;
      }
    }
    return _baudrate;
  }

  bool NexUpload::_checkFile(void) {
    SERIAL_EMT("Actualizacion seleccionada ", _file_name);
    if (!card.selectFile(_file_name)) {
      SERIAL_LM(ER, "Archivo no encontrado");
      return 0;
    }
    _unuploadByte = card.fileSize;
    return 1;
  }

  bool NexUpload::_searchBaudrate(uint32_t baudrate) {
    String string = String("");
    nexSerial.end();
    HAL::delayMilliseconds(100);
    nexSerial.begin(baudrate);
    this->sendCommand("");
    this->sendCommand("connect");
    this->recvRetString(string);

    if(string.indexOf("comok") != -1)
      return 1;

    return 0;
  }

  void NexUpload::sendCommand(const char* cmd) {
    while (nexSerial.available())
      nexSerial.read();

    nexSerial.print(cmd);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
  }

  uint16_t NexUpload::recvRetString(String &string, uint32_t timeout,bool recv_flag) {
    uint16_t ret = 0;
    uint8_t c = 0;
    long start;
    bool exit_flag = false;
    start = millis();
    while (millis() - start <= timeout) {
      while (nexSerial.available()) {
        c = nexSerial.read();

        if(c == 0) continue;

        string += (char)c;
        if(recv_flag) {
          if(string.indexOf(0x05) != -1)
            exit_flag = true;
        }
      }
      if(exit_flag) break;
    }
    ret = string.length();
    return ret;
  }

  bool NexUpload::_setUploadBaudrate(uint32_t baudrate) {
    String string = String("");
    String cmd = String("");

    String filesize_str = String(_unuploadByte, 10);
    String baudrate_str = String(baudrate, 10);
    cmd = "whmi-wri " + filesize_str + "," + baudrate_str + ",0";

    this->sendCommand("");
    this->sendCommand(cmd.c_str());
    HAL::delayMilliseconds(50);
    nexSerial.begin(baudrate);
    this->recvRetString(string, 500);
    if(string.indexOf(0x05) != -1)
      return 1;

    return 0;
  }

  bool NexUpload::_uploadTftFile(void) {
    uint8_t c;
    uint16_t send_timer = 0;
    uint16_t last_send_num = 0;
    String string = String("");
    send_timer = _unuploadByte / 4096 + 1;
    last_send_num = _unuploadByte % 4096;

    while(send_timer) {
      if(send_timer == 1) {
        for(uint16_t j = 1; j <= 4096; j++) {
          if(j <= last_send_num) {
            c = card.get();
            nexSerial.write(c);
          }
          else
            break;
        }
      }
      else {
        for(uint16_t i = 1; i <= 4096; i++) {
          c = card.get();
          nexSerial.write(c);
        }
      }

      this->recvRetString(string, 500, true);
      if(string.indexOf(0x05) != -1)
        string = "";
      else
        return 0;

      --send_timer;
    }
  }

#endif  // SDSUPPORT
