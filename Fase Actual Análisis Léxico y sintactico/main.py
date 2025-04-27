
# main.py
from analizador_lexico import analizar_lexico
from analizador_sintactico import analizar_sintactico

def main():
    with open('ejemplos/ejemplo_valido.txt', 'r') as file:
        codigo = file.read()

    print("Código fuente:")
    print(codigo)
    print("\nTokens:")

    try:
        tokens = analizar_lexico(codigo)
        for token in tokens:
            print(token)
        
        print("\nAnálisis sintáctico:")
        analizar_sintactico(tokens)
        print("Análisis sintáctico exitoso.")
    except RuntimeError as e:
        print(e)

if __name__ == "__main__":
    main()
