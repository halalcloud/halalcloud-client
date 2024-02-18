@setlocal
@echo off

rem Change to the current folder.
cd "%~dp0"

rem Remove the Generated folder for a fresh generate.
rd /s /q Generated

rem Make the Generated folder.
mkdir Generated

rem Generate source code.
"%~dp0..\..\vcpkg\installed\x64-windows\tools\protobuf\protoc.exe" --cpp_out=".\Generated" --grpc_out=".\Generated" --plugin=protoc-gen-grpc="%~dp0..\..\vcpkg\installed\x64-windows\tools\grpc\grpc_cpp_plugin.exe" .\*.proto

@endlocal