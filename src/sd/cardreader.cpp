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

#include "../../base.h"

#if ENABLED(SDSUPPORT)

#ifdef __SAM3X8E__
  #include <avr/dtostrf.h>
#endif

char tempLongFilename[LONG_FILENAME_LENGTH + 1];
char fullName[LONG_FILENAME_LENGTH * SD_MAX_FOLDER_DEPTH + SD_MAX_FOLDER_DEPTH + 1];

bool estadoImprimiendo = false;
bool estabaImprimiendo;

CardReader::CardReader() {
  fileSize = 0;
  sdpos = 0;
  sdprinting = false;
  cardOK = false;
  saving = false;
  //estadoImprimiendo = false;

  workDirDepth = 0;
  memset(workDirParents, 0, sizeof(workDirParents));

  autostart_stilltocheck = true; //the SD start is delayed, because otherwise the serial cannot answer fast enough to make contact with the host software.

  //power to SD reader
  #if SDPOWER > -1
    OUT_WRITE(SDPOWER, HIGH);
  #endif // SDPOWER

  next_autostart_ms = millis() + SPLASH_SCREEN_DURATION;
}

char* CardReader::createFilename(char* buffer, const dir_t& p) { //buffer > 12characters
  char* pos = buffer, *src = (char*)p.name;
  for (uint8_t i = 0; i < 11; i++, src++) {
    if (*src == ' ') continue;
    if (i == 8) *pos++ = '.';
    *pos++ = *src;
  }
  *pos = 0;
  return pos;
}

/**
 * Dive into a folder and recurse depth-first to perform a pre-set operation lsAction:
 *   LS_Count       - Add +1 to nrFiles for every file within the parent
 *   LS_GetFilename - Get the filename of the file indexed by nrFiles
 */
void CardReader::lsDive(SdBaseFile parent, const char* const match/*=NULL*/) {
  dir_t* p;
  uint8_t cnt = 0;

  // Read the next entry from a directory
  while ((p = parent.getLongFilename(p, fullName, 0, NULL)) != NULL) {
    char pn0 = p->name[0];
    if (pn0 == DIR_NAME_FREE) break;
    if (pn0 == DIR_NAME_DELETED || pn0 == '.') continue;
    if (fullName[0] == '.') continue;

    if (!DIR_IS_FILE_OR_SUBDIR(p)) continue;

    filenameIsDir = DIR_IS_SUBDIR(p);

    if (!filenameIsDir && (p->name[8] != 'G' || p->name[9] == '~')) continue;
    switch (lsAction) {
      case LS_Count:
        nrFiles++;
        break;
      case LS_GetFilename:
        if (match != NULL) {
          if (strcasecmp(match, fullName) == 0) return;
        }
        else if (cnt == nrFiles) return;
        cnt++;
        break;
    }

  } // while readDir
}

void CardReader::ls()  {
  root.openRoot(fat.vol());
  root.ls(0, 0);
  workDir = root;
  curDir = &root;
}

void CardReader::initsd() {
  cardOK = false;
  if (root.isOpen()) root.close();

  #if ENABLED(SDEXTRASLOW)
    #define SPI_SPEED SPI_QUARTER_SPEED
  #elif ENABLED(SDSLOW)
    #define SPI_SPEED SPI_HALF_SPEED
  #else
    #define SPI_SPEED SPI_FULL_SPEED
  #endif

  if(!fat.begin(SDSS, SPI_SPEED)
    #if defined(LCD_SDSS) && (LCD_SDSS != SDSS)
      && !fat.begin(LCD_SDSS, SPI_SPEED)
    #endif
  ) {
    SERIAL_LM(ER, MSG_SD_INIT_FAIL);
    #if(TACTIL)
      cargarSdError();
    #endif
  }
  else {
    cardOK = true;
    SERIAL_EM(MSG_SD_CARD_OK);
    #if(TACTIL)
      cargarSdOk();
    #endif
  }
  fat.chdir(true);
  root = *fat.vwd();
  workDir = root;
  curDir = &root;
}

void CardReader::mount() {
  initsd();
}

void CardReader::unmount() {
  cardOK = false;
  sdprinting = false;
  //estadoImprimiendo = false;
}

