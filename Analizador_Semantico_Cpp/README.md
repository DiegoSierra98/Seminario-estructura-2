# Analizador Semántico (C++)

Este proyecto implementa un **analizador semántico simplificado** para un
subconjunto muy básico de C.  Detecta errores de tipos y de uso de
identificadores usando únicamente expresiones `+`, llamadas a funciones
y asignaciones.

## Compilar

```bash
g++ -std=c++17 analyzer.cpp -o analyzer
```

(En Windows con MinGW: `g++ -std=c++17 analyzer.cpp -o analyzer.exe`).

## Uso

```bash
./analyzer example1.c
./analyzer example2.c
```

La salida muestra:

* `❌` Errores semánticos.
* `⚠️` Advertencias (por ejemplo, asignar `float` a `int`).
* `✅` Si no se encontraron errores.

## Archivos

| Archivo         | Descripción                                     |
|-----------------|-------------------------------------------------|
| `analyzer.cpp`  | Código fuente del analizador en C++.            |
| `example1.c`    | Primer fragmento del enunciado.                 |
| `example2.c`    | Segundo fragmento del enunciado.                |
