#include <nan.h>
#include "nscrypto_ecdh.h"

using namespace v8;
using namespace node;

static inline bool _is_buffer(Handle<Value> v) {
    return node::Buffer::HasInstance(v);
}

static inline Local<Object> _to_buffer(std::string s) {
    return NanNewBufferHandle((char *) s.data(), s.size());
}

NAN_METHOD(GenerateKeyPair) {
    NanScope();

    // Generate private/public key pair and return as JS object.
    auto keypair      = ec_keypair();
    Local<Object> obj = NanNew<Object>();

    obj->Set(NanNew("private"), _to_buffer(std::get<0>(keypair)));
    obj->Set(NanNew("public"),  _to_buffer(std::get<1>(keypair)));

    NanReturnValue(obj);
}

Local<Value> Encrypt(
    _NAN_METHOD_ARGS,
    ecdh_encrypted_t (*EncryptionMethod)(const std::string&, const std::string&,
                                         const std::string&, const std::string&,
                                         const std::string&)
) {
    // Argument validation.
    if (args.Length() != 5) {
        return NanError("Wrong number of arguments");
    }
    if (! _is_buffer(args[0])) {
        return NanError("Private key argument must be Buffer");
    }
    if (! _is_buffer(args[1])) {
        return NanError("Public key argument must be Buffer");
    }
    if (! args[2]->IsString()) {
        return NanError("Server id argument must be String");
    }
    if (! args[3]->IsString()) {
        return NanError("Recipient id argument must be String");
    }
    if (! _is_buffer(args[4]) && ! args[4]->IsString()) {
        return NanError("Message argument must be Buffer or String");
    }

    // Convert arguments.
    Local<Object> sPriv = args[0]->ToObject();
    std::string s_priv(Buffer::Data(sPriv), Buffer::Length(sPriv));

    Local<Object> rPub  = args[1]->ToObject();
    std::string r_pub(Buffer::Data(rPub), Buffer::Length(rPub));

    std::string s_id(*NanUtf8String(args[2]));
    std::string r_id(*NanUtf8String(args[3]));

    // Convert message to std::string
    std::string message;
    if (_is_buffer(args[4])) {
        Local<Object> msg = args[4]->ToObject();
        message = std::string(Buffer::Data(msg), Buffer::Length(msg));
    } else {
        message = std::string(*NanUtf8String(args[4]));
    }

    // Encrypt.
    std::string enc, tag, eph;
    std::tie(enc, tag, eph) = EncryptionMethod(s_priv, r_pub, s_id, r_id, message);

    // TODO: want to check for errors.

    // Return object.
    Local<Object> obj = NanNew<Object>();
    obj->Set(NanNew("enc"), _to_buffer(enc + tag));
    obj->Set(NanNew("eph"), _to_buffer(eph));

    return obj;
}

NAN_METHOD(ClientEncrypt) {
    NanScope();
    Local<Value> ret = Encrypt(args, ecdh_client_encrypt);

    if (ret->IsNativeError()) {
        NanThrowTypeError(*NanUtf8String(ret));
        NanReturnUndefined();
    } else {
        NanReturnValue(ret);
    }
}

NAN_METHOD(ServerEncrypt) {
    NanScope();
    Local<Value> ret = Encrypt(args, ecdh_server_encrypt);

    if (ret->IsNativeError()) {
        NanThrowTypeError(*NanUtf8String(ret));
        NanReturnUndefined();
    } else {
        NanReturnValue(ret);
    }
}

Local<Value> Decrypt(
    _NAN_METHOD_ARGS,
    std::string (*DecryptionMethod)(const std::string&, const std::string&,
                                    const std::string&, const std::string&,
                                    const ecdh_encrypted_t&)
) {
    // Argument validation.
    if (args.Length() != 5) {
        return NanError("Wrong number of arguments");
    }
    if (! _is_buffer(args[0])) {
        return NanError("Private key argument must be Buffer");
    }
    if (! _is_buffer(args[1])) {
        return NanError("Public key argument must be Buffer");
    }
    if (! args[2]->IsString()) {
        return NanError("Server id argument must be String");
    }
    if (! args[3]->IsString()) {
        return NanError("Recipient id argument must be String");
    }
    if (! args[4]->IsObject()) {
        return NanError("Message argument must be Object");
    }

    // Convert arguments.
    Local<Object> rPriv = args[0]->ToObject();
    std::string r_priv(Buffer::Data(rPriv), Buffer::Length(rPriv));

    Local<Object> sPub = args[1]->ToObject();
    std::string s_pub(Buffer::Data(sPub), Buffer::Length(sPub));

    std::string s_id(*NanUtf8String(args[2]));
    std::string r_id(*NanUtf8String(args[3]));

    // Process message object
    Local<Object> messageObject = args[4]->ToObject();
    Local<Value> encBuffer      = messageObject->Get(NanNew("enc"));
    Local<Value> ephBuffer      = messageObject->Get(NanNew("eph"));
    if (! _is_buffer(encBuffer) || ! _is_buffer(ephBuffer)) {
        return NanError("Message argument must be have 'enc' and 'eph' properties of type Buffer.");
    }

    std::string enc(Buffer::Data(encBuffer), Buffer::Length(encBuffer));
    std::string eph(Buffer::Data(ephBuffer), Buffer::Length(ephBuffer));

    ecdh_encrypted_t message;
    if (enc.size() > 16 && ! eph.empty()) {
        message = ecdh_encrypted_t(enc.substr(0, enc.size() - 16), enc.substr(enc.size() - 16), eph);
    } else {
        message = ecdh_encrypted_t(std::string(), std::string(), std::string());
    }

    // Decrypt.
    std::string decrypted = DecryptionMethod(r_priv, s_pub, s_id, r_id, message);

    // XXX: want to check for errors.

    // Return as JS buffer.
    return _to_buffer(decrypted);
}

NAN_METHOD(ClientDecrypt) {
    NanScope();
    Local<Value> ret = Decrypt(args, ecdh_client_decrypt);

    if (ret->IsNativeError()) {
        NanThrowTypeError(*NanUtf8String(ret));
        NanReturnUndefined();
    } else {
        NanReturnValue(ret);
    }
}

NAN_METHOD(ServerDecrypt) {
    NanScope();
    Local<Value> ret = Decrypt(args, ecdh_server_decrypt);

    if (ret->IsNativeError()) {
        NanThrowTypeError(*NanUtf8String(ret));
        NanReturnUndefined();
    } else {
        NanReturnValue(ret);
    }
}

void InitAll(Handle<Object> exports) {
    exports->Set(NanNew("generateKeyPair"), NanNew<FunctionTemplate>(GenerateKeyPair)->GetFunction());

    Local<Object> client = NanNew<Object>();
    Local<Object> server = NanNew<Object>();
    client->Set(NanNew("encrypt"), NanNew<FunctionTemplate>(ClientEncrypt)->GetFunction());
    client->Set(NanNew("decrypt"), NanNew<FunctionTemplate>(ClientDecrypt)->GetFunction());
    server->Set(NanNew("encrypt"), NanNew<FunctionTemplate>(ServerEncrypt)->GetFunction());
    server->Set(NanNew("decrypt"), NanNew<FunctionTemplate>(ServerDecrypt)->GetFunction());

    exports->Set(NanNew("client"), client);
    exports->Set(NanNew("server"), server);
}

NODE_MODULE(nscrypto, InitAll)
