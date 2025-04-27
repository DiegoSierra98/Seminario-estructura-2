const { validarDatos } = require('../validador-generado');

// Caso válido
const usuarioValido = {
  nombre: "Diego Sierra",
  edad: 25,
  email: "diego@example.com"
};

// Caso inválido
const usuarioInvalido = {
  nombre: "Diego",
  edad: -5,
  email: "no-es-un-email"
};

console.log('Probando usuario válido:');
validarDatos(usuarioValido);

console.log('\nProbando usuario inválido:');
validarDatos(usuarioInvalido);