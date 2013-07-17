set src_lib=..\lib
set src_bin=..\bin
set src_inc=%cd%
call %src_bin%\comm_set.bat

set src_dir=%src_inc%
set dst_dir=%dst_inc%
call %src_bin%\cf.bat wxf_comm.h 
set src_dir=%src_lib%
set dst_dir=%dst_lib%
call %src_bin%\cf.bat wxf_comm.lib

:del /F /Q %dst_inc%\wxf_comm.h
:del /F /Q %dst_lib%\wxf_comm.lib
:copy /Y %src_inc%\wxf_comm.h %dst_inc%
:copy /Y %src_lib%\wxf_comm.lib %dst_lib%
:attrib +r %dst_inc%\wxf_comm.h
:attrib +r %dst_lib%\wxf_comm.lib
