import bin_2_c
import sys
import io
import os
import argparse

build_dir="./"
in_dir=build_dir
out_dir=build_dir

table = str.maketrans(".", "_")


def file_2_c(in_file, out_directory):
    in_directory, filename = os.path.split(in_file)
    out_dir = out_directory if out_directory else in_directory
    bin_2_c.bin_to_c(f"{in_file}", filename.translate(table), f"{os.path.join(out_dir, filename)}.c")

def is_file(path):
    """Check if the path is an existing file."""
    if not os.path.isfile(path):
        raise argparse.ArgumentTypeError(f"'{path}' is not a valid file.")
    return path

def is_dir(path):
    """Check if the path is a valid directory or an empty string."""
    if not os.path.isdir(path):
        raise argparse.ArgumentTypeError(f"'{path}' is not a valid directory.")
    return path



if __name__ == "__main__":
    failed = 0
    parser = argparse.ArgumentParser()

    # Mandatory: Must be a file

    # 'nargs='+' collects all positional args into a list
    parser.add_argument("input_files", nargs='+', type=is_file, 
                        help="One or more input files")

    # Named argument for the directory
    parser.add_argument("-o", "--output-dir", type=is_dir, default=".",
                         help="Optional output directory")

    args = parser.parse_args()
    print(f"Directory: {args.output_dir}")
    for input_file in args.input_files:
        print(f"File: {input_file}")
        try:
            file_2_c(input_file, args.output_dir)
        except Exception as e:
            # Catching errors like Permission Denied or File Corrupt
            print(f"❌ Failed: {original_name} - Error: {e}")
            failed_files.append(original_name)
            failed = 1
    sys.exit(failed)
