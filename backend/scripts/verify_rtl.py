import sys
import pyverilog
from pyverilog.vparser.parser import parse

def verify_rtl(filepath):
    try:
        ast, directives = parse([filepath])
        print("PASS")
        sys.exit(0)
    except Exception as e:
        print(f"Error parsing {filepath}:")
        print(e)
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python verify_rtl.py <path_to_verilog_file>")
        sys.exit(1)
        
    verilog_file = sys.argv[1]
    verify_rtl(verilog_file)