void CardReader::startPrint() {
  if (cardOK) {
    sdprinting = true;
    estadoImprimiendo = true;
  }
}

void CardReader::pausePrint() {
  if (sdprinting) {
    sdprinting = false;
    estadoImprimiendo = false;
  }
}

void CardReader::continuePrint(bool intern) {}
void CardReader::PararPrint( /*=false*/) {
  sdprinting = false;
  estadoImprimiendo = false;
  file.sync();
  file.close();
  clear_command_queue();
  autotempShutdown();
  print_job_counter.stop();

}
void CardReader::stopPrint(bool store_location /*=false*/) {
  sdprinting = false;
  estadoImprimiendo = true;
  if (isFileOpen()) closeFile(store_location);
}

void CardReader::write_command(char* buf) {
  char* begin = buf;
  char* npos = 0;
  char* end = buf + strlen(buf) - 1;

  file.writeError = false;
  if ((npos = strchr(buf, 'N')) != NULL) {
    begin = strchr(npos, ' ') + 1;
    end = strchr(npos, '*') - 1;
  }
  end[1] = '\r';
  end[2] = '\n';
  end[3] = '\0';
  file.write(begin);
  if (file.writeError) {
    SERIAL_LM(ER, MSG_SD_ERR_WRITE_TO_FILE);
  }
}

const char *nombreArchivo;

bool CardReader::selectFile(const char* filename, bool silent/*=false*/) {
  const char *oldP = filename;
  nombreArchivo = filename;
  if(!cardOK) return false;

  file.close();

  if (!file.exists("restart.gcode")) {
    file.createContiguous(&workDir, "restart.gcode", 1);
    file.close();
  }

  if (file.open(curDir, filename, O_READ)) {
    if ((oldP = strrchr(filename, '/')) != NULL)
      oldP++;
    else
      oldP = filename;
    if(!silent) {
      if(!estabaImprimiendo){
        //guarda que esta imprimiendo
        estabaImprimiendo = true;
        SERIAL_EMV("START PRINTER:", estabaImprimiendo);
        enqueue_and_echo_commands_P(PSTR("M500"));
      }
      SERIAL_MT(MSG_SD_FILE_OPENED, oldP);
      SERIAL_EMT(MSG_SD_SIZE, file.fileSize());
    }

    for (int c = 0; c < sizeof(fullName); c++)
  		const_cast<char&>(fullName[c]) = '\0';
    strncpy(fullName, filename, strlen(filename));
    #if ENABLED(JSON_OUTPUT)
      parsejson(file);
    #endif
    sdpos = 0;
    fileSize = file.fileSize();
    SERIAL_EM(MSG_SD_FILE_SELECTED);
    return true;
  }
  else {
    if(!silent) SERIAL_EMT(MSG_SD_OPEN_FILE_FAIL, oldP);
    return false;
  }
}

void CardReader::printStatus() {
  if (cardOK) {
    SERIAL_MV(MSG_SD_PRINTING_BYTE, sdpos);
    SERIAL_EMV(MSG_SD_SLASH, fileSize);
  }
  else
    SERIAL_EM(MSG_SD_NOT_PRINTING);
}

void CardReader::startWrite(char *filename, bool lcd_status/*=true*/) {
  if(!cardOK) return;
  file.close();

  if(!file.open(curDir, filename, O_CREAT | O_APPEND | O_WRITE | O_TRUNC)) {
    SERIAL_LMT(ER, MSG_SD_OPEN_FILE_FAIL, filename);
  }
  else {
    saving = true;
    SERIAL_EMT(MSG_SD_WRITE_TO_FILE, filename);
    if (lcd_status) lcd_setstatus(filename);
  }
}
/*
void CardReader::seEntroAUnaCarpeta(char *filename, bool lcd_status){
  if(!cardOK) return;
  file.close();

  if(!file.open(curDir, filename, O_CREAT | O_APPEND | O_WRITE | O_TRUNC)) {
    SERIAL_LMT(ER, MSG_SD_OPEN_FILE_FAIL, filename);
  }
  else {
    saving = true;
    SERIAL_EMT(MSG_SD_WRITE_TO_FILE, filename);
    if (lcd_status) lcd_setstatus(filename);
  }
*/
void CardReader::deleteFile(char *filename) {
  if(!cardOK) return;
  sdprinting = false;
  //estadoImprimiendo = false;
  file.close();
  if(fat.remove(filename)) {
    SERIAL_EMT(MSG_SD_FILE_DELETED, filename);
  }
  else {
    if(fat.rmdir(filename))
      SERIAL_EMT(MSG_SD_FILE_DELETED, filename);
    else
      SERIAL_EM(MSG_SD_FILE_DELETION_ERR);
  }
}

