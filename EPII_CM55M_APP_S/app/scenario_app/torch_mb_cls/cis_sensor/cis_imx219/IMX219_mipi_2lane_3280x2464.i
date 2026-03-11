		/**
		 *   imx219_common_regs
		 */
		{HX_CIS_I2C_Action_W, 0x0100, 0x00},	/* Mode Select: Stream Off */

		/* To Access Addresses 3000-5fff, send the following commands */
		{HX_CIS_I2C_Action_W, 0x30eb, 0x0c},
		{HX_CIS_I2C_Action_W, 0x30eb, 0x05},
		{HX_CIS_I2C_Action_W, 0x300a, 0xff},
		{HX_CIS_I2C_Action_W, 0x300b, 0xff},
		{HX_CIS_I2C_Action_W, 0x30eb, 0x05},
		{HX_CIS_I2C_Action_W, 0x30eb, 0x09},

		/* PLL Clock Table */
		{HX_CIS_I2C_Action_W, 0x0301, 0x05},	/* VTPXCK_DIV */
		{HX_CIS_I2C_Action_W, 0x0303, 0x01},	/* VTSYSCK_DIV */
		{HX_CIS_I2C_Action_W, 0x0304, 0x03},	/* PREPLLCK_VT_DIV 0x03 = AUTO set */
		{HX_CIS_I2C_Action_W, 0x0305, 0x03}, 	/* PREPLLCK_OP_DIV 0x03 = AUTO set */
		{HX_CIS_I2C_Action_W, 0x0306, 0x00},	/* PLL_VT_MPY */
		{HX_CIS_I2C_Action_W, 0x0307, 0x39},
		{HX_CIS_I2C_Action_W, 0x030b, 0x01},	/* OP_SYS_CLK_DIV */
		{HX_CIS_I2C_Action_W, 0x030c, 0x00},	/* PLL_OP_MPY */
		{HX_CIS_I2C_Action_W, 0x030d, 0x72},

		/* Undocumented registers */
		{HX_CIS_I2C_Action_W, 0x455e, 0x00},
		{HX_CIS_I2C_Action_W, 0x471e, 0x4b},
		{HX_CIS_I2C_Action_W, 0x4767, 0x0f},
		{HX_CIS_I2C_Action_W, 0x4750, 0x14},
		{HX_CIS_I2C_Action_W, 0x4540, 0x00},
		{HX_CIS_I2C_Action_W, 0x47b4, 0x14},
		{HX_CIS_I2C_Action_W, 0x4713, 0x30},
		{HX_CIS_I2C_Action_W, 0x478b, 0x10},
		{HX_CIS_I2C_Action_W, 0x478f, 0x10},
		{HX_CIS_I2C_Action_W, 0x4793, 0x10},
		{HX_CIS_I2C_Action_W, 0x4797, 0x0e},
		{HX_CIS_I2C_Action_W, 0x479b, 0x0e},

		/* Frame Bank Register Group "A" */
		{HX_CIS_I2C_Action_W, 0x0162, 0x0d},	/* Line_Length_A */
		{HX_CIS_I2C_Action_W, 0x0163, 0x78},
		{HX_CIS_I2C_Action_W, 0x0170, 0x01},	/* X_ODD_INC_A */
		{HX_CIS_I2C_Action_W, 0x0171, 0x01},	/* Y_ODD_INC_A */

		/* Output setup registers */
		{HX_CIS_I2C_Action_W, 0x0114, 0x01},	/* CSI 2-Lane Mode */
		{HX_CIS_I2C_Action_W, 0x0128, 0x00},	/* DPHY Auto Mode */
		{HX_CIS_I2C_Action_W, 0x012a, 0x18},	/* EXCK_Freq */
		{HX_CIS_I2C_Action_W, 0x012b, 0x00},

		/**
		 *   Configure two or four Lane mode
		 */
		{HX_CIS_I2C_Action_W, 0x0114, 0x01},	/* CSI 2-Lane Mode */

		/**
		 * imx219_reg mode_3280x2464_regs
		 */
		{HX_CIS_I2C_Action_W, 0x0164, 0x00},
		{HX_CIS_I2C_Action_W, 0x0165, 0x00},
		{HX_CIS_I2C_Action_W, 0x0166, 0x0c},
		{HX_CIS_I2C_Action_W, 0x0167, 0xcf},
		{HX_CIS_I2C_Action_W, 0x0168, 0x00},
		{HX_CIS_I2C_Action_W, 0x0169, 0x00},
		{HX_CIS_I2C_Action_W, 0x016a, 0x09},
		{HX_CIS_I2C_Action_W, 0x016b, 0x9f},
		{HX_CIS_I2C_Action_W, 0x016c, 0x0c},
		{HX_CIS_I2C_Action_W, 0x016d, 0xd0},
		{HX_CIS_I2C_Action_W, 0x016e, 0x09},
		{HX_CIS_I2C_Action_W, 0x016f, 0xa0},
		{HX_CIS_I2C_Action_W, 0x0624, 0x0c},
		{HX_CIS_I2C_Action_W, 0x0625, 0xd0},
		{HX_CIS_I2C_Action_W, 0x0626, 0x09},
		{HX_CIS_I2C_Action_W, 0x0627, 0xa0},

		/**
		 * imx219_set_framefmt: raw10_framefmt_regs
		 */
		{HX_CIS_I2C_Action_W, 0x018c, 0x0a},
		{HX_CIS_I2C_Action_W, 0x018d, 0x0a},
		{HX_CIS_I2C_Action_W, 0x0309, 0x0a},