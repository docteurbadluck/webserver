#!/bin/bash

# ====== CONFIGURATION ======
CONFIG_ORIGINAL="config.test.original"
CONFIG_FILE="config.test"
TEST_ORIGINAL="test_delete.original.c"
TEST_FILE="test_delete.c"
FIX_PATH_SCRIPT="fix_path.sh"

# ====== VARIABLES ======
PROJECT_ROOT="$(pwd)"
WEBSERVER_EXEC="./webserver"
TESTER_EXEC="./testeur_webserver/testeur_webserver"
TESTER_DIR="$PROJECT_ROOT/testeur_webserver"
WWW_FOLDER="$PROJECT_ROOT/www"
FORBIDDEN_DIR="$WWW_FOLDER/forbidden_directory"

# ====== STEP 0: Reset config ======
echo "Resetting config..."
cp "$CONFIG_ORIGINAL" "$CONFIG_FILE"

# ====== STEP 1: Fix paths in config ======
echo "Fixing paths in $CONFIG_FILE..."
sed -i "s|/home/docteurbadluck/Desktop/rearanged_webserver/|$PROJECT_ROOT/|g" "$CONFIG_FILE"
mv "$CONFIG_FILE" webserv/config.conf

# ====== STEP 2: Prepare forbidden directory ======
echo "Creating forbidden directory..."
mkdir -p "$FORBIDDEN_DIR"
touch "$FORBIDDEN_DIR/index.html"
chmod 000 "$FORBIDDEN_DIR/index.html"

# ====== STEP 3: Reset test_delete.c ======
echo "Resetting test file..."
cp "$TEST_ORIGINAL" "$TEST_FILE"
# optionally fix paths in test_delete.c if needed
sed -i "s|/home/docteurbadluck/Desktop/rearanged_webserver/|$PROJECT_ROOT/|g" "$TEST_FILE"

# Move test_delete.c to testeur_webserver
cp "$TEST_FILE" "$TESTER_DIR/"

# ====== STEP 4: Setup fix_path script ======
echo "Updating fix_path script..."
sed -i "s|config.conf|$TEST_FILE|g" "$FIX_PATH_SCRIPT"
rm "$TEST_FILE"

echo "All done."