void CardReader::finishWrite() {
    if(!saving) return; // already closed or never opened
    file.sync();
    file.close();
    saving = false;
    SERIAL_EM(MSG_SD_FILE_SAVED);
}

void CardReader::makeDirectory(char *filename) {
  if(!cardOK) return;
  sdprinting = false;
  //estadoImprimiendo = false;
  file.close();
  if(fat.mkdir(filename)) {
    SERIAL_EM(MSG_SD_DIRECTORY_CREATED);
  }
  else {
    SERIAL_EM(MSG_SD_CREATION_FAILED);
  }
}

/**
 * Get the name of a file in the current directory by index
 */
void CardReader::getfilename(uint16_t nr, const char* const match/*=NULL*/) {
  curDir = &workDir;
  lsAction = LS_GetFilename;
  nrFiles = nr;
  curDir->rewind();
  lsDive(*curDir, match);
}

uint16_t CardReader::getnrfilenames() {
  curDir = &workDir;
  lsAction = LS_Count;
  nrFiles = 0;
  curDir->rewind();
  lsDive(*curDir);
  return nrFiles;
}

void CardReader::chdir(const char* relpath) {
  SdBaseFile newfile;
  SdBaseFile* parent = &root;
  if (workDir.isOpen()) parent = &workDir;
  if (!newfile.open(parent, relpath, O_READ)) {
    SERIAL_EMT(MSG_SD_CANT_ENTER_SUBDIR, relpath);
  }
  else {
    if (workDirDepth < SD_MAX_FOLDER_DEPTH) {
      ++workDirDepth;
      for (int d = workDirDepth; d--;) workDirParents[d + 1] = workDirParents[d];
      workDirParents[0] = *parent;
    }
    workDir = newfile;
  }
}

void CardReader::updir() {
  if (workDirDepth > 0) {
    --workDirDepth;
    workDir = workDirParents[0];
    for (uint16_t d = 0; d < workDirDepth; d++)
      workDirParents[d] = workDirParents[d + 1];
  }
}


/*
bool CardReader::hola(char* filename) {
  if (selectFile(NEXTION_FIRMWARE_FILE, false)){
    return true;
  }else{
    return false;
  }
}
*/
//bool blink_save = true;
#if(TACTIL)
  void CardReader::indiceDeLectura(){
    //char bufferSdpos[11];
    //snprintf(bufferSdpos, sizeof bufferSdpos, "%lu", (unsigned long)sdpos);
    //SERIAL_MV("llamado uno", bufferSdpos);
    //SERIAL_MV("llamado dos", sdpos);
    //SERIAL_EM("fin de llamado\n");
  }
#endif

