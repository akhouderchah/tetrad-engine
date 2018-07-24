class Token:
    """Object representing a lexical token"""

    TYPE_IDENTIFIER = 'IDENTIFIER' # names chosen by programmer
    PREVIEW_LENGTH = 32

    def __init__(self, token_type, text: str):
        self._type = token_type
        self._text = text


    def __repr__(self) -> str:
        return '(Token: %s - "%s")' % (self._type.upper(), self._text)


    def __str__(self) -> str:
        if self.text <= self.PREVIEW_LENGTH:
            text_preview = self.text
        else:
            text_preview = self.text[:self.PREVIEW_LENGTH-3] + '...'
        return '(Token: %s - "%s")' % (self._type.upper(),
                                       text_preview)
