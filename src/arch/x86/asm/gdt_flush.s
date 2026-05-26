.global gdt_flush
.type gdt_flush, @function

gdt_flush:
    movl 4(%esp), %eax
    lgdt (%eax)

    movw $0x10, %ax       /* kernel data - 0x10 */
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    movw $0x18, %ax       /* kernel STACK - 0x18 */
    movw %ax, %ss

.flush:
    ret
