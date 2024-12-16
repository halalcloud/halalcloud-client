# HalalCloud.Dependencies

Prebuilt binaries for Halal Cloud Client dependencies

## Build

```
vcpkg install curl[core,schannel]:x86-windows-static curl[core,schannel]:x64-windows-static curl[core,schannel]:arm64-windows-static
vcpkg install jsoncpp:x86-windows-static jsoncpp:x64-windows-static jsoncpp:arm64-windows-static
vcpkg install cpr:x86-windows-static cpr:x64-windows-static cpr:arm64-windows-static
```

## Note

libuuid is extracted from BCE SDK for C++.
