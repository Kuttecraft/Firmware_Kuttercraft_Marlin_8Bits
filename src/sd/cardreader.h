/**
 * MK & MK4due 3D Printer Firmware
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 - 2016 Alberto Cotronei @MagoKimbra
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

#ifndef CARDREADER_H
#define CARDREADER_H

#if ENABLED(SDSUPPORT)

#define SD_MAX_FOLDER_DEPTH 10     // Maximum folder depth
#define MAX_VFAT_ENTRIES (2)
#define FILENAME_LENGTH 13
/** Total size of the buffer used to store the long filenames */
#define LONG_FILENAME_LENGTH (FILENAME_LENGTH * MAX_VFAT_ENTRIES + 1)
#define SHORT_FILENAME_LENGTH 14
#define GENBY_SIZE 16

extern char tempLongFilename[LONG_FILENAME_LENGTH + 1];
extern char fullName[LONG_FILENAME_LENGTH * SD_MAX_FOLDER_DEPTH + SD_MAX_FOLDER_DEPTH + 1];

enum LsAction { LS_Count, LS_GetFilename };

#include "SDFat.h"

class CardReader {
public:
  SdFat fat;
  SdFile file;
  SdFile fileRestart;
  SdFile fileCarpeta;
  CardReader();

  void initsd();
  void mount();
  void unmount();
  void ls();
  void PararPrint();
  void getfilename(uint16_t nr, const char* const match = NULL);
  void startPrint();
  void pausePrint();
  void continuePrint(bool intern = false);
  void stopPrint(bool store_location = false);
  void write_command(char* buf);
  bool selectFile(const char *filename, bool silent = false);
  void printStatus();
  void startWrite(char* filename, bool lcd_status = true);
  void deleteFile(char* filename);
  void finishWrite();
  void makeDirectory(char* filename);
  void closeFile(bool store_location = false);
  //void seEntroAUnaCarpeta();
  char *createFilename(char *buffer, const dir_t &p);
  void printingHasFinished();
  void chdir(const char* relpath);
  void updir();
  //bool hola(char* filename);
  void setroot(bool temporary = false);
  void setlast();
  #if(TACTIL)
    void indiceDeLectura();
  #endif

  uint16_t getnrfilenames();

  void parseKeyLine(char* key, char* value, int &len_k, int &len_v);
  void unparseKeyLine(const char* key, char* value);

  FORCE_INLINE void setIndex(uint32_t newpos) { sdpos = newpos; file.seekSet(sdpos); }
  FORCE_INLINE bool isFileOpen() { return file.isOpen(); }
  FORCE_INLINE bool eof() { return sdpos >= fileSize; }
  FORCE_INLINE int16_t get() { sdpos = file.curPosition(); return (int16_t)file.read(); }
  FORCE_INLINE uint8_t percentDone() { return (isFileOpen() && fileSize) ? sdpos / ((fileSize + 99) / 100) : 0; }
  FORCE_INLINE char* getWorkDirName() { workDir.getFilename(fullName); return fullName; }

  //files init.g on the sd card are performed in a row
  //this is to delay autostart and hence the initialisaiton of the sd card to some seconds after the normal init, so the device is available quick after a reset
  void checkautostart(bool x);

  bool saving, sdprinting, cardOK, filenameIsDir, estadoImprimiendo, estabaImprimiendo;
  uint32_t fileSize, sdpos;
  float objectHeight, firstlayerHeight, layerHeight, filamentNeeded;
  char generatedBy[GENBY_SIZE];

  static void printEscapeChars(const char* s);

private:
  SdBaseFile root, *curDir, workDir, lastDir, workDirParents[SD_MAX_FOLDER_DEPTH];
  Sd2Card card;
  uint16_t workDirDepth;
  millis_t next_autostart_ms;
  uint16_t nrFiles; // counter for the files in the current directory and recycled as position counter for getting the nrFiles'th name in the directory.
  LsAction lsAction; //stored for recursion.
  bool autostart_stilltocheck; //the sd start is delayed, because otherwise the serial cannot answer fast enought to make contact with the hostsoftware.
  void lsDive(SdBaseFile parent, const char* const match = NULL);
  void parsejson(SdBaseFile &file);
  bool findGeneratedBy(char* buf, char* genBy);
  bool findFirstLayerHeight(char* buf, float& firstlayerHeight);
  bool findLayerHeight(char* buf, float& layerHeight);
  bool findFilamentNeed(char* buf, float& filament);
  bool findTotalHeight(char* buf, float& objectHeight);
};

extern CardReader card;

#define IS_SD_PRINTING (card.sdprinting)

#if PIN_EXISTS(SD_DETECT)
  #if ENABLED(SD_DETECT_INVERTED)
    #define IS_SD_INSERTED (READ(SD_DETECT_PIN) != 0)
  #else
    #define IS_SD_INSERTED (READ(SD_DETECT_PIN) == 0)
  #endif
#else
  //No card detect line? Assume the card is inserted.
  #define IS_SD_INSERTED true
#endif

#else

#define IS_SD_PRINTING (false)

#endif //SDSUPPORT

#endif //__CARDREADER_H
