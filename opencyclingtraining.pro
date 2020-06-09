QT += charts
requires(qtConfig(combobox))

HEADERS += \
    themewidget.h

SOURCES += \
    main.cpp \
    themewidget.cpp

target.path = build
INSTALLS += target

FORMS += \
    themewidget.ui
