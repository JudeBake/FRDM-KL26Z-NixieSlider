/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v1.0
processor: MKL26Z128xxx4
package_id: MKL26Z128VLH4
mcu_data: ksdk2_0
processor_version: 3.0.1
board: FRDM-KL26Z
functionalGroups:
- name: BOARD_InitPeripherals
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system_54b53072540eeeb8f8e9343e71f28176'
- global_system_definitions: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "peripherals.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * PWM initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'PWM'
- type: 'tpm'
- mode: 'EdgeAligned'
- type_id: 'tpm_e7472ea12d53461b8d293488f3ed72ec'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'TPM0'
- config_sets:
  - tpm_main_config:
    - tpm_config:
      - clockSource: 'kTPM_SystemClock'
      - tpmSrcClkFreq: '8000000'
      - prescale: 'kTPM_Prescale_Divide_1'
      - timerFrequency: '200'
      - useGlobalTimeBase: 'false'
      - triggerSelect: 'kTPM_Trigger_Select_0'
      - enableDoze: 'false'
      - enableDebugMode: 'false'
      - enableReloadOnTrigger: 'false'
      - enableStopOnOverflow: 'false'
      - enableStartOnTrigger: 'false'
    - timer_interrupts: ''
    - enable_irq: 'false'
    - tpm_interrupt:
      - IRQn: 'TPM0_IRQn'
      - enable_priority: 'false'
      - enable_custom_name: 'false'
    - EnableTimerInInit: 'true'
  - tpm_edge_aligned_mode:
    - tpm_edge_aligned_channels_config:
      - 0:
        - edge_aligned_mode: 'kTPM_EdgeAlignedPwm'
        - edge_aligned_pwm:
          - chnlNumber: 'kTPM_Chnl_0'
          - level: 'kTPM_HighTrue'
          - dutyCyclePercent: '50'
          - enable_chan_irq: 'false'
      - 1:
        - edge_aligned_mode: 'kTPM_EdgeAlignedPwm'
        - edge_aligned_pwm:
          - chnlNumber: 'kTPM_Chnl_2'
          - level: 'kTPM_LowTrue'
          - dutyCyclePercent: '50'
          - enable_chan_irq: 'false'
      - 2:
        - edge_aligned_mode: 'kTPM_EdgeAlignedPwm'
        - edge_aligned_pwm:
          - chnlNumber: 'kTPM_Chnl_4'
          - level: 'kTPM_LowTrue'
          - dutyCyclePercent: '50'
          - enable_chan_irq: 'false'
      - 3:
        - edge_aligned_mode: 'kTPM_EdgeAlignedPwm'
        - edge_aligned_pwm:
          - chnlNumber: 'kTPM_Chnl_5'
          - level: 'kTPM_LowTrue'
          - dutyCyclePercent: '50'
          - enable_chan_irq: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const tpm_config_t PWM_config = {
  .prescale = kTPM_Prescale_Divide_1,
  .useGlobalTimeBase = false,
  .triggerSelect = kTPM_Trigger_Select_0,
  .enableDoze = false,
  .enableDebugMode = false,
  .enableReloadOnTrigger = false,
  .enableStopOnOverflow = false,
  .enableStartOnTrigger = false,
};

const tpm_chnl_pwm_signal_param_t PWM_pwmSignalParams[] = { 
  {
    .chnlNumber = kTPM_Chnl_0,
    .level = kTPM_HighTrue,
    .dutyCyclePercent = 50
  },
  {
    .chnlNumber = kTPM_Chnl_2,
    .level = kTPM_LowTrue,
    .dutyCyclePercent = 50
  },
  {
    .chnlNumber = kTPM_Chnl_4,
    .level = kTPM_LowTrue,
    .dutyCyclePercent = 50
  },
  {
    .chnlNumber = kTPM_Chnl_5,
    .level = kTPM_LowTrue,
    .dutyCyclePercent = 50
  }
};

void PWM_init(void) {
  TPM_Init(PWM_PERIPHERAL, &PWM_config);
  TPM_SetupPwm(PWM_PERIPHERAL, PWM_pwmSignalParams, sizeof(PWM_pwmSignalParams) / sizeof(tpm_chnl_pwm_signal_param_t), kTPM_EdgeAlignedPwm, 200U, PWM_CLOCK_SOURCE);
  TPM_StartTimer(PWM_PERIPHERAL, kTPM_SystemClock);
}

/***********************************************************************************************************************
 * MODE initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'MODE'
- type: 'gpio'
- mode: 'GPIO'
- type_id: 'gpio_f970a92e447fa4793838db25a2947ed7'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'GPIOC'
- config_sets:
  - fsl_gpio:
    - enable_irq: 'true'
    - port_interrupt:
      - IRQn: 'PORTC_PORTD_IRQn'
      - enable_priority: 'false'
      - enable_custom_name: 'false'
    - gpio_config:
      - 0:
        - signal_number: 'GPIO.5'
        - pinDirection: 'kGPIO_DigitalInput'
        - interrupt_configuration: 'kPORT_InterruptOrDMADisabled'
        - outputLogic: '0U'
      - 1:
        - signal_number: 'GPIO.3'
        - pinDirection: 'kGPIO_DigitalInput'
        - interrupt_configuration: 'kPORT_InterruptFallingEdge'
        - outputLogic: '0U'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
gpio_pin_config_t MODE_config[2] = {
  {
    .pinDirection = kGPIO_DigitalInput,
    .outputLogic = 0U
  },
  {
    .pinDirection = kGPIO_DigitalInput,
    .outputLogic = 0U
  }
};

void MODE_init(void) {
  /* Make sure, the clock gate for port C is enabled (e. g. in pin_mux.c) */
  /* Configure interrupt from pin PTC3 */
  PORT_SetPinInterruptConfig(MODE_PORT, 3U, kPORT_InterruptFallingEdge);
  /* Enable interrupt PORTC_PORTD_IRQn request in the NVIC */
  EnableIRQ(PORTC_PORTD_IRQn);
  /* Initialize GPIO functionality on pin PTC5 */
  GPIO_PinInit(MODE_GPIO, 5U, &MODE_config[0]);
  /* Initialize GPIO functionality on pin PTC3 */
  GPIO_PinInit(MODE_GPIO, 3U, &MODE_config[1]);
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void)
{
  /* Initialize components */
  PWM_init();
  MODE_init();
}

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void)
{
  BOARD_InitPeripherals();
}
