
#ifndef __FPGAREGS_H__APOGEE_APN__
#define __FPGAREGS_H__APOGEE_APN__


#define FPGA_TOTAL_REGISTER_COUNT			105


#define FPGA_REG_COMMAND_A					0
#define		FPGA_BIT_CMD_EXPOSE				0x0001
#define		FPGA_BIT_CMD_DARK				0x0002
#define		FPGA_BIT_CMD_TEST				0x0004
#define		FPGA_BIT_CMD_TDI				0x0008
#define		FPGA_BIT_CMD_FLUSH				0x0010
#define		FPGA_BIT_CMD_TRIGGER_EXPOSE		0x0020
#define		FPGA_BIT_CMD_KINETICS			0x0040

#define FPGA_REG_COMMAND_B					1
#define		FPGA_BIT_CMD_RESET				0x0002
#define		FPGA_BIT_CMD_CLEAR_ALL			0x0010
#define		FPGA_BIT_CMD_END_EXPOSURE		0x0080
#define		FPGA_BIT_CMD_RAMP_TO_SETPOINT	0x0200
#define		FPGA_BIT_CMD_RAMP_TO_AMBIENT	0x0400
#define		FPGA_BIT_CMD_START_TEMP_READ	0x2000
#define		FPGA_BIT_CMD_DAC_LOAD			0x4000
#define		FPGA_BIT_CMD_AD_CONFIG			0x8000

#define FPGA_REG_OP_A						2
#define		FPGA_BIT_LED_DISABLE			0x0001
#define		FPGA_BIT_PAUSE_TIMER			0x0002
#define		FPGA_BIT_RATIO					0x0004
#define		FPGA_BIT_DELAY_MODE				0x0008
#define		FPGA_BIT_DUAL_AD_READOUT		0x0010
#define		FPGA_BIT_LED_EXPOSE_DISABLE		0x0020
#define		FPGA_BIT_DISABLE_H_CLK			0x0040
#define		FPGA_BIT_SHUTTER_AMP_CONTROL	0x0080
#define		FPGA_BIT_HALT_DISABLE			0x0100			
#define		FPGA_BIT_SHUTTER_MODE			0x0200
#define		FPGA_BIT_DIGITIZATION_RES		0x0400
#define		FPGA_BIT_FORCE_SHUTTER			0x0800
#define		FPGA_BIT_DISABLE_SHUTTER		0x1000
#define		FPGA_BIT_TEMP_SUSPEND			0x2000
#define		FPGA_BIT_SHUTTER_SOURCE			0x4000
#define		FPGA_BIT_TEST_MODE				0x8000

#define FPGA_REG_OP_B						3
#define		FPGA_BIT_CONT_IMAGE_ENABLE		0x0004
#define		FPGA_BIT_HCLAMP_ENABLE			0x0008
#define		FPGA_BIT_HSKIP_ENABLE			0x0010
#define		FPGA_BIT_HRAM_ENABLE			0x0020
#define		FPGA_BIT_VRAM_ENABLE			0x0040
#define		FPGA_BIT_DAC_SELECT_ZERO		0x0080
#define		FPGA_BIT_DAC_SELECT_ONE			0x0100
#define		FPGA_BIT_FIFO_WRITE_BLOCK		0x0200
#define		FPGA_BIT_DISABLE_FLUSH_COMMANDS	0x0800
#define		FPGA_BIT_DISABLE_POST_EXP_FLUSH	0x1000
#define		FPGA_BIT_AD_SIM_RATE_ZERO		0x2000
#define		FPGA_BIT_AD_SIM_RATE_ONE		0x4000
#define		FPGA_BIT_AD_SIMULATION			0x8000

#define FPGA_REG_TIMER_UPPER				4
#define FPGA_REG_TIMER_LOWER				5

#define FPGA_REG_HRAM_INPUT					6
#define FPGA_REG_VRAM_INPUT					7

#define FPGA_REG_HRAM_INV_MASK				8
#define FPGA_REG_VRAM_INV_MASK				9

#define FPGA_REG_HCLAMP_INPUT				10
#define FPGA_REG_HSKIP_INPUT				11

#define FPGA_REG_CLAMP_COUNT				13
#define FPGA_REG_PREROI_SKIP_COUNT			14
#define FPGA_REG_ROI_COUNT					15
#define FPGA_REG_POSTROI_SKIP_COUNT			16
#define FPGA_REG_OVERSCAN_COUNT				17
#define FPGA_REG_IMAGE_COUNT				18

#define FPGA_REG_VFLUSH_BINNING				19

#define FPGA_REG_SHUTTER_CLOSE_DELAY		20

#define FPGA_REG_POSTOVERSCAN_SKIP_COUNT	21

#define FPGA_REG_SHUTTER_STROBE_POSITION	23
#define FPGA_REG_SHUTTER_STROBE_PERIOD		24

#define FPGA_REG_FAN_SPEED_CONTROL			25
#define FPGA_REG_LED_DRIVE					26
#define FPGA_REG_SUBSTRATE_ADJUST			27
#define FPGA_MASK_FAN_SPEED_CONTROL			0x0FFF
#define FPGA_MASK_LED_ILLUMINATION			0x0FFF
#define FPGA_MASK_SUBSTRATE_ADJUST			0x0FFF

#define FPGA_REG_TEST_COUNT_UPPER			28
#define FPGA_REG_TEST_COUNT_LOWER			29

