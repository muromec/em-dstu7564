const getdstu7564 = require('./dist/dstu7564.js');

function copyTo(c, buffer) {
  const ptr = c._malloc(buffer.length);
  for(let idx = 0; idx < buffer.length; idx++) {
    c.HEAPU8[ptr + idx] = buffer[idx];
  }
  return ptr;
}

function copyFrom(c, ptr, len) {
  return Buffer.from(c.HEAPU8.slice(ptr, ptr + len));
}

function computeHash(c, buffer) {
  const cbuffer = copyTo(c, buffer);
  const chash = c._malloc(32);

  const ctx = c._dstu7564_alloc(0);
  c._dstu7564_init(ctx, 32);
  c._dstu7564_update(ctx, cbuffer, buffer.length);
  c._dstu7564_final(ctx, chash);
  c._free(cbuffer);

  const ret = copyFrom(c, chash, 32);
  c._free(chash);
  return ret;
}

async function main() {
  const buffer = Buffer.from('000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F', 'hex');

  const c = await getdstu7564();
  const hash = computeHash(c, buffer);

  console.log('buffer', hash);
}

main();
