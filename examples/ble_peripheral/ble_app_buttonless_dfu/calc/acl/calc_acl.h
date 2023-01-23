#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../calc_data.h"
#include "../../i2c/i2c.h"
#include "../../config.h"

// レジスタアドレス
#define ACL_REG_ADR_WHO_AM_I			0x0F				// WHO AM I
#define ACL_REG_ADR_DATA_XYZ			0x06				// XOUT,YOUT,ZOUT
#define ACL_REG_ADR_INT_SRC1			0x16				// INT_SOURCE1
#define ACL_REG_ADR_INT_REL			0x1A				// INT_REL
#define ACL_REG_ADR_CTRL_REG1			0x1B				// CTRL_REG1
#define ACL_REG_ADR_CTRL_REG2			0x1F				// CTRL_REG2


// レジスタデータ
#define ACL_REG_RECOGNITION_CODE		0x35				// 認識コード(0x35)

void acl_init(void);
void main_acl_stop(void);
void main_acl_start(void);
void main_acl_read(void);