void CardReader::closeFile(bool store_location /*=false*/) {
  file.sync();
  file.close();
  saving = false;

  if (store_location) {
    char bufferFilerestart[50];
    char bufferX[11];
    char bufferY[11];
    char bufferZ[11];
    char bufferE[11];
    char bufferCoord[50];
    char bufferCoord1[50];
    char bufferCoord2[50];
    char bufferSdpos[11];
    char nameFile[15];
    char nameSAVE[50];

    unsigned long sdpos_save = (unsigned long)sdpos;

    snprintf(bufferSdpos, sizeof bufferSdpos, "%lu", (unsigned long)sdpos);

    strcpy(nameFile, "restart.gcode");
    if (!fileRestart.exists(nameFile)) {
      fileRestart.createContiguous(&workDir, nameFile, 1);
      fileRestart.close();
    }

    fileRestart.open(&workDir, nameFile, O_WRITE);
    fileRestart.truncate(0);

    dtostrf(current_position[X_AXIS], 1, 3, bufferX);
    dtostrf(current_position[Y_AXIS], 1, 3, bufferY);
    dtostrf(current_position[Z_AXIS], 1, 3, bufferZ);
    dtostrf(current_position[E_AXIS], 1, 3, bufferE);

    #if MECH(DELTA)
      strcpy(bufferCoord1, "G1 Z");
      strcat(bufferCoord1, bufferZ);
      strcat(bufferCoord1, " F8000");
    #else
      strcpy(bufferCoord1, "G92 Z");
      strcat(bufferCoord1, bufferZ);
    #endif

    strcpy(bufferCoord, "G1 X");
    strcat(bufferCoord, bufferX);
    strcat(bufferCoord, " Y");
    strcat(bufferCoord, bufferY);
    strcat(bufferCoord, " Z");
    strcat(bufferCoord, bufferZ);
    strcat(bufferCoord, " F3600");
    strcpy(bufferCoord2, "G92 E");
    strcat(bufferCoord2, bufferE);

    for (int8_t i = 0; i < (int8_t)strlen(fullName); i++)
      fullName[i] = tolower(fullName[i]);

    strcat(nameSAVE, fullName);


    strcpy(bufferFilerestart, "M34 S");
    strcat(bufferFilerestart, bufferSdpos);
    //guardado_codigo=bufferSdpos;
    strcat(bufferFilerestart, " @");
    strcat(bufferFilerestart, fullName);
    //guardado_nombre=fullName;


    fileRestart.write(bufferCoord1);
    fileRestart.write("\n");



    if (degTargetBed() > 0) {
      char Bedtemp[15];
      sprintf(Bedtemp, "M190 S%i\n", (int)degTargetBed());
      fileRestart.write(Bedtemp);
    }

    //char CurrHotend[10];
    //sprintf(CurrHotend, "T%i\n", active_extruder);
    //fileRestart.write(CurrHotend);

    for (uint8_t h = 0; h < HOTENDS; h++) {
      if (degTargetHotend(h) > 0) {
        char Hotendtemp[15];
        sprintf(Hotendtemp, "M109 T%i S%i\n", h, (int)degTargetHotend(h));
        fileRestart.write(Hotendtemp);
      }
    }

    fileRestart.write("G28 X Y\n");

    fileRestart.write(bufferCoord);
    fileRestart.write("\n");

   	if (fanSpeed > 0) {
      char fanSp[15];
      sprintf(fanSp, "M106 S%i\n", fanSpeed);
      fileRestart.write(fanSp);
    }

    char capas_total_sd[15];
    sprintf(capas_total_sd, "M711 S%i\n", total_capas);
    fileRestart.write(capas_total_sd);

    char capa_actul_sd[15];
    sprintf(capa_actul_sd, "M710 S%i\n", actual_capas);
    fileRestart.write(capa_actul_sd);

    fileRestart.write(bufferCoord2);
    fileRestart.write("\n");
    fileRestart.write(bufferFilerestart);
    fileRestart.write("\n");

    fileRestart.sync();
    fileRestart.close();

    while (!(commands_in_queue == 0)) {
      loop();
    }
    clear_command_queue();
    enqueue_and_echo_commands_P(PSTR(""));
    //SERIAL_EMV(" nameSAVE", nameSAVE);
    abrir_restart(nameSAVE ,sdpos_save);

  }
}


void CardReader::checkautostart(bool force) {
  if (!force && (!autostart_stilltocheck || next_autostart_ms >= millis()))
    return;

  autostart_stilltocheck = false;

  if (!cardOK) {
    initsd();
    if (!cardOK) return; // fail
  }

  fat.chdir(true);
  if(selectFile("init.g", true)) startPrint();
}

void CardReader::printingHasFinished() {
  stepper.synchronize();
  file.close();
  sdprinting = false;
  estadoImprimiendo = false;

  //Pregunta si se esta imprimiendo el archivo "restart.gcode"
  if(nombreArchivo != "restart.gcode"){
    estabaImprimiendo = false;

    enqueue_and_echo_commands_P(PSTR("M500"));
  }
  if(!se_viene_de_un_corte_de_luz){
    //se indica que se termino de imprimir
    print_job_counter.imprimiendo_estado = false;
    //no hay que abrir el cartel de corte de luz el archivo termino bien
    se_estaba_imprimiendo = false;

  }else{
    se_viene_de_un_corte_de_luz = !se_viene_de_un_corte_de_luz;
  }
  if (SD_FINISHED_STEPPERRELEASE) {
    enqueue_and_echo_commands_P(PSTR(SD_FINISHED_RELEASECOMMAND));
    print_job_counter.stop();
    enqueue_and_echo_commands_P(PSTR("M31"));
  }
  #if(TACTIL)
    finalizoImpresion();
  #endif
}

