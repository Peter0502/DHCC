@echo off 
color 0a 
title IP�л��ű�-Create by Adobo@qq.com 
:start
@echo --------------------------------- 
@echo ��ѡ�������ڵ�λ��: 
@echo 1.�����棨����DHCP) 
@echo 2.�칫�ң��̶�IP) 
@echo 3.����  ���̶�IP)
@echo --------------------------------- 
set /p sLocal=��ѡ��: 
if %sLocal%==1 goto home 
if %sLocal%==2 goto office 
if %sLocal%==3 goto shuse
cls 
@echo ѡ�����������ѡ��
goto start 
:home 
@echo ��������������������ΪDHCPģʽ 
netsh interface ip set address ������������ dhcp 
goto end

:office 
@echo �������ñ�����������Ϊ�̶�IP:10.1.61.42 
netsh interface ip set address "��������" static 10.1.61.42 255.255.255.128 10.1.61.126 1 
netsh interface ip set dns "��������" static 202.102.134.68
netsh interface ip add dns "��������" 202.102.134.68
goto end

:shuse 
@echo �������ñ�����������Ϊ�̶�IP:10.1.14.131 
netsh interface ip set address "��������" static 10.1.14.131 255.255.255.192 10.1.14.129 1 
netsh interface ip set dns "��������" static 219.146.0.130 
netsh interface ip add dns "��������" 202.102.134.68
:end 
echo ������ɣ��Զ��˳�



