@echo on
..\para 99.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para -E "1 2.3 'str' ( a ( b c ) )" check-scriptFile-args.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para countPrimeMT-short.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para dumpMPmt.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para fact-tr.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para FizzBuzz.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para Mersenne.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para PE10mt.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para PE21mt.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para PE25.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para PE56.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para PolishNotation.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para zdk.p8e test
@echo off
if %errorlevel% lss 0 goto FAILED

echo "===== The all test passed. ====="
goto END

:FAILED
echo "!!!!! TEST FAILED !!!!!"

:END
