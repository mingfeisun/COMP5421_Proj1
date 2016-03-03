QT += widgets

HEADERS       = mainwindow.h \
    imagepro.h \
    showlabel.h \
    typedef.h \
    fibheap.h \
    fibheapnode.h
SOURCES       = main.cpp \
                mainwindow.cpp \
    showlabel.cpp \
    fibheap.cpp \
    fibheapnode.cpp
RESOURCES     = application.qrc \
    application.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/mainwindows/application
INSTALLS += target
