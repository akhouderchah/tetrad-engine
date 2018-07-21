

class CppParser:
    def parse_file(self, filename):
        # Read file into memory
        with open(filename, 'r') as f:
            self._text = f.read()

        # Strip comments
        
