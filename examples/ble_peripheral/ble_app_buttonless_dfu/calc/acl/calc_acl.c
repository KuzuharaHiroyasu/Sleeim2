#include "calc_acl.h"

void main_acl_init(void);
void main_acl_stop(void);
void main_acl_start(void);
void main_acl_read(void);

void main_acl_init(void)
{
    uint8_t rd_data[2] = {0};

//    wait_ms( 30 );		// �����x�Z���T�@���d��ON�҂�

    i2c_acl_read( ACCELEROMETER_ADDR, ACL_REG_ADR_WHO_AM_I, &rd_data[0], 1 );
    if( rd_data[0] != ACL_REG_RECOGNITION_CODE ){
    }

    main_acl_stop();
}

void main_acl_stop(void)
{
    uint8_t rd_data[2] = {0};
    uint8_t wr_data[2] = {0};
    
    wr_data[0] = ACL_REG_ADR_CTRL_REG1;
    wr_data[1] = 0x00;
    // ���샂�[�h�ݒ�
    i2c_acl_write( ACCELEROMETER_ADDR, &wr_data[0], 2, 0 );
    
    i2c_acl_read( ACCELEROMETER_ADDR, ACL_REG_ADR_CTRL_REG1, &rd_data[0], 1 );
    if( rd_data[0] != 0x00 ){
    }
}

void main_acl_start(void)
{
    uint8_t wr_data[2] = {0};

    // ���샂�[�h������
    wr_data[0] = ACL_REG_ADR_CTRL_REG1;
    wr_data[1] = 0x00;
    i2c_acl_write( ACCELEROMETER_ADDR, &wr_data[0], 2, 0 );

    // ���샂�[�h�ݒ�
    wr_data[0] = ACL_REG_ADR_CTRL_REG1;
    wr_data[1] = 0x20;
    i2c_acl_write( ACCELEROMETER_ADDR, &wr_data[0], 2, 0 );

    // ���샂�[�h�J�n
    wr_data[0] = ACL_REG_ADR_CTRL_REG1;
    wr_data[1] = 0xA0;
    i2c_acl_write( ACCELEROMETER_ADDR, &wr_data[0], 2, 0 );
}

void main_acl_read(void)
{
    uint8_t rd_data[10] = {0};

    // INT_SOURCE1		
    i2c_acl_read( ACCELEROMETER_ADDR, ACL_REG_ADR_INT_SRC1, &rd_data[0], 1 );
    if( 0 == ( rd_data[0] & 0x10 )){
        // �f�[�^���B

        // �����x�Z���T�[�ċN��
        main_acl_init();
        main_acl_start();
        return;
    }

    // �f�[�^�擾
    i2c_acl_read( ACCELEROMETER_ADDR, ACL_REG_ADR_DATA_XYZ, &rd_data[0], 6 );
    s_calc_acl.acl_x = rd_data[1];
    s_calc_acl.acl_y = rd_data[3];
    s_calc_acl.acl_z = rd_data[5];

    // INT_REL�ǂݏo���@�����荞�ݗv���N���A
    i2c_acl_read( ACCELEROMETER_ADDR, ACL_REG_ADR_INT_REL, &rd_data[0], 1 );

    calc_acl();
}