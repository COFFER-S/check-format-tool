#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 COFFER-S Systems (Shanghai) CO., LTD
#
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import platform
import subprocess
from pathlib import Path

# Constants
BINARY_NAME = 'check_commit_format'
BINARY_NAME_WINDOWS = 'check_commit_format.exe'

# OS names
OS_LINUX = 'linux'
OS_WINDOWS = 'windows'
OS_MACOS = 'macos'
OS_DARWIN = 'darwin'


def get_os_folder():
    """Return the folder name corresponding to the operating system"""
    system = platform.system().lower()

    if system == OS_LINUX:
        return OS_LINUX
    elif system == OS_WINDOWS:
        return OS_WINDOWS
    elif system == OS_DARWIN:
        return OS_MACOS
    else:
        raise RuntimeError(f"Unsupported operating system: {system}")


def get_binary_path():
    """Get the binary file path corresponding to the current operating system"""
    os_folder = get_os_folder()

    # Determine binary name based on operating system
    if platform.system().lower() == OS_WINDOWS:
        binary_name = BINARY_NAME_WINDOWS
    else:
        binary_name = BINARY_NAME

    script_file = Path(__file__).resolve()
    script_str = str(script_file)

    # Detect if in pre-commit environment: check if __file__ path contains .cache/pre-commit
    is_pre_commit = '.cache/pre-commit' in script_str

    if is_pre_commit:
        parts = script_file.parts
        try:
            # Find the position of .cache/pre-commit in the path
            cache_idx = None
            for i, part in enumerate(parts):
                if part == '.cache' and i + 1 < len(parts) and parts[i + 1] == 'pre-commit':
                    cache_idx = i
                    break

            if cache_idx is not None and cache_idx + 2 < len(parts):
                repo_root = Path(*parts[:cache_idx + 3])
                binary_path = repo_root / os_folder / binary_name
            else:
                current = script_file.parent
                while current != current.parent:
                    test_path = current / os_folder / binary_name
                    if test_path.exists():
                        binary_path = test_path
                        break
                    current = current.parent
                else:
                    raise FileNotFoundError("Unable to find pre-commit cache directory")
        except Exception:
            current = script_file.parent
            while current != current.parent:
                test_path = current / os_folder / binary_name
                if test_path.exists():
                    binary_path = test_path
                    break
                current = current.parent
            else:
                raise FileNotFoundError(
                    f"Unable to find binary file in pre-commit environment\n"
                    f"Script path: {script_file}\n"
                    f"Please ensure {binary_name} file exists in the {os_folder} folder of the repository root"
                )
    else:
        # Normal usage scenario: search from script directory
        script_dir = script_file.parent
        binary_path = script_dir / os_folder / binary_name

    if not binary_path.exists():
        if is_pre_commit:
            raise FileNotFoundError(
                f"Binary file not found: {binary_path}\n"
                f"pre-commit environment: script path is {script_file}\n"
                f"Please ensure {binary_name} file exists in the {os_folder} folder of the repository root"
            )
        else:
            raise FileNotFoundError(
                f"Binary file not found: {binary_path}\n"
                f"Please ensure {binary_name} file exists in the {os_folder} folder"
            )

    if not os.access(binary_path, os.X_OK):
        if platform.system().lower() != OS_WINDOWS:
            os.chmod(binary_path, 0o755)

    return binary_path


def main():
    """Main function: Execute the binary file corresponding to the operating system, passing all commands and parameters"""
    try:
        binary_path = get_binary_path()
        
        # Get all command line arguments (except script name itself)
        # These arguments include:
        # - Options: -h, --help, -V, --version, -f, --format, -F, --full,
        #            -d, --dry_run, -v, --verbose, -l, --lines, etc.
        # - Commands: branch-name, commit-message, code-style
        # - File paths: optional <file_path>
        args = sys.argv[1:]

        # Build complete command: binary file path + all user-provided arguments
        cmd = [str(binary_path)] + args

        result = subprocess.run(
            cmd,
            check=False,
        )

        sys.exit(result.returncode)

    except FileNotFoundError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except RuntimeError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except KeyboardInterrupt:
        # Handle user interrupt (Ctrl+C)
        print("\nOperation cancelled", file=sys.stderr)
        sys.exit(130)
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()
