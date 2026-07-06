.global paging_enable
.type paging_enable, @function

paging_enable:
    mov 4(%esp), %eax
    mov %eax, %cr3

    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    ret
