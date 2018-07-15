import threading

class WorkerThread(threading.Thread):
    """
    Thread to pull items off of a work queue and run a specified function.

    The thread will continue waiting for more work items until it recieves a
    work item that is None. Thus, for proper functioning, the end of the
    provided Queue must have 1 None element for each WorkerThread assigned to
    it.
    """
    def __init__(self, work_queue, func):
        super().__init__()
        self._queue = work_queue
        self._func = func


    def run(self):
        while True:
            work_item = self._queue.get()
            if work_item is None:
                self._queue.task_done()
                break
            self._func(work_item)
            self._queue.task_done()