void CardReader::setroot(bool temporary /*=false*/) {
  if(temporary) lastDir = workDir;
  workDir = root;
  curDir = &workDir;
}

void CardReader::setlast() {
  workDir = lastDir;
  curDir = &workDir;
}

// --------------------------------------------------------------- //
// Code that gets gcode information is adapted from RepRapFirmware //
// Originally licenced under GPL                                   //
// Authors: reprappro, dc42, dcnewman, others                      //
// Source: https://github.com/dcnewman/RepRapFirmware              //
// Copy date: 27 FEB 2016                                          //
// --------------------------------------------------------------- //

void CardReader::parsejson(SdBaseFile &file) {
  fileSize = file.fileSize();
  filamentNeeded    = 0.0;
  objectHeight      = 0.0;
  firstlayerHeight  = 0.0;
  layerHeight       = 0.0;

  if (!file.isOpen()) return;

  bool genByFound = false, firstlayerHeightFound = false, layerHeightFound = false, filamentNeedFound = false;

  #if CPU_ARCH==ARCH_AVR
    #define GCI_BUF_SIZE 120
  #else
    #define GCI_BUF_SIZE 1024
  #endif

  // READ 4KB FROM THE BEGINNING
  char buf[GCI_BUF_SIZE];
  for (int i = 0; i < 4096; i += GCI_BUF_SIZE - 50) {
    if(!file.seekSet(i)) break;
    file.read(buf, GCI_BUF_SIZE);
    if (!genByFound && findGeneratedBy(buf, generatedBy)) genByFound = true;
    if (!firstlayerHeightFound && findFirstLayerHeight(buf, firstlayerHeight)) firstlayerHeightFound = true;
    if (!layerHeightFound && findLayerHeight(buf, layerHeight)) layerHeightFound = true;
    if (!filamentNeedFound && findFilamentNeed(buf, filamentNeeded)) filamentNeedFound = true;
    if(genByFound && layerHeightFound && filamentNeedFound) goto get_objectHeight;
  }

  // READ 4KB FROM END
  for (int i = 0; i < 4096; i += GCI_BUF_SIZE - 50) {
    if(!file.seekEnd(-4096 + i)) break;
    file.read(buf, GCI_BUF_SIZE);
    if (!genByFound && findGeneratedBy(buf, generatedBy)) genByFound = true;
    if (!firstlayerHeightFound && findFirstLayerHeight(buf, firstlayerHeight)) firstlayerHeightFound = true;
    if (!layerHeightFound && findLayerHeight(buf, layerHeight)) layerHeightFound = true;
    if (!filamentNeedFound && findFilamentNeed(buf, filamentNeeded)) filamentNeedFound = true;
    if(genByFound && layerHeightFound && filamentNeedFound) goto get_objectHeight;
  }

  get_objectHeight:
  // MOVE FROM END UP IN 1KB BLOCKS UP TO 30KB
  for (int i = GCI_BUF_SIZE; i < 30000; i += GCI_BUF_SIZE - 50) {
    if(!file.seekEnd(-i)) break;
    file.read(buf, GCI_BUF_SIZE);
    if (findTotalHeight(buf, objectHeight)) break;
  }
  file.seekSet(0);
}

void CardReader::printEscapeChars(const char* s) {
  for (unsigned int i = 0; i < strlen(s); ++i) {
    switch (s[i]) {
      case '"':
      case '/':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
      case '\\':
      SERIAL_C('\\');
      break;
    }
    SERIAL_C(s[i]);
  }
}

