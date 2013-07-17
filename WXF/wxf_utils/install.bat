set src_lib=..\lib
set src_bin=..\bin
set src_inc=%cd%
call %src_bin%\comm_set.bat
set CF=call %src_bin%\cf.bat
set src_dir=%src_inc%
set dst_dir=%dst_inc%
%CF% wxf_utils.h
%CF% wxf_ipc.h
%CF% wxf_gxmem.h
%CF% wxf_fq.h
%CF% wxf_fbuf.h
set src_dir=%src_lib%
set dst_dir=%dst_lib%
%CF% wxf_utils.lib