
; System V x86 ABI
; 调用方保存：eax、ecx、edx
; 实现方保存：ebx、esi、edi、ebp、esp

global task_switch:
task_switch:
    push ebp
    mov ebp, esp

    push ebx
    push esi
    push edi

    ; 保存当前任务的栈顶指针
    mov eax, esp
    and eax, 0xfffff000 ; 当前任务
    mov [eax], esp

    ; 恢复下一个任务的栈顶指针
    mov eax, [ebp + 8]  ; 下一个任务
    mov esp, [eax]

    pop edi
    pop esi
    pop ebx

    pop ebp

    ret