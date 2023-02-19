#include "vib_ctrl.h"

    uint8_t snpMemCopy[100] = {0}; 
    uint8_t lastPosWritten = 0; 
//    uint8_t mode = DRO_MODE;

//    bool begin(TwoWire &wirePort = Wire); // begin function

    bool setActuatorType(uint8_t);
    bool setOperationMode(uint8_t mode); 
    uint8_t getOperationMode(); 

    bool defaultMotor();
    bool setMotor(hapticSettings userSettings);
    hapticSettings getSettings();
    bool setActuatorABSVolt(float);
    float getActuatorABSVolt();
    bool setActuatorNOMVolt(float);
    float getActuatorNOMVolt();
    bool setActuatorIMAX(float);
    uint16_t getActuatorIMAX();
    bool setActuatorImpedance(float);
    uint16_t getActuatorImpedance();
    uint16_t readImpAdjus();
    bool setActuatorLRAfreq(float);
    bool enableCoinERM();
    bool enableAcceleration(bool);
    bool enableRapidStop(bool);
    bool enableAmpPid(bool);
    bool enableFreqTrack(bool);
    bool setBemfFaultLimit(bool);
    bool enableV2iFactorFreeze(bool);
    bool calibrateImpedanceDistance(bool);
    bool setVibrate(uint8_t);
    uint8_t getVibrate();
    float getFullBrake();
    bool setMask(uint8_t);
    uint8_t getMask();
    bool setFullBrake(uint8_t);
    bool setBemf(uint8_t val);
    float getBemf();
    void createHeader(uint8_t, uint8_t);
    void clearIrq(uint8_t);
//    bool addSnippet(uint8_t ramp = RAMP, uint8_t amplitude = 2, uint8_t timeBase = 2);
    bool addSnippet(uint8_t ramp, uint8_t amplitude, uint8_t timeBase);
    bool addSnippet_temp(uint8_t snippets[], uint8_t);
    void eraseWaveformMemory(uint8_t);
    event_t getIrqEvent();
    diag_status_t getEventDiag();
    status_t getIrqStatus();
//    bool playFromMemory(bool enable = true);
    bool playFromMemory(bool enable);
    bool setSeqControl(uint8_t, uint8_t);
    uint8_t addFrame(uint8_t, uint8_t, uint8_t);

    hapticSettings sparkSettings;

    // Private Variables
    uint8_t _address;

    // This generic function handles I2C write commands for modifying individual
    // bits in an eight bit register. Paramaters include the register's address, a mask 
    // for bits that are ignored, the bits to write, and the bits' starting
    // position.
    bool _writeRegister(uint8_t, uint8_t, uint8_t, uint8_t);

    // Consecutive Write Mode: I2C_WR_MODE = 0
    // Allows for n-number of writes on consecutive registers, beginning at the
    // given register. 
    // This particular write does not care what is currently in the register and
    // overwrites whatever is there.
    bool _writeConsReg(uint8_t regs[], size_t);

    // Non-Consecutive Write Mode: I2C_WR_MODE = 1
    // Allows for n-number of writes on non-consecutive registers, beginning at the
    // given register but able to jump locations by giving another address. 
    // This particular write does not care what is currently in the register and
    // overwrites whatever is there.
    bool _writeNonConsReg(uint8_t regs[], size_t);

    // This generic function does a basic I-squared-C write transaction at the
    // given address, and writes the given _command argument. 
    void _writeCommand(uint8_t);

    bool _writeWaveFormMemory(uint8_t waveFormArray[]);

    // This generic function reads an eight bit register. It takes the register's
    // address as its' parameter. 
    uint8_t _readRegister(uint8_t);

    bool _readConsReg(uint8_t regs[], size_t);
    bool _readNonConsReg(uint8_t regs[], size_t);
    // This generic function does a basic I-squared-C read transaction at the given
    // addres, taking the number of reads as argument. 
    uint8_t _readCommand(uint8_t);

ret_code_t vib_init(void)
{
    ret_code_t ret = NRF_SUCCESS;

    uint8_t wr_data[2] = {0};

    wr_data[0] = IRQ_EVENT1;	// 0x03
    wr_data[1] = 0x04;
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
}



