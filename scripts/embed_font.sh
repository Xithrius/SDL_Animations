#!/bin/bash

# Script to embed font files into the binary
# Usage: ./scripts/embed_font.sh [font_file]

FONT_FILE=${1:-"assets/fonts/SpaceMono-Regular.ttf"}
OUTPUT_FILE="src/graphics/embedded_font.h"

if [ ! -f "$FONT_FILE" ]; then
    echo "Error: Font file '$FONT_FILE' not found!"
    exit 1
fi

echo "Embedding font: $FONT_FILE -> $OUTPUT_FILE"

# Generate the embedded font header with extern declaration
xxd -i "$FONT_FILE" > "$OUTPUT_FILE"
# Replace the variable definition with extern declaration
sed -i 's/unsigned int assets_fonts_SpaceMono_Regular_ttf_len = \([0-9]*\);/extern unsigned int assets_fonts_SpaceMono_Regular_ttf_len;/' "$OUTPUT_FILE"

echo "Font embedded successfully!"
echo "File size: $(stat -c%s "$FONT_FILE") bytes"
