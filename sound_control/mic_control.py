#!/usr/bin/env python3
"""
Microphone Control Module

This module provides functionality to control the system microphone input on macOS
by enabling or disabling microphone input devices.
"""

import subprocess
import time
import os


class MicrophoneController:
    """
    A class to control the system microphone input on macOS.
    """

    def __init__(self):
        """Initialize the MicrophoneController."""
        self.current_state = self._get_current_state()

    def _get_current_state(self):
        """
        Get the current state of the microphone input.

        Returns:
            bool: True if microphone is enabled, False if disabled.
        """
        try:
            # Use applescript to check the input volume
            cmd = ['osascript', '-e', 'input volume of (get volume settings)']
            result = subprocess.run(cmd, capture_output=True, text=True, check=True)
            volume = int(result.stdout.strip())
            return volume > 0
        except Exception as e:
            print(f"Error getting microphone state: {e}")
            return None

    def enable_microphone(self):
        """
        Enable the system microphone input by setting the input volume to 100%.

        Returns:
            bool: True if the operation was successful, False otherwise.
        """
        try:
            # Set input volume to 100
            cmd = ['osascript', '-e', 'set volume input volume 100']
            subprocess.run(cmd, check=True)
            self.current_state = True
            print("Microphone enabled")
            return True
        except Exception as e:
            print(f"Error enabling microphone: {e}")
            return False

    def disable_microphone(self):
        """
        Disable the system microphone input by setting the input volume to 0.

        Returns:
            bool: True if the operation was successful, False otherwise.
        """
        try:
            # Set input volume to 0
            cmd = ['osascript', '-e', 'set volume input volume 0']
            subprocess.run(cmd, check=True)
            self.current_state = False
            print("Microphone disabled")
            return True
        except Exception as e:
            print(f"Error disabling microphone: {e}")
            return False

    def toggle_microphone(self):
        """
        Toggle the microphone state (enable if disabled, disable if enabled).

        Returns:
            bool: True if the operation was successful, False otherwise.
        """
        if self.current_state:
            return self.disable_microphone()
        else:
            return self.enable_microphone()

    def get_state(self):
        """
        Get the current state of the microphone.

        Returns:
            bool: True if enabled, False if disabled, None if error
        """
        self.current_state = self._get_current_state()
        return self.current_state


def main():
    """Main function to demonstrate microphone control."""
    mic = MicrophoneController()
    
    # Print current state
    state = mic.get_state()
    print(f"Current microphone state: {'enabled' if state else 'disabled'}")
    
    # Toggle microphone
    print("Toggling microphone...")
    mic.toggle_microphone()
    
    # Wait a bit and check new state
    time.sleep(1)
    state = mic.get_state()
    print(f"New microphone state: {'enabled' if state else 'disabled'}")
    
    # Toggle back
    print("Toggling microphone back to original state...")
    mic.toggle_microphone()


if __name__ == "__main__":
    main()
