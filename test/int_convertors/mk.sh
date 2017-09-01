PYTHON=3.6
PYTHON_ARCH="$PYTHON"m
#PYTHON=2.7
#PYTHON_ARCH=$PYTHON

QT=5.9.1
QT_SHORT=5.9.1

PYROOT=/Library/Frameworks/Python.framework/Versions/$PYTHON

QTROOT=$HOME/bob/Qt$QT/$QT_SHORT/clang_64

# Run sip.
$PYROOT/bin/sip -c . -j 1 test.sip
#$PYROOT/bin/sip -c . -j 1 -t Qt_5_9_1 -t WS_MACX -I $PYROOT/share/sip/PyQt5 test.sip

# Compile C++.
c++ -c -pipe -fPIC -Os -w -I. -I$PYROOT/include/python"$PYTHON_ARCH" -o siptestpart0.o siptestpart0.cpp
#c++ -c -pipe -fPIC -Os -w -std=gnu++11 -F$QTROOT/lib -I. -I$PYROOT/include/python"$PYTHON_ARCH" -I$QTROOT/lib/QtCore.framework/Headers -o siptestpart0.o siptestpart0.cpp

# Link C++.
c++ -headerpad_max_install_names -bundle -undefined dynamic_lookup -o test.so siptestpart0.o
#c++ -headerpad_max_install_names -bundle -undefined dynamic_lookup -o test.so siptestpart0.o -F$QTROOT/lib -framework QtCore
