set BASE_DIR=%~dp0


cmake -S ./spdlog -B ./build/spdlog/debug -G "Visual Studio 18 2026" -A x64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=./build/spdlog/install -DSPDLOG_BUILD_SHARED=ON -DSPDLOG_BUILD_EXAMPLE=OFF -DSPDLOG_BUILD_TESTS=OFF -DSPDLOG_INSTALL=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebugDLL
cmake --build ./build/spdlog/debug --config Debug --parallel 
cmake --install ./build/spdlog/debug --config Debug


cmake -S ./spdlog -B ./build/spdlog/release -G "Visual Studio 18 2026" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./build/spdlog/install -DSPDLOG_BUILD_SHARED=ON -DSPDLOG_BUILD_EXAMPLE=OFF -DSPDLOG_BUILD_TESTS=OFF -DSPDLOG_INSTALL=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL
cmake --build ./build/spdlog/release --config release --parallel 
cmake --install ./build/spdlog/release --config Release

xcopy "%BASE_DIR%build\spdlog\install\lib\spdlog.lib" "%BASE_DIR%bin\x64\" /Y
xcopy "%BASE_DIR%build\spdlog\install\lib\spdlogd.lib" "%BASE_DIR%bin\x64\" /Y
xcopy "%BASE_DIR%build\spdlog\install\bin\spdlog.dll" "%BASE_DIR%bin\x64\" /Y
xcopy "%BASE_DIR%build\spdlog\install\bin\spdlogd.dll" "%BASE_DIR%bin\x64\" /Y

xcopy "%BASE_DIR%build\spdlog\install\bin\spdlog.dll" "%BASE_DIR%..\..\Output\sdk\bin\" /Y
xcopy "%BASE_DIR%build\spdlog\install\bin\spdlogd.dll" "%BASE_DIR%..\..\Output\sdk\bin\" /Y

xcopy "%BASE_DIR%build\spdlog\install\bin\spdlog.dll" "%BASE_DIR%..\..\Samples\Output\" /Y
xcopy "%BASE_DIR%build\spdlog\install\bin\spdlogd.dll" "%BASE_DIR%..\..\Samples\Output\" /Y

xcopy "%BASE_DIR%build\spdlog\install\include\mimalloc-3.3\*.*" "%BASE_DIR%inc\mimalloc\" /Y /S

rmdir "%BASE_DIR%build" /s /q

