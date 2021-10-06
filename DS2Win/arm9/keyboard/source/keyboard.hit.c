#include "keyboard.def.h"

/*
 * 32x24 hit data
 */

const unsigned char keyboard_Hit[512] = {
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,ESC,ESC,0x0,F_1,F_1,F_2,F_2,F_3,F_3,F_4,F_4,F_5,F_5,F_6,F_6,F_7,F_7,F_8,F_8,F_9,F_9,F10,F10,F11,F11,F12,F12,0x0,EXT,EXT,0x0,
0x0,ESC,ESC,0x0,F_1,F_1,F_2,F_2,F_3,F_3,F_4,F_4,F_5,F_5,F_6,F_6,F_7,F_7,F_8,F_8,F_9,F_9,F10,F10,F11,F11,F12,F12,0x0,EXT,EXT,0x0,
0x0,'1','1','2','2','3','3','4','4','5','5','6','6','7','7','8','8','9','9','0','0','-','-','=','=',BSP,BSP,BSP,BSP,HOM,HOM,0x0,
0x0,'1','1','2','2','3','3','4','4','5','5','6','6','7','7','8','8','9','9','0','0','-','-','=','=',BSP,BSP,BSP,BSP,HOM,HOM,0x0,
0x0,TAB,'q','q','w','w','e','e','r','r','t','t','y','y','u','u','i','i','o','o','p','p','[','[',']',']','\\','\\',0x0,PGU,PGU,0x0,
0x0,TAB,'q','q','w','w','e','e','r','r','t','t','y','y','u','u','i','i','o','o','p','p','[','[',']',']','\\','\\',0x0,PGU,PGU,0x0,
0x0,CAP,CAP,'a','a','s','s','d','d','f','f','g','g','h','h','j','j','k','k','l','l',';',';','\'','\'',RET,RET,RET,RET,PGD,PGD,0x0,
0x0,CAP,CAP,'a','a','s','s','d','d','f','f','g','g','h','h','j','j','k','k','l','l',';',';','\'','\'',RET,RET,RET,RET,PGD,PGD,0x0,
0x0,SHF,SHF,SHF,'z','z','x','x','c','c','v','v','b','b','n','n','m','m',',',',','.','.','/','/',SHF,SHF,SHF,CRU,CRU,END,END,0x0,
0x0,SHF,SHF,SHF,'z','z','x','x','c','c','v','v','b','b','n','n','m','m',',',',','.','.','/','/',SHF,SHF,SHF,CRU,CRU,END,END,0x0,
0x0,CTL,CTL,NDS,NDS,ALT,ALT,'`','`',SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,INS,INS,DEL,DEL,SCN,SCN,CRL,CRL,CRD,CRD,CRR,CRR,0x0,
0x0,CTL,CTL,NDS,NDS,ALT,ALT,'`','`',SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,INS,INS,DEL,DEL,SCN,SCN,CRL,CRL,CRD,CRD,CRR,CRR,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
};

const unsigned char keyboard_Hit_Shift[512] = {
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,ESC,ESC,0x0,F_1,F_1,F_2,F_2,F_3,F_3,F_4,F_4,F_5,F_5,F_6,F_6,F_7,F_7,F_8,F_8,F_9,F_9,F10,F10,F11,F11,F12,F12,0x0,EXT,EXT,0x0,
0x0,ESC,ESC,0x0,F_1,F_1,F_2,F_2,F_3,F_3,F_4,F_4,F_5,F_5,F_6,F_6,F_7,F_7,F_8,F_8,F_9,F_9,F10,F10,F11,F11,F12,F12,0x0,EXT,EXT,0x0,
0x0,'!','!','@','@','#','#','$','$','%','%','^','^','&','&','*','*','(','(',')',')','_','_','+','+',BSP,BSP,BSP,BSP,HOM,HOM,0x0,
0x0,'!','!','@','@','#','#','$','$','%','%','^','^','&','&','*','*','(','(',')',')','_','_','+','+',BSP,BSP,BSP,BSP,HOM,HOM,0x0,
0x0,TAB,'Q','Q','W','W','E','E','R','R','T','T','Y','Y','U','U','I','I','O','O','P','P','{','{','}','}','|','|',0x0,PGU,PGU,0x0,
0x0,TAB,'Q','Q','W','W','E','E','R','R','T','T','Y','Y','U','U','I','I','O','O','P','P','{','{','}','}','|','|',0x0,PGU,PGU,0x0,
0x0,CAP,CAP,'A','A','S','S','D','D','F','F','G','G','H','H','J','J','K','K','L','L',':',':','"','"',RET,RET,RET,RET,PGD,PGD,0x0,
0x0,CAP,CAP,'A','A','S','S','D','D','F','F','G','G','H','H','J','J','K','K','L','L',':',':','"','"',RET,RET,RET,RET,PGD,PGD,0x0,
0x0,SHF,SHF,SHF,'Z','Z','X','X','C','C','V','V','B','B','N','N','M','M','<','<','>','>','?','?',SHF,SHF,SHF,CRU,CRU,END,END,0x0,
0x0,SHF,SHF,SHF,'Z','Z','X','X','C','C','V','V','B','B','N','N','M','M','<','<','>','>','?','?',SHF,SHF,SHF,CRU,CRU,END,END,0x0,
0x0,CTL,CTL,NDS,NDS,ALT,ALT,'~','~',SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,INS,INS,DEL,DEL,SCN,SCN,CRL,CRL,CRD,CRD,CRR,CRR,0x0,
0x0,CTL,CTL,NDS,NDS,ALT,ALT,'~','~',SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,SPC,INS,INS,DEL,DEL,SCN,SCN,CRL,CRL,CRD,CRD,CRR,CRR,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
};