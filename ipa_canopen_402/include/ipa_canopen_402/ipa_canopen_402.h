#ifndef H_IPA_CANOPEN_402
#define H_IPA_CANOPEN_402

#include <ipa_canopen_master/canopen.h>

namespace ipa_canopen
{
class Node_402 : public ipa_canopen::Layer
{
public:
  Node_402(boost::shared_ptr <ipa_canopen::Node> n, const std::string &name) : Layer(name), n_(n)
  {
    configureEntries();
    status_word_mask.set(SW_Ready_To_Switch_On);
    status_word_mask.set(SW_Switched_On);
    status_word_mask.set(SW_Operation_enabled);
    status_word_mask.set(SW_Fault);
    status_word_mask.reset(SW_Voltage_enabled);
    status_word_mask.set(SW_Quick_stop);
    status_word_mask.set(Switch_On_Disabled);

    homing_mask.set(SW_Target_reached);
    homing_mask.set(SW_Operation_specific0);
    homing_mask.set(SW_Operation_specific0);

    homing_needed_ = false;
    motor_ready_ = false;
  }

  enum StatusWord
  {
    SW_Ready_To_Switch_On,
    SW_Switched_On,
    SW_Operation_enabled,
    SW_Fault,
    SW_Voltage_enabled,
    SW_Quick_stop,
    SW_Switch_on_disabled,
    SW_Warning,
    SW_Manufacturer_specific0,
    SW_Remote,
    SW_Target_reached,
    SW_Internal_limit,
    SW_Operation_specific0,
    SW_Operation_specific1,
    SW_Manufacturer_specific1,
    SW_Manufacturer_specific2
  };

  enum ControlWord
  {
    CW_Switch_On,
    CW_Enable_Voltage,
    CW_Quick_Stop,
    CW_Enable_Operation,
    CW_Operation_mode_specific0,
    CW_Operation_mode_specific1,
    CW_Operation_mode_specific2,
    CW_Fault_Reset,
    CW_Halt,
    CW_Reserved0,
    CW_Reserved1,
    CW_Manufacturer_specific0,
    CW_Manufacturer_specific1,
    CW_Manufacturer_specific2,
    CW_Manufacturer_specific3,
    CW_Manufacturer_specific4,
  };

  enum OperationMode
  {
    No_Mode = 0,
    Profiled_Position = 1,
    Velocity = 2,
    Profiled_Velocity = 3,
    Profiled_Torque = 4,
    Reserved=5,
    Homing = 6,
    Interpolated_Position = 7,
    Cyclic_Synchronous_Position = 8,
    Cyclic_Synchronous_Velocity = 9,
    Cyclic_Synchronous_Torque = 10,
  };

  enum SupportedOperationMode
  {
    Sup_Profiled_Position = 0,
    Sup_Velocity = 1,
    Sup_Profiled_Velocity = 2,
    Sup_Profiled_Torque = 3,
    Sup_Reserved = 4,
    Sup_Homing = 5,
    Sup_Interpolated_Position = 6,
    Sup_Cyclic_Synchronous_Position = 7,
    Sup_Cyclic_Synchronous_Velocity = 8,
    Sup_Cyclic_Synchronous_Torque = 9
  };

  enum State
  {
    Start=0,
    Not_Ready_To_Switch_On=1,
    Switch_On_Disabled=2,
    Ready_To_Switch_On=3,
    Switched_On=4,
    Operation_Enable=5,
    Quick_Stop_Active=6,
    Fault_Reaction_Active=7,
    Fault=8,
  };

  const OperationMode getMode();
  bool enterMode(const OperationMode &op_mode);
  bool isModeSupported(const OperationMode &op_mode);
  static uint32_t getModeMask(const OperationMode &op_mode);
  bool isModeMaskRunning(const uint32_t &mask);

  const State& getState();
  void enterState(const State &s);


  virtual void read(LayerStatus &status);
  virtual void pending(LayerStatus &status);
  virtual void write(LayerStatus &status);

  virtual void diag(LayerReport &report);

  virtual void init(LayerStatus &status);
  virtual void shutdown(LayerStatus &status);

  void getDeviceState();
  void switchMode(LayerStatus &status);

  void motorShutdown();
  void motorSwitchOn();
  void motorSwitchOnandEnableOp();
  void motorDisableVoltage();
  void motorQuickStop();
  void motorDisableOp();
  void motorEnableOp();
  void motorFaultReset();

  virtual void halt(LayerStatus &status);
  virtual void recover(LayerStatus &status);

  const double getActualPos();
  const double getActualInternalPos();

  const double getActualVel();
  const double getActualEff();

  void setTargetPos(const double &target_pos);
  void setTargetVel(const double &target_vel);
  void setTargetEff(const double &v) {} //TODO

  const double getTargetPos();
  const double getTargetVel();
  const double getTargetEff()  { return 0; } // TODO

  bool turnOn();
  bool turnOff();

  void configureEntries();
  void configureModeSpecificEntries();

private:
  boost::shared_ptr <ipa_canopen::Node> n_;
  volatile bool running;
  State state_;
  State target_state_;

  bool new_target_pos_;

  bool motor_ready_;
  bool homing_needed_;

  boost::mutex cond_mutex;
  boost::condition_variable cond;

  ipa_canopen::ObjectStorage::Entry<ipa_canopen::ObjectStorage::DataType<0x006>::type >  status_word;
  ipa_canopen::ObjectStorage::Entry<ipa_canopen::ObjectStorage::DataType<0x006>::type >  control_word;
  ipa_canopen::ObjectStorage::Entry<int8_t>  op_mode_display;
  ipa_canopen::ObjectStorage::Entry<int8_t>  op_mode;
  ipa_canopen::ObjectStorage::Entry<int16_t>  ip_mode_sub_mode;
  ipa_canopen::ObjectStorage::Entry<uint32_t>  supported_drive_modes;

  ipa_canopen::ObjectStorage::Entry<int8_t>  homing_method;

  ipa_canopen::ObjectStorage::Entry<int32_t> actual_vel;
  ipa_canopen::ObjectStorage::Entry<int16_t> target_velocity;
  ipa_canopen::ObjectStorage::Entry<uint32_t> profile_velocity;
  ipa_canopen::ObjectStorage::Entry<int32_t> actual_pos;
  ipa_canopen::ObjectStorage::Entry<int32_t> actual_internal_pos;
  ipa_canopen::ObjectStorage::Entry<int32_t> target_position;
  ipa_canopen::ObjectStorage::Entry<int32_t> target_interpolated_position;
  ipa_canopen::ObjectStorage::Entry<int32_t> target_interpolated_velocity;
  ipa_canopen::ObjectStorage::Entry<int32_t> target_profiled_velocity;


  double ac_vel_;
  double ac_eff_;

  OperationMode operation_mode_;
  OperationMode operation_mode_to_set_;
  bool check_mode;

  double ac_pos_;
  double internal_pos_;
  double oldpos_;

  std::bitset<16> status_word_bitset;
  std::bitset<16> control_word_bitset;
  std::bitset<16> status_word_mask;
  std::bitset<16> homing_mask;

  double target_vel_;
  double target_pos_;

  std::vector<int> control_word_buffer;

};
}  //  namespace ipa_canopen
#endif  // H_IPA_CANOPEN_402
