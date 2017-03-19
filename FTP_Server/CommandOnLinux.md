#accumulation of some command on linux 
1.**SUID** = set user ID  
  usage 当一个文件被设置了之后，执行该文件的时候是以文件所有者的权限进行执行的，如果文件可执行，那么rwx中的x位会变成s，否则则是大写的X  
 exampe:  
	chmod 4711 filelist  
	chmod u+s filelist  
2.ln 命令  
  **ln Target Link-Name**  
  **ln -s Target Link-Name** somewhat similar to shortcut on windows system  
3.less命令  
  e 或者回车可以滚动一行，向下滚动  
  d 和 u 分别是滚动半页  
4.command &  
  命令后边加上逻辑与符号，可以让命令在后台执行 for example  
  find / -name file1 -print>file.p 2 > dev/null &  
  后面的2是将错误信息重定向到linux“黑洞”  
  fg 命令可以将后台执行的进程转移到前台