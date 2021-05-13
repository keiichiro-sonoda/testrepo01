csv2html01.exe: csv2html01.c
	gcc csv2html01.c -o csv2html01.exe
csv2html02.exe: csv2html02.c
	gcc csv2html02.c -o csv2html02.exe
csv2html03.exe: csv2html03.c
	gcc csv2html03.c -o csv2html03.exe -finput-charset=UTF-8 -fexec-charset=CP932