bool CardReader::findGeneratedBy(char* buf, char* genBy) {
  // Slic3r & S3D
  const char* generatedByString = PSTR("generated by ");
  char* pos = strstr_P(buf, generatedByString);
  if (pos) {
    pos += strlen_P(generatedByString);
    size_t i = 0;
    while (i < GENBY_SIZE - 1 && *pos >= ' ') {
      char c = *pos++;
      if (c == '"' || c == '\\') {
        // Need to escape the quote-mark for JSON
        if (i > GENBY_SIZE - 3) break;
        genBy[i++] = '\\';
      }
      genBy[i++] = c;
    }
    genBy[i] = 0;
    return true;
  }

  // CURA
  const char* slicedAtString = PSTR(";Sliced at: ");
  pos = strstr_P(buf, slicedAtString);
  if (pos) {
    strcpy_P(genBy, PSTR("Cura"));
    return true;
  }

  // UNKNOWN
  strcpy_P(genBy, PSTR("Unknown"));
  return false;
}

bool CardReader::findFirstLayerHeight(char* buf, float& firstlayerHeight) {
  // SLIC3R
  firstlayerHeight = 0;
  const char* layerHeightSlic3r = PSTR("; first_layer_height ");
  char *pos = strstr_P(buf, layerHeightSlic3r);
  if (pos) {
    pos += strlen_P(layerHeightSlic3r);
    while (*pos == ' ' || *pos == 't' || *pos == '=' || *pos == ':') {
      ++pos;
    }
    firstlayerHeight = strtod(pos, NULL);
    return true;
  }

  // CURA
  const char* layerHeightCura = PSTR("First layer height: ");
  pos = strstr_P(buf, layerHeightCura);
  if (pos) {
    pos += strlen_P(layerHeightCura);
    while (*pos == ' ' || *pos == 't' || *pos == '=' || *pos == ':') {
      ++pos;
    }
    firstlayerHeight = strtod(pos, NULL);
    return true;
  }

  return false;
}

bool CardReader::findLayerHeight(char* buf, float& layerHeight) {
  // SLIC3R
  layerHeight = 0;
  const char* layerHeightSlic3r = PSTR("; layer_height ");
  char *pos = strstr_P(buf, layerHeightSlic3r);
  if (pos) {
    pos += strlen_P(layerHeightSlic3r);
    while (*pos == ' ' || *pos == 't' || *pos == '=' || *pos == ':') {
      ++pos;
    }
    layerHeight = strtod(pos, NULL);
    return true;
  }

  // CURA
  const char* layerHeightCura = PSTR("Layer height: ");
  pos = strstr_P(buf, layerHeightCura);
  if (pos) {
    pos += strlen_P(layerHeightCura);
    while (*pos == ' ' || *pos == 't' || *pos == '=' || *pos == ':') {
      ++pos;
    }
    layerHeight = strtod(pos, NULL);
    return true;
  }

  return false;
}

bool CardReader::findFilamentNeed(char* buf, float& filament) {
  const char* filamentUsedStr = PSTR("filament used");
  const char* pos = strstr_P(buf, filamentUsedStr);
  filament = 0;
  if (pos != NULL) {
    pos += strlen_P(filamentUsedStr);
    while (*pos == ' ' || *pos == 't' || *pos == '=' || *pos == ':') {
      ++pos;    // this allows for " = " from default slic3r comment and ": " from default Cura comment
    }
    if (isDigit(*pos)) {
      char *q;
      filament += strtod(pos, &q);
      if (*q == 'm' && *(q + 1) != 'm') {
        filament *= 1000.0;        // Cura outputs filament used in metres not mm
      }
    }
    return true;
  }
  return false;
}

