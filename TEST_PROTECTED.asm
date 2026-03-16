[bits 32]
[org 0x10]
mov eax, 1
mov ebx, 2
mov ecx, 3
mov edx, 4

add eax, ebx
add eax, ecx
add eax, edx
add eax, 5


sub ebx, eax
sub ecx, eax
sub edx, eax
sub eax, 10







or	eax,eax
or eax, 10

and eax, eax
and eax, 10

not eax

shr eax, 1

shl eax, 1