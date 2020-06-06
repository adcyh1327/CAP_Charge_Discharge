#ifndef _GUI_PAGE_H
#define _GUI_PAGE_H

#include "config.h"
#include "cmsis_os.h"

#define GUI_DATA_TYPE_INT16 0x00 //0x00=整数（2 字节），范围为-32768 到 32767 
#define GUI_DATA_TYPE_INT32 0x01 //0x01=长整数（4 字节），范围为-2147483648 到 2147483647 
#define GUI_DATA_TYPE_UINT8_H 0x02 //0x02=*VP 高字节，无符号数，范围 0 到 255 
#define GUI_DATA_TYPE_UINT8_L 0x03 //0x03=*VP 低字节，无符号数，范围 0 到 255 
#define GUI_DATA_TYPE_INT64 0x04 //0x04= 超 长 整 数 （ 8 字 节 ）， 范 围 为 -9223372036854775808 到 9223372036854775807 
#define GUI_DATA_TYPE_UINT16 0x05 //0x05=无符号整数（2 字节），范围为 0 到 65535 
#define GUI_DATA_TYPE_UINT32 0x06 //0x06=无符号长整数（4 字节），范围为 0 到 4294967295 

typedef struct GUI_WDG_Data_Display {
  uint16_t data_display_addr;
  uint16_t data_val;
  uint8_t data_type;
}GUI_WDG_DataDisplay_Type;

typedef struct GUI_WDG_Data_Input {
  uint16_t data_input_addr;
  uint16_t data_val;
  uint8_t data_type;
}GUI_WDG_DataInput_Type;

typedef struct GUI_WDG_Ret_Key_Code {
  uint16_t ret_key_code_addr;
  uint16_t ret_key_code;
}GUI_WDG_RetKeyCode_Type;

typedef struct GUI_Page_Data {
  uint8_t page_id;
  uint8_t WdgDataDisplayCnt;
  GUI_WDG_DataDisplay_Type *pWdgDataDispaly;
  uint8_t WdgDataInputCnt;
  GUI_WDG_DataInput_Type *pWdgDatInput;
  GUI_WDG_RetKeyCode_Type *pWdgRetKeyCode;
}GUI_Page_Info_Type;

#define PAGE_0_WDG_DATA_DISPLAY_NUM 2
#define PAGE_0_WDG_DATA_DISPLAY \
                /*  VP Adress,  Data Type */ \
/*电压显示*/    {   0x0000,     GUI_DATA_TYPE_UINT32,  0x0},\
/*电流显示*/    {   0x0001,     GUI_DATA_TYPE_UINT32,  0x0},\


#define PAGE_0_WDG_DATA_INPUT_NUM 2
#define PAGE_0_WDG_DATA_INPUT \
                    /*  VP Adress,  Data Type */ \
/*电压设置输入*/    {   0x0000,     GUI_DATA_TYPE_UINT32,  0x0},\
/*电流设置输入*/    {   0x0001,     GUI_DATA_TYPE_UINT32,  0x0},\


#define PAGE_0_WDG_RET_KEY_CODE_NUM 1
#define PAGE_0_WDG_RET_KEY_CODE \
/*              VP Adress , Key Code*/  \
/*按键设置*/    {0x0000,     0x0},\

/* Page 0 definition */
GUI_WDG_DataDisplay_Type UI_Page_0_DatDisplay[PAGE_0_WDG_DATA_DISPLAY_NUM] =
{
  PAGE_0_WDG_DATA_DISPLAY
};
GUI_WDG_DataInput_Type UI_Page_0_DatInput[PAGE_0_WDG_DATA_INPUT_NUM] = 
{
  PAGE_0_WDG_DATA_INPUT
};
GUI_WDG_RetKeyCode_Type UI_Page_0_RetKeyCode[PAGE_0_WDG_RET_KEY_CODE_NUM] =
{
  PAGE_0_WDG_RET_KEY_CODE
};

enum ePage_0_Keys
{
  ePage_0_Key_SysSetting = 0x0001,
  ePage_0_Key_Save,
  ePage_0_Key_Exit,
  ePage_0_Key_Max,
};

GUI_Page_Info_Type UI_Page_0 = 
{
  0,
  PAGE_0_WDG_DATA_DISPLAY_NUM,  &UI_Page_0_DatDisplay[0],
  PAGE_0_WDG_DATA_INPUT_NUM,    &UI_Page_0_DatInput[0],
  &UI_Page_0_RetKeyCode[0]
};

GUI_Page_Info_Type *pGUI_Pages[2] = 
{
  &UI_Page_0,
  &UI_Page_0
};

GUI_Page_Info_Type *pGUI_Current_Pages;

#endif
