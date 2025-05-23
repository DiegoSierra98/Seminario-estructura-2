"""
Analizador semántico MUY sencillo para un subconjunto de C:
· Tipos soportados: int, float
· Declara variables y funciones
· Detecta: variables no declaradas, funciones no declaradas,
  número/tipo de argumentos y asignaciones int↔float

Uso:
    python analyzer.py ejemplo.c
"""

import re, sys, textwrap

# ---------- utilidades de análisis --------------------------------
def parse_decl(line):
    """
    Reconoce 'int a;'  o  'float x, y;'
    Devuelve (tipo, [nombres])  o  (None, [])
    """
    m = re.match(r'\s*(int|float)\s+([^;]+);', line)
    if not m:
        return None, []
    typ = m.group(1)
    names = [n.strip() for n in m.group(2).split(',')]
    return typ, names

def literal_type(tok):
    if re.fullmatch(r'[0-9]+\.[0-9]+', tok):
        return 'float'
    if re.fullmatch(r'[0-9]+', tok):
        return 'int'
    return None

# ---------- analizador principal ----------------------------------
def analyze(code:str):
    lines = [l.strip() for l in code.splitlines() if l.strip() and not l.strip().startswith('//')]

    global_vars   = {}        # nombre → tipo
    functions     = {}        # nombre → {'return': tipo, 'params': [(nombre,tipo),...]}
    errors, warns = [], []

    i = 0
    while i < len(lines):
        line = lines[i]

        # --------- declaración de función -----------
        m_fun = re.match(r'(int|float|void)\s+(\w+)\s*\(([^)]*)\)\s*\{', line)
        if m_fun:
            ret_type, fn_name, params_raw = m_fun.groups()
            local = {}  # tabla de símbolos local
            params = []

            if params_raw.strip():
                for p in params_raw.split(','):
                    t, names = parse_decl(p + ';')  # reuse
                    for n in names:
                        params.append((n, t))
                        local[n] = t
            functions[fn_name] = {'return': ret_type, 'params': params}

            # leer cuerpo
            i += 1
            while i < len(lines) and lines[i] != '}':
                inner = lines[i]

                # variables locales
                t, names = parse_decl(inner)
                if t:
                    for n in names:
                        local[n] = t
                    i += 1
                    continue

                # asignaciones
                m_asg = re.match(r'(\w+)\s*=\s*([^;]+);', inner)
                if m_asg:
                    lhs, rhs = m_asg.groups()
                    lhs_t = local.get(lhs) or global_vars.get(lhs)
                    if lhs_t is None:
                        errors.append(f"[{fn_name}] variable '{lhs}' no declarada")
                        lhs_t = 'undef'

                    rhs_t = eval_expr(rhs, local, global_vars, functions, errors, fn_name)

                    if lhs_t != rhs_t and rhs_t not in ('undef',):
                        warns.append(f"[{fn_name}] asignación {rhs_t}->{lhs_t} en '{lhs}'")
                i += 1
            # saltar la llave de cierre
            i += 1
            continue

        # --------- declaración global de variable -----------
        t, names = parse_decl(line)
        if t:
            for n in names:
                global_vars[n] = t
        i += 1

    return errors, warns

# ---------- evaluación de expresiones muy simple ------------------
def eval_expr(expr, local, global_, fns, errors, ctx):
    expr = expr.strip()

    # llamada de función  f(x,y)
    m_call = re.match(r'(\w+)\((.*)\)', expr)
    if m_call:
        name, argstr = m_call.groups()
        if name not in fns:
            errors.append(f"[{ctx}] función '{name}' no declarada")
            return 'undef'
        fn = fns[name]
        arg_tokens = [a.strip() for a in argstr.split(',')] if argstr.strip() else []
        if len(arg_tokens) != len(fn['params']):
            errors.append(f"[{ctx}] número de argumentos incorrecto en '{name}'")
        # comprobar tipos
        for tok, (_pname, ptype) in zip(arg_tokens, fn['params']):
            atype = token_type(tok, local, global_)
            if atype != ptype:
                errors.append(f"[{ctx}] tipo arg '{tok}' ({atype}) esperado {ptype}")
        return fn['return']

    # suma  a+b
    m_sum = re.match(r'(.+)\+(.+)', expr)
    if m_sum:
        lt = token_type(m_sum.group(1).strip(), local, global_)
        rt = token_type(m_sum.group(2).strip(), local, global_)
        return 'float' if 'float' in (lt, rt) else 'int'

    # literal o variable
    return token_type(expr, local, global_)

def token_type(tok, local, global_):
    return local.get(tok) or global_.get(tok) or literal_type(tok) or 'undef'

# ---------- CLI ---------------------------------------------------
def main():
    if len(sys.argv) != 2:
        print("Uso: python analyzer.py <archivo.c>")
        return
    with open(sys.argv[1], encoding='utf-8') as f:
        code = f.read()

    errs, warns = analyze(code)
    if errs:
        for e in errs: print("❌", e)
    else:
        print("✅ Sin errores semánticos")
    for w in warns: print("⚠️", w)

if __name__ == "__main__":
    main()
