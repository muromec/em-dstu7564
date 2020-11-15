const dstu7564 = require('./dist/wrapper.js');

async function main() {
  await dstu7564.ready();
  const buffer = Buffer.from('000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F', 'hex');
  const expected = Buffer.from('08F4EE6F1BE6903B324C4E27990CB24EF69DD58DBE84813EE0A52F6631239875', 'hex');

  const hash = dstu7564.computeHash(buffer);

  console.log('buffer', hash);
  console.log('expected', expected);
  console.log('OK?', hash.equals(expected));
}

main();
