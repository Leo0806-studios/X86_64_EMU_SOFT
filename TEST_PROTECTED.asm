[bits 32]
[org 0x10]
mov eax, 1
mov ebx, 2
mov ecx, 3
mov edx, 4
mov eax,eax

add eax, ebx
add eax, ecx
add eax, edx
add eax, 5


sub ebx, eax
sub ecx, eax
sub edx, eax
sub eax, 10







or	eax,eax
or	eax,ebx
or	eax,ecx
or	eax,edx
or eax, 10

and eax, eax
and eax, ebx
and eax, ecx
and eax, edx
and eax, 10

not eax
not ebx
not ecx
not edx

shr eax, 1
shr ebx, 1
shr edx, 1
shr edx, 1

shl eax, 1
shl ebx, 1
shl ecx, 1
shl edx, 1
