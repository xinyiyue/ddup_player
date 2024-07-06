#!/bin/bash

# 1) Check if doxygen is installed
check_doxygen() {
    if ! dpkg -l | grep -q '^ii.*doxygen'; then
        echo "Doxygen is not installed. Installing now..."
        # Use apt-get to install (requires root privileges or sudo)
        sudo apt-get update
        sudo apt-get install -y doxygen
    else
        echo "Doxygen is already installed."
    fi
}

# 2) Check if there is a Doxyfile configuration file in the current directory
generate_docs() {
    if [ -f "Doxyfile" ]; then
        echo "Generating Doxygen documentation using Doxyfile..."
        # Directly call doxygen
        doxygen Doxyfile
        echo "Doxygen documentation generation completed."
        xdg-open ./doc/html/index.html
    else
        echo "No Doxyfile configuration file found in the current directory."
    fi
}

# Call the functions
check_doxygen
generate_docs