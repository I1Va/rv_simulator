import sys
import os
import subprocess
import glob

def run_program(program_path, input_file, flags, output_file):
    try:
        result = subprocess.run(
            [program_path, input_file] + flags, 
            input="",
            text=True,
            capture_output=True,
            timeout=5
        )
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT", ""

    if output_file:
        with open(output_file, 'r') as f:
            expected_output = f.read().strip()
        
        actual_output = result.stdout.strip()
        return actual_output == expected_output, actual_output, expected_output

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 e2eTester.py <program_path> <test_cases_dir>")
        sys.exit(1)
    
    program_path = os.path.abspath(sys.argv[1])
    test_dir = sys.argv[2]
    
    search_pattern = os.path.join(test_dir, "**", "*.in")
    test_files = glob.glob(search_pattern, recursive=True)
    test_files.sort()
    
    if not test_files:
        print(f"No test files found in {test_dir}")
        sys.exit(1)
    
    all_passed = True
    
    for test_in in test_files:
        rel_path = os.path.relpath(test_in, test_dir)
        test_out = test_in.rsplit('.in', 1)[0] + '.out'
        
        print(f"Running {rel_path}...", end=' ')
        
        if not os.path.exists(test_out):
            print("SKIP (no .out file)")
            continue
        
        passed, actual, expected = run_program(program_path, test_in, ["--disable-logs"], test_out)
        
        if passed:
            print("PASSED")
        else:
            print("FAILED")
            print(f"   Expected: {repr(expected)}")
            print(f"   Got:      {repr(actual)}")
            all_passed = False
    
    sys.exit(0 if all_passed else 1)

if __name__ == "__main__":
    main()