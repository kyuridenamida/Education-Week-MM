cat main.cpp | sed  "s/\/\/.*//g" | sed "s/#define LOG(\.\.\.).*/#define LOG(\.\.\.)/" | clang-format > frozen.cpp
