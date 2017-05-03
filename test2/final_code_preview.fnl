L1: jmp L0
L2: 
L3: 
L4: 
L5: 
L6: movi M[20+R[0]], R[1]
L7: movi R[255], M[4+R[0]]
movi R[255], M[4+R[255]]
movi R[255], M[4+R[255]]
movi R[255], M[4+R[255]]
movi R[254], 16
addi R[255], R[254], R[255]
movi M[R[255]], R[1]
L8: movi R[255], M[612]
movi M[32+R[0]], R[255]
L9: movi R[255], R[0]
movi R[254], 16
addi R[255], R[254], R[255]
movi M[40+R[0]], R[255]
L10: movi R[255], M[4+R[0]]
movi M[24+R[0]], R[255]
movi R[255], 20
addi R[0], R[255], R[0]
movi R[255], $
movi R[254], 15
addi R[255], R[255], R[254]
movi M[R[0]], R[255]
jmp L5
movi R[255], 20
subi R[0], R[0], R[255]
L11: movi R[1], M[16+R[0]]
movi R[255], M[8+R[0]]
movi M[R[255]], R[1]
L12: jmp M[R[0]]
L13: jmp L0
L14: jmp M[R[0]]
L15: jmp L0
L16: jmp M[R[0]]
L17: jmp L0
L0: 
L18: halt
L19: jmp M[R[0]]