// Address: 0x00 , bit[7:0]: default value is 0xBA. 
// Checks the WHOAMI value in the CHIP_REV register.
/*
bool Haptic_Driver::begin( TwoWire &wirePort )
{
  
  delay(2);
  _i2cPort = &wirePort;
  uint8_t chipRev;

  uint8_t tempRegVal = _readRegister(CHIP_REV_REG); 
  chipRev |= tempRegVal << 8;
  chipRev |= tempRegVal; 

  if( chipRev != CHIP_REV )
    return false;
  else
    return true; 

}
*/
// Address: 0x13 , bit[5]: default value is 0x00. 
// This sets the "actuator" (motor) type, which is either an LRA_MOTOR
// or is an ERM_MOTOR. 
bool setActuatorType(uint8_t actuator){
  
  if( actuator != LRA_TYPE && actuator != ERM_TYPE )
    return false; 

   if( _writeRegister(TOP_CFG1, 0xDF, actuator, 5) )
    return true; 
  else
    return false; 
}

// Address: 0x22, bits[2:0]
// Sets how the user will be operating the motor, which is one of four modes:
// PWM_MODE, DRO_MODE (I2C contorl), RTWM_MODE (Register-Triggered-Waveform-Memory 
// Mode), or ETWM_MODE (Edge-Triggered-Waveform-Memory Mode).
bool setOperationMode(uint8_t mode){ 

  if( mode < 0 || mode > 3) 
    return false;

  if( _writeRegister(TOP_CTL1, 0xF8, mode, 0) ) 
    return true; 
  else
    return false; 

  delay(1);

}

// Address: 0x22, bits[2:0]
// Returns one of the four operation modes.
// PWM_MODE, DRO_MODE (I2C contorl), RTWM_MODE (Register-Triggered-Waveform-Memory 
// Mode), or ETWM_MODE (Edge-Triggered-Waveform-Memory Mode).
uint8_t getOperationMode(){ 

  uint8_t mode = _readRegister(TOP_CTL1); 
  return (mode & 0x07);

}

// This function calls a number of other functions to set the following
// electrical characteristics of the motor that comes with the SparkFun Haptic
// Motor Driver. This can be set individually by the user, using the individual
// function calls - see datasheet on the motor being used. 
bool defaultMotor(){


  sparkSettings.motorType = LRA_TYPE;
  sparkSettings.nomVolt = 0; //volts - set to 2.5 when acceleration is ON
  sparkSettings.absVolt = 2.26; // volts - when acceleration is off, set this only
  sparkSettings.currMax = 165.4; // milliamps - 
  sparkSettings.impedance = 13.8; // ohms
  sparkSettings.lraFreq = 170; // hertz

  if( setActuatorType(sparkSettings.motorType) && 
      setActuatorABSVolt(sparkSettings.absVolt) &&
      setActuatorNOMVolt(sparkSettings.nomVolt) &&
      setActuatorIMAX(sparkSettings.currMax) &&
      setActuatorImpedance(sparkSettings.impedance) &&
      setActuatorLRAfreq(sparkSettings.lraFreq) )
    return true;
  else
    return false; 
}

// This function returns a struct of the motor's settings set by the user.
hapticSettings getSettings(){
  
  hapticSettings temp; 
  uint16_t v2i_factor;
  temp.nomVolt =  _readRegister(ACTUATOR1) * (23.4 * pow(10, -3)); 
  temp.absVolt =  _readRegister(ACTUATOR2) * (23.4 * pow(10, -3)); 
  temp.currMax =  (_readRegister(ACTUATOR3) * 7.2) + 28.6;
  v2i_factor = (_readRegister(CALIB_V2I_H) << 8) | _readRegister(CALIB_IMP_L);
  temp.impedance = (v2i_factor * 1.6104) / (_readRegister(ACTUATOR3) + 4);
  return temp; 

}
 


// This function takes a hapticSettings type and calls the respective function
// to set the various motor characteristics. 
bool setMotor(hapticSettings userSettings){

  if( setActuatorType(userSettings.motorType) && 
      setActuatorABSVolt(userSettings.absVolt) &&
      setActuatorNOMVolt(userSettings.nomVolt) &&
      setActuatorIMAX(userSettings.currMax) &&
      setActuatorImpedance(userSettings.impedance) &&
      setActuatorLRAfreq(userSettings.lraFreq) )
    return true;
  else
    return false; 
}

