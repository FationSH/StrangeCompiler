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
L14: movi R[255], M[16+R[0]]
movi R[1], M[R[255]]
movi M[20+R[0]], R[1]
L15: movi R[1], M[12+R[0]]
movi R[255], M[16+R[0]]
movi M[R[255]], R[1]
L16: movi R[255], M[20+R[0]]
movi M[44+R[0]], R[255]
L17: movi R[255], R[0]
movi R[254], 24
addi R[255], R[254], R[255]
movi M[52+R[0]], R[255]
L18: movi M[36+R[0]], R[0]
movi R[255], 20
addi R[0], R[255], R[0]
movi R[255], $
movi R[254], 15
addi R[255], R[255], R[254]
movi M[R[0]], R[255]
jmp L1
movi R[255], 32
subi R[0], R[0], R[255]
L19: movi R[1], M[24+R[0]]
movi R[255], M[8+R[0]]
movi M[R[255]], R[1]
L20: jmp M[R[0]]
L21: jmp L0
L22: movi R[1], 100
movi M[616], R[1]
L23: movi R[255], M[612]
movi M[52+R[0]], R[255]
L24: L25: movi R[255], R[0]
movi R[254], 28
addi R[255], R[254], R[255]
movi M[60+R[0]], R[255]
L26: movi M[44+R[0]], R[0]
movi R[255], 32
addi R[0], R[255], R[0]
movi R[255], $
movi R[254], 15
addi R[255], R[255], R[254]
movi M[R[0]], R[255]
jmp L1
movi R[255], 40
subi R[0], R[0], R[255]
L27: movi R[1], M[28+R[0]]
movi M[612], R[1]
L28: movi R[1], M[616]
movi R[2], M[20+R[0]]
addi R[3], R[1], R[2]
movi R[255], M[4+R[0]]
movi R[255], M[4+R[255]]
movi R[254], 32
addi R[255], R[254], R[255]
movi M[R[255]], R[3]
L29: movi R[1], M[32+R[0]]
movi R[255], M[8+R[0]]
movi M[R[255]], R[1]
L30: jmp M[R[0]]
L31: jmp L0
L0: 
L32: movi R[255], M[612]
movi M[40+R[0]], R[255]
L33: movi R[255], R[0]
muli R[254], 0
addi R[255], R[254], R[255]
movi M[48+R[0]], R[255]
L34: movi R[255], R[0]
movi R[254], 20
addi R[255], R[254], R[255]
movi M[48+R[0]], R[255]
L35: movi R[255], M[4+R[0]]
movi M[32+R[0]], R[255]
movi R[255], 40
addi R[0], R[255], R[0]
movi R[255], $
movi R[254], 15
addi R[255], R[255], R[254]
movi M[R[0]], R[255]
jmp L1
movi R[255], 28
subi R[0], R[0], R[255]
L36: movi R[1], M[620]
movi M[612], R[1]
L37: halt
L38: jmp M[R[0]]
