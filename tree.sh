#!/bin/bash

# Function to list files and directories in the tree-like format
list_tree() {
    local dir="$1"
    local prefix="$2"

    # Iterate over the files and directories in the specified directory
    for entry in "$dir"/*; do
        if [ -d "$entry" ]; then
            # If it's a directory, add a branch with a slash and recurse
            echo "${prefix}└── $(basename "$entry")/"
            list_tree "$entry" "    $prefix"
        elif [ -f "$entry" ]; then
            # If it's a file, just list it
            echo "${prefix}├── $(basename "$entry")"
        fi
    done
}

# Specify the directory you want to start the tree from (e.g., "your_kernel")
list_tree "../kernel" ""
