/*
 * Analizador semántico MUY simplificado en C++
 * ------------------------------------------------
 * - Tipos soportados: int, float
 * - Variables globales, locales y funciones.
 * - Reglas:
 *     · variable usada debe estar declarada
 *     · función llamada debe existir
 *     · número y tipo de argumentos debe coincidir
 *     · advertencia cuando se asigna float -> int
 *
 * Compilación:
 *     g++ -std=c++17 analyzer.cpp -o analyzer
 *
 * Uso:
 *     ./analyzer ejemplo.c
 *
 * Este código NO es un compilador real, solo un chequeo
 * lineal con expresiones muy básicas (suma y llamadas).
 */
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

struct Function {
    char ret;
    std::vector<char> params;  // tipos de parámetros
};

// Helpers ----------------------------------------------------
bool isFloatLiteral(const std::string &tok) {
    return std::regex_match(tok, std::regex(R"(^\d+\.\d+$)"));
}
bool isIntLiteral(const std::string &tok) {
    return std::regex_match(tok, std::regex(R"(^\d+$)"));
}
char literalType(const std::string &tok) {
    if (isFloatLiteral(tok)) return 'f';
    if (isIntLiteral(tok))   return 'i';
    return 0;
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        item.erase(item.begin(), std::find_if(item.begin(), item.end(),
                     [](unsigned char ch){ return !std::isspace(ch); }));
        item.erase(std::find_if(item.rbegin(), item.rend(),
                     [](unsigned char ch){ return !std::isspace(ch); }).base(), item.end());
        if (!item.empty()) out.push_back(item);
    }
    return out;
}
char tokenType(const std::string &tok,
               const std::unordered_map<std::string,char> &localVars,
               const std::unordered_map<std::string,char> &globalVars)
{
    if (auto it = localVars.find(tok); it != localVars.end()) return it->second;
    if (auto it = globalVars.find(tok); it != globalVars.end()) return it->second;
    char lit = literalType(tok);
    if (lit) return lit;
    return 0; // undefined
}

// Expression evaluation (only + or function call or literal/var)
char evalExpr(const std::string &expr,
              const std::unordered_map<std::string,char> &localVars,
              const std::unordered_map<std::string,char> &globalVars,
              const std::unordered_map<std::string,Function> &funcs,
              std::vector<std::string> &errors,
              const std::string &ctx)
{
    std::string e = expr;
    e.erase(remove_if(e.begin(), e.end(), ::isspace), e.end()); // remove spaces

    // function call f(...)
    std::smatch m;
    if (std::regex_match(e, m, std::regex(R"(^(\w+)\((.*)\)$)"))) {
        std::string name = m[1];
        std::string argstr = m[2];
        auto fit = funcs.find(name);
        if (fit == funcs.end()) {
            errors.push_back("[" + ctx + "] función '" + name + "' no declarada");
            return 0;
        }
        const Function &fn = fit->second;
        std::vector<std::string> argTokens = split(argstr, ',');
        if (argTokens.size() != fn.params.size()) {
            errors.push_back("[" + ctx + "] número de argumentos incorrecto en '" + name + "'");
        }
        size_t n = std::min(argTokens.size(), fn.params.size());
        for (size_t i=0;i<n;++i) {
            char at = tokenType(argTokens[i], localVars, globalVars);
            if (at == 0) errors.push_back("[" + ctx + "] identificador '" + argTokens[i] + "' no declarado");
            else if (at != fn.params[i]) {
                errors.push_back("[" + ctx + "] tipo arg '" + argTokens[i] + "' (" +
                                 std::string(1,at)+") esperado "+ std::string(1,fn.params[i]));
            }
        }
        return fn.ret;
    }

    // addition a+b
    size_t plusPos = e.find('+');
    if (plusPos != std::string::npos) {
        std::string left = e.substr(0, plusPos);
        std::string right = e.substr(plusPos+1);
        char lt = tokenType(left, localVars, globalVars);
        char rt = tokenType(right, localVars, globalVars);
        return (lt=='f' || rt=='f') ? 'f' : 'i';
    }

    // variable or literal
    return tokenType(e, localVars, globalVars);
}

