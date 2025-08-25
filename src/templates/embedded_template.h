#pragma once
#include "template_base.h"

class EmbeddedTemplate : public TemplateBase {
public:
  EmbeddedTemplate(const CliOptions &options);

  bool create() override;

protected:
  bool createProjectStructure() override;
  bool createBuildSystem() override;
  bool setupPackageManager() override;
  bool setupTestFramework() override;
  bool setupHardwareAbstraction();
  bool setupDrivers();
  bool setupRTOS();
  bool setupDebugging();

private:
  // Core application files
  std::string getMainCppContent();
  std::string getMainHeaderContent();
  std::string getConfigHeaderContent();
  std::string getConfigCppContent();

  // Hardware abstraction layer
  std::string getHalHeaderContent();
  std::string getHalCppContent();
  std::string getGpioHeaderContent();
  std::string getGpioCppContent();
  std::string getUartHeaderContent();
  std::string getUartCppContent();
  std::string getSpiHeaderContent();
  std::string getSpiCppContent();
  std::string getI2cHeaderContent();
  std::string getI2cCppContent();

  // Drivers
  std::string getDriversHeaderContent();
  std::string getDriversCppContent();

  // RTOS related
  std::string getRtosHeaderContent();
  std::string getRtosCppContent();
  std::string getTasksHeaderContent();
  std::string getTasksCppContent();

  // Utilities
  std::string getUtilsHeaderContent();
  std::string getUtilsCppContent();
  std::string getLoggerHeaderContent();
  std::string getLoggerCppContent();

  // Build system files
  std::string getReadmeContent();
  std::string getCMakeContent();
  std::string getMakefileContent();
  std::string getPlatformIOContent();
  std::string getArduinoContent();

  // Configuration files
  std::string getLinkerScriptContent();
  std::string getStartupCodeContent();
  std::string getOpenOCDConfigContent();
  std::string getGdbInitContent();

  // Testing
  std::string getUnityTestContent();
  std::string getHardwareTestContent();

  // Documentation
  std::string getHardwareDocsContent();
  std::string getPinoutContent();

  // Helper methods
  std::string getTargetMCU() const;
  std::string getTargetBoard() const;
  std::string getRTOSType() const;
  bool useHAL() const;
  bool useFreeRTOS() const;
  bool useArduino() const;
  bool usePlatformIO() const;
};
