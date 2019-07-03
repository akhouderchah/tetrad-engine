import logging
import os
import re
from queue import Queue

from build_tool.cpp_parser.cpp_standard import CppStandard
from build_tool.cpp_parser.lexer import Lexer
from build_tool.cpp_parser.token import Token
from build_tool.manifest import Manifest
from build_tool.utils import verify_dir
from build_tool.worker_thread import WorkerThread

class BuildTool:
    """Top-level entry into the build tool."""
    HEADER_MANIFEST = 'header_manifest'


    def __init__(self, src_dir: str, bin_dir: str):
        """
        Create a BuildTool instance.

        @param src_dir top-level directory of the project's source.
        @param bin_dir top-level directory in which to output files. Note that
               generated C++ source files will be created within the
               '_generated' subdirectory of bin_dir.
        """
        logging.info('Initializing build-tool')

        self._src_dir = verify_dir(src_dir)
        self._bin_dir = verify_dir(bin_dir)
        self._manifest = Manifest(
                os.path.join(self._bin_dir, self.HEADER_MANIFEST))
        self._work_queue = Queue()

        self._lexer = Lexer(CppStandard())


    def run(self) -> None:
        """
        Run the build tool and output all relevant files.
        """
        # Setup worker threads
        thread_count = 4
        worker_threads = [WorkerThread(self._work_queue, self._handle_header)
                          for _ in range(thread_count)]
        list(map(lambda x: x.start(), worker_threads))

        # Add all newly-modified headers (since the last run) to the work queue
        base_len = len(self._src_dir.split(os.sep)) - 1
        for root, dirs, files in os.walk(self._src_dir.split('tools')[0]):
            if 'external' in root:
                continue

            path = root.split(os.sep)

            logging.debug('%s%s' %
                    ((len(path)-base_len) * '---',
                    os.path.basename(root)))
            files = filter(lambda x: re.match('.*\.h$', x), files)
            for file in files:
                filename = os.path.join(root, file)
                if self._manifest.is_changed_file(filename):
                    logging.debug('Adding %s to the queue' % filename)
                    self._work_queue.put(filename)

        # Wait for work on all headers to complete
        list(map(lambda _: self._work_queue.put(None), range(thread_count)))
        self._work_queue.join()
        list(map(lambda x: x.join(), worker_threads))

        self._shutdown()
        logging.info('build-tool completed successfully')


    def _shutdown(self) -> None:
        self._manifest.close()


    def _handle_header(self, filename: str) -> None:
        """
        Perform the relevant operations on the given header file.

        This method should analyze the header to find information of interest
        and update the manifest with this information. Generated code should
        *not* be produced in this method, as that allows for the potential of
        generating code with information not stored in the manifest which can be
        a problem because only files that have changed are analyzed in this
        method.
        """
        components = set()
        is_class_name = False
        for token in self._lexer.analyze_source(filename):
            if "class" in token._text:
                is_class_name = True
            elif is_class_name:
                is_class_name = False
                if "Component" in token._text and token._type == Token.TYPE_IDENTIFIER:
                    components.add(token._text)
        print(components)
        self._manifest.update_header(filename, components)

