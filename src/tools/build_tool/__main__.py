import logging
import os
import sys
from build_tool.build_tool import BuildTool
from build_tool.utils import verify_dir

def display_usage():
    print('usage: build_tool SRC_DIR BIN_DIR')
    print('')
    print('build_tool is a tool that runs before c++ pre-processing and ')
    print('compilation. It extracts metadata from the indicated classes, ')
    print('methods, and class members, generating c++ code that allows for a ')
    print('limited amount of reflection functionality within C++')


def setup_logging(bin_dir):
    def setup_handler(handler):
        nonlocal root_logger
        handler.setFormatter(log_formatter)
        root_logger.addHandler(handler)

    # Set up logging - place in both stdout and file
    root_logger = logging.getLogger()
    root_logger.setLevel(logging.DEBUG)

    log_filename = os.path.join(bin_dir, 'build-tool.log')
    file_handler = logging.FileHandler(log_filename, mode='w')
    file_handler.setFormatter(logging.Formatter('%(asctime)s [%(levelname)-5.5s] %(message)s', datefmt='%Y-%m-%d %H:%M:%S'))
    root_logger.addHandler(file_handler)

    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setFormatter(logging.Formatter('-- [build-tool] [%(levelname)-5.5s] %(message)s'))
    console_handler.setLevel(logging.INFO)
    root_logger.addHandler(console_handler)


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('-- [FAILURE] build_tool: Invalid arguments')
        display_usage()
        sys.exit(1)

    bin_dir = verify_dir(sys.argv[2])
    setup_logging(bin_dir)

    builder = BuildTool(sys.argv[1], bin_dir)
    builder.run()
