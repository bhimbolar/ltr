[ -z $JOBS ] && JOBS=2
! [ -d build ] && mkdir build
rm -rf build/*
cd build
if [ "$OS" = "Windows_NT" ]; then
  $COMSPEC /c '..\jenkins\build.cmd'
else
  cmake ../
  make -j$JOBS
fi
./ltr_test