#define FPGA_REG_A1_ROW_COUNT				30
#define FPGA_REG_A1_VBINNING				31
#define FPGA_REG_A2_ROW_COUNT				32
#define FPGA_REG_A2_VBINNING				33
#define FPGA_REG_A3_ROW_COUNT				34
#define FPGA_REG_A3_VBINNING				35
#define FPGA_REG_A4_ROW_COUNT				36
#define FPGA_REG_A4_VBINNING				37
#define FPGA_REG_A5_ROW_COUNT				38
#define FPGA_REG_A5_VBINNING				39

#define FPGA_MASK_VBINNING					0x0FFF
#define	FPGA_BIT_ARRAY_DIGITIZE				0x1000
#define	FPGA_BIT_ARRAY_FASTDUMP				0x4000

#define FPGA_REG_TDI_BINNING				44
 
#define FPGA_REG_PRIMARY_AD_OFFSET			45
#define FPGA_REG_SECONDARY_AD_OFFSET		56

#define FPGA_REG_SEQUENCE_DELAY				47
#define FPGA_REG_TDI_RATE					48

#define FPGA_REG_IO_PORT_WRITE				49

#define FPGA_REG_IO_PORT_DIRECTION			50
#define FPGA_MASK_IO_PORT_DIRECTION			0x003F

#define FPGA_REG_IO_PORT_ASSIGNMENT			51
#define FPGA_MASK_IO_PORT_ASSIGNMENT		0x003F

#define FPGA_REG_LED_SELECT					52
#define FPGA_MASK_LED_SELECT_A				0x000F
#define FPGA_MASK_LED_SELECT_B				0x00F0
#define		FPGA_BIT_LED_EXPOSE				0x0001
#define		FPGA_BIT_LED_IMAGE_ACTIVE		0x0002
#define		FPGA_BIT_LED_FLUSHING			0x0004
#define		FPGA_BIT_LED_TRIGGER_WAIT		0x0008
#define		FPGA_BIT_LED_EXT_TRIGGER		0x0010
#define		FPGA_BIT_LED_EXT_SHUTTER_INPUT	0x0020
#define		FPGA_BIT_LED_EXT_START_READOUT	0x0040
#define		FPGA_BIT_LED_AT_TEMP			0x0080

#define FPGA_REG_SCRATCH					53

#define FPGA_REG_TDI_COUNT					54

#define FPGA_REG_TEMP_DESIRED				55

#define FPGA_REG_TEMP_RAMP_DOWN_A			57
#define FPGA_REG_TEMP_RAMP_DOWN_B			58

#define FPGA_REG_OP_C						59
#define		FPGA_BIT_TDI_TRIGGER_GROUP		0x0001
#define		FPGA_BIT_TDI_TRIGGER_EACH		0x0002
#define		FPGA_BIT_IMAGE_TRIGGER_EACH		0x0004
#define		FPGA_BIT_IMAGE_TRIGGER_GROUP	0x0008

#define FPGA_REG_TEMP_BACKOFF				60
#define FPGA_REG_TEMP_COOLER_OVERRIDE		61
#define	FPGA_MASK_TEMP_PARAMS				0x0FFF		// 12 bits

#define FPGA_REG_AD_CONFIG_DATA				62
#define FPGA_MASK_AD_GAIN					0x07FF		// 11 bits

#define FPGA_REG_IO_PORT_READ				90
#define FPGA_MASK_IO_PORT_DATA				0x003F

#define FPGA_REG_GENERAL_STATUS					91
#define		FPGA_BIT_STATUS_IMAGE_EXPOSING		0x0001
#define		FPGA_BIT_STATUS_IMAGING_ACTIVE		0x0002
#define		FPGA_BIT_STATUS_DATA_HALTED			0x0004
#define		FPGA_BIT_STATUS_IMAGE_DONE			0x0008
#define		FPGA_BIT_STATUS_FLUSHING			0x0010
#define		FPGA_BIT_STATUS_WAITING_TRIGGER		0x0020
#define		FPGA_BIT_STATUS_SHUTTER_OPEN		0x0040
#define		FPGA_BIT_STATUS_PATTERN_ERROR		0x0080
#define		FPGA_BIT_STATUS_TEMP_SUSPEND_ACK	0x0100
#define		FPGA_BIT_STATUS_TEMP_REVISION		0x2000
#define		FPGA_BIT_STATUS_TEMP_AT_TEMP		0x4000
#define		FPGA_BIT_STATUS_TEMP_ACTIVE			0x8000

#define FPGA_REG_TEMP_HEATSINK				93
#define FPGA_REG_TEMP_CCD					94
#define FPGA_REG_TEMP_DRIVE					95

#define FPGA_REG_INPUT_VOLTAGE				96
#define FPGA_MASK_INPUT_VOLTAGE				0x0FFF

#define FPGA_REG_TEMP_REVISED				97

#define FPGA_REG_FIFO_DATA					98
#define FPGA_REG_FIFO_STATUS				99

#define FPGA_REG_CAMERA_ID					100
#define FPGA_MASK_CAMERA_ID					0x007F

#define FPGA_REG_FIRMWARE_REV				101

#define FPGA_REG_FIFO_FULL_COUNT			102
#define FPGA_REG_FIFO_EMPTY_COUNT			103

#define FPGA_REG_TDI_COUNTER				104
#define FPGA_REG_SEQUENCE_COUNTER			105

#endif
