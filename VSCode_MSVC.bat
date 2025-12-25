@echo off
:: Настройка среды MSVC
call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

:: Переход в папку с проектом
cd /d "C:\путь\к\твоему\проекту"

:: Запуск VS Code в этой папке
code .
