
# analizador_lexico.py
import re

# Definición de tokens
token_specification = [
    ('NUMBER',   r'\d+'),
    ('ID',       r'[A-Za-z_][A-Za-z0-9_]*'),
    ('ASSIGN',   r'='),
    ('PLUS',     r'\+'),
    ('MINUS',    r'-'),
    ('TIMES',    r'\*'),
    ('DIVIDE',   r'/'),
    ('SEMI',     r';'),
    ('LPAREN',   r'\('),
    ('RPAREN',   r'\)'),
    ('LBRACE',   r'\{'),
    ('RBRACE',   r'\}'),
    ('SKIP',     r'[ \t]+'),
    ('MISMATCH', r'.'),
]

tok_regex = '|'.join('(?P<%s>%s)' % pair for pair in token_specification)
get_token = re.compile(tok_regex).match

def analizar_lexico(codigo):
    pos = 0
    tokens = []
    while pos < len(codigo):
        match = get_token(codigo, pos)
        if match:
            tipo = match.lastgroup
            lexema = match.group(tipo)
            if tipo == 'SKIP':
                pass
            elif tipo == 'MISMATCH':
                raise RuntimeError(f'Error léxico: carácter inesperado {lexema!r} en posición {pos}')
            else:
                tokens.append((tipo, lexema))
            pos = match.end()
        else:
            raise RuntimeError(f'Error léxico en posición {pos}')
    return tokens
