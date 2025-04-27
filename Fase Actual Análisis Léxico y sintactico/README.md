
# Proyecto Traductor - Fase: Análisis Léxico y Sintáctico

## Estructura
- `analizador_lexico.py`: Analiza y tokeniza el código fuente.
- `analizador_sintactico.py`: Valida el orden y estructura de los tokens.
- `main.py`: Ejecuta el análisis completo.

## Uso
Coloca el código fuente en los archivos de ejemplo y ejecuta:

```
python main.py
```

## Casos de Prueba
- `ejemplo_valido.txt`: Código sin errores.
- `ejemplo_error.txt`: Código con errores léxicos y sintácticos.

## Manejo de errores
- **Errores léxicos:** Se detectan caracteres no permitidos y se reporta la posición.
- **Errores sintácticos:** Se detectan desórdenes de tokens y se informa el tipo de error.

