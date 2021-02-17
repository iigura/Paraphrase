@echo on
..\para 99.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para -E "1 2.3 'str' ( a ( b c ) )" check-scriptFile-args.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para countPrimeMT-short.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para dumpMPmt.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para fact-tr.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para FizzBuzz.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para Mersenne.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para PE21mt.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para PE25.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para PE56.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

@echo on
..\para zdk.pp test
@echo off
if %errorlevel% lss 0 goto FAILED

echo "===== The all test passed. ====="
goto END

:FAILED
echo "!!!!! TEST FAILED !!!!!"

:END
