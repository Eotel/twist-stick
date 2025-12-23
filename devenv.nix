{ pkgs, lib, config, inputs, ... }:

{
  packages = with pkgs; [
    platformio-core
    python3
    python3Packages.pyserial
    esptool
    git

    # C++ tooling
    clang-tools  # clangd, clang-format, clang-tidy

    # Task runner
    just
  ];

  env = {
    PLATFORMIO_CORE_DIR = "${config.devenv.root}/.platformio";
  };

  enterShell = ''
    # Add PlatformIO toolchain to PATH
    export PATH="${config.devenv.root}/.platformio/packages/toolchain-xtensa-esp-elf/bin:$PATH"

    echo "twist-stick Development Environment"
    echo "PlatformIO: $(pio --version)"
    echo ""
    echo "Run 'just' to see available commands"
  '';
}
