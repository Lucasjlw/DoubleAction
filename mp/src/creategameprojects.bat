@echo off
pushd %~dp0
sed -ri '/PLACEHOLDER_COMPILER_ARGS/ s/^.*$/git rev-parse HEAD/e; T; s/^.*$/$PreprocessorDefinitions "$BASE;DA_GIT_VERSION=\0"/' game/client/client_da.vpc game/server/server_da.vpc
devtools\bin\vpc.exe /da +game /mksln DoubleAction.sln /define:VS2022 /verbose
popd
pause
