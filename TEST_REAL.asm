[bits 16]
[org 0x10]
mov ax, 1
mov bx, 2
mov cx, 3
mov dx, 4

add ax, bx
add ax, cx
add ax, dx
add ax, 5

sub bx, ax
sub cx, ax
sub dx, ax
sub ax, 10







or	ax,ax
or ax, 10

and ax, ax
and ax, 10

not ax

shr ax, 1

shl ax, 1
