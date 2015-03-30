var nscrypto = require('..');
var should   = require('should');

should.Assertion.add('atLeast', function(expected) {
  this.params = { operator : 'to be at least', expected : expected };
  this.obj.should.be.a.Number;
  (this.obj >= expected).should.be.true;
});

// Test values.
var apiKey   = 'some-api-key';
var serverId = 'some-server-id';

describe('nscrypto', function() {

  it('should have loaded properly', function() {
    should.exist(nscrypto);
  });

  describe('generateKeyPair()', function() {
    var keys;

    beforeEach(function() {
      keys = nscrypto.generateKeyPair();
    });

    it('should have generated a public key', function() {
      should.exist(keys.public);
    });

    it('...of type Buffer', function() {
      Buffer.isBuffer(keys.public).should.be.true;
    });

    it('should have generated a private key', function() {
      should.exist(keys.private);
    });

    it('...of type Buffer', function() {
      Buffer.isBuffer(keys.private).should.be.true;
    });

    it('should return new keys each time', function() {
      for (var i = 0; i < 100; i++) { // XXX: obviously not very elaborate
        var newkeys = nscrypto.generateKeyPair();
        newkeys.public.should.not.equal(keys.public);
        newkeys.private.should.not.equal(keys.private);
      }
    });

  });

  describe('encryption', function() {

    [ 'server', 'client' ].forEach(function(type) {

      describe('nscrypto.' + type + '.encrypt()', function() {
        var func = nscrypto[type].encrypt;

        describe('argument validation', function() {
          it('should validate arguments', function() {
            func.bind(nscrypto).should.throw();
            func.bind(nscrypto, Buffer('')).should.throw();
            func.bind(nscrypto, Buffer(''), Buffer('')).should.throw();
            func.bind(nscrypto, Buffer(''), Buffer(''), '').should.throw();
            func.bind(nscrypto, Buffer(''), Buffer(''), '', '').should.throw();
            func.bind(nscrypto, Buffer(''), Buffer(''), '', '', Buffer('')).should.not.throw();
          });
        });

        describe('encryption', function() {
          var serverKeys = nscrypto.generateKeyPair();
          var clientKeys = nscrypto.generateKeyPair();
          var pubKey     = type === 'server' ? serverKeys.public  : clientKeys.public;
          var prvKey     = type === 'server' ? serverKeys.private : clientKeys.private;
          var data, enc;

          beforeEach(function() {
            data = Buffer('test');
            enc  = func(prvKey, pubKey, apiKey, serverId, data);
          });

          it('should return an object', function() {
            enc.should.be.a.Object;
          });

          it('should have an "enc" property...', function() {
            enc.should.have.property('enc');
          });

          it('...that looks encrypted', function() {
            enc.enc.length.should.be.atLeast(16);
            // XXX: better testing
          });

          it('should have an "eph" property', function() {
            enc.should.have.property('eph');
          });

        });

      });

    });

  });

  describe('decryption', function() {

    [ 'server', 'client' ].forEach(function(type) {

      describe('nscrypto.' + type + '.decrypt()', function() {

        describe('argument validation', function() {
          it('should validate arguments', function() {
            var func = nscrypto[type].decrypt;

            func.bind(nscrypto).should.throw();
            func.bind(nscrypto, Buffer('')).should.throw();
            func.bind(nscrypto, Buffer(''), Buffer('')).should.throw();
            func.bind(nscrypto, Buffer(''), Buffer(''), '').should.throw();
            func.bind(nscrypto, Buffer(''), Buffer(''), '', '').should.throw();
            func.bind(nscrypto, Buffer(''), Buffer(''), '', '', {}).should.throw();
            func.bind(nscrypto, Buffer(''), Buffer(''), '', '', { enc : '', eph : '' }).should.throw();
            func.bind(nscrypto, Buffer(''), Buffer(''), '', '', { enc : Buffer(''), eph : Buffer('') }).should.not.throw();
          });
        });

        describe('decryption', function() {
          var data, enc, dec, keys = {
            server : nscrypto.generateKeyPair(),
            client : nscrypto.generateKeyPair(),
          };

          beforeEach(function() {
            data = Buffer('receipe for super secret sauce');
            enc  = nscrypto[type].encrypt(
              keys[ type === 'server' ? 'client' : 'server' ].private,
              keys[ type === 'server' ? 'server' : 'client' ].public,
              apiKey,
              serverId,
              data
            );
          });

          describe('valid decryption', function() {
            beforeEach(function() {
              dec = nscrypto[type === 'server' ? 'client' : 'server'].decrypt(
                keys[ type === 'server' ? 'server' : 'client' ].private,
                keys[ type === 'server' ? 'client' : 'server' ].public,
                apiKey,
                serverId,
                enc
              );
            });

            it('should have returned a Buffer', function() {
              should.exist(dec);
              dec.should.be.a.Buffer;
            });

            it('should equal the input', function() {
              data.toString().should.equal(dec.toString());
            });
          });

          describe('invalid decryption', function() {

            it('should reject decryption of same type', function() {
              nscrypto[type === 'server' ? 'server' : 'client'].decrypt(
                keys[ type === 'server' ? 'server' : 'client' ].private,
                keys[ type === 'server' ? 'client' : 'server' ].public,
                apiKey,
                serverId,
                enc
              ).length.should.equal(0);
            });

            it('should reject tampered message', function() {
              enc.enc[0] = enc.enc[0] + 1;
              nscrypto[type === 'server' ? 'client' : 'server'].decrypt(
                keys[ type === 'server' ? 'server' : 'client' ].private,
                keys[ type === 'server' ? 'client' : 'server' ].public,
                apiKey,
                serverId,
                enc
              ).length.should.equal(0);
            });

            it('should reject stripped tag', function() {
              enc.enc = enc.enc.slice(0, -16);
              nscrypto[type === 'server' ? 'client' : 'server'].decrypt(
                keys[ type === 'server' ? 'server' : 'client' ].private,
                keys[ type === 'server' ? 'client' : 'server' ].public,
                apiKey,
                serverId,
                enc
              ).length.should.equal(0);
            });

            it('should reject invalid sender id', function() {
              nscrypto[type === 'server' ? 'client' : 'server'].decrypt(
                keys[ type === 'server' ? 'server' : 'client' ].private,
                keys[ type === 'server' ? 'client' : 'server' ].public,
                apiKey + 'X',
                serverId,
                enc
              ).length.should.equal(0);
            });

            it('should reject invalid receiver id', function() {
              nscrypto[type === 'server' ? 'client' : 'server'].decrypt(
                keys[ type === 'server' ? 'server' : 'client' ].private,
                keys[ type === 'server' ? 'client' : 'server' ].public,
                apiKey,
                serverId + 'X',
                enc
              ).length.should.equal(0);
            });

          });

        });

      });

    });

  });

});
