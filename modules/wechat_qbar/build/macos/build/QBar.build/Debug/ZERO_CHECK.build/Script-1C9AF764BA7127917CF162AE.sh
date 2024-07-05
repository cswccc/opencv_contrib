#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/build/macos
  make -f /Users/caoshiwen/work/opencv_contrib/modules/wechat_qbar/build/macos/CMakeScripts/ReRunCMake.make
fi

