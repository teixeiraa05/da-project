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
    EXPECTED_FILE="tests/output/output_dataset${i}.csv"
    
    # Check if input file exists
    if [ ! -f "$INPUT_FILE" ]; then
        echo "⚠️  Dataset $i: SKIPPED (Input file not found)"
        continue
    fi

    # Run the program in batch mode
    $EXEC -b "$INPUT_FILE" > /dev/null

    # Dynamically find the generated file from the input data
    # It finds OutputFileName, "filename.csv" and extracts filename.csv
    GENERATED_FILE=$(grep "OutputFileName" "$INPUT_FILE" | cut -d',' -f2 | tr -d ' "[:space:]\r\n')
    if [ -z "$GENERATED_FILE" ]; then GENERATED_FILE="output.csv"; fi

    # Check if the program actually generated the file
    if [ ! -f "$GENERATED_FILE" ]; then
        echo "❌ Dataset $i: FAILED (No output file generated: $GENERATED_FILE)"
        continue
    fi

    # Compare the files (using diff exactly as requested by user)
    if diff -q "$GENERATED_FILE" "$EXPECTED_FILE"; then
        echo "✅ Dataset $i: PASSED"
    else
        echo "❌ Dataset $i: FAILED (Outputs do not match)"
    fi
    
    # Clean up the generated file for the next loop iteration
    rm -f "$GENERATED_FILE"
    #rm -rf "output_dataset${i}.dot"  # Instruction to quickly clean up any generated .dot files
done

echo "========================================="
echo "Testing complete!"