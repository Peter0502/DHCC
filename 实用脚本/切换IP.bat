@echo off 
color 0a 
title IP切换脚本-Create by Adobo@qq.com 
:start
@echo --------------------------------- 
@echo 请选择你现在的位置: 
@echo 1.家里面（启用DHCP) 
@echo 2.办公室（固定IP) 
@echo 3.宿舍  （固定IP)
@echo --------------------------------- 
set /p sLocal=请选择: 
if %sLocal%==1 goto home 
if %sLocal%==2 goto office 
if %sLocal%==3 goto shuse
cls 
@echo 选择错误，请重新选择
goto start 
:home 
@echo 正在设置无线网络连接为DHCP模式 
netsh interface ip set address 无线网络连接 dhcp 
goto end

:office 
@echo 正在设置本地网络连接为固定IP:10.1.61.42 
netsh interface ip set address "本地连接" static 10.1.61.42 255.255.255.128 10.1.61.126 1 
netsh interface ip set dns "本地连接" static 202.102.134.68
netsh interface ip add dns "本地连接" 202.102.134.68
goto end

:shuse 
@echo 正在设置本地网络连接为固定IP:10.1.14.131 
netsh interface ip set address "本地连接" static 10.1.14.131 255.255.255.192 10.1.14.129 1 
netsh interface ip set dns "本地连接" static 219.146.0.130 
netsh interface ip add dns "本地连接" 202.102.134.68
:end 
echo 设置完成，自动退出



