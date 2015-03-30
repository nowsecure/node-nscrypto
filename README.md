# node-nscrypto

Wrapper around [nscrypto-cpp](https://github.com/nowsecure/nscrypto-cpp).

## PREREQUISITES

* A C++11 compiler.
* A recent OpenSSL library. If you're on Mac OS X: Apple has deprecated OpenSSL in favor of its own TLS and crypto libraries, and you should look at [Homebrew](http://brew.sh/) to install a recent OpenSSL version.

## INSTALLATION

From the NPM repository:

```
$ npm install nscrypto
```

From Github (don't rely on this for stability):

```
$ npm install nowsecure/node-nscrypto
```

Or clone the repository and build it directly:

```
$ git clone https://github.com/viaforensics/node-nscrypto.git
$ cd node-nscrypto
$ npm install
```

## API

All API calls require the `nscrypto` module to be loaded:

```
var nscrypto = require('nscrypto');
```

### Key generation ([ref](https://github.com/nowsecure/nscrypto-cpp#key-generation))

```
nscrypto.generateKeyPair() : Object
```

The returned object contains two properties of type `Buffer`:

* `public`: the generated public key
* `private`: the generated private key

### Encryption ([ref](https://github.com/nowsecure/nscrypto-cpp#encryption-and-decryption))

```
nscrypto.{client,server}.encrypt(
  sPriv     : Buffer,
  rPub      : Buffer,
  sId       : String,
  rId       : String,
  plaintext : Buffer
) : Object
```

Returned object contains two properties:

* `enc`: the encrypted data
* `eph`: the ephemeral key

### Decryption ([ref](https://github.com/nowsecure/nscrypto-cpp#encryption-and-decryption))

```
nscrypto.{client,server}.decrypt(
  rPriv     : Buffer,
  sPub      : Buffer,
  sId       : String,
  rId       : String,
  encrypted : Object
) : Buffer
```

`message` is an object as the one returned by the encryption functions, contains `enc` and `eph` properties.

The returned `Buffer` contains the decrypted plaintext.

## CHANGELOG

* 1.0.0 : Initial release

## SEE ALSO

[nscrypto-cpp](https://github.com/nowsecure/nscrypto-cpp#nscrypto-cpp)

## AUTHOR

Robert Klep <<rklep@nowsecure.com>>

## LICENCE

See [LICENSE.md](LICENSE.md).
