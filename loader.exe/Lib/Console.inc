ifndef	UNICODE
	.err	<@FileName only works in Unicode environment.>
endif
;********************************************************************
;�Թؼ���dw�����ض���,ʹ�������dbһ������Unicode�ַ���
		option renamekeyword:<dw>=dw_
dw		macro	lbl:LABEL,parms:VARARG
		local	wstr
		wstr	textequ	<>
		for	parm,<parms>
			ifidn	<">,@SubStr(parm,1,1)
				% forc chr$, <@SubStr(parm,2,@SizeStr(parm)-2)>
				ifnb	wstr
					wstr	CatStr wstr,<,>
				endif
				wstr	CatStr wstr,<'&chr$'>
				endm
			elseifidn <'>,@SubStr(parm,1,1)
				% forc chr$, <@SubStr(parm,2,@SizeStr(parm)-2)>
				ifnb	wstr
					wstr	CatStr wstr,<,>
				endif
				wstr	CatStr wstr,<'&chr$'>
				endm
			else
				ifnb	wstr
					wstr	CatStr wstr,<,>
				endif
				wstr	CatStr wstr,<parm>
			endif
			endm
		lbl	dw_	wstr
		endm
;********************************************************************
;����Console
		.data?
hStdIn		dd	?	;����̨������
hStdOut		dd	?	;����̨������
hStdErr		dd	?	;����̨������
dwBytesRead	dd	?
dwBytesWrite	dd	?
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		.code
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
_SetConsole	proc	uses ebx edi esi,_lpTitle

		invoke	SetConsoleTitle,_lpTitle
		invoke	GetStdHandle,STD_INPUT_HANDLE
		mov	hStdIn,eax
		invoke	GetStdHandle,STD_OUTPUT_HANDLE
		mov	hStdOut,eax
		invoke	GetStdHandle,STD_ERROR_HANDLE
		mov	hStdErr,eax
		invoke	GetConsoleMode,hStdOut,addr dwBytesWrite
		.if	eax
			push	WriteConsole
			pop	_Cout
		.else
			push	WriteFile
			pop	_Cout
			invoke	WriteFile,hStdOut,addr uszBOM,2,addr dwBytesWrite,NULL
		.endif
		ret
_SetConsole	endp
;********************************************************************
;�ж��Ƿ��ض���,ѡ���ʵ��������ʽ
_ProtoCout	typedef	proto	:dword,:dword,:dword,:dword,:dword
_ApiCout	typedef	ptr	_ProtoCout
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		.data?
_Cout		_ApiCout ?
		.const
uszBOM		dw	0FEFFh,0
uszCrLf		dw	0dh,0ah,0
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		.code
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Cout		macro	uszString

		push	NULL
		push	offset dwBytesWrite
		mov	eax,_Cout
		mov	ecx,WriteConsole
		mov	edx,WriteFile
		.if	eax == ecx
			push	(sizeof uszString - 2) / 2
		.elseif	eax == edx
			push	sizeof uszString - 2
		.else
			;.err	[Line:@Line] ConsoleRediect is undefined.
			push	NULL
		.endif
		push	offset uszString
		push	hStdOut
		call	_Cout
		endm
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Coutn		macro	uszString,nChar

		push	NULL
		push	offset dwBytesWrite
		
		push	nChar	;���ܻḲ��
		mov	eax,_Cout
		mov	ecx,WriteConsole
		mov	edx,WriteFile
		.if	eax == ecx
			;ֱ��ʹ��֮ǰ��nChar
		.elseif	eax == edx
			pop	eax
			mov	ecx,2
			mul	ecx
			push	eax
		.else
			;.err	[Line:@Line] ConsoleRediect is undefined.
			pop	eax
			push	NULL
		.endif
		push	offset uszString
		push	hStdOut
		call	_Cout
		endm
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Creturn		textequ	<Cout	uszCrLf>