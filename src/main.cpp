// Copyright (c) 2021 Inaba
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <vector>
#include <M5EPD.h>

// Create file list
void listFiles(std::vector<String>& files, File& file) {
  while (true) {
    File entry = file.openNextFile();
    if (!entry) {
      return;
    }
    if (entry.isDirectory()) {
      // ignore dot directories
      if (entry.name()[1] != '.') {
        listFiles(files, entry);
      }
    } else {
      files.push_back(String(entry.name()));
    }
    entry.close();
  }
}

// Draw a file list on Canvas.
// INFO: Cannot display too many files :-)
void drawFilelist(std::vector<String>& files, int index, M5EPD_Canvas& canvas) {
  canvas.fillCanvas(BLACK);
  canvas.setTextSize(3);
  int i = 0;
  for (const auto& file : files) {
    if (index == i) {
      canvas.fillRect(0, 8 + i * 32, 540, 32, WHITE);
      canvas.setTextColor(BLACK, WHITE);
    } else {
      canvas.setTextColor(WHITE, BLACK);
    }
    canvas.drawString(file.c_str(), 10, 10 + i * 32);
    ++i;
  }
  canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
}

// Draw image file (bmp, png, jpg)
void drawImage(const String& path, M5EPD_Canvas& canvas) {
  canvas.fillCanvas(BLACK);
  if (path.endsWith("bmp") || path.endsWith("BMP")) {
    canvas.drawBmpFile(SD, path, 0, 0);
  } else if (path.endsWith("png") || path.endsWith("PNG")) {
    canvas.drawPngFile(SD, path.c_str());
  } else if (path.endsWith("jpg") || path.endsWith("JPG")) {
    canvas.drawJpgFile(SD, path.c_str());
  }
  canvas.pushCanvas(0, 300, UPDATE_MODE_GC16);
}

namespace {
// Canvas for file list
M5EPD_Canvas upper(&M5.EPD);
// Canvas for images
M5EPD_Canvas lower(&M5.EPD);

std::vector<String> files;
int fileIndex = 0;

}  // namespace

void setup() {
  M5.begin();
  M5.EPD.SetRotation(90);
  M5.EPD.Clear(true);

  upper.createCanvas(540, 300);
  lower.createCanvas(540, 660);

  File root = SD.open("/");
  listFiles(files, root);
  drawFilelist(files, fileIndex, upper);
}

void loop() {
  M5.update();

  if (M5.BtnL.wasPressed()) {
    fileIndex--;
  } else if (M5.BtnR.wasPressed()) {
    fileIndex++;
  } else if (M5.BtnP.wasPressed()) {
    drawImage(files[fileIndex], lower);
    return;
  } else {
    return;
  }
  fileIndex = constrain(fileIndex, 0, files.size() - 1);
  drawFilelist(files, fileIndex, upper);
}
