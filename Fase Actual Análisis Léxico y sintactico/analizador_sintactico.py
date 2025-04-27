
# analizador_sintactico.py

def analizar_sintactico(tokens):
    i = 0
    n = len(tokens)

    def match(expected_type):
        nonlocal i
        if i < n and tokens[i][0] == expected_type:
            i += 1
        else:
            raise RuntimeError(f"Error sintáctico: se esperaba {expected_type} pero se encontró {tokens[i][0]}")

    # Reglas muy básicas
    def expr():
        term()
        while i < n and tokens[i][0] in ('PLUS', 'MINUS'):
            match(tokens[i][0])
            term()

    def term():
        factor()
        while i < n and tokens[i][0] in ('TIMES', 'DIVIDE'):
            match(tokens[i][0])
            factor()

    def factor():
        if tokens[i][0] == 'ID' or tokens[i][0] == 'NUMBER':
            match(tokens[i][0])
        elif tokens[i][0] == 'LPAREN':
            match('LPAREN')
            expr()
            match('RPAREN')
        else:
            raise RuntimeError(f"Error sintáctico: token inesperado {tokens[i][0]}")

    expr()
    if i != n:
        raise RuntimeError(f"Error sintáctico: tokens restantes sin procesar")

