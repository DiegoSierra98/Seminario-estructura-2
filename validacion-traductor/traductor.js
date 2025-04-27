const fs = require('fs');
const Ajv = require('ajv');

// Leer esquema amigable (simulado como JSON Schema en este ejemplo)
const esquema = require('./esquema-ejemplo.json');

// Crear el código de validación
const contenido = `
const Ajv = require('ajv');
const ajv = new Ajv();
const esquema = ${JSON.stringify(esquema, null, 2)};

const validar = ajv.compile(esquema);

function validarDatos(datos) {
  const valido = validar(datos);
  if (!valido) {
    console.log('Errores:', validar.errors);
  } else {
    console.log('Datos válidos.');
  }
}

module.exports = { validarDatos };
`;

// Escribir archivo generado
fs.writeFileSync('validador-generado.js', contenido);
console.log('✅ Archivo de validación generado exitosamente.');