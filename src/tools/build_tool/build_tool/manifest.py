import json
import logging
import os

class Manifest():
    """
    This class represents a file-backed manifest, which keeps track of files
    and their modification times. The file used to persistently store the
    manifest data currently must be encoded in the JSON format.
    """
    # Version strings for forward/backward compatibility
    VERSION_MAJOR = 1
    VERSION_MINOR = 0
    VERSION_MAJOR_STR = 'VERSION_MAJOR'
    VERSION_MINOR_STR = 'VERSION_MINOR'

    def __init__(self, filename):
        def _json_obj_hook(dct):
            for k, v in dct.items():
                if k == self.VERSION_MAJOR_STR or k == self.VERSION_MINOR_STR:
                    dct[k] = int(v)
                    if (k == self.VERSION_MAJOR_STR and
                        dct[k] > self.VERSION_MAJOR):
                        raise Exception('%s is of a version too high to read'
                                        % filename)
                else:
                    dct[k] = float(v)
            return dct

        self._file_dates = {}
        self._filename = filename

        # Read manifest into memory
        if os.path.exists(self._filename):
           if not os.path.isfile(self._filename):
               raise NameError('%s is not a file')

           with open(self._filename) as f:
               try:
                   self._file_dates = json.load(f, object_hook=_json_obj_hook)
               except:
                   logging.warning('Failed to read %s, continuing '
                                   'with empty manifest.' % self._filename)
        # Update version info
        self._file_dates[self.VERSION_MAJOR_STR] = self.VERSION_MAJOR
        self._file_dates[self.VERSION_MINOR_STR] = self.VERSION_MINOR


    def is_changed_file(self, filename):
        """Returns whether or not a file is newer than that of the manifest"""
        if filename in self._file_dates:
            return self._file_dates[filename] < os.path.getmtime(filename)
        return True


    def update(self, filename):
        """
        Updates the manifest with the given file

        If the file exists in the manifest, the modified date will be re-read
        from the file system. Otherwise, the file will be added to the
        manifest.
        """
        self._file_dates[filename] = os.path.getmtime(filename)


    def close(self):
        with open(self._filename, 'w+') as f:
            json.dump(self._file_dates, f)
