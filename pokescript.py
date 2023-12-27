#!/usr/bin/env python3

import sys
import os
import re
import argparse
import subprocess

CASCADE_COMMAND = "cd buster; make && { echo \"iommu!!!!!\" | sudo insmod buster.ko} || {echo \"iommu\" | sudo dmesg }> dump/dump-$(date +%d-%H-%M-%S).out"

STDOUT_FILE = "log.txt"

"""
This script is used to update the repo, run the cascade, and then update the repo again.

The steps are as follows:
1. 'git pull' to update the repo
2. Run the cascade command and append the output to log.txt
3. 'git add log.txt' to add the log file to the repo
4. 'git commit -m "Updated log.txt"' to commit the log file
5. 'git push' to push the commit to the repo

All error output is redirected to STDOUT_FILE
"""


class Log:
    def __init__(self):
        self.terminal = sys.stdout

        datetime = subprocess.check_output(["date"])
        with open(STDOUT_FILE, "a") as f:
            f.write("--------------------\n")
            f.write(datetime.decode("utf-8"))
            f.write("--------------------\n")

        self.log = open(STDOUT_FILE, "a")

    def write(self, message):
        if isinstance(message, bytes):
            message = message.decode("utf-8")
        self.terminal.write(message)
        self.log.write(message)

    def flush(self):
        pass

    def close(self):
        self.log.close()


LOG = Log()


def pull_repo():
    """
    Updates the repo by running 'git pull'
    """
    is_error = False
    message = ""
    try:
        message = subprocess.check_output(["git", "pull"])
    except subprocess.CalledProcessError as e:
        is_error = True
        message = "Error: an error occurred while updating the repo\n"
        message += str(e.output)

    LOG.write(message)
    if is_error:
        LOG.write("Exiting...\n")
    return not is_error


def run_cascade():
    """
    Runs the cascade command and appends the output to log.txt
    Adds any extra files to the git repo
    """
    is_error = False
    message = ""
    try:
        message = subprocess.check_output(CASCADE_COMMAND, shell=True, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        is_error = True
        message = "Error: an error occurred while running the cascade\n"
        message += str(e.output)

    # check for added files
    files = subprocess.check_output(["git", "status", "--porcelain"])
    files = files.decode("utf-8")
    files = files.split("\n")
    files = [f for f in files if f != ""]
    files = [f[3:] for f in files]
    files = [f for f in files if f != STDOUT_FILE]
    if len(files) > 0:
        message += "Log: the following files were added to the repo:\n"
        for f in files:
            message += f + "\n"

    for f in files:
        try:
            subprocess.check_output(["git", "add", f])
        except subprocess.CalledProcessError as e:
            is_error = True
            message += "Error: an error occurred while adding the file " + f + "\n"
            message += str(e.output)

    LOG.write(message)
    if is_error:
        LOG.write("Exiting...\n")
    return is_error


def add_log():
    """
    Adds the log file to the repo
    Note: we assume log.txt is closed so we cant write to it
    """
    is_error = False
    message = ""
    try:
        message = subprocess.check_output(["git", "add", STDOUT_FILE])
    except subprocess.CalledProcessError as e:
        is_error = True
        message = "Error: an error occurred while adding the log file\n"
        message += str(e.output)

    if is_error:
        print("FATAL ERROR: could not add log file to repo")
        print(message)
    return is_error


def main():
    if pull_repo():
        run_cascade()
    LOG.close()
    add_log()
    os.system("git commit -m \"Updated log.txt\"")
    os.system("git push")


if __name__ == "__main__":
    main()
