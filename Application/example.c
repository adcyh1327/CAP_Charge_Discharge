



//首先定义读寄存器地址偏移值，每16位为一个地址，对应一个寄存器
enum Invertor_Offset{//变频器状态参数地址
    off_InvertorError=0,off_CurrFreqPower,
    NUM_Read_Freq_Total
};


//首先定义一个数据块，包含以下内容
struct RTU_ReqBlock RTU_Req_ReadCurrent_Thres= //RTUÊý¾ÝÇëÇó¿é,¶ÁÈ¡µ±Ç°µç»úÔËÐÐ¹¦ÂÊ
{
	LIST_HEAD_INIT(RTU_Req_ReadPower_5005.Entry),
    1,                                          //执行次数，0-无限次
	UART_CHN_CURRENT,                      		//执行通道
	SLAVEID_SAMPLE,                             //从节点站地址
	FUNC_RD_HOLDREG,                            //功能码03
	EXCUTE_SUCCESS,                             //执行结果,用于读
	0x5005,                                     //操作寄存器地址
	0x01,                                       //操作寄存器数量
	(u16*)&CurrentData[off_Currdata]      		//执行的数据，读取的寄存器数据或写操作的数据
};


//操作时使用下边函数，UART_CHN_CURRENT：该模块使用的uart通道号
RTU_AddReqBlock_Front(&rtu_ctx[UART_CHN_CURRENT],&RTU_Req_ReadCurrent_Thres);//优先级高，最先执行
或
RTU_AddReqBlock(&rtu_ctx[UART_CHN_CURRENT],&RTU_Req_ReadCurrent_Thres);//FIFO方式