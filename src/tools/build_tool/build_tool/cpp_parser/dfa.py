import logging
from typing import Dict, Iterator

from build_tool.cpp_parser.cpp_standard import CppStandard
from build_tool.cpp_parser.token import Token

class DFAState:
    """Class representing a DFA state"""
    def __init__(self, token_type=None, transition={}):
        """
        @param transition map from character to next DFAState. No transition for
               a particular character indicates that termination occurs at this
               state.
        @param token_type the type of token that should be generated if
               terminating at this state. None indicates that this is not a
               terminating state.
        """
        self.token_type = token_type
        self._transition = {}


    def transition(self, next_input: str):
        """
        Returns the next state given the next input character

        This method does not modify the instance it is called on in any way,
        and can thus be called from multiple threads simultaneously without
        the need for any synchronization mechanism.
        """
        if next_input in self._transition:
            return self._transition[next_input]
        return None


    def __contains__(self, item):
        return item in self._transition


class DFAState_WaitUntil(DFAState):
    """
    Class representing a DFA state in which the state will not change until a
    particular sequence is found

    Useful for representing the state a DFA reaches when reaching a
    multi-line comment or string literal in source code.
    """
    def __init__(self, token_type=None, transition={}, center_state=None):
        super().__init__(token_type, transition)
        self._center_state = center_state if center_state else self


    def transition(self, next_input: str) -> DFAState:
        if next_input in self._transition:
            return self._transition[next_input]
        return self._center_state


class DFAState_ExtraMatches(DFAState):
    """
    Class representing a DFA state in which an input character that does not
    otherwise cause a match will be passed to a function that maps input
    character to DFAState.

    Useful for implementing identifier tokenization, in which a mis-match of
    a potential keyword will be interpreted as an identifier.
    """
    def __init__(self, token_type=None, transition={}, map_func=lambda x: None):
        super().__init__(token_type, transition)
        self._map_func = map_func

    def transition(self, next_input: str) -> DFAState:
        if next_input in self._transition:
            return self._transition[next_input]
        return self._map_func(next_input)


class DFA:
    """
    Class to represent the 'structure' of a deterministic finite automaton

    That is, instances of this class represent the states and transition table
    of a DFA.
    """
    def __init__(self, standard: CppStandard):
        self._standard = standard
        # Create structure for identifier tokenization
        self._identifier_state = DFAState_ExtraMatches(Token.TYPE_IDENTIFIER)
        def tail_map(char):
            nonlocal standard
            if standard.is_identifier_char(char):
                return self._identifier_state
            return None
        self._identifier_state._map_func = tail_map

        self._start_state = DFAState_ExtraMatches()
        def start_map(char):
            nonlocal standard
            if standard.is_identifier_start_char(char):
                return self._identifier_state
            return None
        self._start_state._map_func = start_map

        # Add non-identifier structure
        list(map(lambda x: self._add_word(x, x.upper()),
                 standard.language_keywords))
        list(map(lambda x: self._add_word(x, 'PREPROCESSOR_' + x.upper()),
                 standard.preprocessor_keywords))
        list(map(lambda x: self._add_word(*x),
                 standard.operators))
        list(map(lambda x: self._add_word(*x),
                 standard.symbols))
        list(map(lambda x: self._add_matched_pair(*x),
                 standard.matched_pairs))


    @property
    def start_state(self):
        """Returns the DFA's start state"""
        return self._start_state


    def _add_helper(self, sequence: str,
                    start_state: DFAState=None) -> DFAState:
        """Add a given sequence to the DFA and return the final DFAState"""
        if start_state is None:
            current_state = self._start_state
        else:
            current_state = start_state
        for char in sequence:
            if not char in current_state:
                if ((current_state is self._start_state and
                     self._standard.is_identifier_start_char(char)) or
                    (current_state.token_type is Token.TYPE_IDENTIFIER and
                     self._standard.is_identifier_char(char))):
                    next_state = DFAState_ExtraMatches()
                    next_state._map_func = self._identifier_state._map_func
                    next_state.token_type = Token.TYPE_IDENTIFIER
                else:
                    next_state = DFAState()
                current_state._transition[char] = next_state
            current_state = current_state.transition(char)
        return current_state


    def _add_word(self, word: str, token_type) -> bool:
        """
        Add a given word of a specified token type to the DFA

        @return True if the word was added successfully. False otherwise.
        """
        current_state = self._add_helper(word)
        if (current_state.token_type is not None and
            current_state.token_type is not Token.TYPE_IDENTIFIER):
            logging.warning('DFA already contains word: %s' % word)
            return False
        current_state.token_type = token_type
        return True


    def _add_matched_pair(self, start: str, end: str, token_type,
                          escape_chars, prefix_list, suffix_list) -> bool:
        """
        Add a matched pair to the DFA

        A matched pair represents a sequence that invariably and exclusively
        begins with start. The sequence ends when end is found. Useful for
        representing multi-line comments and string literals.

        @return True if the pair was added successfully. False otherwise.
        """
        current_state = self._add_helper(start[:-1])
        if start[-1] in current_state:
            log.warning('Unable to add "%s" as the start sequence of a '
                        'matched pair, due to the existence of conflicts.' %
                        start)
            return False
        center_state = DFAState_WaitUntil()
        current_state._transition[start[-1]] = center_state
        current_state = center_state

        # add prefixes to DFA
        start_state = self._start_state._transition[start[0]]
        for prefix in prefix_list:
            state = self._add_helper(prefix)
            state._transition[start[0]] = start_state

        escape_state = DFAState_ExtraMatches()
        escape_state._map_func = lambda x: center_state
        for char in escape_chars:
            center_state._transition[char] = escape_state
        for char in end[:-1]:
            next_state = DFAState_WaitUntil(center_state=center_state)
            current_state._transition[char] = next_state
            current_state = next_state
        current_state._transition[end[-1]] = DFAState(token_type)

        # TODO(akhouderchah) add suffixes to DFA


    def _add_from_regex(self, regex):
        """
        Add a regex expression to the DFA
        TODO
        """
        def create_next_state():
            pass
        # If regular character, perform usual transition

        # If character set or range, add all transitions. Note that this can
        # lead to a lot of transitions in certain cases, but the current usage
        # of regex functionality is for numeric literals, which do not cause
        # this problem.

        # If group start, keep track of start state for potential repeats


class DFARunner:
    """
    Class to run through a DFA

    As DFA instances merely represent the static structure of a DFA, it does not
    change as it is used. Instead, this class is used to keep track of the
    relevant dynamic information for a DFA run, such that threads may share a
    potentially large DFA while only needing their own DFARunner instances.
    """
    def __init__(self, dfa: DFA):
        """Create a DFARunner instance tied to a given DFA"""
        self._dfa = dfa


    def run(self, input_str: str) -> Iterator[Token]:
        """
        Run through DFA with the provided input string and provide input tokens
        """
        current_state = self._dfa.start_state
        start_index = 0
        index = 0
        while index < len(input_str):
            next_state = current_state.transition(input_str[index])
            if next_state is None:
                yield Token(current_state.token_type,
                            input_str[start_index:index])
                current_state = self._dfa.start_state
                if current_state.transition(input_str[index]) is None:
                    index += 1
                start_index = index
            else:
                current_state = next_state
                index += 1
        if (current_state is not self._dfa.start_state and
            current_state.token_type is not None):
            yield Token(current_state.token_type, input_str[start_index:])
