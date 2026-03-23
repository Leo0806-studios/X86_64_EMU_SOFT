[bits 64]
[org 0x10]
mov rax, 1
mov rbx, 2
mov rcx, 3
mov rdx, 4
mov rdx, 0xFFFFFFFFFFFFFFFF
mov r9,5
mov rax,rax
mov r9, r9
mov r8,rax

add rax, rbx
add rax, rcx
add rax, rdx
add rax, 5
add r9, 9
add rax, r9


sub rbx, rax
sub rcx, rax
sub rdx, rax
sub rax, 10







or	rax,rax
or	rax,rbx
or	rax,rcx
or	rax,rdx
or eax, 10

and rax, rax
and rax, rbx
and rax, rcx
and rax, rdx
and rax, 10

not rax
not rbx
not rcx
not rdx

shr rax, 1
shr rbx, 1
shr rdx, 1
shr rdx, 1

shl rax, 1
shl rbx, 1
shl rcx, 1
shl rdx, 1
