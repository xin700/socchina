#!/usr/bin/env python3
"""
Command-line interface for controlling the system microphone.

Usage:
    python cli.py [command]

Commands:
    enable    - Enable the microphone
    disable   - Disable the microphone
    toggle    - Toggle microphone state
    status    - Show current microphone state
    
If no command is provided, it will show the status and available commands.
"""

import sys
import argparse
from mic_control import MicrophoneController


def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description='Control system microphone')
    parser.add_argument('command', nargs='?', default='status', 
                        choices=['enable', 'disable', 'toggle', 'status'],
                        help='Command to execute (enable, disable, toggle, status)')
    return parser.parse_args()


def main():
    """Main function to handle command-line interface."""
    args = parse_arguments()
    mic = MicrophoneController()
    
    if args.command == 'enable':
        mic.enable_microphone()
    elif args.command == 'disable':
        mic.disable_microphone()
    elif args.command == 'toggle':
        mic.toggle_microphone()
    
    # Always show status after command execution
    state = mic.get_state()
    print(f"Microphone is currently: {'ENABLED' if state else 'DISABLED'}")


if __name__ == "__main__":
    main()