// Address: 0x0D , bit[7:0]: default value is: 0x78 (2.808 Volts)
// Function takes the absolute maximum voltage of the motor intended to
// be paired with the motor driver IC. Argument is of float type, and in volts.
bool setActuatorABSVolt(float absVolt){

  if( absVolt < 0 || absVolt > 6.0)
    return false; 
  
  absVolt = absVolt/(23.4 * pow(10,-3)); 

  if( _writeRegister(ACTUATOR2, 0x00, (uint8_t)absVolt, 0) )
    return true;
  else
    return false; 
  
}

// Address: 0x0D , bit[7:0]: default value is: 0x78 (2.808 Volts)
// Function returns the value in the register below.
float getActuatorABSVolt(){
  
  uint8_t regVal = _readRegister(ACTUATOR2);

  return( regVal * (23.4 * pow(10, -3)) );
}

// Address: 0x0C , bit[7:0]: default value is: 0x5A (2.106 Volts)
// Function takes the nominal voltage range of the motor intended to
// be paired with the motor driver IC. Argument is of float type, and in volts.
bool setActuatorNOMVolt(float rmsVolt){

  if( rmsVolt < 0 || rmsVolt > 3.3 )
    return false; 

  rmsVolt = rmsVolt/(23.4 * pow(10,-3)); 
    
  if( _writeRegister(ACTUATOR1 , 0x00, (uint8_t)rmsVolt, 0 ) )
    return true;
  else
    return false; 
  
}

// Address: 0x0C , bit[7:0]: default value is: 0x5A (2.106 Volts)
// Function returns the value in the register below.
float getActuatorNOMVolt(){
  
  uint8_t regVal = _readRegister(ACTUATOR1);

  return( regVal * (23.4 * pow(10, -3)) );

}

// Address: 0x0E , bit[4:0]: default value is: 0x17 (198mA)
// Function takes the max current rating of the motor intended to
// be paired with the motor driver IC. Argument is of float type, and in
// milliamps.
bool setActuatorIMAX(float maxCurr){

  if( maxCurr < 0 || maxCurr > 300.0) // Random upper limit 
    return false; 
  
  maxCurr = (maxCurr - 28.6)/7.2;

  if( _writeRegister(ACTUATOR3, 0xE0, (uint8_t)maxCurr, 0) )
    return true;
  else
    return false; 
  
}

// Address: 0x0E , bit[4:0]: default value is: 0x17 (198mA)
// Function returns the value in the register below.
uint16_t getActuatorIMAX(){

  uint8_t regVal = _readRegister(ACTUATOR3);
  regVal &= 0x1F;

  return (regVal * 7.2) + 28.6; 

}

// Address: 0x0F and 0x10 , bits[7:0]: default value is: 0x01 and 0x0D
// respectively.
// Function takes the impedance of the motor intended to
// be paired with the motor driver IC.The value is dependent on the max current
// set in 0x0E (ACTUATOR3) so be sure to set that first. Argument is of float type, and in
// ohms. 
bool setActuatorImpedance(float motorImpedance){

  if( motorImpedance < 0 || motorImpedance > 50.0) 
    return false; 

  uint8_t msbImpedance; 
  uint8_t lsbImpedance;
  uint16_t v2iFactor;
  uint8_t maxCurr = _readRegister(ACTUATOR3) | 0x1F;

  v2iFactor = (motorImpedance * (maxCurr + 4))/1.6104;
  msbImpedance = (v2iFactor - (v2iFactor & 0x00FF))/256;
  lsbImpedance = (v2iFactor - (256 * (v2iFactor & 0x00FF)));
  
  if( _writeRegister(CALIB_V2I_L, 0x00, lsbImpedance, 0) &&\
      _writeRegister(CALIB_V2I_H, 0x00, msbImpedance, 0) )
    return true;
  else
    return false; 
  
}

// Address: 0x0F and 0x10 , bits[7:0]: default value is: 0x01 and 0x0D
// Function returns the value in the register below.
uint16_t getActuatorImpedance(){

  uint16_t regValMSB = _readRegister(CALIB_V2I_H);
  uint8_t regValLSB  = _readRegister(CALIB_V2I_L);
  uint8_t currVal    = _readRegister(ACTUATOR3) & 0x1F;

  uint16_t v2iFactor = (regValMSB << 8) | regValLSB; 

  return (v2iFactor * 1.6104)/(currVal + 4);
  
}