// ----------------------------------------------------------------
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Uso: " << argv[0] << " <archivo.c>\n";
        return 1;
    }
    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "No se puede abrir archivo\n";
        return 1;
    }

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in, line)) {
        // eliminar comentarios y espacios
        size_t pos = line.find("//");
        if (pos != std::string::npos) line = line.substr(0,pos);
        std::string trimmed = std::regex_replace(line, std::regex("^\s+|\s+$"), "");
        if (!trimmed.empty())
            lines.push_back(trimmed);
    }

    std::unordered_map<std::string,char> globalVars;
    std::unordered_map<std::string,Function> funcs;
    std::vector<std::string> errors, warns;

    for (size_t i=0;i<lines.size();) {
        std::string l = lines[i];

        // function definition
        std::smatch mfn;
        if (std::regex_match(l, mfn, std::regex(R"((int|float|void)\s+(\w+)\s*\(([^)]*)\)\s*\{)"))) {
            char retType = mfn[1]=="int"? 'i': (mfn[1]=="float"? 'f':'v');
            std::string fname = mfn[2];
            std::string paramsRaw = mfn[3];

            std::unordered_map<std::string,char> localVars;
            Function fn; fn.ret = retType;

            if (!paramsRaw.empty()) {
                std::vector<std::string> paramParts = split(paramsRaw, ',');
                for (std::string p: paramParts) {
                    std::smatch mp;
                    if (std::regex_match(p, mp, std::regex(R"(\s*(int|float)\s+(\w+)\s*)"))) {
                        char pt = mp[1]=="int"? 'i':'f';
                        std::string pname = mp[2];
                        localVars[pname]=pt;
                        fn.params.push_back(pt);
                    }
                }
            }
            funcs[fname]=fn;

            // read body until }
            ++i;
            while (i<lines.size() && lines[i]!="}") {
                std::string b = lines[i];

                // local var decl int x; float y, z;
                std::smatch mvar;
                if (std::regex_match(b, mvar, std::regex(R"(\s*(int|float)\s+([^;]+);)"))) {
                    char vtype = mvar[1]=="int"? 'i':'f';
                    std::vector<std::string> ids = split(mvar[2], ',');
                    for (auto id: ids) {
                        localVars[id]=vtype;
                    }
                } else {
                    // assignment
                    std::smatch masg;
                    if (std::regex_match(b, masg, std::regex(R"(\s*(\w+)\s*=\s*(.+);)"))) {
                        std::string lhs = masg[1];
                        std::string rhs = masg[2];
                        char lhs_t = tokenType(lhs, localVars, globalVars);
                        if (lhs_t==0) {
                            errors.push_back("["+fname+"] variable '"+lhs+"' no declarada");
                            lhs_t='u';
                        }
                        char rhs_t = evalExpr(rhs, localVars, globalVars, funcs, errors, fname);
                        if (lhs_t!=rhs_t && rhs_t!=0) {
                            warns.push_back("["+fname+"] asignación "+
                                            std::string(1,rhs_t)+"->"+std::string(1,lhs_t)+
                                            " en '"+lhs+"'");
                        }
                    }
                }
                ++i;
            }
            ++i; // skip }
            continue;
        }

        // global variable declaration
        std::smatch mvar;
        if (std::regex_match(l, mvar, std::regex(R"(\s*(int|float)\s+([^;]+);)"))) {
            char gtype = mvar[1]=="int"? 'i':'f';
            std::vector<std::string> ids = split(mvar[2], ',');
            for (auto id: ids) globalVars[id]=gtype;
        }
        ++i;
    }

    // ----------- output -----------------
    if (errors.empty()) {
        std::cout << "\u2705 Sin errores semánticos\n";
    } else {
        for (auto &e: errors) std::cout << "\u274C " << e << "\n";
    }
    for (auto &w: warns) std::cout << "\u26A0\uFE0F " << w << "\n";
    return 0;
}
