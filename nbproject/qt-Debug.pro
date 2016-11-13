# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Debug/MinGW_Qt-Windows
TARGET = Quicksilver_Rail
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += debug 
PKGCONFIG +=
QT = core gui widgets
SOURCES += "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/board.cpp" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/boardWindow.cpp" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/connection.cpp" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/delivery.cpp" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/direction.cpp" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/location.cpp" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/player.cpp" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/settlement.cpp" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/terrain.cpp" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/train.cpp" main.cpp
HEADERS += "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/board.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/boardWindow.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/connection.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/connectionEnums.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/delivery.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/direction.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/engineer.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/goods.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/location.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/player.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/settlement.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/terrain.h" "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/train.h"
FORMS +=
RESOURCES += "C:/Users/tnc02_000/Documents/NetBeansProjects/Quicksilver Rail/images.qrc"
TRANSLATIONS +=
OBJECTS_DIR = build/Debug/MinGW_Qt-Windows
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += "/C/Program Files (Unprotected)/boost/boost_1_62_0" 
LIBS += 
equals(QT_MAJOR_VERSION, 4) {
QMAKE_CXXFLAGS += -std=c++14
}
