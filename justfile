# TwistStick Development Tasks
# Run 'just' to see all available commands

# Default target board
default_env := "m5stick-c-plus"

# List available commands
default:
    @just --list

# Build firmware (default: m5stick-c-plus)
build env=default_env:
    pio run -e {{env}}

# Build all environments
build-all:
    pio run

# Upload firmware to device
upload env=default_env:
    pio run -e {{env}} -t upload

# Upload SPIFFS data to device
upload-fs env=default_env:
    pio run -e {{env}} -t uploadfs

# Open serial monitor
monitor:
    pio device monitor

# Build and upload firmware
flash env=default_env: (build env) (upload env)

# Clean build artifacts
clean:
    pio run -t clean

# Full clean (remove .pio directory)
clean-all:
    rm -rf .pio

# Generate compile_commands.json for clangd
compiledb:
    pio run -t compiledb

# Format all source files
format:
    find src -name '*.cpp' -o -name '*.h' | xargs clang-format -i

# Check formatting (dry-run)
format-check:
    find src -name '*.cpp' -o -name '*.h' | xargs clang-format --dry-run --Werror

# Run clang-tidy on all source files
lint:
    find src -name '*.cpp' | xargs clang-tidy

# Run clang-tidy with fixes
lint-fix:
    find src -name '*.cpp' | xargs clang-tidy --fix

# Format and lint
check: format-check lint

# Setup development environment (generate compiledb)
setup: compiledb
    @echo "Development environment ready!"

# Watch and rebuild on changes (requires entr)
watch env=default_env:
    find src -name '*.cpp' -o -name '*.h' | entr -c just build {{env}}
