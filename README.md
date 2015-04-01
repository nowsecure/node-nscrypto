# node-nscrypto

Wrapper around [nscrypto-cpp](https://github.com/nowsecure/nscrypto-cpp).

[![Build Status](https://travis-ci.org/nowsecure/node-nscrypto.svg?branch=travis-ci-support)](https://travis-ci.org/nowsecure/node-nscrypto)

## PREREQUISITES

* A relatively recent Node or io.js version (tested with Node@0.1[02] and io.js@latest).
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
$ git clone https://github.com/nowsecure/node-nscrypto.git
$ cd node-nscrypto
$ npm install
```

For installations on Mac OS X, the default assumption is that you use a Homebrew-installed OpenSSL library that is installed in `/usr/local/opt/openssl/`. If you have OpenSSL installed somewhere else, you can override the library and include paths:

```
$ env OPENSSL_INCLUDE_DIR=/path/to/include/dir OPENSSL_LIB_DIR=/path/to/lib/dir npm install nscrypto
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

The returned object has two properties of type `Buffer`:

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

Returned object has two properties:

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

`message` is an object as the one returned by the encryption functions, having `enc` and `eph` properties.

The returned `Buffer` is the decrypted plaintext.

## CHANGELOG

* 1.0.0 : Initial release

## TODO

* Automate finding the correct OpenSSL library on Mac OS X (instead of hardcoding the path).

## SEE ALSO

[nscrypto-cpp](https://github.com/nowsecure/nscrypto-cpp#nscrypto-cpp)

## AUTHOR

Robert Klep <<rklep@nowsecure.com>>

## LICENCE

See [LICENSE.md](LICENSE.md).
