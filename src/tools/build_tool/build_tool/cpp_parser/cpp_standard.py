import logging

class CppStandard:
    """
    Class representing the relevant information of a particular C++ standard
    """

    STANDARD_NONE = -1
    STANDARD_CPP_98 = 0
    STANDARD_CPP_03 = 1
    STANDARD_CPP_11 = 2
    STANDARD_CPP_14 = 3

    STANDARD_NAMES = ['98', '03', '11', '14']

    def __init__(self, standard_type=None):
        if standard_type is None:
            self._standard_type = self.STANDARD_CPP_14
        else:
            self._standard_type = standard_type

        self.preprocessor_keywords = self._get('PREPROCESSOR_KEYWORDS')
        self.language_keywords = self._get('CPP_KEYWORDS')
        self.operators = self._get('CPP_OPERATORS')
        self.symbols = self._get('CPP_SYMBOLS')
        self.matched_pairs = self._get('CPP_MATCHED_PAIRS')


    def _get(self, name):
        """Return the appropriate variable, using the set languange version"""
        # try specific standard versions, starting from most recent
        index = self._standard_type
        while self.STANDARD_NONE < index:
            var_name = '%s_%s' % (name, self.STANDARD_NAMES[index])
            if hasattr(self, var_name):
                return getattr(self, var_name)
            index -= 1
        # else try base name
        if hasattr(self, name):
            return getattr(self, name)
        raise NameError('CppStandard does not contain a %s variable for '
                        'any standard version' % name)


    def is_identifier_start_char(self, char):
        return char.isalpha() or char == '_'


    def is_identifier_char(self, char):
        return self.is_identifier_start_char(char) or char.isdigit()


    PREPROCESSOR_KEYWORDS_98 = [
        'if',
        'elif',
        'else',
        'endif',
        'defined',
        'ifdef',
        'ifndef',
        'define',
        'undef',
        'include',
        'line',
        'error',
        'pragma']

    PREPROCESSOR_KEYWORDS_11 = PREPROCESSOR_KEYWORDS_98 + ['_Pragma']

    CPP_KEYWORDS_98 = [
        'and',
        'and_eq',
        'asm',
        'auto',
        'bitand',
        'bitor',
        'bool',
        'break',
        'case',
        'catch',
        'char',
        'class',
        'compl',
        'const',
        'const_cast',
        'continue',
        'default',
        'delete',
        'do',
        'double',
        'dynamic_cast',
        'else',
        'enum',
        'explicit',
        'export',
        'extern',
        'false',
        'float',
        'for',
        'friend',
        'goto',
        'if',
        'inline',
        'int',
        'long',
        'mutable',
        'namespace',
        'new',
        'not',
        'not_eq',
        'operator',
        'or',
        'or_eq',
        'private',
        'protected',
        'public',
        'register',
        'reinterpret_cast',
        'return',
        'short',
        'signed',
        'sizeof',
        'static',
        'static_cast',
        'struct',
        'switch',
        'template',
        'this',
        'throw',
        'true',
        'try',
        'typedef',
        'typeid',
        'typename',
        'union',
        'unsigned',
        'using',
        'virtual',
        'void',
        'volatile',
        'wchar_t',
        'while',
        'xor',
        'xor_eq']

    CPP_KEYWORDS_11 = CPP_KEYWORDS_98 + [
        'alignas',
        'alignof',
        'char16_t',
        'char32_t',
        'constexpr',
        'decltype',
        'final',
        'noexcept',
        'nullptr',
        'override',
        'static_assert',
        'thread_local']

    CPP_OPERATORS = [
        ('::', 'OP_SCOPE'),
        ('++', 'OP_INCREMENT'),
        ('--', 'OP_DECREMENT'),
        ('->', 'OP_PTR_MEMBER_ACCESS'),
        ('.', 'OP_MEMBER_ACCESS'),
        ('+', 'OP_PLUS'),
        ('-', 'OP_MINUS'),
        ('!', 'OP_LOGICAL_NOT'),
        ('~', 'OP_BITWISE_NOT'),
        ('*', 'OP_STAR'),
        ('/', 'OP_DIVIDE'),
        ('\\', 'OP_LINE_CONT'),
        ('%', 'OP_MODULUS'),
        ('&', 'OP_AMPERSAND'),
        ('<<', 'OP_SHIFT_LEFT'),
        ('>>', 'OP_SHIFT_RIGHT'),
        ('<=', 'OP_LEQ'),
        ('==', 'OP_EQ'),
        ('!=', 'OP_NOT_EQ'),
        ('>=', 'OP_GEQ'),
        ('<', 'OP_LT'),
        ('>', 'OP_GT'),
        ('^', 'OP_BITWISE_XOR'),
        ('|', 'OP_BITWISE_OR'),
        ('||', 'OP_LOGICAL_OR'),
        ('&&', 'OP_LOGICAL_AND'),
        ('?', 'OP_TERTIARY_IF'),
        ('=', 'OP_ASSIGN'),
        ('+=', 'OP_ADD_ASSIGN'),
        ('-=', 'OP_SUB_ASSIGN'),
        ('/=', 'OP_DIV_ASSIGN'),
        ('*=', 'OP_MULT_ASSIGN'),
        ('%=', 'OP_MOD_ASSIGN'),
        ('<<=', 'OP_SHL_ASSIGN'),
        ('>>=', 'OP_SHR_ASSIGN'),
        ('&=', 'OP_AND_ASSIGN'),
        ('^=', 'OP_XOR_ASSIGN'),
        ('|=', 'OP_OR_ASSIGN'),
        (',', 'OP_COMMA')
    ]

    CPP_SYMBOLS = [
        ('(', 'SYMBOL_LPAREN'),
        (')', 'SYMBOL_RPAREN'),
        ('[', 'SYMBOL_LBRACKET'),
        (']', 'SYMBOL_RBRACKET'),
        ('{', 'SYMBOL_LBRACE'),
        ('}', 'SYMBOL_RBRACE'),
        (';', 'SYMBOL_SEMICOLON'),
        (':', 'SYMBOL_COLON'),
        ('#', 'SYMBOL_POUND'),
        ('...', 'SYMBOL_ELIPSES')
    ]

    CPP_MATCHED_PAIRS = [
        ('/*', '*/', 'COMMENT'),
        ("'", "'", 'LITERAL_CHAR'),
        ('"', '"', 'LITERAL_STRING')
    ]
