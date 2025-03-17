import re

# Definición de tokens
TOKEN_SPECIFICATION = [
    ('NUMBER',   r'\d+'),          # Números enteros
    ('ID',       r'[a-zA-Z_]\w*'), # Identificadores
    ('ASSIGN',   r'='),             # Operador de asignación
    ('PLUS',     r'\+'),           # Operador de suma
    ('MINUS',    r'-'),             # Operador de resta
    ('MULT',     r'\*'),           # Operador de multiplicación
    ('DIV',      r'/'),             # Operador de división
    ('LPAREN',   r'\('),           # Paréntesis izquierdo
    ('RPAREN',   r'\)'),           # Paréntesis derecho
    ('SKIP',     r'[ \t]+'),       # Espacios y tabulaciones
    ('NEWLINE',  r'\n'),           # Nueva línea
    ('ERROR',    r'.')              # Cualquier otro carácter inválido
]

token_regex = '|'.join(f'(?P<{pair[0]}>{pair[1]})' for pair in TOKEN_SPECIFICATION)

def lexer(code):
    tokens = []
    for match in re.finditer(token_regex, code):
        kind = match.lastgroup
        value = match.group()
        if kind == 'SKIP' or kind == 'NEWLINE':
            continue
        elif kind == 'ERROR':
            raise SyntaxError(f'Error léxico: carácter inesperado {value}')
        tokens.append((kind, value))
    return tokens

# Analizador Sintáctico Simple (Ejemplo de expresión matemática)
def parser(tokens):
    def expression(index):
        """Regla de producción para expresiones matemáticas básicas"""
        if index < len(tokens) and tokens[index][0] == 'NUMBER':
            return index + 1
        elif index < len(tokens) and tokens[index][0] == 'ID':
            return index + 1
        else:
            raise SyntaxError(f'Error sintáctico: se esperaba un número o identificador en {tokens[index]}')
    
    index = 0
    while index < len(tokens):
        index = expression(index)
    
    print("Análisis sintáctico completado sin errores")

# Código de prueba
code = "x = 10 + 5"
tokens = lexer(code)
print("Tokens:", tokens)
parser(tokens)
