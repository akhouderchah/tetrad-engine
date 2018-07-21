import logging
from threading import Lock
from typing import Iterator

from build_tool.cpp_parser.cpp_standard import CppStandard
from build_tool.cpp_parser.dfa import DFA, DFARunner
from build_tool.cpp_parser.token import Token

class Lexer:
    """
    Class to perform the lexical analysis of provided source files

    Note that instances of this class may be shared between multiple threads.
    Therefore, methods that involve modifying member variables
    post-initialization must employ a suitable synchronization mechanism.
    """
    def __init__(self, cpp_standard: CppStandard):
        self._cpp_standard = cpp_standard
        self._dfa = None
        self._generate_lock = Lock()


    def _generate_dfa(self):
        """
        Create DFA based on cpp_standard to be used for lexing

        Defer this creation until analyze_source such that workloads not
        involving the lexing of any files will not undergo the overhead of
        constructing this DFA.
        """
        with self._generate_lock:
            if self._dfa is None:
                self._dfa = DFA(self._cpp_standard)


    def analyze_source(self, filename: str) -> Iterator[Token]:
        # TODO(akhouderchah) return an iterator
        if self._dfa is None:
            self._generate_dfa()
        runner = DFARunner(self._dfa)
        with open(filename) as f:
            text = f.read()
        runner.run(text)

