{pkgs, ...}: {
  languages.cplusplus.enable = true;

  packages = with pkgs; [
    valgrind
    clang-tools

    alejandra
    statix
    deadnix
    pre-commit
  ];

  scripts = {
    build.exec = ''
      cmake -B build -S .
    '';

    clean.exec = ''
      rm -rf build/ .cache/
    '';

    rt.exec = ''
      cmake --build build --target tests && {
        if [ -n "$1" ]; then
          build/tests --gtest_filter="*$1*"
        else
          build/tests
        fi
      }
    '';

    vt.exec = ''
      cmake --build build --target tests && {
        FILTER=""
        [ -n "$1" ] && FILTER="--gtest_filter=*$1*"
        valgrind --leak-check=full --show-leak-kinds=all \
          --trace-children=yes --track-origins=yes \
          build/tests $FILTER
      }
    '';
  };
}
