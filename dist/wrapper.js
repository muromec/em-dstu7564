const getdstu7564 = require('./dstu7564.js');

let _wait = getdstu7564().then((_ret)=> {
  c = _ret;
  _wait = true;
});

function copyTo(buffer) {
  const ptr = c._malloc(buffer.length);
  for(let idx = 0; idx < buffer.length; idx++) {
    c.HEAPU8[ptr + idx] = buffer[idx];
  }
  return ptr;
}

function copyFrom(ptr, len) {
  return Buffer.from(c.HEAPU8.slice(ptr, ptr + len));
}

function ready() {
  return _wait
}

function computeHash(buffer) {
  const cbuffer = copyTo(buffer);
  const chash = c._malloc(32);

  const ctx = c._dstu7564_alloc(0);
  c._dstu7564_init(ctx, 32);
  c._dstu7564_update(ctx, cbuffer, buffer.length);
  c._dstu7564_final(ctx, chash);
  c._free(cbuffer);

  const ret = copyFrom(chash, 32);
  c._free(chash);
  return ret;
}

module.exports = {ready, computeHash};