// Address: 0x0F and 0x10 , bits[7:0]: default value is: 0x01 and 0x0D
// respectively.
// Function takes the impedance of the motor intended to
// be paired with the motor driver IC.The value is dependent on the max current
// set in 0x0E (ACTUATOR3) so be sure to set that first. Argument is of float type, and in
// ohms. 
bool setActuatorLRAfreq(float frequency){

  if( frequency < 0 || frequency > 500.0 )
    return false; 
  
  uint8_t msbFrequency;
  uint8_t lsbFrequency;
  uint16_t lraPeriod;

  lraPeriod = 1/(frequency * (1333.32 * pow(10, -9)));
  msbFrequency = (lraPeriod - (lraPeriod & 0x007F))/128;
  lsbFrequency = (lraPeriod - 128 * (lraPeriod & 0xFF00));

  if( _writeRegister(FRQ_LRA_PER_H, 0x00, msbFrequency, 0) && 
      _writeRegister(FRQ_LRA_PER_L, 0x80, lsbFrequency, 0) ){
    return true;
  }
  else
    return false; 
  
}

// Address: 0x11 and 0x12, bits[7:0]: default value is 0x00 for both (22 Ohms)
// This function returns the adjusted impedance of the motor, calculated by the
// IC on playback. This is meant to adjust for the motor's variation in
// impedance given manufacturing tolerances. Value is in Ohms. 
uint16_t readImpAdjus(){

   uint8_t tempMSB =  _readRegister(CALIB_IMP_H);
   uint8_t tempLSB =  _readRegister(CALIB_IMP_L);

   uint16_t totalImp = (4 * 62.5 * pow(10, -3) * tempMSB) + (62.5 * pow(10, -3) * tempLSB);
   return totalImp; 
}

// This function sets the various settings that allow for ERM vibration motor 
// operation by calling the relevant individual functions. 
bool enableCoinERM(){

  if( enableAcceleration(false) &&
      enableRapidStop(false) &&
      enableAmpPid(false) && 
      enableV2iFactorFreeze(true) &&
      calibrateImpedanceDistance(true) &&
      setBemfFaultLimit(true) ) 
    return true;
  else
    return false; 
  
}

// Address: 0x13, bit[2]: default value is 0x1 
// Enables or disables active acceleration.
bool enableAcceleration(bool enable){

  if( _writeRegister(TOP_CFG1, 0xFB, enable, 2) )
    return true;
  else
    return false; 
}


// Address: 0x13, bit[1]: default value is 0x1 
// Enables or disables the "rapid stop" technology.
bool enableRapidStop(bool enable){

  if( _writeRegister(TOP_CFG1, 0xFD, enable, 1) )
    return true;
  else
    return false; 
}

// Address: 0x13, bit[0]: default value is 0x0 
// Enables or disables the "amplitude PID" technology.
bool enableAmpPid(bool enable){

  if( _writeRegister(TOP_CFG1, 0xFE, enable, 0) )
    return true;
  else
    return false; 
}

// Address: 0x13, bit[0]: default value is 0x0 
// Enables or disables the "frequency tracking" technology.
bool enableFreqTrack(bool enable){

  if( _writeRegister(TOP_CFG1, 0xF7, enable, 3) )
    return true;
  else
    return false; 
}


// Address: 0x13, bit[0]: default value is 0x1 
// Enables or disables internal loop computations, which should only be
// disabled when using custom waveform or wideband operation.
bool setBemfFaultLimit(bool enable){

  if( _writeRegister(TOP_CFG1, 0xEF, enable, 4) )
    return true;
  else
    return false; 
}

// Address: 0x16, bit[7]: default value is 0x0 
// Enables or disables internal loop computations, which should only be
// disabled when using custom waveform or wideband operation.
bool enableV2iFactorFreeze(bool enable){

  if( _writeRegister(TOP_CFG4, 0x7F, enable, 7) )
    return true;
  else
    return false; 
}

