import json
import logging
import os
from typing import Set

import build_tool.manifest_pb2 as proto
from build_tool.cpp_parser.token import Token


class Manifest():
    """
    This class represents a file-backed manifest, which keeps track of files
    and their modification times. The file used to persistently store the
    manifest data currently must be encoded in the JSON format.
    """
    # Version values for forward/backward compatibility.
    VERSION_MAJOR = 2
    VERSION_MINOR = 0

    class FileInfo():
        """
        This class represents the information stored about a file in a manifest.
        """
        def __init__(self, declared_components: Set[Token]=None):
            self.components = declared_components or set()
            self._mtime = float(0)
            self._visited = False


    def __init__(self, filename: str):
        self._filename = filename
        self._proto = proto.Manifest()

        # Read manifest into memory
        if os.path.exists(self._filename):
           if not os.path.isfile(self._filename):
               raise NameError('%s is not a file')

           with open(self._filename, 'rb') as f:
               try:
                   self._proto.ParseFromString(f.read())
                   if self._proto.version_major > self.VERSION_MAJOR:
                       raise Exception('%s is of a version too high to read'
                                       % filename)
               except:
                   logging.warning('Failed to read %s, continuing '
                                   'with empty manifest.' % self._filename)
                   self._proto = proto.Manifest()

        # Update version info
        self._proto.version_major = self.VERSION_MAJOR
        self._proto.version_minor = self.VERSION_MINOR


    def is_changed_file(self, filename: str) -> bool:
        """Returns whether or not a file is newer than that of the manifest."""
        if filename in self._proto.headers:
            return self._proto.headers[filename].mtime < os.path.getmtime(filename)
        return True


    def update_header(self, filename: str, components: Set[str]) -> None:
        """
        Updates the manifest with the given file.

        If the file exists in the manifest, the modified date will be re-read
        from the file system. Otherwise, the file will be added to the
        manifest.
        """
        info = self._proto.headers[filename]
        info.mtime = os.path.getmtime(filename)
        del info.components[:]
        list(map(lambda x: info.components.append(x), components))


    def close(self) -> None:
        with open(self._filename, 'wb+') as f:
            f.write(self._proto.SerializeToString())
