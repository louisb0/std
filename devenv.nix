{pkgs, ...}: {
  languages.cplusplus.enable = true;

  packages = with pkgs; [
    pkgs.clang-tools
    pkgs.alejandra
    pkgs.statix
    pkgs.deadnix
    pkgs.pre-commit
  ];

  scripts = {
    configure.exec = ''
      cmake -B build -S .
    '';

    rt.exec = ''
      cmake --build build --target tests
      if [ -n "$1" ]; then
        build/tests --gtest_filter="*$1*"
      else
        build/tests
      fi
    '';

    rb.exec = ''
      cmake --build build --target benchmarks
      if [ -n "$1" ]; then
        build/benchmarks --benchmark_filter="$1"
      else
        build/benchmarks
      fi
    '';
  };
}