// Address: 0x16 , bit[6]: default value is: 0x1 (disabled)
// Enables or disables automatic updates to impedance value. 
bool calibrateImpedanceDistance(bool enable){

  if( _writeRegister(TOP_CFG4, 0xBF, enable, 6) )
    return true;
  else
    return false; 
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

float getFullBrake(){

  uint8_t tempThresh = _readRegister(TOP_CFG2);
  return (tempThresh & 0x0F) * 6.66;
}

bool setFullBrake(uint8_t thresh){

  if( thresh < 0 || thresh > 15)
    return false;

  if( _writeRegister(TOP_CFG2, 0xF0, thresh, 0) )
    return true; 
  else
    return false;
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

// Address: 0x17, bit[1:0]: default value is: 0x01 (4.9mV)
// Limits the voltage that triggers a BEMF fault (E_ACTUATOR_FAULT)
// A value of zero disable tracking.
bool setBemf(uint8_t val){
  
  if (val < 0 || val > 3)
    return false; 

  if( _writeRegister(TOP_INT_CFG1, 0xFC, val, 0) )
    return true; 
  else
    return false;
}

// Address: 0x17, bit[1:0]: default value is: 0x01 (4.9mV)
// Returns the BEMF value in millivolts (mV).
float getBemf(){
  
  int bemf = _readRegister(TOP_INT_CFG1);

  switch( bemf ){
    case 0x00:
      return 0.0;
    case 0x01:
      return 4.9;
    case 0x02:
      return 27.9;
    case 0x03:
      return 49.9;
  }
}

void createHeader(uint8_t numSnippets, uint8_t numSequences){
}

void clearIrq(uint8_t irq){
  _writeRegister(IRQ_EVENT1, ~irq, irq, 0);
}


bool addSnippet(uint8_t ramp, uint8_t timeBase, uint8_t amplitude){
 
  if( ramp < 0 | ramp > 1) 
    return false;

  if( amplitude < 0 | amplitude > 15 )
    return false; 

  if( timeBase < 0 | timeBase > 7) 
    return false; 

  setOperationMode(INACTIVE);  

  if( (_readRegister(MEM_CTL2) >> 7) == LOCKED )
    _writeRegister(MEM_CTL2, 0x7F, UNLOCKED, 7);
  
  
  uint8_t pwlVal = (ramp << 7) | (timeBase << 4) | (amplitude << 0);  
  uint8_t snipAddrLoc = _readRegister(MEM_CTL1); 

  snpMemCopy[NUM_SNIPPETS] = snpMemCopy[NUM_SNIPPETS] + 1; // Number of Snippets
  snpMemCopy[NUM_SEQUENCES] = snpMemCopy[NUM_SEQUENCES] + 1; // Number of sequences

  uint8_t frameByte = addFrame(0, 3, 1);

  for(uint8_t i = 0; i < snpMemCopy[NUM_SNIPPETS]; i++){
    snpMemCopy[SNP_ENDPOINTERS + i] = SNP_ENDPOINTERS_REGS + i;  //snippet endpointer
    lastPosWritten = SNP_ENDPOINTERS + i;
  }

  lastPosWritten = lastPosWritten + 1; 

  for(uint8_t i = 0; i < snpMemCopy[NUM_SEQUENCES]; i++){
    snpMemCopy[lastPosWritten] = SNP_ENDPOINTERS_REGS + snpMemCopy[NUM_SNIPPETS] + i; //sequence endpointer 
    lastPosWritten = SNP_ENDPOINTERS + snpMemCopy[NUM_SNIPPETS] + i;
  }
  
  lastPosWritten = lastPosWritten + 1; 
  snpMemCopy[lastPosWritten] = pwlVal; // Write snippet
  lastPosWritten = lastPosWritten + 1; 
  snpMemCopy[lastPosWritten] = frameByte; 
  

  setSeqControl(1, 0);

  if( _writeWaveFormMemory(snpMemCopy) ) 
    return true; 
  else
    return false; 
}

bool addSnippet_temp(uint8_t snippets[], uint8_t numOfSnippets){
  return true; 
}

uint8_t addFrame(uint8_t gain, uint8_t timeBase, uint8_t snipIdLow) {

  uint8_t commandByteZero = 0;  //Command byte zero is mandatory, snip-id begins at one
  commandByteZero = (gain << 5) | (timeBase << 3) | (snipIdLow << 0); 
  return commandByteZero; 

}

bool playFromMemory(bool enable){

 if( _writeRegister(TOP_CTL1, 0xEF, enable, 4) )
   return true;
 else 
   return false;
  
}

void eraseWaveformMemory(uint8_t mode){
  

  for( uint8_t i; i = BEGIN_SNP_MEM; i < TOTAL_MEM_REGISTERS ){
    snpMemCopy[i] = 0;
  } 
  _writeWaveFormMemory(snpMemCopy);
  
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

// Address: 0x06 , bit[7:0]
// Retu
status_t getIrqStatus(){

  uint8_t status = _readRegister(IRQ_STATUS1);

  switch( status ){
    case STA_SEQ_CONTINUE:
      return STA_SEQ_CONTINUE; 
    case STA_UVLO_VBAT_OK:
      return STA_UVLO_VBAT_OK; 
    case STA_PAT_DONE:
      return STA_PAT_DONE;
    case STA_OVERTEMP_CRIT:
      return STA_OVERTEMP_CRIT;
    case STA_PAT_FAULT:
      return STA_PAT_FAULT;
    case STA_WARNING:
      return STA_WARNING;
    case STA_ACTUATOR:
      return STA_ACTUATOR;
    case STA_OC:
      return STA_OC;
    default:
      return STATUS_NOM;
  }

}

bool setSeqControl(uint8_t repetitions, uint8_t sequenceID){


  if( sequenceID < 0 | sequenceID > 15 )
    return false; 

  if( repetitions < 0 | repetitions > 15 )
    return false; 

  if( _writeRegister(SEQ_CTL2, 0xF0, sequenceID, 0) &&
      _writeRegister(SEQ_CTL2, 0x0F, repetitions, 4) )
    return true;
  else
    return false; 
  
}

// This generic function handles I2C write commands for modifying individual
// bits in an eight bit register. Paramaters include the register's address, a mask 
// for bits that are ignored, the bits to write, and the bits' starting
// position.
bool _writeRegister(uint8_t _wReg, uint8_t _mask, uint8_t _bits, uint8_t _startPosition)
{
    return true; 
}

// This generic function reads an eight bit register. It takes the register's
// address as its' parameter. 
uint8_t _readRegister(uint8_t _reg)
{
    uint8_t _regValue;
    return _regValue;
}

// Consecutive Write Mode: I2C_WR_MODE = 0
// Allows for n-number of writes on consecutive registers, beginning at the
// given register. 
// This particular write does not care what is currently in the register and
// overwrites whatever is there.
bool _writeConsReg(uint8_t regs[], size_t numWrites)
{
/*
  _writeRegister(CIF_I2C1, 0x7F, 0, 7);      

  _i2cPort->beginTransmission(_address);

  for( size_t i = 0; i <= numWrites; i++){
      _i2cPort->write(regs[i]);
  }

  if(!_i2cPort->endTransmission())
    return true;
  else
*/
    return false;
}

// Non-Consecutive Write Mode: I2C_WR_MODE = 1
// Allows for n-number of writes on non-consecutive registers, beginning at the
// given register but able to jump locations by giving another address. 
// This particular write does not care what is currently in the register and
// overwrites whatever is there.
bool _writeNonConsReg(uint8_t regs[], size_t numWrites)
{
  _writeRegister(CIF_I2C1, 0x7F, 1, 7);      
/*
  _i2cPort->beginTransmission(_address); // Start communication.
  for( size_t i = 0; i <= numWrites; i++){
    // Here's to hoping that the register pointer will indeed jump locations as
    // advertised.
    _i2cPort->write(regs[i]);
  }

  if(!_i2cPort->endTransmission())
    return true;
  else
*/
    return false;
}

bool _writeWaveFormMemory(uint8_t waveFormArray[] )
{
/*
  _i2cPort->beginTransmission(_address); // Start communication.
  _i2cPort->write(NUM_SNIPPETS_REG); // Moves pointer to register.
  for( size_t i = BEGIN_SNP_MEM; i < TOTAL_MEM_REGISTERS; i++){
    _i2cPort->write(waveFormArray[i]);
  }

  if(!_i2cPort->endTransmission())
    return true;
  else
*/
    return false; 
}