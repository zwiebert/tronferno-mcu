import sys
import os
import argparse

def bin_to_c(input_file, array_name, output_file):
    with open(input_file, "rb") as f:
        data = f.read()
    
    with open(output_file, "w") as f:
        f.write(f"#include <stdint.h>\n\n")
        f.write(f"#include <stddef.h>\n\n")
        f.write(f"const uint8_t {array_name}[] = {{\n")
        # Format as hex bytes, 12 per line
        for i, byte in enumerate(data):
            f.write(f"0x{byte:02x}, ")
            if (i + 1) % 12 == 0:
                f.write("\n")
        f.write(f"\n}};\n")
        f.write(f"const size_t {array_name}_size = {len(data)};\n")
        encoding = "gzip" if input_file.endswith(".gz") else "br" if input_file.endswith(".br") else ""
        f.write(f"const char *{array_name}_encoding = \"{encoding}\";\n")
        f.write("\nstruct web_content { \n"
                 "  const unsigned char *content; const char *content_encoding; unsigned content_size; \n"
                 f"}} const {array_name}_fm = {{ \n"
                 f" .content = {array_name}, \n"
	         f" .content_encoding = \"{encoding}\", \n"
	         f" .content_size = {len(data)}, \n"
                 "};") 

if __name__ == "__main__":
    # Usage: python bin_to_c.py <input> <array_name> <output>
    bin_to_c(sys.argv[1], sys.argv[2], sys.argv[3])

