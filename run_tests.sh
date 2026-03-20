#!/bin/bash

# ==============================================================================
# CONFIGURATION
EXEC="./da_project" 
GENERATED_FILE="output.csv" 
# ==============================================================================

echo "========================================="
echo "Compiling the project..."
echo "========================================="

# 1. Compile all .cpp files inside Code/src/ and output to da_project
g++ Code/src/*.cpp -o $EXEC

# Check if the compilation command threw an error
if [ $? -ne 0 ]; then
    echo "❌ Compilation failed! Please check your C++ errors. Aborting tests."
    exit 1
fi

echo "✅ Compilation successful!"
echo ""
echo "========================================="
echo "Starting automated tests..."
echo "========================================="

# Loop through datasets 1 to 14
for i in {1..14}; do
    INPUT_FILE="tests/input/dataset${i}.csv"
    EXPECTED_FILE="tests/output/dataset${i}.csv"
    
    # Check if input file exists
    if [ ! -f "$INPUT_FILE" ]; then
        echo "⚠️  Dataset $i: SKIPPED (Input file not found)"
        continue
    fi

    # Run the program in batch mode
    $EXEC -b "$INPUT_FILE" > /dev/null

    # Check if the program actually generated the file
    if [ ! -f "$GENERATED_FILE" ]; then
        echo "❌ Dataset $i: FAILED (No output file generated)"
        continue
    fi

    # Compare the files (ignoring whitespace and blank lines)
    if diff -q -w -B "$GENERATED_FILE" "$EXPECTED_FILE" > /dev/null; then
        echo "✅ Dataset $i: PASSED"
    else
        echo "❌ Dataset $i: FAILED (Outputs do not match)"
    fi
    
    # Clean up the generated file for the next loop iteration
    rm -f "$GENERATED_FILE"
done

echo "========================================="
echo "Testing complete!"