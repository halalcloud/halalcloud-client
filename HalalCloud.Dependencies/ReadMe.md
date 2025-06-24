# HalalCloud.Dependencies

Prebuilt binaries for Halal Cloud Client dependencies

## Build

```
vcpkg install curl[core,mbedtls]:x86-windows-static curl[core,mbedtls]:x64-windows-static curl[core,mbedtls]:arm64-windows-static
vcpkg install "--overlay-ports=.\Source\ports" tvision:x86-windows-static tvision:x64-windows-static tvision:arm64-windows-static
```
