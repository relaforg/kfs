.global gdt_flush
.type gdt_flush, @function

gdt_flush:
    movl 4(%esp), %eax    /* récupère l'argument (adresse de gp) */
    lgdt (%eax)           /* CPU : "ta GDT est ici maintenant" */

    /* Recharge les data segments depuis la nouvelle GDT */
    movw $0x10, %ax       /* kernel data */
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    /* Recharge SS depuis la nouvelle GDT (vers le kernel stack) */
    movw $0x18, %ax       /* kernel stack */
    movw %ax, %ss

    /* Recharge CS via far jump — SEULE façon de changer CS */
    ljmp $0x08, $.flush

.flush:
    ret
