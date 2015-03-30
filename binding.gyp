{
  "targets": [{
    "target_name"    : "nscrypto",
    "sources"        : [ "./src/nscrypto_wrapper.cc", "./src/nscrypto_ecdh.cc" ],
    "libraries"      : [ "-lcrypto" ],
    "cflags"         : [ "-std=c++11 -Wno-unused-value -Wno-unused-function -Wno-unknown-pragmas -Wno-format" ],
    "include_dirs"   : [ "<!(node -e \"require('nan')\")" ],
    "configurations" : {
      "Release" : { "defines" : [ "__NODE_JS__" ] },
      "Debug"   : { "defines" : [ "__NODE_JS__", "_NSCRYPTO_EXTRA_LOGGING" ] },
    },
    "conditions"   : [
      ["OS == 'mac'", {
        "include_dirs" : [ "/usr/local/opt/openssl/include" ],
        "library_dirs" : [ "/usr/local/opt/openssl/lib" ],
        "xcode_settings" : {
          "MACOSX_DEPLOYMENT_TARGET" : "10.7",
          "OTHER_CFLAGS"             : [
            "-std=c++11",
            "-stdlib=libc++",
            "-Wno-unused-function",
            "-Wno-deprecated-declarations",
            "-Wno-format"
          ]
        }
      }]
    ]
  }]
}
