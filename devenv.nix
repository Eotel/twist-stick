{ pkgs, lib, config, inputs, ... }:

{
  packages = with pkgs; [
    platformio-core
    python3
    python3Packages.pyserial
    esptool
    git
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
    echo "Commands:"
    echo "  pio run              - Build"
    echo "  pio run -t upload    - Upload firmware"
    echo "  pio run -t uploadfs  - Upload SPIFFS data"
    echo "  pio device monitor   - Serial monitor"
  '';
}
