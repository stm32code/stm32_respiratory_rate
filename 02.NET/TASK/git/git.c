#include "git.h"

Data_TypeDef Data_init;						  // �豸���ݽṹ��
Threshold_Value_TypeDef threshold_value_init; // �豸��ֵ���ýṹ��
Device_Satte_Typedef device_state_init;		  // �豸״̬

// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data)
{
	//�������
	Device_Data->state = ADC1_value();
	return MY_SUCCESSFUL;
}

// ��ʼ��
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state)
{


	// ״̬����
	device_state_init.breath_time = 11;
	device_state_init.breath_state = 3;
	
	return MY_SUCCESSFUL;
}
// ����OLED��ʾ��������
mySta Update_oled_massage()
{
#if OLED // �Ƿ��
	char str[50];

	sprintf(str, "��������:1-2/S");
	OLED_ShowCH(0, 0, (unsigned char *)str);
	if(device_state_init.breath_state){
		sprintf(str, "����״̬: ����  ");
	}else
		sprintf(str, "����״̬: �쳣  ");
	OLED_ShowCH(0, 2, (unsigned char *)str);
	sprintf(str, "���ֵ: %02d /10S", device_state_init.breath_copy );
	OLED_ShowCH(0, 4, (unsigned char *)str);
	sprintf(str, "����ʱ: %02dS ", device_state_init.breath_time);
	OLED_ShowCH(0, 6, (unsigned char *)str);
#endif

	return MY_SUCCESSFUL;
}

// �����豸״̬
mySta Update_device_massage()
{
	// �������
	if(Data_init.state < 100){
		device_state_init.breath++;
		device_state_init.breath_state = 3;
	}
	// ����
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

// ��ʱ��
void Automation_Close(void)
{
	// ��ʱ��
	if(device_state_init.breath_time ){
		device_state_init.breath_time --;
		// �����������
		if(device_state_init.breath_time == 1){
		
			device_state_init.breath_copy =device_state_init.breath/3;
			SendMqtt(1); // �������ݵ�APP
			device_state_init.breath =0;
			device_state_init.breath_time = 11;
		}

	}
	// ����״̬���ж�
	if(device_state_init.breath_state){
		device_state_init.breath_state -- ;
	}
	
}
// ��ⰴ���Ƿ���
static U8 num_on = 0;
static U8 key_old = 0;
void Check_Key_ON_OFF()
{
	U8 key;
	key = KEY_Scan(1);
	// ����һ�εļ�ֵ�Ƚ� �������ȣ������м�ֵ�ı仯����ʼ��ʱ
	if (key != 0 && num_on == 0)
	{
		key_old = key;
		num_on = 1;
	}
	if (key != 0 && num_on >= 1 && num_on <= Key_Scan_Time) // 25*10ms
	{
		num_on++; // ʱ���¼��
	}
	if (key == 0 && num_on > 0 && num_on < Key_Scan_Time) // �̰�
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
	else if (key == 0 && num_on >= Key_Scan_Time) // ����
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
// ����json����
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // ���json��ʽ
	//cJSON *cjson_data = NULL; // ����
	const char *massage;
	// ������������
	u8 cjson_cmd; // ָ��,����

	/* ��������JSO���� */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// ����ʧ��
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* ���θ���������ȡJSON���ݣ���ֵ�ԣ� */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* ����Ƕ��json���� */
	//cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // ��Ϣ��
		
		break;
	case 0x02: // ��Ϣ��

		break;
	case 0x03: // ���ݰ�

		break;
	case 0x04: // ���ݰ�
		Data_init.App = cjson_cmd + 1;

		break;
	default:
		break;
	}

	/* ���JSON����(��������)���������� */
	cJSON_Delete(cjson_test);

	return MY_SUCCESSFUL;
}
