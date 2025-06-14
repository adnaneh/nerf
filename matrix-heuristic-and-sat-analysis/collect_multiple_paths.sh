#!/bin/bash
# Script to collect data from multiple successful paths

echo "Collecting data from multiple winning paths..."

# Remove old data
rm -f v_analysis_multi.csv

# Run the program multiple times to collect diverse winning paths
for i in {1..20}; do
    echo "Run $i..."
    
    # Run once and check if successful
    if ./hireme2_v_analysis 2>&1 | grep -q "SUCCESS"; then
        echo "  Success! Appending to v_analysis_multi.csv"
        
        # If first success, copy with header
        if [ ! -f v_analysis_multi.csv ]; then
            cp v_analysis.csv v_analysis_multi.csv
        else
            # Append without header
            tail -n +2 v_analysis.csv >> v_analysis_multi.csv
        fi
        
        # Move the log to prevent overwriting
        mv v_analysis.csv v_analysis_run${i}.csv
    else
        echo "  Failed"
    fi
    
    # Small delay to ensure different random seeds
    sleep 1
done

echo "Data collection complete!"
echo "Checking results..."

# Count successful paths
if [ -f v_analysis_multi.csv ]; then
    echo "Total lines collected: $(wc -l v_analysis_multi.csv | awk '{print $1}')"
else
    echo "No successful runs found!"
fi