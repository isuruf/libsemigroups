#!/bin/bash
set -e

bold() {
    printf "\033[1m%s\033[0m\n" "$*"
}

dim() {
    printf "\033[2m%s\033[0m\n" "$*"
}

delete_gcda_files() {
  bold "Deleting .gcda files . . .";
  printf "\033[2m";
  find . -name '*.gcda' -delete -print | sed -e 's:./::'
  printf "\033[0m";
}

if [[ $# -gt 2 || $# -le 0 ]]; then
  bold "error expected 1 or 2 arguments, got $#!"
  exit 1
elif [[ -f $1 && ! -x $1 ]]; then
  bold "error expected an executable file, $1 is not one!"
  exit 1
fi

if ! [[ -x configure ]]; then
  bold "No configure file found, running ./autogen.sh . . .";
  printf "\033[2m";
  ./autogen.sh;
  printf "\033[0m";
fi
if [[ ! -f config.log ]]; then
  bold "No config.log file found, running ./configure --enable-code-coverage CXXFLAGS=\"--coverage\". . ."
  printf "\033[2m";
  ./configure --enable-code-coverage CXXFLAGS="--coverage";
  ./autogen.sh;
  printf "\033[0m";
elif [[ ! -f Makefile ]]; then
  bold "No Makefile found, running ./configure --enable-code-coverage CXXFLAGS=\"--coverage\". . .";
  printf "\033[2m";
  ./configure --enable-code-coverage CXXFLAGS="--coverage";
  printf "\033[0m";
elif ! grep -q "\.\/configure.*\-\-enable-code\-coverage \-\-CXXFLAGS=\"\-\-coverage\"" config.log; then
  bold "Didn't find --enable-code-coverage flag in config.log, running make clean && ./configure --enable-code-coverage CXXFLAGS=\"--coverage\". . .";
  printf "\033[2m";
  make clean
  ./configure --enable-code-coverage CXXFLAGS="--coverage";
  printf "\033[0m";
fi

bold "Running make -j8 . . .";
printf "\033[2m";
make $1 -j8
printf "\033[0m";

delete_gcda_files

if [[ $# -eq 1 ]]; then
  bold "Running ./$1 \"[quick]\" . . .";
  if ! ./$1 "[quick]"; then
    delete_gcda_files
    exit 1
  fi
else
  bold "Running ./$1 $2 . . .";
  if ! ./$1 "$2"; then
    delete_gcda_files
    exit 1
  fi
fi

bold "Running lcov and genhtml . . .";
printf "\033[2m";
lcov  --directory . --capture --output-file "coverage.info.tmp" --test-name "libsemigroups_1_0_0" --no-checksum --no-external --compat-libtool --gcov-tool "gcov" | grep -v "ignoring data for external file"
lcov  --directory . --remove "coverage.info.tmp" "/tmp/*" "/Applications/*" --output-file "coverage.info"
LANG=C genhtml  --prefix . --output-directory "coverage" --title "libsemigroups Code Coverage" --legend --show-details "coverage.info"
rm -f coverage.info.tmp
printf "\033[0m";

delete_gcda_files

fnam=$1
fnam=${fnam/test_/}
fnam=${fnam//_/-}
dir=$(pwd)
hpp=coverage${dir}/include/${fnam}.hpp.gcov.html
echo "See: $hpp"
cpp=coverage${dir}/src/${fnam}.cpp.gcov.html
echo "Or:  $cpp"

exit 0
