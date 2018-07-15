import logging
import os
import re
from queue import Queue

from build_tool.manifest import Manifest
from build_tool.utils import verify_dir
from build_tool.worker_thread import WorkerThread

class BuildTool:
    """Top-level entry into the build tool."""
    HEADER_MANIFEST = 'header_manifest.json'


    def __init__(self, src_dir, bin_dir):
        logging.info('Initializing build-tool')

        self._src_dir = verify_dir(src_dir)
        self._bin_dir = verify_dir(bin_dir)
        self._manifest = Manifest(
                os.path.join(self._bin_dir, self.HEADER_MANIFEST))
        self._work_queue = Queue()


    def run(self):
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
                    self._manifest.update(filename)

        # Wait for work on all headers to complete
        self._work_queue.join()
        list(map(lambda _: self._work_queue.put(None), range(thread_count)))
        list(map(lambda x: x.join(), worker_threads))

        self._shutdown()
        logging.info('build-tool completed successfully')


    def _shutdown(self):
        self._manifest.close()


    def _handle_header(self, filename):
        logging.info(filename)
