#include "vib_ctrl.h"

bool vib_begin(void);
uint8_t getVibrate(void);
bool setMask(uint8_t);
uint8_t getMask(void);
event_t getIrqEvent(void);
diag_status_t getEventDiag(void);
bool _writeRegister(uint8_t _wReg, uint8_t _mask, uint8_t _bits, uint8_t _startPosition);
uint8_t _readRegister(uint8_t _reg);

ret_code_t vib_init(void)
{
    uint8_t wr_data[2] = {0};
    ret_code_t ret = NRF_SUCCESS;

    vib_begin();

    wr_data[0] = IRQ_EVENT1;	// 0x03
    wr_data[1] = 0xFF;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = IRQ_EVENT_WARN_DIAG;	// 0x04
    wr_data[1] = 0x00;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = IRQ_EVENT_SEQ_DIAG;	//0x05
    wr_data[1] = 0x00;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = IRQ_MASK1; //0x07
    wr_data[1] = 0x00;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = CIF_I2C1;  // 0x08
    wr_data[1] = 0x40;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = FRQ_LRA_PER_H;	// 0x0A
    wr_data[1] = 0x24;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = FRQ_LRA_PER_L;	// 0x0B
    wr_data[1] = 0x4F;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = ACTUATOR1; // 0x0C
    wr_data[1] = 0x00;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = ACTUATOR2; // 0x0D
    wr_data[1] = 0x15;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = ACTUATOR3; // 0x0E
    wr_data[1] = 0x0F;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = CALIB_V2I_H; // 0x0F
    wr_data[1] = 0x00;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = CALIB_V2I_L; // 0x10
    wr_data[1] = 0x69;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_CFG1; // 0x13
    wr_data[1] = 0x18;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_CFG2; // 0x14
    wr_data[1] = 0x11;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_CFG3; // 0x15
    wr_data[1] = 0x03;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_CFG4; // 0x16
    wr_data[1] = 0x40;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_INT_CFG1; // 0x17
    wr_data[1] = 0x80;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_INT_CFG6_H; // 0x1C
    wr_data[1] = 0x15;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_INT_CFG6_L; // 0x1D
    wr_data[1] = 0x7C;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_INT_CFG7_H; // 0x1E
    wr_data[1] = 0x01;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_INT_CFG7_L; // 0x1F
    wr_data[1] = 0xF4;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_INT_CFG8; // 0x20
    wr_data[1] = 0x43;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_CTL1; // 0x22
    wr_data[1] = 0x04;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_CTL2; // 0x23
    wr_data[1] = 0x4B;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = SEQ_CTL1; // 0x24
    wr_data[1] = 0x08;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = SWG_C1; // 0x25
    wr_data[1] = 0x61;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = SWG_C2; // 0x26
    wr_data[1] = 0xB4;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = SWG_C3; // 0x27
    wr_data[1] = 0xEC;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }
    
    wr_data[0] = SEQ_CTL2; // 0x28
    wr_data[1] = 0x00;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }
    
    uint8_t regVal = _readRegister(SEQ_CTL2);

    wr_data[0] = GPI_0_CTL; // 0x29
    wr_data[1] = 0x01;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = GPI_1_CTL; // 0x2A
    wr_data[1] = 0x09;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = GPI_2_CTL; // 0x2B
    wr_data[1] = 0x00;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = MEM_CTL2; // 0x2D
    wr_data[1] = 0x80;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = FRQ_PHASE_H; // 0x48
    wr_data[1] = 0x21;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = FRQ_PHASE_L; // 0x49
    wr_data[1] = 0x02;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = FRQ_CTL; // 0x4C
    wr_data[1] = 0x00;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TRIM3; // 0x5F
    wr_data[1] = 0x0E;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TRIM4; // 0x60
    wr_data[1] = 0x9E;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TRIM6; // 0x62
    wr_data[1] = 0x6F;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = TOP_CFG5; // 0x6E
    wr_data[1] = 0x00;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = IRQ_EVENT_ACTUATOR_FAULT; // 0x81
    wr_data[1] = 0x04;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    wr_data[0] = IRQ_MASK2; // 0x83
    wr_data[1] = 0x80;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return ret;
    }

    setOperationMode(INACTIVE);  
    if( (_readRegister(MEM_CTL2) >> 7) == LOCKED )
    {
	_writeRegister(MEM_CTL2, 0x7F, UNLOCKED, 7);
    }

    for(int i = 0; i <= 99; i++)
    {
        wr_data[0] = SNP_MEM_X + i; // 0x84 + i
        wr_data[1] = 0;
        ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
	if(ret != NRF_SUCCESS)
        {
	    return ret;
        }
    }

    for(int i = 0; i <= 99; i++)
    {
        wr_data[0] = SNP_MEM_X + i; // 0x84 + i
        wr_data[1] = vib_pattern[i];
        ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
	if(ret != NRF_SUCCESS)
        {
	    return ret;
        }
    }
    _writeRegister(MEM_CTL2, 0x7F, LOCKED, 7);
    return ret;
}

// Address: 0x00 , bit[7:0]: default value is 0xBA. 
// Checks the WHOAMI value in the CHIP_REV register.
bool vib_begin( void )
{
  
  uint8_t chipRev = 0;

  uint8_t tempRegVal = _readRegister(CHIP_REV_REG); 
  chipRev |= tempRegVal << 8;
  chipRev |= tempRegVal; 

  if( chipRev != CHIP_REV )
    return false;
  else
    return true; 

}

