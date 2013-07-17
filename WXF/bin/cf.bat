:install filename
set obj_file=%1
del /F /Q %dst_dir%\%obj_file%
copy /Y %src_dir%\%obj_file% %dst_dir%
attrib +r %dst_dir%\%obj_file%