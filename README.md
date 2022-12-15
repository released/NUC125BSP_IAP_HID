# NUC125BSP_IAP_HID
 NUC125BSP_IAP_HID


update @ 2022/12/15

1. due to USP HID need to add USB trim function , and file size will over LDROM , move ISP project and Application project to APROM

- ISP USB HID : at APROM address : 0x0000 

- Application : at APROM address : 0x2000 

2. ISP USB HID project modify , 

- ISP_USER.c modify ,

	after update finish , reset the MCU (make sure to remove DetectPin)

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/ISP_User_01.jpg)

	erase APROM  , need to shift to address : 0x2000 (ISP code @ APROM 0x0000 , and application @ APROM 0x2000)

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/ISP_User_02.jpg)

	update APROM  , need to shift to address : 0x2000 (ISP code @ APROM 0x0000 , and application @ APROM 0x2000)

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/ISP_User_03.jpg)

- KEIL project modify , Linker add sct file , and fix ISP code to address : 0x0000

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/LDROM_KEIL_sct.jpg)

- main.c modify , 

	add USB trim function
	
	jump to APROM address : 0x2000 , when entry application code 

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/main_01.jpg)

3. Application project modify , 

- need to modify RO BASE , 0x2000

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/application_RO.jpg)

4. how to use ICP tool update

- config , set boot : APROM with IAP

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/ICP_Config.jpg)

- programming ISP code to APROM address : 0x0000 

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/ICP_isp_code.jpg)

- programming application code to APROM address : 0x2000 

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/ICP_application.jpg)

5. how to use ISP tool update

when DetectPin IS LOW , reset MCU ( to make sure boot start from APROM address : 0x0000 , and start ISP tool programming 

after update finish , remove DetectPin and reset MCU

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/ISP_update.jpg)

6. below is log message screen , 

![image](https://github.com/released/NUC125BSP_IAP_HID/blob/main/log_.jpg)