bool CardReader::findTotalHeight(char* buf, float& height) {
  int len = 1024;
  bool inComment, inRelativeMode = false;
  unsigned int zPos;
  for (int i = len - 5; i > 0; i--) {
    if (inRelativeMode) {
      inRelativeMode = !(buf[i] == 'G' && buf[i + 1] == '9' && buf[i + 2] == '1' && buf[i + 3] <= ' ');
    }
    else if (buf[i] == 'G') {
      // Ignore G0/G1 codes if absolute mode was switched back using G90 (typical for Cura files)
      if (buf[i + 1] == '9' && buf[i + 2] == '0' && buf[i + 3] <= ' ') {
        inRelativeMode = true;
      }
      else if ((buf[i + 1] == '0' || buf[i + 1] == '1') && buf[i + 2] == ' ') {
        // Look for last "G0/G1 ... Z#HEIGHT#" command as generated by common slicers
        // Looks like we found a controlled move, however it could be in a comment, especially when using slic3r 1.1.1
        inComment = false;
        size_t j = i;
        while (j != 0) {
          --j;
          char c = buf[j];
          if (c == '\n' || c == '\r') break;
          if (c == ';') {
            // It is in a comment, so give up on this one
            inComment = true;
            break;
          }
        }
        if (inComment) continue;

        // Find 'Z' position and grab that value
        zPos = 0;
        for (int j = i + 3; j < len - 2; j++) {
          char c = buf[j];
          if (c < ' ') {
            // Skip all whitespaces...
            while (j < len - 2 && c <= ' ') {
              c = buf[++j];
            }
            // ...to make sure ";End" doesn't follow G0 .. Z#HEIGHT#
            if (zPos != 0) {
              //debugPrintf("Found at offset %u text: %.100s\n", zPos, &buf[zPos + 1]);
              height = strtod(&buf[zPos + 1], NULL);
              return true;
            }
            break;
          }
          else if (c == ';') break;
          else if (c == 'Z') zPos = j;
        }
      }
    }
  }
  return false;
}

/**
 * File parser for KEY->VALUE format from files
 *
 * Author: Simone Primarosa
 *
 */
void CardReader::parseKeyLine(char* key, char* value, int &len_k, int &len_v) {
  if (!cardOK || !isFileOpen()) {
    key[0] = value[0] = '\0';
    len_k = len_v = 0;
    return;
  }
  int ln_buf = 0;
  char ln_char;
  bool ln_space = false, ln_ignore = false, key_found = false;
  while(!eof()) {   //READ KEY
    ln_char = (char)get();
    if(ln_char == '\n') {
      ln_buf = 0;
      ln_ignore = false;  //We've reached a new line try to find a key again
      continue;
    }
    if(ln_ignore) continue;
    if(ln_char == ' ') {
      ln_space = true;
      continue;
    }
    if(ln_char == '=') {
      key[ln_buf] = '\0';
      len_k = ln_buf;
      key_found = true;
      break; //key finded and buffered
    }
    if(ln_char == ';' || ln_buf+1 >= len_k || ln_space && ln_buf > 0) { //comments on key is not allowd. Also key len can't be longer than len_k or contain spaces. Stop buffering and try the next line
      ln_ignore = true;
      continue;
    }
    ln_space = false;
    key[ln_buf] = ln_char;
    ln_buf++;
  }
  if(!key_found) { //definitly there isn't no more key that can be readed in the file
    key[0] = value[0] = '\0';
    len_k = len_v = 0;
    return;
  }
  ln_buf = 0;
  ln_ignore = false;
  while(!eof()) {   //READ VALUE
    ln_char = (char)get();
    if(ln_char == '\n') {
      value[ln_buf] = '\0';
      len_v = ln_buf;
      break;  //new line reached, we can stop
    }
    if(ln_ignore|| ln_char == ' ' && ln_buf == 0) continue; //ignore also initial spaces of the value
    if(ln_char == ';' || ln_buf+1 >= len_v) { //comments reached or value len longer than len_v. Stop buffering and go to the next line.
      ln_ignore = true;
      continue;
    }
    value[ln_buf] = ln_char;
    ln_buf++;
  }
}

void CardReader::unparseKeyLine(const char* key, char* value) {
  if (!cardOK || !isFileOpen()) return;
  file.writeError = false;
  file.write(key);
  if (file.writeError) {
    SERIAL_LM(ER, MSG_SD_ERR_WRITE_TO_FILE);
    return;
  }

  file.writeError = false;
  file.write("=");
  if (file.writeError) {
    SERIAL_LM(ER, MSG_SD_ERR_WRITE_TO_FILE);
    return;
  }

  file.writeError = false;
  file.write(value);
  if (file.writeError) {
    SERIAL_LM(ER, MSG_SD_ERR_WRITE_TO_FILE);
    return;
  }

  file.writeError = false;
  file.write("\n");
  if (file.writeError) {
    SERIAL_LM(ER, MSG_SD_ERR_WRITE_TO_FILE);
    return;
  }
}

#endif //SDSUPPORT