// Address: 0x22, bits[2:0]
// Sets how the user will be operating the motor, which is one of four modes:
// PWM_MODE, DRO_MODE (I2C contorl), RTWM_MODE (Register-Triggered-Waveform-Memory 
// Mode), or ETWM_MODE (Edge-Triggered-Waveform-Memory Mode).
bool setOperationMode(uint8_t mode){ 

    if( mode < 0 || mode > 3) 
        return false;

    if( _writeRegister(TOP_CTL1, 0xF8, mode, 0) ) 
    {
	return true;
    }
    else{
        return false; 
    }
}

// Address: 0x22, bits[2:0]
// Returns one of the four operation modes.
// PWM_MODE, DRO_MODE (I2C contorl), RTWM_MODE (Register-Triggered-Waveform-Memory 
// Mode), or ETWM_MODE (Edge-Triggered-Waveform-Memory Mode).
uint8_t getOperationMode(){ 

  uint8_t mode = _readRegister(TOP_CTL1); 
  return (mode & 0x07);

}

// Address: 0x23, bit[7:0]
// Applies the argument "wave" to the register that controls the strength of
// the vibration. The function first checks if acceleration mode is enabled
// which limits the maximum value that can be written to the register.
bool setVibrate(uint8_t val){

  if ( val < 0 )
    return false; 

  uint8_t accelState = _readRegister(TOP_CFG1);
  accelState &= 0x04; 
  accelState = accelState >> 2;  

  if( accelState == ENABLE ){
    if( val >  0x7F ) 
      val = 0x7F; // Just limit the argument to the physical limit
  }
  else {
    if( val > 0xFF ) 
      val = 0xFF;// Just limit the argument to the physical limit
  }

  if( _writeRegister(TOP_CTL2, 0x00, val, 0) )
    return true;
  else
    return false; 
  
}

// Address: 0x23, bit[7:0]
// Reads the vibration value. This will have a result when the user writes to
// this register and also when a PWM_MODE is enabled and a duty cycle is
// applied to the GPI0/PWM pin. 
uint8_t getVibrate(){

  uint8_t vibVal = _readRegister(TOP_CTL2);
  return vibVal;
}

// Address: 0x07, bit[7:0]: default value is: 0x0
// Function sets the register to ignore the given "mask" i.e. irq event. 
bool setMask(uint8_t mask){
  
 if( _writeRegister(IRQ_MASK1, 0x00, mask, 0) ) 
    return true;
  else
    return false;
}

// Address: 0x07, bit[7:0]: default value is: 0x0
// Function returns the event ignoring register. 
uint8_t getMask(){

  uint8_t regVal = _readRegister(IRQ_MASK1);
  return regVal; 
}

bool playFromMemory(bool enable){
    
    if( _writeRegister(TOP_CTL1, 0x03, enable, 4) )
    {
	return true;
    }
    else {
        return false;
    }
}

// Address: 0x03, bit[7:0]
// This retrieves the interrupt value and returns the corresponding interrupt
// found or success otherwise. 
event_t getIrqEvent(){

  uint8_t irqEvent = _readRegister(IRQ_EVENT1); 

  if( !irqEvent)
    return HAPTIC_SUCCESS;

  switch( irqEvent ){
    case E_SEQ_CONTINUE:
        return E_SEQ_CONTINUE;
    case E_UVLO:
        return E_UVLO;
    case E_SEQ_DONE:
        return E_SEQ_DONE;
    case E_OVERTEMP_CRIT:
        return E_OVERTEMP_CRIT;
    case E_SEQ_FAULT:
        return E_SEQ_FAULT;
    case E_WARNING:
        return E_WARNING;
    case E_ACTUATOR_FAULT:
        return E_ACTUATOR_FAULT;
    case E_OC_FAULT:
        return E_OC_FAULT;
    default:
        return (event_t)irqEvent; //In the case that there are more than one.
  }
}

// Address: 0x05 , bit[7:5]
// Given an interrupt corresponding to an error with using memory or pwm mode,
// this returns further information on the error.
diag_status_t getEventDiag(){

  uint8_t diag = _readRegister(IRQ_EVENT_SEQ_DIAG);

  switch( diag ){
    case E_PWM_FAULT:
      return E_PWM_FAULT; 
    case E_MEM_FAULT:
      return E_MEM_FAULT; 
    case E_SEQ_ID_FAULT:
      return E_SEQ_ID_FAULT;
    default:
      return NO_DIAG;
  }
}

bool setSeqControl(uint8_t repetitions, uint8_t sequenceID)
{
    uint8_t wr_data[2] = {0};
    ret_code_t ret = NRF_SUCCESS;

    wr_data[0] = SEQ_CTL2; // 0x28
    wr_data[1] = repetitions << 4;
    wr_data[1] |= sequenceID;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return false;
    }
    return true;
}

// This generic function handles I2C write commands for modifying individual
// bits in an eight bit register. Paramaters include the register's address, a mask 
// for bits that are ignored, the bits to write, and the bits' starting
// position.
bool _writeRegister(uint8_t _wReg, uint8_t _mask, uint8_t _bits, uint8_t _startPosition)
{
    uint8_t wr_data[2] = {0};
    ret_code_t ret = NRF_SUCCESS;

    _mask |= (_bits << _startPosition);

    wr_data[0] = _wReg;
    wr_data[1] = _mask;
    ret = i2c_vib_write( DEF_ADDR, &wr_data[0], 2, 0 );
    if(ret != NRF_SUCCESS)
    {
	return false;
    }
    return true; 
}

// This generic function reads an eight bit register. It takes the register's
// address as its' parameter. 
uint8_t _readRegister(uint8_t _reg)
{
    uint8_t _regValue[2] = {0};
    i2c_vib_read(DEF_ADDR, _reg, &_regValue[0], 2, 0);

    return _regValue[0];
}
