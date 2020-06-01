#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

enum Func_Code{
  Func_WriteRegister=0x80, Func_ReadRegister, Func_WriteDisplayRAM, Func_ReadDisplayRAM
};


void LCD_Display_Init(void);
void LCD_Display_MainFunction(void *p_arg);

#endif
