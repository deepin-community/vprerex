FORMS         = window.ui
HEADERS       = documentwidget.h \
                window.h
SOURCES       = documentwidget.cpp \
                main.cpp \
                window.cpp

QT += widgets

# Modify these to refer to the directories on your system
# that contain the poppler-qt5.h header file and [lib]poppler-qt5 library.

INCLUDEPATH  += /usr/include/poppler/qt5 /usr/include/qt5 /usr/include/qt5/QtWidgets
LIBS         += -L/usr/lib64  -lpoppler-qt5 
