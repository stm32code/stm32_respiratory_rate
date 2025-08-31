#include "git.h"

Data_TypeDef Data_init;						  // 设备数据结构体
Threshold_Value_TypeDef threshold_value_init; // 设备阈值设置结构体
Device_Satte_Typedef device_state_init;		  // 设备状态

// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data)
{
	//呼吸检测
	Device_Data->state = ADC1_value();
	return MY_SUCCESSFUL;
}

// 初始化
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state)
{


	// 状态重置
	device_state_init.breath_time = 11;
	device_state_init.breath_state = 3;
	
	return MY_SUCCESSFUL;
}
// 更新OLED显示屏中内容
mySta Update_oled_massage()
{
#if OLED // 是否打开
	char str[50];

	sprintf(str, "正常区间:1-2/S");
	OLED_ShowCH(0, 0, (unsigned char *)str);
	if(device_state_init.breath_state){
		sprintf(str, "呼吸状态: 正常  ");
	}else
		sprintf(str, "呼吸状态: 异常  ");
	OLED_ShowCH(0, 2, (unsigned char *)str);
	sprintf(str, "检测值: %02d /10S", device_state_init.breath_copy );
	OLED_ShowCH(0, 4, (unsigned char *)str);
	sprintf(str, "倒计时: %02dS ", device_state_init.breath_time);
	OLED_ShowCH(0, 6, (unsigned char *)str);
#endif

	return MY_SUCCESSFUL;
}

// 更新设备状态
mySta Update_device_massage()
{
	// 呼吸监测
	if(Data_init.state < 100){
		device_state_init.breath++;
		device_state_init.breath_state = 3;
	}
	// 报警
	if (device_state_init.breath_copy < 5 || device_state_init.breath_copy >20)
	{
		BEEP = ~BEEP;
		device_state_init.waning = 1;
	}
	else
	{	
		BEEP=0;
		device_state_init.waning = 0;
	}

	return MY_SUCCESSFUL;
}

// 定时器
void Automation_Close(void)
{
	// 计时间
	if(device_state_init.breath_time ){
		device_state_init.breath_time --;
		// 计算呼吸次数
		if(device_state_init.breath_time == 1){
		
			device_state_init.breath_copy =device_state_init.breath/3;
			SendMqtt(1); // 发送数据到APP
			device_state_init.breath =0;
			device_state_init.breath_time = 11;
		}

	}
	// 呼吸状态的判断
	if(device_state_init.breath_state){
		device_state_init.breath_state -- ;
	}
	
}
// 检测按键是否按下
static U8 num_on = 0;
static U8 key_old = 0;
void Check_Key_ON_OFF()
{
	U8 key;
	key = KEY_Scan(1);
	// 与上一次的键值比较 如果不相等，表明有键值的变化，开始计时
	if (key != 0 && num_on == 0)
	{
		key_old = key;
		num_on = 1;
	}
	if (key != 0 && num_on >= 1 && num_on <= Key_Scan_Time) // 25*10ms
	{
		num_on++; // 时间记录器
	}
	if (key == 0 && num_on > 0 && num_on < Key_Scan_Time) // 短按
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Short\n");
			if(device_state_init.waning){
				device_state_init.waning = 0;
			}else{
				device_state_init.waning = 1;
			}
			if (Data_init.App == 0) {
       Data_init.App = 1;
			}
			break;
		case KEY2_PRES:
			printf("Key2_Short\n");

			break;
		case KEY3_PRES:
			printf("Key3_Short\n");

			break;

		default:
			break;
		}
		num_on = 0;
	}
	else if (key == 0 && num_on >= Key_Scan_Time) // 长按
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Long\n");

			break;
		case KEY2_PRES:
			printf("Key2_Long\n");

			break;
		case KEY3_PRES:
			printf("Key3_Long\n");

			break;
		default:
			break;
		}
		num_on = 0;
	}
}
// 解析json数据
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // 检测json格式
	//cJSON *cjson_data = NULL; // 数据
	const char *massage;
	// 定义数据类型
	u8 cjson_cmd; // 指令,方向

	/* 解析整段JSO数据 */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// 解析失败
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* 依次根据名称提取JSON数据（键值对） */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* 解析嵌套json数据 */
	//cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // 消息包
		
		break;
	case 0x02: // 消息包

		break;
	case 0x03: // 数据包

		break;
	case 0x04: // 数据包
		Data_init.App = cjson_cmd + 1;

		break;
	default:
		break;
	}

	/* 清空JSON对象(整条链表)的所有数据 */
	cJSON_Delete(cjson_test);

	return MY_SUCCESSFUL;
}
