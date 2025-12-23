"""
PlatformIO script to generate compile_commands.json for clangd.

This script runs before each build to ensure compile_commands.json
is always up-to-date for the clangd language server.
"""
import os
import subprocess
import sys

Import("env")  # noqa: F821 - PlatformIO special import


def generate_compiledb(*args, **kwargs):
    """Generate compile_commands.json using pio run -t compiledb."""
    project_dir = env.subst("$PROJECT_DIR")
    compile_db = os.path.join(project_dir, "compile_commands.json")

    # Only regenerate if compile_commands.json doesn't exist or is older than platformio.ini
    platformio_ini = os.path.join(project_dir, "platformio.ini")
    needs_update = not os.path.exists(compile_db)

    if not needs_update and os.path.exists(platformio_ini):
        db_mtime = os.path.getmtime(compile_db)
        ini_mtime = os.path.getmtime(platformio_ini)
        needs_update = ini_mtime > db_mtime

    if needs_update:
        print("Generating compile_commands.json for clangd...")
        try:
            subprocess.run(
                ["pio", "run", "-t", "compiledb"],
                cwd=project_dir,
                check=True,
                capture_output=True,
            )
            print("compile_commands.json generated successfully")
        except subprocess.CalledProcessError as e:
            print(f"Warning: Failed to generate compile_commands.json: {e}", file=sys.stderr)
        except FileNotFoundError:
            print("Warning: pio command not found, skipping compiledb generation", file=sys.stderr)


# Register the callback to run before build
env.AddPreAction("buildprog", generate_compiledb)
